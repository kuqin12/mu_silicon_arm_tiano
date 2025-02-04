#/** @file
# ARM processor package.
#
# Copyright (c) 2009 - 2010, Apple Inc. All rights reserved.<BR>
# Copyright (c) 2011 - 2021, Arm Limited. All rights reserved.<BR>
# Copyright (c) 2016, Linaro Ltd. All rights reserved.<BR>
# Copyright (c) Microsoft Corporation.<BR>
# Copyright (c) 2021, Ampere Computing LLC. All rights reserved.
#
#    SPDX-License-Identifier: BSD-2-Clause-Patent
#
#**/

################################################################################
#
# Defines Section - statements that will be processed to create a Makefile.
#
################################################################################
[Defines]
  PLATFORM_NAME                  = ArmPkg
  PLATFORM_GUID                  = 5CFBD99E-3C43-4E7F-8054-9CDEAFF7710F
  PLATFORM_VERSION               = 0.1
  DSC_SPECIFICATION              = 0x00010005
  OUTPUT_DIRECTORY               = Build/Arm
  SUPPORTED_ARCHITECTURES        = ARM|AARCH64
  BUILD_TARGETS                  = DEBUG|RELEASE|NOOPT
  SKUID_IDENTIFIER               = DEFAULT

[BuildOptions]
  RELEASE_*_*_CC_FLAGS  = -DMDEPKG_NDEBUG
  *_*_*_CC_FLAGS  = -DDISABLE_NEW_DEPRECATED_INTERFACES

[PcdsFixedAtBuild]
  gEfiMdePkgTokenSpaceGuid.PcdDefaultTerminalType|4

!include MdePkg/MdeLibs.dsc.inc

