/*
 * Unit: sdk/sce/intr
 *
 * Functions:
 *   0x002796c8 DisableIntc
 *   0x00279730 EnableIntc
 *   0x00279798 DisableDmac
 *   0x00279800 EnableDmac
 *   0x00279868 iEnableIntc
 *   0x00279888 iDisableIntc
 *   0x002798a8 iEnableDmac
 *   0x002798c8 iDisableDmac
 *   0x002798e8 setup
 *   0x002798f8 Copy
 *   0x00279908 kCopy
 *   0x00279940 GetEntryAddress
 *   0x00279950 InitAlarm
 */

struct syscall_entry {
    int number;
    void *handler;
};

extern struct syscall_entry D_00613328[8];
extern unsigned int D_00612BC0[];
extern unsigned int D_00613300[];

#define alarm_syscalls D_00613328
#define alarm_handler_code D_00612BC0
#define alarm_trampoline_code D_00613300

#define ALARM_STAT (*(volatile int *)0x10001810)
#define ALARM_HANDLER_ADDR ((void *)0x80076000)
#define ALARM_TRAMPOLINE_ADDR ((void *)0x82000)

#define STATUS_EIE 0x10000

extern int DIntr(void);
extern int EIntr(void);
extern int _EnableIntc(int ch);
extern int _DisableIntc(int ch);
extern int _EnableDmac(int ch);
extern int _DisableDmac(int ch);
extern int _iEnableIntc(int ch);
extern int _iDisableIntc(int ch);
extern int _iEnableDmac(int ch);
extern int _iDisableDmac(int ch);
void FlushCache(int mode);


int DisableIntc(int ch)
{
    int eie, res;

    __asm__ volatile ("mfc0 %0, $12" : "=r"(eie));
    eie &= STATUS_EIE;
    if (eie) {
        DIntr();
    }
    res = _DisableIntc(ch);
    __asm__ volatile ("sync");
    if (eie) {
        EIntr();
    }
    return res;
}


int EnableIntc(int ch)
{
    int eie, res;

    __asm__ volatile ("mfc0 %0, $12" : "=r"(eie));
    eie &= STATUS_EIE;
    if (eie) {
        DIntr();
    }
    res = _EnableIntc(ch);
    __asm__ volatile ("sync");
    if (eie) {
        EIntr();
    }
    return res;
}


int DisableDmac(int ch)
{
    int eie, res;

    __asm__ volatile ("mfc0 %0, $12" : "=r"(eie));
    eie &= STATUS_EIE;
    if (eie) {
        DIntr();
    }
    res = _DisableDmac(ch);
    __asm__ volatile ("sync");
    if (eie) {
        EIntr();
    }
    return res;
}


int EnableDmac(int ch)
{
    int eie, res;

    __asm__ volatile ("mfc0 %0, $12" : "=r"(eie));
    eie &= STATUS_EIE;
    if (eie) {
        DIntr();
    }
    res = _EnableDmac(ch);
    __asm__ volatile ("sync");
    if (eie) {
        EIntr();
    }
    return res;
}


int iEnableIntc(int ch)
{
    int res;

    res = _iEnableIntc(ch);
    __asm__ volatile ("sync");
    return res;
}


int iDisableIntc(int ch)
{
    int res;

    res = _iDisableIntc(ch);
    __asm__ volatile ("sync");
    return res;
}


int iEnableDmac(int ch)
{
    int res;

    res = _iEnableDmac(ch);
    __asm__ volatile ("sync");
    return res;
}


int iDisableDmac(int ch)
{
    int res;

    res = _iDisableDmac(ch);
    __asm__ volatile ("sync");
    return res;
}


static void setup(int number, void *handler)
{
    __asm__ volatile ("addiu $3, $0, 0x74; syscall");
}


static void Copy(void *dst, void *src, int size)
{
    __asm__ volatile ("addiu $3, $0, 0x5a; syscall");
}


int kCopy(unsigned int *dst, unsigned int *src, unsigned int size)
{
    unsigned int i;

    for (i = 0; i < size / 4; i++) {
        *dst++ = *src++;
    }
    return 0;
}


static void *GetEntryAddress(int number)
{
    __asm__ volatile ("addiu $3, $0, 0x5b; syscall");
}


void InitAlarm(void)
{
    unsigned int i;

    if (ALARM_STAT & 0x100) {
        return;
    }
    setup(alarm_syscalls[0].number, alarm_syscalls[0].handler);
    Copy(ALARM_HANDLER_ADDR, alarm_handler_code, 0x740);
    Copy(ALARM_TRAMPOLINE_ADDR, alarm_trampoline_code, 0x28);
    FlushCache(0);
    FlushCache(2);
    setup(alarm_syscalls[1].number, alarm_syscalls[1].handler);
    for (i = 2; i < 8; i++) {
        setup(alarm_syscalls[i].number, GetEntryAddress(alarm_syscalls[i].number));
    }
}
