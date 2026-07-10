/* Fixed-address region an external bridge (PCSX2 PINE) reads/writes.
 * Address is printed by build.py and stored in the blob header at
 * blob_base+0x30. This layout is the PC-side contract -- version-bump on
 * any change. */
#ifndef MODSDK_MAILBOX_H
#define MODSDK_MAILBOX_H

#define MODSDK_MAILBOX_MAGIC 0x424D5743u /* "CWMB" */
#define MODSDK_MAILBOX_VERSION 1u

typedef struct ModsdkMailbox {
    unsigned int magic;
    unsigned int version;
    unsigned int frame;
    unsigned int reserved[5];
    unsigned char payload[0x1E0];
} ModsdkMailbox;

extern volatile ModsdkMailbox modsdk_mailbox;

#endif