[LibraryClasses.common]
  BaseLib|MdePkg/Library/BaseLib/BaseLib.inf
  BaseMemoryLib|MdePkg/Library/BaseMemoryLib/BaseMemoryLib.inf
  BootLogoLib|MdeModulePkg/Library/BootLogoLib/BootLogoLib.inf
  CacheMaintenanceLib|ArmPkg/Library/ArmCacheMaintenanceLib/ArmCacheMaintenanceLib.inf
  CapsuleLib|MdeModulePkg/Library/DxeCapsuleLibNull/DxeCapsuleLibNull.inf
  DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
  DxeServicesLib|MdePkg/Library/DxeServicesLib/DxeServicesLib.inf
  HobLib|MdePkg/Library/DxeHobLib/DxeHobLib.inf
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf
  TimerLib|MdePkg/Library/BaseTimerLibNullTemplate/BaseTimerLibNullTemplate.inf
  UefiBootManagerLib|MdeModulePkg/Library/UefiBootManagerLib/UefiBootManagerLib.inf
  UefiBootServicesTableLib|MdePkg/Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
  UefiDriverEntryPoint|MdePkg/Library/UefiDriverEntryPoint/UefiDriverEntryPoint.inf
  UefiLib|MdePkg/Library/UefiLib/UefiLib.inf
  DevicePathLib|MdePkg/Library/UefiDevicePathLib/UefiDevicePathLib.inf
  UefiRuntimeServicesTableLib|MdePkg/Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf
  PeCoffLib|MdePkg/Library/BasePeCoffLib/BasePeCoffLib.inf
  PeCoffGetEntryPointLib|MdePkg/Library/BasePeCoffGetEntryPointLib/BasePeCoffGetEntryPointLib.inf
  PeCoffExtraActionLib|MdePkg/Library/BasePeCoffExtraActionLibNull/BasePeCoffExtraActionLibNull.inf

  UefiHiiServicesLib|MdeModulePkg/Library/UefiHiiServicesLib/UefiHiiServicesLib.inf
  HiiLib|MdeModulePkg/Library/UefiHiiLib/UefiHiiLib.inf

  SemihostLib|ArmPkg/Library/SemihostLib/SemihostLib.inf
  DxeServicesTableLib|MdePkg/Library/DxeServicesTableLib/DxeServicesTableLib.inf
  DefaultExceptionHandlerLib|ArmPkg/Library/DefaultExceptionHandlerLib/DefaultExceptionHandlerLib.inf
  CpuExceptionHandlerLib|ArmPkg/Library/ArmExceptionLib/ArmExceptionLib.inf

  CpuLib|MdePkg/Library/BaseCpuLib/BaseCpuLib.inf
  ArmGicLib|ArmPkg/Drivers/ArmGic/ArmGicLib.inf
  ArmGicArchLib|ArmPkg/Library/ArmGicArchLib/ArmGicArchLib.inf
  ArmGenericTimerCounterLib|ArmPkg/Library/ArmGenericTimerPhyCounterLib/ArmGenericTimerPhyCounterLib.inf
  ArmSvcLib|ArmPkg/Library/ArmSvcLib/ArmSvcLib.inf
  ArmSmcLib|ArmPkg/Library/ArmSmcLib/ArmSmcLib.inf
  ArmDisassemblerLib|ArmPkg/Library/ArmDisassemblerLib/ArmDisassemblerLib.inf
  OpteeLib|ArmPkg/Library/OpteeLib/OpteeLib.inf

  UefiApplicationEntryPoint|MdePkg/Library/UefiApplicationEntryPoint/UefiApplicationEntryPoint.inf
  PerformanceLib|MdePkg/Library/BasePerformanceLibNull/BasePerformanceLibNull.inf
  SerialPortLib|MdePkg/Library/BaseSerialPortLibNull/BaseSerialPortLibNull.inf

  FdtLib|EmbeddedPkg/Library/FdtLib/FdtLib.inf

  ShellLib|ShellPkg/Library/UefiShellLib/UefiShellLib.inf
  FileHandleLib|MdePkg/Library/UefiFileHandleLib/UefiFileHandleLib.inf
  SortLib|MdeModulePkg/Library/UefiSortLib/UefiSortLib.inf

  IoLib|MdePkg/Library/BaseIoLibIntrinsic/BaseIoLibIntrinsic.inf

  ArmLib|ArmPkg/Library/ArmLib/ArmBaseLib.inf
  ArmMmuLib|ArmPkg/Library/ArmMmuLib/ArmMmuBaseLib.inf
  ArmTransferListLib|ArmPkg/Library/ArmTransferListLib/ArmTransferListLib.inf
  ArmFfaLib|ArmPkg/Library/ArmFfaLib/ArmFfaDxeLib.inf

  ArmMtlLib|ArmPkg/Library/ArmMtlNullLib/ArmMtlNullLib.inf

  OemMiscLib|ArmPkg/Universal/Smbios/OemMiscLibNull/OemMiscLibNull.inf

  ArmSvcLib|ArmPkg/Library/ArmSvcLib/ArmSvcLib.inf              # MU_CHANGE
  SafeIntLib|MdePkg/Library/BaseSafeIntLib/BaseSafeIntLib.inf   # MU_CHANGE
  NULL|MdePkg/Library/StackCheckLibNull/StackCheckLibNull.inf # MU_CHANGE: /GS and -fstack-protector support
  DxeMemoryProtectionHobLib|MdeModulePkg/Library/MemoryProtectionHobLibNull/DxeMemoryProtectionHobLibNull.inf # MU_CHANGE

# MU_CHANGE [BEGIN]: Use the PEI ARM Mmu Lib for PEIMs and PEI_CORE
[LibraryClasses.common.PEIM, LibraryClasses.common.PEI_CORE]
  ArmMmuLib|ArmPkg/Library/ArmMmuLib/ArmMmuPeiLib.inf
# MU_CHANGE [END]

[LibraryClasses.common.PEIM]
  HobLib|MdePkg/Library/PeiHobLib/PeiHobLib.inf
  PeimEntryPoint|MdePkg/Library/PeimEntryPoint/PeimEntryPoint.inf
  MemoryAllocationLib|MdePkg/Library/PeiMemoryAllocationLib/PeiMemoryAllocationLib.inf
  PeiServicesLib|MdePkg/Library/PeiServicesLib/PeiServicesLib.inf
  PeiServicesTablePointerLib|MdePkg/Library/PeiServicesTablePointerLib/PeiServicesTablePointerLib.inf
  ArmFfaLib|ArmPkg/Library/ArmFfaLib/ArmFfaPeiLib.inf

