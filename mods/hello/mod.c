#include "mailbox.h"
#include "retail.h"

void hello_tick(void)
{
    if (modsdk_mailbox.magic != MODSDK_MAILBOX_MAGIC) {
        modsdk_mailbox.magic = MODSDK_MAILBOX_MAGIC;
        modsdk_mailbox.version = MODSDK_MAILBOX_VERSION;
        modsdk_mailbox.frame = 0;
    }
    modsdk_mailbox.frame++;
    if (modsdk_mailbox.frame % 50 == 0) {
        plr_wumpas++;
    }
}
