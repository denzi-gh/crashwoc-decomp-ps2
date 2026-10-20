/*
 * Unit: sdk/sce/diei
 *
 * Functions:
 *   0x00280b50 DIntr
 *   0x00280b98 EIntr
 */

#define STATUS_EIE 0x10000


int DIntr(void)
{
    int eie, res;

    __asm__ volatile ("mfc0 %0, $12" : "=r"(eie));
    eie &= STATUS_EIE;
    if (eie) {
        int stat;

        res = eie != 0;
        __asm__ volatile (".p2align 3");
    retry:
        __asm__ volatile ("di");
        __asm__ volatile ("sync.p");
        __asm__ volatile ("mfc0 %0, $12" : "=r"(stat));
        if (stat & STATUS_EIE) {
            goto retry;
        }
    } else {
        res = 0;
    }
    return res;
}


int EIntr(void)
{
    int eie;

    __asm__ volatile ("mfc0 %0, $12" : "=r"(eie));
    eie &= STATUS_EIE;
    __asm__ volatile ("ei");
    return eie != 0;
}
