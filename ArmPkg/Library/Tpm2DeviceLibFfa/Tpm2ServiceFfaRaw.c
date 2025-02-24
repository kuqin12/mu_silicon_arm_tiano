/** @file
  This library provides an implementation of Tpm2DeviceLib
  using ARM64 SMC calls to request TPM service.

  The implementation is only supporting the Command Response Buffer (CRB)
  for sharing data with the TPM.

Copyright (c) 2021, Microsoft Corporation. All rights reserved. <BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Uefi.h>
#include <IndustryStandard/ArmFfaSvc.h>
#include <IndustryStandard/ArmFfaBootInfo.h>
#include <IndustryStandard/ArmFfaPartInfo.h>
#include <Guid/Tpm2ServiceFfa.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/Tpm2DeviceLib.h>
#include <Library/ArmFfaLib.h>

#include "Tpm2DeviceLibFfa.h"

UINT32  mFfaTpm2PartitionId = MAX_UINT32;

/**
  Check the return status from the FF-A call and returns EFI_STATUS

  @param EFI_LOAD_ERROR  FF-A status code returned in x0

  @retval EFI_SUCCESS    The entry point is executed successfully.
**/
EFI_STATUS
EFIAPI
TranslateTpmReturnStatus (
  UINTN  TpmReturnStatus
  )
{
  EFI_STATUS  Status;

  switch (TpmReturnStatus) {
    case TPM2_FFA_SUCCESS_OK:
    case TPM2_FFA_SUCCESS_OK_RESULTS_RETURNED:
      Status = EFI_SUCCESS;
      break;

    case TPM2_FFA_ERROR_NOFUNC:
      Status = EFI_NOT_FOUND;
      break;

    case TPM2_FFA_ERROR_NOTSUP:
      Status = EFI_UNSUPPORTED;
      break;

    case TPM2_FFA_ERROR_INVARG:
      Status = EFI_INVALID_PARAMETER;
      break;

    case TPM2_FFA_ERROR_INV_CRB_CTRL_DATA:
      Status = EFI_COMPROMISED_DATA;
      break;

    case TPM2_FFA_ERROR_ALREADY:
      Status = EFI_ALREADY_STARTED;
      break;

    case TPM2_FFA_ERROR_DENIED:
      Status = EFI_ACCESS_DENIED;
      break;

    case TPM2_FFA_ERROR_NOMEM:
      Status = EFI_OUT_OF_RESOURCES;
      break;

    default:
      Status = EFI_DEVICE_ERROR;
  }

  return Status;
}

/*
  This function is used to get the TPM service partition id.

  @param[out] PartitionId - Supplies the pointer to the TPM service partition id.

  @retval EFI_SUCCESS           The TPM command was successfully sent to the TPM
                                and the response was copied to the Output buffer.
  @retval EFI_INVALID_PARAMETER The TPM command buffer is NULL or the TPM command
                                buffer size is 0.
  @retval EFI_DEVICE_ERROR      An error occurred in communication with the TPM.
*/
EFI_STATUS
GetTpmServicePartitionId (
  OUT UINT32  *PartitionId
  )
{
  EFI_STATUS              Status;
  UINT32                  Count;
  UINT32                  Size;
  EFI_FFA_PART_INFO_DESC  *TpmPartInfo;
  VOID                    *TxBuffer;
  UINT64                  TxBufferSize;
  VOID                    *RxBuffer;
  UINT64                  RxBufferSize;
  UINT16                  PartId;

  if (PartitionId == NULL) {
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }

  if (mFfaTpm2PartitionId != MAX_UINT32) {
    *PartitionId = mFfaTpm2PartitionId;
    Status       = EFI_SUCCESS;
    goto Exit;
  }

  if (PcdGet16 (PcdTpmServiceFfaPartitionId) != 0) {
    mFfaTpm2PartitionId = PcdGet16 (PcdTpmServiceFfaPartitionId);
    *PartitionId        = mFfaTpm2PartitionId;
    Status              = EFI_SUCCESS;

    goto Exit;
  }

  Status = ArmFfaLibPartitionIdGet (&PartId);
  if (EFI_ERROR (Status)) {
    DEBUG ((
      DEBUG_ERROR,
      "Failed to get partition id. Status: %r\n",
      Status
      ));
    goto Exit;
  }

  Status = ArmFfaLibGetRxTxBuffers (
             &TxBuffer,
             &TxBufferSize,
             &RxBuffer,
             &RxBufferSize
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to get Rx/Tx Buffer. Status: %r\n", Status));
    goto Exit;
  }

  Status = ArmFfaLibPartitionInfoGet (
             &gEfiTpm2ServiceFfaGuid,
             FFA_PART_INFO_FLAG_TYPE_DESC,
             &Count,
             &Size
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to get Tpm2 partition info. Status: %r\n", Status));
    goto Exit;
  }

  if ((Count != 1) || (Size < sizeof (EFI_FFA_PART_INFO_DESC))) {
    Status = EFI_INVALID_PARAMETER;
    DEBUG ((DEBUG_ERROR, "Invalid partition Info(%g). Count: %d, Size: %d\n", &gEfiTpm2ServiceFfaGuid, Count, Size));
  } else {
    TpmPartInfo         = (EFI_FFA_PART_INFO_DESC *)RxBuffer;
    mFfaTpm2PartitionId = TpmPartInfo->PartitionId;
    *PartitionId        = mFfaTpm2PartitionId;

    Status = PcdSet16S (PcdTpmServiceFfaPartitionId, mFfaTpm2PartitionId);
  }

  ArmFfaLibRxRelease (PartId);

Exit:
  return Status;
}

