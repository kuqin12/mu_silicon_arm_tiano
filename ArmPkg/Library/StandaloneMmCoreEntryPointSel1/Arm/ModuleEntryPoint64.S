#------------------------------------------------------------------------------
#
# Entrypoint of StandaloneMm.
#
# Copyright (c) 2024, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
# @par Reference(s):
#   - [1] SPM based on the MM interface.
#        (https://trustedfirmware-a.readthedocs.io/en/latest/components/
#         secure-partition-manager-mm.html)
#   - [2] Arm Firmware Framework for Armv8-A, DEN0077, version 1.2
#        (https://developer.arm.com/documentation/den0077/latest/)
#   - [3] FF-A Memory Management Protocol for Armv8-A, DEN0140, version 1.2
#        (https://developer.arm.com/documentation/den0140/latest/)
#
#------------------------------------------------------------------------------

#include <AsmMacroIoLibV8.h>
#include <IndustryStandard/ArmMmSvc.h>
#include <IndustryStandard/ArmFfaSvc.h>
#include <Uefi/UefiBaseType.h>

  .align 12
// Define a data section to be used for setting up the
// stack for StandaloneMm
stmm_stack:
  .space FixedPcdGet32 (PcdStMmStackSize)

.text
//
// Check whether it is possible to use FF-A.
// If FF-A is supported return TRUE, otherwise return FALSE.
//
// BOOLEAN
// EFIAPI
// CheckFfaSupport (
//   VOID
//   )
//
ASM_FUNC(CheckFfaSupport)
  //
  // Try to check FF-A support via FFA_VERSION
  // See [2], Section 13.2 FFA_VERSION
  //
  MOV32 (x0, ARM_FID_FFA_VERSION)

  // Set x1 as request version.
  MOV32 (x1, ARM_FFA_CREATE_VERSION (
               ARM_FFA_MAJOR_VERSION,
               ARM_FFA_MINOR_VERSION))

  smc #0

  MOV64 (x9, ARM_FFA_RET_NOT_SUPPORTED)
  cmp x0, x9
  cset x0, ne
  mov x9, xzr
  ret

//
// Entry point of StandaloneMm
//
ASM_FUNC(_ModuleEntryPoint)
  // Stash boot information registers from the SPMC
  mov x19, x0
  mov x20, x1
  mov x21, x2
  mov x22, x3
  mov x23, x4

  bl CheckFfaSupport
  mov x1, x0

  // Get StandaloneMm Stack top address and save in x0
  adrp x4, stmm_stack
  mov x0, x4

  // Set stack permission
  cmp x1, #0x01
  b.eq .Lset_stack_perm_ffa

  // If SetStackPermission* failed, x0 is #0x00.
  // Otherwise, x0 is base address of stack.
.Lset_stmm_sp:
  cmp x0, #0x00
  b.eq .Lerror

  mov sp, x0

  // Restore boot information registers from the SPMC
  mov x0, x19
  mov x1, x20
  mov x2, x21
  mov x3, x22
  mov x4, x23
  mov x19, xzr
  mov x20, xzr
  mov x21, xzr
  mov x22, xzr
  mov x23, xzr

  // Invoke the C entrypoint
  b     CEntryPoint

.Lerror:
  b .

.Lset_stack_perm_ffa:
  b .Lset_stmm_sp
