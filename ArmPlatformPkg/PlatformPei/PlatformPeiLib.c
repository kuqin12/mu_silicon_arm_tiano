/** @file

  Copyright (c) 2011-2014, ARM Limited. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>

#include <Library/ArmPlatformLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>

EFI_STATUS
EFIAPI
PlatformPeim (
  VOID
  )
{
  VOID *new = AllocateCopyPool ((UINTN)PcdGet32 (PcdFvSize), (VOID*)(UINTN)PcdGet64 (PcdFvBaseAddress));

  if (new == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  BuildFvHob ((EFI_PHYSICAL_ADDRESS)(UINTN)new, PcdGet32 (PcdFvSize));

  return EFI_SUCCESS;
}