EFI_STATUS
Tpm2GetInterfaceVersion (
  OUT UINT32  *Version
  )
{
  EFI_STATUS       Status;
  DIRECT_MSG_ARGS  FfaDirectReq2Args;

  if (Version == NULL) {
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }

  if (mFfaTpm2PartitionId == MAX_UINT32) {
    GetTpmServicePartitionId (&mFfaTpm2PartitionId);
  }

  ZeroMem (&FfaDirectReq2Args, sizeof (DIRECT_MSG_ARGS));
  FfaDirectReq2Args.Arg0 = TPM2_FFA_GET_INTERFACE_VERSION;

  Status = ArmFfaLibMsgSendDirectReq2 (mFfaTpm2PartitionId, &gEfiTpm2ServiceFfaGuid, &FfaDirectReq2Args);
  if (EFI_ERROR (Status)) {
    goto Exit;
  }

  Status = TranslateTpmReturnStatus (FfaDirectReq2Args.Arg0);

  if (!EFI_ERROR (Status)) {
    *Version = FfaDirectReq2Args.Arg1;
  }

Exit:
  return Status;
}

/*
  This function is used to get the TPM feature information.

  @param[out] FeatureInfo - Supplies the pointer to the feature information.

  @retval EFI_SUCCESS           The TPM command was successfully sent to the TPM
                                and the response was copied to the Output buffer.
  @retval EFI_INVALID_PARAMETER The TPM command buffer is NULL or the TPM command
                                buffer size is 0.
  @retval EFI_DEVICE_ERROR      An error occurred in communication with the TPM.
*/
EFI_STATUS
Tpm2GetFeatureInfo (
  OUT UINT32  *FeatureInfo
  )
{
  EFI_STATUS       Status;
  DIRECT_MSG_ARGS  FfaDirectReq2Args;

  if (FeatureInfo == NULL) {
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }

  if (mFfaTpm2PartitionId == MAX_UINT32) {
    GetTpmServicePartitionId (&mFfaTpm2PartitionId);
  }

  ZeroMem (&FfaDirectReq2Args, sizeof (DIRECT_MSG_ARGS));
  FfaDirectReq2Args.Arg0 = TPM2_FFA_GET_FEATURE_INFO;
  FfaDirectReq2Args.Arg1 = TPM_SERVICE_FEATURE_SUPPORT_NOTIFICATION;

  Status = ArmFfaLibMsgSendDirectReq2 (mFfaTpm2PartitionId, &gEfiTpm2ServiceFfaGuid, &FfaDirectReq2Args);
  if (EFI_ERROR (Status)) {
    goto Exit;
  }

  Status = TranslateTpmReturnStatus (FfaDirectReq2Args.Arg0);

Exit:
  return Status;
}

