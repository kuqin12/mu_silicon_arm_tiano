/** @file

  Copyright (c), Microsoft Corporation.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef FFA_DIRECT_REQ2_PROTOCOL_H_
#define FFA_DIRECT_REQ2_PROTOCOL_H_

#define FFA_DIRECT_REQ2_PROTOCOL_GUID \
  { 0x18cf84a2, 0x14bb, 0x413e, { 0x93, 0xb8, 0xf8, 0x8e, 0x4f, 0xd7, 0x64, 0x7e } }

typedef struct _FFA_DIRECT_REQ2_PROTOCOL FFA_DIRECT_REQ2_PROTOCOL;

/**
  This structure is used to pass the input arguments to the FFA_PROCESS_INPUT_ARGS
  function, as well as the message to be returned to the sender.
**/
typedef struct {
  // The message to be sent to the receiver. The message is an array of 14 UINT64 values, 
  // each corresponding to a register value that is passed to the receiver (X4-x17).
  UINT64  Message[14];
} FFA_MSG_DIRECT_2;

/**
  This function is called to process the input arguments passed to the FFA_MSG_SEND_DIRECT_REQ2
  service. The function is responsible for processing the input arguments and returning the
  output arguments to the caller.

  @param[in]  This        A pointer to the FFA_DIRECT_REQ2_PROTOCOL instance.
  @param[in]  SenderID    The ID of the sender of the message.
  @param[in]  ReceiverID  The ID of the receiver of the message.
  @param[in]  Input       A pointer to the input arguments passed to the service.
  @param[out] Output      A pointer to the output arguments to be returned to the sender.

  @retval EFI_SUCCESS           The address range from Start to Start+Length was flushed from
                                the processor's data cache.
  @retval EFI_UNSUPPORTED       The processor does not support the cache flush type specified
                                by FlushType.
  @retval EFI_DEVICE_ERROR      The address range from Start to Start+Length could not be flushed
                                from the processor's data cache.

**/
typedef
EFI_STATUS
(EFIAPI *FFA_PROCESS_INPUT_ARGS)(
  IN  FFA_DIRECT_REQ2_PROTOCOL  *This,
  IN  UINT16                    SenderID,
  IN  UINT16                    ReceiverID,
  IN  FFA_MSG_DIRECT_2          *Input,
  OUT FFA_MSG_DIRECT_2          *Output
  );

struct _FFA_DIRECT_REQ2_PROTOCOL {
  EFI_GUID                ProtocolID;
  FFA_PROCESS_INPUT_ARGS  ProcessInputArgs;
};

extern EFI_GUID gFfaDirectReq2ProtocolGuid;

#endif /* FFA_DIRECT_REQ2_PROTOCOL_H_ */
