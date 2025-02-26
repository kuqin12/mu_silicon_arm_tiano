/** @file

  Copyright (c) 2016 HP Development Company, L.P.
  Copyright (c) 2016 - 2021, Arm Limited. All rights reserved.
  Copyright (c) 2021, Linaro Limited

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <Pi/PiMmCis.h>

#include <Library/PcdLib.h>   // MU_CHANGE ARM_CP_997351F8E3
#include <Library/ArmSvcLib.h>
#include <Library/ArmLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/SafeIntLib.h>

#include <Protocol/DebugSupport.h> // for EFI_SYSTEM_CONTEXT

#include <Guid/ZeroGuid.h>
#include <Guid/MmramMemoryReserve.h>

#include <IndustryStandard/ArmFfaSvc.h>
#include <IndustryStandard/ArmStdSmc.h>

#include "StandaloneMmCpu.h"

EFI_STATUS
EFIAPI
MmFoundationEntryRegister (
  IN CONST EFI_MM_CONFIGURATION_PROTOCOL  *This,
  IN EFI_MM_ENTRY_POINT                   MmEntryPoint
  );

//
// On ARM platforms every event is expected to have a GUID associated with
// it. It will be used by the MM Entry point to find the handler for the
// event. It will either be populated in a EFI_MM_COMMUNICATE_HEADER by the
// caller of the event (e.g. MM_COMMUNICATE SMC) or by the CPU driver
// (e.g. during an asynchronous event). In either case, this context is
// maintained in an array which has an entry for each CPU. The pointer to this
// array is held in PerCpuGuidedEventContext. Memory is allocated once the
// number of CPUs in the system are made known through the
// MP_INFORMATION_HOB_DATA.
//
EFI_MM_COMMUNICATE_HEADER  **PerCpuGuidedEventContext = NULL;

// Descriptor with whereabouts of memory used for communication with the normal world
EFI_MMRAM_DESCRIPTOR  mNsCommBuffer;

MP_INFORMATION_HOB_DATA  *mMpInformationHobData;

EFI_MM_CONFIGURATION_PROTOCOL  mMmConfig = {
  0,
  MmFoundationEntryRegister
};

STATIC EFI_MM_ENTRY_POINT  mMmEntryPoint = NULL;

/**
  The PI Standalone MM entry point for the TF-A CPU driver.

  @param  [in] EventId            The event Id.
  @param  [in] CpuNumber          The CPU number.
  @param  [in] NsCommBufferAddr   Address of the NS common buffer.

  @retval   EFI_SUCCESS             Success.
  @retval   EFI_INVALID_PARAMETER   A parameter was invalid.
  @retval   EFI_ACCESS_DENIED       Access not permitted.
  @retval   EFI_OUT_OF_RESOURCES    Out of resources.
  @retval   EFI_UNSUPPORTED         Operation not supported.
**/
EFI_STATUS
PiMmStandaloneArmTfCpuDriverEntry (
  IN UINTN  EventId,
  IN UINTN  CpuNumber,
  IN UINTN  NsCommBufferAddr
  )
{
  EFI_MM_COMMUNICATE_HEADER  *GuidedEventContext;
  EFI_MM_ENTRY_CONTEXT       MmEntryPointContext;
  EFI_STATUS                 Status;
  UINTN                      NsCommBufferSize;

  DEBUG ((DEBUG_INFO, "Received event - 0x%x on cpu %d\n", EventId, CpuNumber));

  Status = EFI_SUCCESS;
  //
  // ARM TF passes SMC FID of the MM_COMMUNICATE interface as the Event ID upon
  // receipt of a synchronous MM request. Use the Event ID to distinguish
  // between synchronous and asynchronous events.
  //
  if ((ARM_SMC_ID_MM_COMMUNICATE != EventId) &&
      (ARM_SVC_ID_FFA_MSG_SEND_DIRECT_REQ != EventId))
  {
    DEBUG ((DEBUG_INFO, "UnRecognized Event - 0x%x\n", EventId));
    return EFI_INVALID_PARAMETER;
  }

  // Perform parameter validation of NsCommBufferAddr
  if (NsCommBufferAddr == (UINTN)NULL) {
    return EFI_INVALID_PARAMETER;
  }

  // MU_CHANGE ARM_CP_997351F8E3 [BEGIN]
  if (!FixedPcdGetBool (PcdArmMmCommunicateFromEl3Workaround)) {
    if (NsCommBufferAddr < mNsCommBuffer.PhysicalStart) {
      return EFI_ACCESS_DENIED;
    }
  }

  // MU_CHANGE ARM_CP_997351F8E3 [END]

  if ((NsCommBufferAddr + sizeof (EFI_MM_COMMUNICATE_HEADER)) >=
      (mNsCommBuffer.PhysicalStart + mNsCommBuffer.PhysicalSize))
  {
    return EFI_INVALID_PARAMETER;
  }

  // Find out the size of the buffer passed
  NsCommBufferSize = ((EFI_MM_COMMUNICATE_HEADER *)NsCommBufferAddr)->MessageLength +
                     sizeof (EFI_MM_COMMUNICATE_HEADER);

  // perform bounds check.
  if (NsCommBufferAddr + NsCommBufferSize >=
      mNsCommBuffer.PhysicalStart + mNsCommBuffer.PhysicalSize)
  {
    return EFI_ACCESS_DENIED;
  }

  GuidedEventContext = NULL;
  // Now that the secure world can see the normal world buffer, allocate
  // memory to copy the communication buffer to the secure world.
  Status = mMmst->MmAllocatePool (
                    EfiRuntimeServicesData,
                    NsCommBufferSize,
                    (VOID **)&GuidedEventContext
                    );

  if (Status != EFI_SUCCESS) {
    DEBUG ((DEBUG_INFO, "Mem alloc failed - 0x%x\n", EventId));
    return EFI_OUT_OF_RESOURCES;
  }

  // X1 contains the VA of the normal world memory accessible from
  // S-EL0
  CopyMem (GuidedEventContext, (CONST VOID *)NsCommBufferAddr, NsCommBufferSize);

  // Stash the pointer to the allocated Event Context for this CPU
  PerCpuGuidedEventContext[CpuNumber] = GuidedEventContext;

  ZeroMem (&MmEntryPointContext, sizeof (EFI_MM_ENTRY_CONTEXT));

  MmEntryPointContext.CurrentlyExecutingCpu = CpuNumber;
  MmEntryPointContext.NumberOfCpus          = mMpInformationHobData->NumberOfProcessors;

  // Populate the MM system table with MP and state information
  mMmst->CurrentlyExecutingCpu = CpuNumber;
  mMmst->NumberOfCpus          = mMpInformationHobData->NumberOfProcessors;
  mMmst->CpuSaveStateSize      = 0;
  mMmst->CpuSaveState          = NULL;

  if (mMmEntryPoint == NULL) {
    DEBUG ((DEBUG_INFO, "Mm Entry point Not Found\n"));
    return EFI_UNSUPPORTED;
  }

  mMmEntryPoint (&MmEntryPointContext);

  // Free the memory allocation done earlier and reset the per-cpu context
  ASSERT (GuidedEventContext);
  if (GuidedEventContext->MessageLength) {
    CopyMem ((VOID *)NsCommBufferAddr, (CONST VOID *)GuidedEventContext, NsCommBufferSize);
  }

  Status = mMmst->MmFreePool ((VOID *)GuidedEventContext);
  if (Status != EFI_SUCCESS) {
    return EFI_OUT_OF_RESOURCES;
  }

  PerCpuGuidedEventContext[CpuNumber] = NULL;

  return Status;
}