EFI_STATUS
Tpm2ServiceStart (
  IN UINT64  FuncQualifier,
  IN UINT64  LocalityQualifier
  )
{
  EFI_STATUS       Status;
  DIRECT_MSG_ARGS  FfaDirectReq2Args;

  if (mFfaTpm2PartitionId == MAX_UINT32) {
    GetTpmServicePartitionId (&mFfaTpm2PartitionId);
  }

  ZeroMem (&FfaDirectReq2Args, sizeof (DIRECT_MSG_ARGS));
  FfaDirectReq2Args.Arg0 = TPM2_FFA_START;
  FfaDirectReq2Args.Arg1 = (FuncQualifier & 0xFF);
  FfaDirectReq2Args.Arg2 = (LocalityQualifier & 0xFF);

  Status = ArmFfaLibMsgSendDirectReq2 (mFfaTpm2PartitionId, &gEfiTpm2ServiceFfaGuid, &FfaDirectReq2Args);
  if (EFI_ERROR (Status)) {
    goto Exit;
  }

  Status = TranslateTpmReturnStatus (FfaDirectReq2Args.Arg0);

Exit:
  return Status;
}

EFI_STATUS
Tpm2RegisterNotification (
  IN BOOLEAN  NotificationTypeQualifier,
  IN UINT16   vCpuId,
  IN UINT64   NotificationId
  )
{
  EFI_STATUS       Status;
  DIRECT_MSG_ARGS  FfaDirectReq2Args;

  if (mFfaTpm2PartitionId == MAX_UINT32) {
    GetTpmServicePartitionId (&mFfaTpm2PartitionId);
  }

  ZeroMem (&FfaDirectReq2Args, sizeof (DIRECT_MSG_ARGS));
  FfaDirectReq2Args.Arg0 = TPM2_FFA_REGISTER_FOR_NOTIFICATION;
  FfaDirectReq2Args.Arg1 = (NotificationTypeQualifier << 16 | vCpuId);
  FfaDirectReq2Args.Arg2 = (NotificationId & 0xFF);

  Status = ArmFfaLibMsgSendDirectReq2 (mFfaTpm2PartitionId, &gEfiTpm2ServiceFfaGuid, &FfaDirectReq2Args);
  if (EFI_ERROR (Status)) {
    goto Exit;
  }

  Status = TranslateTpmReturnStatus (FfaDirectReq2Args.Arg0);

Exit:
  return Status;
}

EFI_STATUS
Tpm2UnregisterNotification (
  VOID
  )
{
  EFI_STATUS       Status;
  DIRECT_MSG_ARGS  FfaDirectReq2Args;

  if (mFfaTpm2PartitionId == MAX_UINT32) {
    GetTpmServicePartitionId (&mFfaTpm2PartitionId);
  }

  ZeroMem (&FfaDirectReq2Args, sizeof (DIRECT_MSG_ARGS));
  FfaDirectReq2Args.Arg0 = TPM2_FFA_UNREGISTER_FROM_NOTIFICATION;

  Status = ArmFfaLibMsgSendDirectReq2 (mFfaTpm2PartitionId, &gEfiTpm2ServiceFfaGuid, &FfaDirectReq2Args);
  if (EFI_ERROR (Status)) {
    goto Exit;
  }

  Status = TranslateTpmReturnStatus (FfaDirectReq2Args.Arg0);

Exit:
  return Status;
}

EFI_STATUS
Tpm2FinishNotified (
  VOID
  )
{
  EFI_STATUS       Status;
  DIRECT_MSG_ARGS  FfaDirectReq2Args;

  if (mFfaTpm2PartitionId == MAX_UINT32) {
    GetTpmServicePartitionId (&mFfaTpm2PartitionId);
  }

  ZeroMem (&FfaDirectReq2Args, sizeof (DIRECT_MSG_ARGS));
  FfaDirectReq2Args.Arg0 = TPM2_FFA_FINISH_NOTIFIED;

  Status = ArmFfaLibMsgSendDirectReq2 (mFfaTpm2PartitionId, &gEfiTpm2ServiceFfaGuid, &FfaDirectReq2Args);
  if (EFI_ERROR (Status)) {
    goto Exit;
  }

  Status = TranslateTpmReturnStatus (FfaDirectReq2Args.Arg0);

Exit:
  return Status;
}