[LibraryClasses.ARM, LibraryClasses.AARCH64]
  #NULL|ArmPkg/Library/CompilerIntrinsicsLib/CompilerIntrinsicsLib.inf          # MU_CHANGE
  NULL|MdePkg/Library/CompilerIntrinsicsLib/ArmCompilerIntrinsicsLib.inf    # MU_CHANGE

  # Add support for GCC stack protector
  # NULL|MdePkg/Library/BaseStackCheckLib/BaseStackCheckLib.inf               # MU_CHANGE

[LibraryClasses.common.MM_CORE_STANDALONE]
  StandaloneMmCoreEntryPoint|ArmPkg/Library/ArmStandaloneMmCoreEntryPoint/ArmStandaloneMmCoreEntryPoint.inf
  HobLib|StandaloneMmPkg/Library/StandaloneMmHobLib/StandaloneMmHobLib.inf

[LibraryClasses.common.MM_STANDALONE]
  StandaloneMmDriverEntryPoint|MdePkg/Library/StandaloneMmDriverEntryPoint/StandaloneMmDriverEntryPoint.inf
  HobLib|StandaloneMmPkg/Library/StandaloneMmHobLib/StandaloneMmHobLib.inf
  MmServicesTableLib|MdePkg/Library/StandaloneMmServicesTableLib/StandaloneMmServicesTableLib.inf
  MemoryAllocationLib|StandaloneMmPkg/Library/StandaloneMmMemoryAllocationLib/StandaloneMmMemoryAllocationLib.inf  # MU_CHANGE