/**
  Registers the MM foundation entry point.

  @param  [in] This               Pointer to the MM Configuration protocol.
  @param  [in] MmEntryPoint       Function pointer to the MM Entry point.

  @retval   EFI_SUCCESS             Success.
**/
EFI_STATUS
EFIAPI
MmFoundationEntryRegister (
  IN CONST EFI_MM_CONFIGURATION_PROTOCOL  *This,
  IN EFI_MM_ENTRY_POINT                   MmEntryPoint
  )
{
  // store the entry point in a global
  mMmEntryPoint = MmEntryPoint;
  return EFI_SUCCESS;
}

/**
  This function is the main entry point for an MM handler dispatch
  or communicate-based callback.

  @param  DispatchHandle  The unique handle assigned to this handler by
                          MmiHandlerRegister().
  @param  Context         Points to an optional handler context which was
                          specified when the handler was registered.
  @param  CommBuffer      A pointer to a collection of data in memory that will
                          be conveyed from a non-MM environment into an
                          MM environment.
  @param  CommBufferSize  The size of the CommBuffer.

  @return Status Code

**/
EFI_STATUS
EFIAPI
PiMmCpuTpFwRootMmiHandler (
  IN     EFI_HANDLE  DispatchHandle,
  IN     CONST VOID  *Context         OPTIONAL,
  IN OUT VOID        *CommBuffer      OPTIONAL,
  IN OUT UINTN       *CommBufferSize  OPTIONAL
  )
{
  EFI_STATUS  Status;
  UINTN       CpuNumber;
  UINTN       LocalMessageLength;       // MU_CHANGE - Pin to UINT64 MessageLength.

  ASSERT (Context == NULL);
  ASSERT (CommBuffer == NULL);
  ASSERT (CommBufferSize == NULL);

  CpuNumber = mMmst->CurrentlyExecutingCpu;
  if (PerCpuGuidedEventContext[CpuNumber] == NULL) {
    return EFI_NOT_FOUND;
  }

  DEBUG ((
    DEBUG_INFO,
    "CommBuffer - 0x%x, CommBufferSize - 0x%x\n",
    PerCpuGuidedEventContext[CpuNumber],
    PerCpuGuidedEventContext[CpuNumber]->MessageLength
    ));

  // MU_CHANGE [BEGIN] - Pin to UINT64 MessageLength.
  LocalMessageLength = 0;
  Status             = SafeUint64ToUintn (PerCpuGuidedEventContext[CpuNumber]->MessageLength, &LocalMessageLength);
  if (EFI_ERROR (Status)) {
    DEBUG ((
      DEBUG_ERROR,
      "[%a] Message length too long! 0x%016lX > 0x%016lX\n",
      __FUNCTION__,
      PerCpuGuidedEventContext[CpuNumber]->MessageLength,
      MAX_UINTN
      ));
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  // MU_CHANGE [END] - Pin to UINT64 MessageLength.

  Status = mMmst->MmiManage (
                    &PerCpuGuidedEventContext[CpuNumber]->HeaderGuid,
                    NULL,
                    PerCpuGuidedEventContext[CpuNumber]->Data,
                    &LocalMessageLength         // MU_CHANGE
                    );

  if (Status != EFI_SUCCESS) {
    DEBUG ((DEBUG_WARN, "Unable to manage Guided Event - %d\n", Status));
    // MU_CHANGE [BEGIN] - Pin to UINT64 MessageLength.
  } else {
    PerCpuGuidedEventContext[CpuNumber]->MessageLength = LocalMessageLength;
    // MU_CHANGE [END] - Pin to UINT64 MessageLength.
  }

  return Status;
}
