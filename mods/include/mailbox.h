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
    /* Fills the [mailbox] region declared in mod.toml (size minus the 0x20
     * header words above). Coop v8 needs 0x240 of payload, so the region is
     * 0x400; no SDK field moved, the coop layout bump (COOP_MAILBOX_VERSION)
     * carries the change to the PC side. */
    unsigned char payload[0x3E0];
} ModsdkMailbox;

extern volatile ModsdkMailbox modsdk_mailbox;

#endif
