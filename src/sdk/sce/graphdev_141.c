/*
 * Unit: sdk/sce/graphdev_141
 *
 * Functions:
 *   0x00262790 sceGsPutDrawEnv
 */


typedef unsigned int u_int;
typedef unsigned long u_long;

typedef struct {
    u_long NLOOP : 15;
    u_long EOP : 1;
    u_long pad16 : 16;
    u_long id : 14;
    u_long PRE : 1;
    u_long PRIM : 11;
    u_long FLG : 2;
    u_long NREG : 4;
    u_long REGS0 : 4;
    u_long REGS1 : 4;
    u_long REGS2 : 4;
    u_long REGS3 : 4;
    u_long REGS4 : 4;
    u_long REGS5 : 4;
    u_long REGS6 : 4;
    u_long REGS7 : 4;
    u_long REGS8 : 4;
    u_long REGS9 : 4;
    u_long REGS10 : 4;
    u_long REGS11 : 4;
    u_long REGS12 : 4;
    u_long REGS13 : 4;
    u_long REGS14 : 4;
    u_long REGS15 : 4;
} sceGifTag;

#define D2_CHCR ((volatile u_int *)0x1000A000)
#define D2_MADR ((volatile u_int *)0x1000A010)
#define D2_QWC ((volatile u_int *)0x1000A020)

int printf(const char *format, ...);


int sceGsPutDrawEnv(sceGifTag *giftag)
{
    u_int count;

    count = 0;
    while (*D2_CHCR & 0x100) {
        if (count++ > 0x1000000) {
            printf("sceGsPutDrawEnv: DMA Ch.2 does not terminate\r\n");
            return -1;
        }
    }
    *D2_QWC = giftag->NLOOP + 1;
    if (((u_int)giftag & 0x70000000) == 0x70000000) {
        *D2_MADR = ((u_int)giftag & 0x0FFFFFFF) | 0x80000000;
    } else {
        *D2_MADR = (u_int)giftag & 0x0FFFFFFF;
    }
    *D2_CHCR = 0x101;
    return 0;
}