[Components.common]
  ArmPkg/Library/ArmCacheMaintenanceLib/ArmCacheMaintenanceLib.inf
  ArmPkg/Library/ArmDisassemblerLib/ArmDisassemblerLib.inf
  #ArmPkg/Library/CompilerIntrinsicsLib/CompilerIntrinsicsLib.inf               # MU_CHANGE
  ArmPkg/Library/DebugAgentSymbolsBaseLib/DebugAgentSymbolsBaseLib.inf
  ArmPkg/Library/DebugPeCoffExtraActionLib/DebugPeCoffExtraActionLib.inf
  ArmPkg/Library/DefaultExceptionHandlerLib/DefaultExceptionHandlerLib.inf
  ArmPkg/Library/SemiHostingDebugLib/SemiHostingDebugLib.inf
  ArmPkg/Library/SemiHostingSerialPortLib/SemiHostingSerialPortLib.inf
  ArmPkg/Library/SemihostLib/SemihostLib.inf
  ArmPkg/Library/ArmExceptionLib/ArmExceptionLib.inf
  ArmPkg/Library/ArmExceptionLib/ArmRelocateExceptionLib.inf
  ArmPkg/Library/SecurePartitionEntryPoint/SecurePartitionEntryPoint.inf
  ArmPkg/Library/PlatformFfaInterruptLibNull/PlatformFfaInterruptLib.inf
  ArmPkg/Library/ArmFfaLibEx/ArmFfaLibEx.inf
  ArmPkg/Library/Tpm2DeviceLibFfa/Tpm2DeviceLibFfa.inf
  ArmPkg/Library/SecurePartitionServicesTableLib/SecurePartitionServicesTableLib.inf
  ArmPkg/Library/SecurePartitionMemoryAllocationLib/SecurePartitionMemoryAllocationLib.inf

  ArmPkg/Drivers/CpuDxe/CpuDxe.inf
  ArmPkg/Drivers/CpuPei/CpuPei.inf
  ArmPkg/Drivers/ArmGic/ArmGicDxe.inf
  ArmPkg/Drivers/ArmGic/ArmGicLib.inf
  # ArmPkg/Drivers/ArmPsciMpServicesDxe/ArmPsciMpServicesDxe.inf    # MU_CHANGE: Only build MP driver for AARCH64
  ArmPkg/Drivers/GenericWatchdogDxe/GenericWatchdogDxe.inf
  ArmPkg/Drivers/TimerDxe/TimerDxe.inf

  ArmPkg/Library/ArmGenericTimerPhyCounterLib/ArmGenericTimerPhyCounterLib.inf
  ArmPkg/Library/ArmGenericTimerVirtCounterLib/ArmGenericTimerVirtCounterLib.inf

  ArmPkg/Library/ArmTrngLib/ArmTrngLib.inf
  ArmPkg/Library/ArmHvcLib/ArmHvcLib.inf
  ArmPkg/Library/ArmHvcLibNull/ArmHvcLibNull.inf
  ArmPkg/Library/ArmMonitorLib/ArmMonitorLib.inf
  ArmPkg/Library/ArmSmcLib/ArmSmcLib.inf
  ArmPkg/Library/ArmSmcLibNull/ArmSmcLibNull.inf
  ArmPkg/Library/ArmSvcLib/ArmSvcLib.inf
  ArmPkg/Library/OpteeLib/OpteeLib.inf
  ArmPkg/Library/ArmTransferListLib/ArmTransferListLib.inf
  ArmPkg/Library/ArmFfaLib/ArmFfaPeiLib.inf
  ArmPkg/Library/ArmFfaLib/ArmFfaDxeLib.inf
  ArmPkg/Library/ArmFfaLib/ArmFfaStandaloneMmCoreLib.inf
  ArmPkg/Library/ArmFfaLib/ArmFfaStandaloneMmLib.inf

  ArmPkg/Filesystem/SemihostFs/SemihostFs.inf

  ArmPkg/Library/ArmMmuLib/ArmMmuBaseLib.inf

  ArmPkg/Drivers/ArmPciCpuIo2Dxe/ArmPciCpuIo2Dxe.inf
  ArmPkg/Library/ArmArchTimerLib/ArmArchTimerLib.inf
  ArmPkg/Library/ArmGicArchLib/ArmGicArchLib.inf
  ArmPkg/Library/ArmGicArchSecLib/ArmGicArchSecLib.inf
  ArmPkg/Library/ArmLib/ArmBaseLib.inf
  ArmPkg/Library/ArmMtlNullLib/ArmMtlNullLib.inf
  ArmPkg/Library/ArmSoftFloatLib/ArmSoftFloatLib.inf
  ArmPkg/Library/ArmSmcPsciResetSystemLib/ArmSmcPsciResetSystemLib.inf
  ArmPkg/Library/PeiServicesTablePointerLib/PeiServicesTablePointerLib.inf
  ArmPkg/Library/PlatformBootManagerLib/PlatformBootManagerLib.inf
  ArmPkg/Library/LinuxBootBootManagerLib/LinuxBootBootManagerLib.inf

  ArmPkg/Drivers/ArmCrashDumpDxe/ArmCrashDumpDxe.inf
  ArmPkg/Drivers/ArmScmiDxe/ArmScmiDxe.inf

  ArmPkg/Universal/Smbios/ProcessorSubClassDxe/ProcessorSubClassDxe.inf
  ArmPkg/Universal/Smbios/SmbiosMiscDxe/SmbiosMiscDxe.inf
  ArmPkg/Universal/Smbios/OemMiscLibNull/OemMiscLibNull.inf
  ArmPkg/Drivers/MmCommunicationPei/MmCommunicationPei.inf
  # MU_CHANGE [BEGIN]
  ArmPkg/Drivers/SmmuDxe/SmmuDxe.inf
  # MU_CHANGE [END]

  ArmPkg/Library/NotificationServiceLib/NotificationServiceLib.inf
  ArmPkg/Library/TestServiceLib/TestServiceLib.inf
  ArmPkg/Library/TpmServiceLib/TpmServiceLib.inf
  ArmPkg/Library/ArmStandaloneMmCoreEntryPoint/ArmStandaloneMmCoreEntryPoint.inf

# [Components.common.MM_STANDALONE] MU_CHANGE
  ArmPkg/Drivers/StandaloneMmCpu/StandaloneMmCpu.inf

[Components.AARCH64]
  ArmPkg/Drivers/ArmPsciMpServicesDxe/ArmPsciMpServicesDxe.inf
  ArmPkg/Drivers/MmCommunicationDxe/MmCommunication.inf
  ArmPkg/Library/ArmMmuLib/ArmMmuPeiLib.inf

[Components.AARCH64, Components.ARM]
  ArmPkg/Library/StandaloneMmMmuLib/ArmMmuStandaloneMmLib.inf
  ArmPkg/Library/MmuLib/BaseMmuLib.inf    # MU_CHANGE - Add BaseMmuLib
