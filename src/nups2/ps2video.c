/*
 * Unit: nups2/ps2video
 *
 * Functions:
 *   0x001687c0 tbRender
 *   0x00168c80 NuPs2Init
 *   0x00168db8 NuPs2InitIOP
 *   0x00169160 idmacGs
 *   0x001692a0 idmacVif1
 *   0x001693d0 NuPs2InitVideo
 *   0x00169870 NuPs2VideoScreenDump
 *   0x00169e90 NuPs2ScreenSwap
 *   0x0016a148 NuMcOpen
 *   0x0016a188 NuMcRead
 *   0x0016a1c8 NuMcWrite
 *   0x0016a208 NuMcClose
 *   0x0016a248 NuMcDelete
 *   0x0016a288 NuMcSeek
 *   0x0016a2c8 NuMcWriteFloat
 *   0x0016a318 NuMcWriteInt
 *   0x0016a368 NuMcWriteUnsignedInt
 *   0x0016a3b8 NuMcWriteShort
 *   0x0016a408 NuMcWriteChar
 *   0x0016a458 NuMcReadFloat
 *   0x0016a490 NuMcReadInt
 *   0x0016a4c8 NuMcReadUnsignedInt
 *   0x0016a500 NuMcReadShort
 *   0x0016a538 NuMcReadChar
 *   0x0016a570 tbsetCreate
 *   0x0016a680 tbsetDestroy
 *   0x0016a6a8 tbslotReset
 *   0x0016a6e8 tbslotLastValue
 *   0x0016a718 tbslotBeginFn
 *   0x0016a748 tbslotBeginExFn
 *   0x0016a778 tbslotEndFn
 *   0x0016a800 tbInit
 *   0x0016a8e8 NuPs2GetRenderWidth
 *   0x0016a8f8 NuPs2GetRenderHeight
 *   0x0016a908 NuPs2VideoSetPos
 *   0x0016a988 NuPs2GrabScreen
 *   0x0016aae0 NuPs2EnableAutoScreenSwap
 *   0x0016ab60 NuPs2DisableAutoScreenSwap
 *   0x0016abd0 NuPs2GetViewport
 *   0x0016ac58 NuEnableVBlankE
 *   0x0016ac90 NuDisableVBlankE
 *   0x0016acd0 NuIntAddVBlankE
 *   0x0016ad88 NuIntRemVBlankE
 *   0x0016ade8 NuEnableVBlankS
 *   0x0016ae20 NuDisableVBlankS
 *   0x0016ae60 NuIntAddVBlankS
 *   0x0016af18 NuIntRemVBlankS
 *   0x0016af78 NuEnableVBlank
 *   0x0016afd0 NuDisableVBlank
 *   0x0016b038 ps2videoGetFreeMem
 *   0x0016b090 NuPs2CopyCurrentBackBuffer
 *   0x0016b100 NuPs2CopyCurrentFrontBuffer
 *   0x0016b170 NuPs2CopyCurrentZBuffer
 *   0x0016b1e0 NuPs2GetCurrentBackBuffer
 *   0x0016b210 NuPs2GetCurrentFrontBuffer
 *   0x0016b240 NuPs2GetCurrentZBuffer
 *   0x0016b270 NuPs2GetRenderRange
 *   0x0016b280 NuPs2GetRenderXCentre
 *   0x0016b290 NuPs2GetRenderYCentre
 *   0x0016b2a0 timeUserReset
 *   0x0016b2c0 timeUserRead
 *   0x0016b2d0 NuIntAddAllHandlers
 *   0x0016b3a8 NuIntRemoveAllHandlers
 *   0x0016b430 ifnVif1
 *   0x0016b440 ifnVBlankE
 *   0x0016b4b0 ifnVBlankS
 *   0x0016b518 NuPs2FlushVRam
 *   0x0016b690 GenerateGammaTable
 *   0x0016b770 GetElapsedTime
 */

#include "nu_libc.h"
#include "nu_types.h"

#include "nucore/nuerror.h"
#include "nu3d/nutex.h"
#include "nu3d/nuvport.h"
#include "nups2/nups2types.h"

typedef struct GsDisplay {
    u64 dx : 12;
    u64 dy : 11;
    u64 magh : 4;
    u64 magv : 2;
    u64 pad29 : 3;
    u64 dw : 12;
    u64 dh : 11;
    u64 pad55 : 9;
} GsDisplay;

typedef struct GsDispEnv {
    u64 pmode;
    u64 smode2;
    u64 dispfb;
    union {
        GsDisplay display;
        u64 display_reg;
    };
    u64 bgcolor;
} GsDispEnv;

typedef struct TbSet {
    int values[16][2];
    int begin[16];
    int peak[16];
    int colour[16];
    int current[16];
    char *name[16];
} TbSet;

typedef struct sceDmaChan {
    unsigned int chcr;
} sceDmaChan;

typedef struct NuVBlankHandler {
    struct NuVBlankHandler *next;
    struct NuVBlankHandler *prev;
    void (*fn)(void);
    int priority;
} NuVBlankHandler;

typedef struct sceGsStoreImage {
    u32 vifcode[4];
    u64 giftag[2];
    u64 bitbltbuf;
    u64 bitbltbufaddr;
    u64 trxpos;
    u64 trxposaddr;
    u64 trxreg;
    u64 trxregaddr;
    u64 finish;
    u64 finishaddr;
    u64 trxdir;
    u64 trxdiraddr;
} sceGsStoreImage;

/* EE timer 1 counter: a free-running 16-bit tick used for screen-swap timing. */
#define EE_T1_COUNT (*(volatile unsigned int *)0x10000800)
#define EE_T0_COUNT (*(volatile unsigned int *)0x10000000)
#define EE_T0_MODE  (*(volatile unsigned int *)0x10000010)
#define D1_CHCR     (*(volatile unsigned int *)0x10009000)
#define D1_MADR     (*(volatile unsigned int *)0x10009010)
#define D1_TADR     (*(volatile unsigned int *)0x10009030)
#define D2_CHCR     (*(volatile unsigned int *)0x1000A000)
#define D2_MADR     (*(volatile unsigned int *)0x1000A010)
#define D2_TADR     (*(volatile unsigned int *)0x1000A030)
#define D_ENABLEW   (*(volatile unsigned int *)0x1000F590)
#define EE_T1_MODE  (*(volatile unsigned int *)0x10000810)
#define GIF_MODE    (*(volatile unsigned int *)0x10003010)

#define GS_PRIM       0x00
#define GS_RGBAQ      0x01
#define GS_XYZF2      0x04
#define GS_XYOFFSET_1 0x18
#define GS_PRMODECONT 0x1A
#define GS_SCISSOR_1  0x40
#define GS_TEST_1     0x47
#define GS_FRAME_1    0x4C
#define GS_A_D        0x0E

extern int sceMcOpen(int port, int slot, const char *name, int mode);
extern int sceMcRead(int fd, void *buf, int size);
extern int sceMcWrite(int fd, const void *buf, int size);
extern int sceMcClose(int fd);
extern int sceMcDelete(int port, int slot, const char *name);
extern int sceMcSeek(int fd, int offset, int mode);
extern int sceMcSync(int mode, int *cmd, int *result);
extern int NuPs2ScreenSwap(int vsync);
extern int EnableIntc(int cause);
extern int VU1_DmaGetStatus(void);
extern int sceGsSyncPath(int mode, unsigned short timeout);
extern int DisableIntc(int cause);
extern int AddIntcHandler(int cause, int (*handler)(int), int next);
extern int RemoveIntcHandler(int cause, int handler_id);
extern int AddDmacHandler(int channel, int (*handler)(int), int next);
extern int RemoveDmacHandler(int channel, int handler_id);
extern int EnableDmac(int channel);
extern int DisableDmac(int channel);
extern void *sceGsSyncVCallback(int (*func)(int));
extern void sceDevVif0Reset(void);
extern void sceDevVif1Reset(void);
extern void sceDevVu0Reset(void);
extern void sceDevVu1Reset(void);
extern void sceGsResetPath(void);
extern void sceDmaReset(int mode);
extern sceDmaChan *sceDmaGetChan(int id);
extern int GetThreadId(void);
extern int NuFileSifLoadModule(char *name, int arglen, char *args);
extern int sceSifLoadModule(const char *name, int arglen, const char *args);
extern void Exit(int status);
extern int sceMcInit(void);
extern int scePadInit(int mode);
extern int sceSifInitIopHeap(void);
extern int sceSdRemoteInit(void);
extern int sceGsSyncV(int mode);
extern void sceGsSetHalfOffset(struct NuPs2GsReg *draw, short centre_x, short centre_y, short half_offset);
extern void sceGsSetHalfOffset2(struct NuPs2GsReg *draw, short centre_x, short centre_y, short half_offset);
extern int sceGsSwapDBuffDc(GsDispEnv *db, int id);
extern int FlushCache(int operation);
extern int iFlushCache(int operation);
extern int ifnVif1(int ca);
extern int ifnVBlankE(int ca);
extern int ifnVBlankS(int ca);
extern int idmacGs(int channel);
extern int idmacVif1(int channel);
extern void NuMemFreeFn(void *ptr, char *file, int line);
extern void *NuMemAllocFn(int size, char *file, int line);
extern int sceGsSetDefStoreImage(sceGsStoreImage *sp, short sbp, short sbw, short spsm, short x, short y, short w, short h);
extern int sceGsExecStoreImage(sceGsStoreImage *sp, u128 *dstaddr);
extern int NuFileOpen(char *name, int mode);
extern int NuFileWrite(int fh, void *data, int size);
extern void NuFileClose(int fh);
extern int NuFileSize(char *name);
extern struct DmaTag *vpDmaTag_EndEx(struct DmaTag *tag, u32 vifcode0, u32 vifcode1);
extern void *vpDmaTag_Close(void *packet);
extern void *gsBeginGifPacket(void *packet, int eop, int nloop, int pre, int prim, int flg, int nreg, ...);
extern void *gsCloseGifPacket(void *packet);
extern void GS_DmaWrite(void *p, int mode);
extern int sceGsResetGraph(short mode, short inter, short omode, short ffmode);
extern void sceGsSetDefDBuffDc(GsDispEnv *db, short psm, short w, short h, short ztest, short zpsm, short clear);

#define ExitHandler() __asm__ volatile ("sync\n\tei")

extern int PS2_REZ_W;
extern int PS2_REZ_H;
extern int PS2_VCNTR_X;
extern int PS2_VCNTR_Y;
extern int D_006331C4;
extern volatile int D_0062F03C;
extern int D_006331F8;
extern TbSet *D_0062F034;
extern int D_006331E8;
extern int D_006331EC;
extern int D_006331F0;
extern int D_006331F4;
extern GsDispEnv ps2video_display_cfg[2];
extern int D_0062F08C;
extern int D_0062F090;
extern int D_0062F094;
extern int D_0062F098;
extern int D_0062F09C;
extern int D_0062F0A0;
extern int gs_dma_status;
extern int vu1_dma_status;
extern int nutex_use_path3;
extern struct NuPs2GsReg D_002EA190;
extern struct NuPs2GsReg D_002EA1A0;
extern struct NuPs2GsReg D_002EA300;
extern struct NuPs2GsReg D_002EA310;
extern char D_006164E0[];
extern int D_0062F06C;
extern int D_0062F074;
extern int D_0062F078;
extern int D_0062F080;
extern int D_0062F058;
extern NuVBlankHandler *D_0062F070;
extern NuVBlankHandler *D_0062F07C;
extern int D_006331CC;
extern volatile int nuvideo_global_vbcnt;
extern int D_002EA0D0[16];
extern int D_0062F084;
extern int D_006331D0;
extern int D_006331D4;
extern int D_006331D8;
extern int D_006331DC;
extern int D_006331E0;
extern float D_006B3730[256];
extern sceDmaChan *dmachan_vif1;
extern int D_006331C8;
extern int D_0062F068;
extern int nusound_irx_loaded;
extern unsigned int D_0062F040;
extern unsigned int D_0062F044;
extern int D_006331C0;
extern int ps2video_ignore_frameout;
extern int ps2video_novsync;
extern unsigned int ps2_video_size;
extern int nuvideo_progressive_scan;
extern int nurndr_half_offset;
extern int ps2_video_fmt;
extern int ps2_zb_fmt;
extern int nuvideo_global_framerate;
extern int D_002EA118[6];
extern struct NuPs2GsReg D_002EA210;
extern struct NuPs2GsReg D_002EA380;

#define NuPs2FrameCount D_006331C4
#define NuPs2SwapTime   D_0062F03C
#define NuPs2SwapDelay  D_006331F8
#define NuPs2DisplayX0  D_006331E8
#define NuPs2DisplayX1  D_006331EC
#define NuPs2DisplayY0  D_006331F0
#define NuPs2DisplayY1  D_006331F4
#define NuPs2DispEnv    ps2video_display_cfg
#define idmac_vif1_waiting  D_0062F08C
#define idmac_gs_waiting    D_0062F090
#define idmac_gs_count      D_0062F094
#define idmac_vif1_count    D_0062F098
#define idmac_gs_burst      D_0062F09C
#define idmac_gs_burst_left D_0062F0A0
#define NuPs2FrameReg0  D_002EA190
#define NuPs2ZBufReg0   D_002EA1A0
#define NuPs2FrameReg1  D_002EA300
#define NuPs2ZBufReg1   D_002EA310
#define ps2video_file   D_006164E0
#define vblank_end_handler    D_0062F06C
#define vblank_end_disable    D_0062F074
#define vblank_start_handler  D_0062F078
#define vblank_start_disable  D_0062F080
#define auto_screen_swap      D_0062F058
#define vblank_end_list       D_0062F070
#define vblank_start_list     D_0062F07C
#define in_vblank_end         D_006331CC
#define tb_default_colours    D_002EA0D0
#define tb_main_set           D_0062F034
#define handlers_installed    D_0062F084
#define vif1_intc_id          D_006331D0
#define vblank_end_intc_id    D_006331D4
#define vblank_start_intc_id  D_006331D8
#define vif1_dmac_id          D_006331DC
#define gs_dmac_id            D_006331E0
#define gamma_table           D_006B3730
#define main_thread_id        D_006331C8
#define iop_initialised       D_0062F068
#define swap_now              D_0062F040
#define swap_elapsed          D_0062F044
#define field_parity          D_006331C0
#define NuPs2Draw2FrameReg0   D_002EA210
#define NuPs2Draw2FrameReg1   D_002EA380
#define display_magh          D_002EA118


inline void NuPs2FlushVRam(int r, int g, int b)
{
    struct DmaTag packet[16];
    u64 *p;

    p = gsBeginGifPacket(vpDmaTag_EndEx(packet, 0, 0), 0, 1, 0, 0, 0, 1, GS_A_D);
    *p++ = 1;
    *p++ = GS_PRMODECONT;
    *p++ = 0x100000;
    *p++ = GS_FRAME_1;
    *p++ = 0;
    *p++ = GS_XYOFFSET_1;
    *p++ = ((u64)0x400 << 48) | (0x400 << 16);
    *p++ = GS_SCISSOR_1;
    *p++ = 0;
    *p++ = GS_TEST_1;
    *p++ = 6;
    *p++ = GS_PRIM;
    *p++ = r | ((u64)g << 8) | ((u64)b << 16) | ((u64)0x3F800000 << 32);
    *p++ = GS_RGBAQ;
    *p++ = 0;
    *p++ = GS_XYZF2;
    *p++ = 0x40004000;
    *p++ = GS_XYZF2;
    vpDmaTag_Close(gsCloseGifPacket(p));
    FlushCache(0);
    GS_DmaWrite(packet, 1);
}


inline void GenerateGammaTable(float gamma)
{
    int i;
    float scale;

    scale = 255.0 / pow(255.0, gamma);
    for (i = 0; i < 256; i++) {
        gamma_table[i] = scale * pow((float)i, gamma);
    }
}


inline int GetElapsedTime(void)
{
    int elapsed;

    elapsed = EE_T1_COUNT - NuPs2SwapTime;
    NuPs2SwapDelay = elapsed;
    if (elapsed < 0) {
        NuPs2SwapDelay = elapsed + 0x10000;
    }
    return NuPs2SwapDelay;
}


static inline int tbslotEnd(TbSet *set, int slot)
{
    unsigned int elapsed;

    set = set != 0 ? set : D_0062F034;
    slot &= 0xF;
    elapsed = EE_T1_COUNT - set->begin[slot];
    if (elapsed > 0xFFFF) {
        elapsed += 0xFFFF;
    }
    set->values[slot][1 - set->current[slot]] += elapsed;
    return set->values[slot][1 - set->current[slot]];
}


static inline TbSet *tbsetAlloc(int *colours)
{
    TbSet *set;
    int i;

    set = NuMemAllocFn(sizeof(TbSet), ps2video_file, 0x1CF);
    if (set) {
        memset(set, 0, sizeof(TbSet));
        if (colours) {
            for (i = 0; i < 16; i++) {
                set->colour[i] = colours[i];
            }
        } else {
            memcpy(set->colour, tb_default_colours, sizeof(set->colour));
        }
    }
    return set;
}


static inline void ps2videoAddAllHandlers(void)
{
    if (!handlers_installed) {
        vif1_intc_id = AddIntcHandler(5, ifnVif1, 0);
        EnableIntc(5);
        vblank_end_list = 0;
        vblank_end_intc_id = AddIntcHandler(3, ifnVBlankE, -1);
        EnableIntc(3);
        vblank_end_handler = 1;
        vblank_start_list = 0;
        sceGsSyncVCallback(ifnVBlankS);
        vblank_start_intc_id = 0;
        EnableIntc(2);
        vblank_start_handler = 1;
        vif1_dmac_id = AddDmacHandler(1, idmacVif1, 0);
        EnableDmac(1);
        gs_dmac_id = AddDmacHandler(2, idmacGs, 0);
        EnableDmac(2);
        handlers_installed = 1;
    }
}


void NuPs2Init(void)
{
    sceDevVif0Reset();
    sceDevVif1Reset();
    sceDevVu0Reset();
    sceDevVu1Reset();
    sceGsResetPath();
    sceDmaReset(1);
    dmachan_vif1 = sceDmaGetChan(1);
    dmachan_vif1->chcr |= 0x40;
    main_thread_id = GetThreadId();
    EE_T1_MODE = 0x83;
    tb_main_set = tbsetAlloc(0);
}


void NuPs2InitIOP(void)
{
    if (!iop_initialised) {
        iop_initialised = 1;

        if (NuFileSifLoadModule("SYS\\SIO2MAN.IRX", 0, 0) < 0
            && NuFileSifLoadModule("SYS\\SIO2MAN.IRX", 0, 0) < 0
            && NuFileSifLoadModule("/usr/local/sce/iop/modules/SIO2MAN.IRX", 0, 0) < 0
            && sceSifLoadModule("host0:/usr/local/sce/iop/modules/sio2man.irx", 0, 0) < 0) {
            printf("Can't load module sio2man
");
            Exit(0);
        }
        printf("Loaded module sio2man
");

        if (NuFileSifLoadModule("SYS\\PADMAN.IRX", 0, 0) < 0
            && NuFileSifLoadModule("/usr/local/sce/iop/modules/PADMAN.IRX", 0, 0) < 0
            && sceSifLoadModule("host0:/usr/local/sce/iop/modules/padman.irx", 0, 0) < 0) {
            printf("Can't load module padman
");
            Exit(0);
        }
        printf("Loaded module padman
");

        if (NuFileSifLoadModule("SYS\\MCMAN.IRX", 0, 0) < 0
            && NuFileSifLoadModule("/usr/local/sce/iop/modules/MCMAN.IRX", 0, 0) < 0
            && sceSifLoadModule("host0:/usr/local/sce/iop/modules/mcman.irx", 0, 0) < 0) {
            printf("Can't load module mcman
");
            Exit(0);
        }
        printf("Loaded module mcman
");

        if (NuFileSifLoadModule("SYS\\MCSERV.IRX", 0, 0) < 0
            && NuFileSifLoadModule("/usr/local/sce/iop/modules/mcserv.irx", 0, 0) < 0
            && sceSifLoadModule("host0:/usr/local/sce/iop/modules/mcserv.irx", 0, 0) < 0) {
            printf("Can't load module mcserv
");
            Exit(0);
        }
        printf("Loaded module mcserv
");

        if (NuFileSifLoadModule("SYS\\LIBSD.IRX", 0, 0) < 0
            && NuFileSifLoadModule("/usr/local/sce/iop/modules/libsd.irx", 0, 0) < 0
            && sceSifLoadModule("host0:/usr/local/sce/iop/modules/libsd.irx", 0, 0) < 0) {
            printf("Can't load module libsd
");
            Exit(0);
        }
        printf("Loaded module libsd
");

        nusound_irx_loaded = 1;
        if (NuFileSifLoadModule("SYS\\NUSOUND.IRX", 0, 0) < 0
            && NuFileSifLoadModule("/usr/local/sce/iop/modules/nusound.irx", 0, 0) < 0
            && sceSifLoadModule("host0:/usr/local/sce/iop/modules/nusound.irx", 0, 0) < 0) {
            printf("Can't load module nusound - sound services will be denied
");
            nusound_irx_loaded = 0;
        }
        printf("Loaded module nusound
");

        if (NuFileSifLoadModule("SYS\\SDRDRV.IRX", 0, 0) < 0
            && NuFileSifLoadModule("/usr/local/sce/iop/modules/sdrdrv.irx", 0, 0) < 0
            && sceSifLoadModule("host0:/usr/local/sce/iop/modules/sdrdrv.irx", 0, 0) < 0) {
            printf("Can't load module sdrdrv
");
            Exit(0);
        }
        printf("Loaded module sdrdrv
");

        printf("sceMcInit result %d
", sceMcInit());
        scePadInit(0);
        sceSifInitIopHeap();
        sceSdRemoteInit();
    }
}


int idmacGs(int channel)
{
    unsigned int tadr;
    unsigned int addr;
    unsigned int tag;

    if (channel == 2) {
        tadr = D2_TADR;
        addr = D2_MADR & 0x7FFFFFFF;
        tag = (D2_CHCR >> 16) & 0xFF;
        if (((D2_CHCR >> 28) & 7) == 7) {
            gs_dma_status = 0;
        }
        if (tag == 0x19) {
            idmac_gs_count++;
            if (idmac_vif1_waiting) {
                D_ENABLEW = 0x10000;
                D2_CHCR |= 0x100;
                D1_CHCR |= 0x100;
                D_ENABLEW = 0;
                idmac_vif1_waiting = 0;
            } else {
                if (nutex_use_path3) {
                    addr = *(unsigned int *)addr | 0x20000000;
                    ((unsigned char *)addr)[3] &= 0x7F;
                }
                if (idmac_gs_burst_left) {
                    if (gs_dma_status) {
                        D_ENABLEW = 0x10000;
                        D2_CHCR |= 0x100;
                        D_ENABLEW = 0;
                    }
                    idmac_gs_burst_left--;
                } else {
                    idmac_gs_waiting = 1;
                    idmac_gs_burst_left = idmac_gs_burst;
                }
            }
        } else {
            gs_dma_status = 0;
        }
    }
    return 1;
}


int idmacVif1(int channel)
{
    unsigned int tadr;
    unsigned char *packet;

    if (channel == 1) {
        tadr = D1_TADR;
        packet = (unsigned char *)((D1_MADR & 0x7FFFFFFF) | 0x20000000);
        if (((D1_CHCR >> 16) & 0xFF) == 0x18) {
            idmac_vif1_count++;
            if (packet[-0xD] & 0x80) {
                idmac_vif1_waiting = channel;
            } else {
                D_ENABLEW = 0x10000;
                D1_CHCR |= 0x100;
                D_ENABLEW = 0;
            }
            if (idmac_gs_waiting) {
                D_ENABLEW = 0x10000;
                D2_CHCR |= 0x100;
                D_ENABLEW = 0;
                idmac_gs_waiting = 0;
            }
        } else {
            tbslotEnd(0, 1);
            vu1_dma_status = 0;
        }
        return 1;
    }
    return 1;
}


void NuPs2InitVideo(int width, int height, int psm, int zpsm, int format)
{
    int interlace;
    int ffmode;

    ps2_video_fmt = format;
    ps2_zb_fmt = zpsm;
    switch (format) {
    case 0:
        ps2_video_size = 480;
        nuvideo_global_framerate = 60;
        interlace = 0;
        ffmode = 1;
        format = 2;
        nuvideo_progressive_scan = 0;
        break;
    case 3:
        ps2_video_size = 312;
        nuvideo_global_framerate = 50;
        interlace = 1;
        ffmode = 1;
        nuvideo_progressive_scan = 0;
        break;
    default:
        ps2_video_size = 262;
        nuvideo_global_framerate = 60;
        interlace = 1;
        ffmode = 1;
        nuvideo_progressive_scan = 0;
        break;
    case 4:
        ps2_video_size = 312;
        interlace = 1;
        nuvideo_global_framerate = 50;
        ffmode = 0;
        format = 3;
        height *= 2;
        nuvideo_progressive_scan = 1;
        break;
    case 5:
        ps2_video_size = 262;
        interlace = 1;
        nuvideo_global_framerate = 60;
        ffmode = 0;
        format = 2;
        height *= 2;
        nuvideo_progressive_scan = 1;
        break;
    case 6:
        ps2_video_size = 312;
        nuvideo_global_framerate = 50;
        interlace = 0;
        ffmode = 1;
        format = 3;
        nuvideo_progressive_scan = 0;
        break;
    case 7:
        ps2_video_size = 262;
        nuvideo_global_framerate = 60;
        interlace = 0;
        ffmode = 1;
        format = 2;
        nuvideo_progressive_scan = 0;
        break;
    }
    PS2_VCNTR_X = 0x800;
    PS2_VCNTR_Y = 0x800;
    PS2_REZ_W = width;
    PS2_REZ_H = height;
    sceGsResetGraph(0, interlace, format, ffmode);
    sceGsSetDefDBuffDc(NuPs2DispEnv, psm, width, height, 3, zpsm, 0);
    field_parity = !sceGsSyncV(0);
    NuPs2FrameCount = 0;
    EE_T0_MODE = 0x9F;
    ps2videoAddAllHandlers();
    switch (PS2_REZ_W) {
    case 256:
        display_magh[0] = 9;
        break;
    case 320:
        display_magh[0] = 7;
        break;
    case 384:
        display_magh[0] = 6;
        break;
    case 512:
        display_magh[0] = 4;
        break;
    case 640:
        display_magh[0] = 3;
        break;
    }
    NuPs2DisplayX0 = (int)NuPs2DispEnv[0].display_reg & 0xFFF;
    NuPs2DisplayY0 = (int)(NuPs2DispEnv[0].display_reg >> 12) & 0x7FF;
    NuPs2DisplayX1 = (int)NuPs2DispEnv[1].display_reg & 0xFFF;
    NuPs2DisplayY1 = (int)(NuPs2DispEnv[1].display_reg >> 12) & 0x7FF;
    NuPs2FlushVRam(0, 0, 0);
    GIF_MODE = 2;
}


static inline void ps2videoDumpRow(int fh, u8 *line, u128 *frame, int y)
{
    u8 *src;
    int x;

    src = (u8 *)(frame + y * PS2_REZ_W / 4);
    for (x = 0; x < PS2_REZ_W; x++) {
        src[0] = gamma_table[src[0]];
        src[1] = gamma_table[src[1]];
        src[2] = gamma_table[src[2]];
        line[x * 3] = src[2];
        line[x * 3 + 1] = src[1];
        line[x * 3 + 2] = src[0];
        src += 4;
    }
    NuFileWrite(fh, line, PS2_REZ_W * 3);
}


void NuPs2VideoScreenDump(char *name, int lace, float gamma, int idx)
{
    BmpFileHeader header;
    BmpInfoHeader info;
    u128 *frame[2];
    sceGsStoreImage image;
    char filename[256];
    u8 line[PS2_REZ_W * 3];
    int fields;
    int bytes;
    int fh;
    int n;
    int buffer;
    int y;

    GenerateGammaTable(gamma);
    if (idx >= 0) {
        sprintf(filename, "%s%d.bmp", name, idx);
    } else {
        sprintf(filename, "%s.bmp", name);
        n = 0;
        do {
            if (NuFileSize(filename) <= 0) {
                break;
            }
            sprintf(filename, "%s%03d.bmp", name, n++);
        } while (1);
    }
    fields = lace + 1;
    bytes = fields * 3;
    header.type = 0x4D42;
    header.size = PS2_REZ_W * PS2_REZ_H * 3 + 0x36;
    header.reserved1 = 0;
    header.reserved2 = 0;
    header.offbits = 0x36;
    info.size = 0x28;
    info.width = PS2_REZ_W;
    info.height = PS2_REZ_H * fields;
    info.planes = 1;
    info.bitcount = 24;
    info.compression = 0;
    info.imagesize = PS2_REZ_W * PS2_REZ_H * bytes;
    info.xppm = 1;
    info.yppm = 1;
    info.clrused = 0;
    info.clrimportant = 0;
    fh = NuFileOpen(filename, 1);
    NuFileWrite(fh, &header.type, 14);
    NuFileWrite(fh, &info, 40);
    frame[0] = NuMemAllocFn(PS2_REZ_W * PS2_REZ_H * 4, ps2video_file, 0x861);
    frame[1] = NuMemAllocFn(PS2_REZ_W * PS2_REZ_H * 4, ps2video_file, 0x862);
    while (VU1_DmaGetStatus()) {
    }
    sceGsSyncPath(0, 0);
    buffer = ~(NuPs2FrameCount ^ field_parity) & 1;
    sceGsSetDefStoreImage(&image, buffer ? 0 : PS2_REZ_W * PS2_REZ_H / 64, PS2_REZ_W / 64, 0,
                          0, 0, PS2_REZ_W, PS2_REZ_H);
    FlushCache(0);
    sceGsExecStoreImage(&image, frame[0]);
    sceGsSyncPath(0, 0);
    if (lace) {
        sceGsSetDefStoreImage(&image, buffer ? PS2_REZ_W * PS2_REZ_H / 64 : 0, PS2_REZ_W / 64, 0,
                              0, 0, PS2_REZ_W, PS2_REZ_H);
        FlushCache(0);
        sceGsExecStoreImage(&image, frame[1]);
        sceGsSyncPath(0, 0);
    }
    y = PS2_REZ_H;
    do {
        y--;
        if (lace) {
            ps2videoDumpRow(fh, line, frame[1], y);
        }
        ps2videoDumpRow(fh, line, frame[0], y);
    } while (y != 0);
    NuMemFreeFn(frame[0], ps2video_file, 0x891);
    NuMemFreeFn(frame[1], ps2video_file, 0x892);
    NuFileClose(fh);
}


int NuPs2ScreenSwap(int vsync)
{
    int half_offset;

    if (!in_vblank_end) {
        while (VU1_DmaGetStatus()) {
        }
        tbslotEnd(0, 0);
        sceGsSyncPath(0, 0);
    }

    swap_now = EE_T1_COUNT;
    swap_elapsed = swap_now - NuPs2SwapTime;
    if (swap_elapsed > 0xFFFF) {
        swap_elapsed += 0xFFFF;
    }
    if (ps2video_ignore_frameout || in_vblank_end) {
        swap_elapsed = 50;
    }

    if (vsync) {
        NuPs2FrameCount++;
    } else {
        NuDebugMsgProlog(ps2video_file, 0x921)("swap not");
    }

    if (!in_vblank_end) {
        if (ps2video_novsync) {
            field_parity = 0;
        } else {
            field_parity = sceGsSyncV(0);
        }
    } else {
        field_parity = 1 - field_parity;
    }

    if (swap_elapsed <= ps2_video_size && !nuvideo_progressive_scan) {
        sceGsSetHalfOffset((NuPs2FrameCount & 1) ? &NuPs2FrameReg1 : &NuPs2FrameReg0,
                           PS2_VCNTR_X, PS2_VCNTR_Y, field_parity);
        sceGsSetHalfOffset2((NuPs2FrameCount & 1) ? &NuPs2Draw2FrameReg0 : &NuPs2Draw2FrameReg1,
                            PS2_VCNTR_X, PS2_VCNTR_Y, field_parity);
        half_offset = 1;
        nurndr_half_offset = field_parity * 8;
    } else {
        sceGsSetHalfOffset((NuPs2FrameCount & 1) ? &NuPs2FrameReg1 : &NuPs2FrameReg0,
                           PS2_VCNTR_X, PS2_VCNTR_Y, 0);
        sceGsSetHalfOffset2((NuPs2FrameCount & 1) ? &NuPs2Draw2FrameReg0 : &NuPs2Draw2FrameReg1,
                            PS2_VCNTR_X, PS2_VCNTR_Y, 0);
        half_offset = 0;
        nurndr_half_offset = 0;
    }

    if (!in_vblank_end) {
        FlushCache(0);
    } else {
        iFlushCache(0);
    }

    if (vsync) {
        sceGsSwapDBuffDc(NuPs2DispEnv, NuPs2FrameCount & 1);
    } else {
        half_offset = 1;
    }

    NuPs2SwapTime = EE_T1_COUNT;
    if (!in_vblank_end) {
        sceGsSyncPath(0, 0);
    }
    return half_offset;
}


int NuMcOpen(int port, int slot, const char *name, int mode, int async)
{
    int cmd;
    int result;

    result = sceMcOpen(port, slot, name, mode);
    if (!async) {
        sceMcSync(0, &cmd, &result);
    }
    return result;
}


int NuMcRead(int fd, void *buf, int size, int async)
{
    int cmd;
    int result;

    result = sceMcRead(fd, buf, size);
    if (!async) {
        sceMcSync(0, &cmd, &result);
    }
    return result;
}


int NuMcWrite(int fd, const void *buf, int size, int async)
{
    int cmd;
    int result;

    result = sceMcWrite(fd, buf, size);
    if (!async) {
        sceMcSync(0, &cmd, &result);
    }
    return result;
}


int NuMcClose(int fd, int async)
{
    int cmd;
    int result;

    result = sceMcClose(fd);
    if (!async) {
        sceMcSync(0, &cmd, &result);
    }
    return result;
}


int NuMcDelete(int port, int slot, const char *name, int async)
{
    int cmd;
    int result;

    result = sceMcDelete(port, slot, name);
    if (!async) {
        sceMcSync(0, &cmd, &result);
    }
    return result;
}


int NuMcSeek(int fd, int offset, int mode, int async)
{
    int cmd;
    int result;

    result = sceMcSeek(fd, offset, mode);
    if (!async) {
        sceMcSync(0, &cmd, &result);
    }
    return result;
}


int NuMcWriteFloat(int fd, float value, int async)
{
    int cmd;
    int result;

    result = sceMcWrite(fd, &value, sizeof(value));
    if (!async) {
        sceMcSync(0, &cmd, &result);
    }
    return result;
}


int NuMcWriteInt(int fd, int value, int async)
{
    int cmd;
    int result;

    result = sceMcWrite(fd, &value, sizeof(value));
    if (!async) {
        sceMcSync(0, &cmd, &result);
    }
    return result;
}


int NuMcWriteUnsignedInt(int fd, unsigned int value, int async)
{
    int cmd;
    int result;

    result = sceMcWrite(fd, &value, sizeof(value));
    if (!async) {
        sceMcSync(0, &cmd, &result);
    }
    return result;
}


int NuMcWriteShort(int fd, short value, int async)
{
    int cmd;
    int result;

    result = sceMcWrite(fd, &value, sizeof(value));
    if (!async) {
        sceMcSync(0, &cmd, &result);
    }
    return result;
}


int NuMcWriteChar(int fd, char value, int async)
{
    int cmd;
    int result;

    result = sceMcWrite(fd, &value, sizeof(value));
    if (!async) {
        sceMcSync(0, &cmd, &result);
    }
    return result;
}


float NuMcReadFloat(int fd)
{
    float value;
    int cmd;
    int result;

    result = sceMcRead(fd, &value, sizeof(value));
    sceMcSync(0, &cmd, &result);
    return value;
}


int NuMcReadInt(int fd)
{
    int value;
    int cmd;
    int result;

    result = sceMcRead(fd, &value, sizeof(value));
    sceMcSync(0, &cmd, &result);
    return value;
}


unsigned int NuMcReadUnsignedInt(int fd)
{
    unsigned int value;
    int cmd;
    int result;

    result = sceMcRead(fd, &value, sizeof(value));
    sceMcSync(0, &cmd, &result);
    return value;
}


short NuMcReadShort(int fd)
{
    short value;
    int cmd;
    int result;

    result = sceMcRead(fd, &value, sizeof(value));
    sceMcSync(0, &cmd, &result);
    return value;
}


char NuMcReadChar(int fd)
{
    char value;
    int cmd;
    int result;

    result = sceMcRead(fd, &value, sizeof(value));
    sceMcSync(0, &cmd, &result);
    return value;
}


TbSet *tbsetCreate(int *colours)
{
    return tbsetAlloc(colours);
}


void tbsetDestroy(TbSet *set)
{
    NuMemFreeFn(set, ps2video_file, 0x1E7);
}


void tbslotReset(TbSet *set, int slot)
{
    int current;

    set = set != 0 ? set : D_0062F034;
    slot &= 0xF;
    current = set->current[slot] ^ 1;
    set->current[slot] = current;
    set->values[slot][current ? 0 : 1] = 0;
}


int tbslotLastValue(TbSet *set, int slot)
{
    set = set != 0 ? set : D_0062F034;
    slot &= 0xF;
    return set->values[slot][set->current[slot]];
}


void tbslotBeginFn(TbSet *set, int slot)
{
    set = set != 0 ? set : D_0062F034;
    slot &= 0xF;
    set->begin[slot] = EE_T1_COUNT;
    set->name[slot] = 0;
}


void tbslotBeginExFn(TbSet *set, int slot, char *name)
{
    set = set != 0 ? set : D_0062F034;
    slot &= 0xF;
    set->begin[slot] = EE_T1_COUNT;
    set->name[slot] = name;
}


void tbInit(void)
{
    EE_T1_MODE = 0x83;
    tb_main_set = tbsetAlloc(0);
}


float NuPs2GetRenderWidth(void)
{
    return (float)PS2_REZ_W;
}


float NuPs2GetRenderHeight(void)
{
    return (float)PS2_REZ_H;
}


void NuPs2VideoSetPos(int x, int y)
{
    NuPs2DispEnv[0].display.dx = NuPs2DisplayX0 + x;
    NuPs2DispEnv[0].display.dy = NuPs2DisplayY0 + y;
    NuPs2DispEnv[1].display.dx = NuPs2DisplayX1 + x;
    NuPs2DispEnv[1].display.dy = NuPs2DisplayY1 + y;
}


void *NuPs2GrabScreen(void *buf, int front, int x, int y, int w, int h)
{
    sceGsStoreImage image;
    u128 *dest;
    int buffer;
    int back;
    short base;

    if (buf) {
        dest = buf;
    } else {
        dest = NuMemAllocFn(w * h * 4, ps2video_file, 0x8A8);
    }
    while (VU1_DmaGetStatus()) {
    }
    sceGsSyncPath(0, 0);
    buffer = (NuPs2FrameCount ^ field_parity) & 1;
    back = 1 - buffer;
    if (front) {
        back = buffer;
    }
    if (!back) {
        base = PS2_REZ_W * PS2_REZ_H / 64;
    } else {
        base = 0;
    }
    sceGsSetDefStoreImage(&image, base, PS2_REZ_W / 64, 0, x, y, w, h);
    FlushCache(0);
    sceGsExecStoreImage(&image, dest);
    sceGsSyncPath(0, 0);
    FlushCache(0);
    return dest;
}


void NuPs2EnableAutoScreenSwap(void)
{
    if (!auto_screen_swap) {
        while (VU1_DmaGetStatus()) {
        }
        sceGsSyncPath(0, 0);
        while (abs(GetElapsedTime()) < 50) {
        }
        auto_screen_swap = 1;
    }
}


void NuPs2DisableAutoScreenSwap(void)
{
    if (auto_screen_swap) {
        while (abs(GetElapsedTime()) >= 100) {
        }
        auto_screen_swap = 0;
        sceGsSyncPath(0, 0);
    }
}


void NuPs2GetViewport(struct nuviewport_s *vp)
{
    memset(vp, 0, sizeof(struct nuviewport_s));
    vp->zmin = 8388607.0f;
    vp->x = (PS2_VCNTR_X - PS2_REZ_W / 2) * 16;
    vp->y = (PS2_VCNTR_Y - PS2_REZ_H / 2) * 16;
    vp->width = PS2_REZ_W * 16;
    vp->height = PS2_REZ_H * 16;
    vp->zmax = 0.0f;
}


void NuEnableVBlankE(void)
{
    if (vblank_end_handler) {
        if (--vblank_end_disable == 0) {
            EnableIntc(3);
        }
    }
}


void NuDisableVBlankE(void)
{
    if (vblank_end_handler) {
        if (vblank_end_disable == 0) {
            DisableIntc(3);
        }
        vblank_end_disable++;
    }
}


void NuIntAddVBlankE(NuVBlankHandler *handler)
{
    NuVBlankHandler *prev;
    NuVBlankHandler *cur;

    DisableIntc(3);
    prev = 0;
    cur = vblank_end_list;
    while (cur && cur->priority <= handler->priority) {
        prev = cur;
        cur = cur->next;
    }
    if (prev) {
        handler->next = prev->next;
        handler->prev = prev;
        prev->next = handler;
        if (handler->next) {
            handler->next->prev = handler;
        }
    } else {
        if (vblank_end_list) {
            vblank_end_list->prev = handler;
        }
        handler->prev = 0;
        handler->next = vblank_end_list;
        vblank_end_list = handler;
    }
    EnableIntc(3);
}


void NuIntRemVBlankE(NuVBlankHandler *handler)
{
    DisableIntc(3);
    if (handler->prev) {
        handler->prev->next = handler->next;
    } else {
        vblank_end_list = handler->next;
    }
    if (handler->next) {
        handler->next->prev = handler->prev;
    }
    EnableIntc(3);
}


void NuEnableVBlankS(void)
{
    if (vblank_start_handler) {
        if (--vblank_start_disable == 0) {
            EnableIntc(2);
        }
    }
}


void NuDisableVBlankS(void)
{
    if (vblank_start_handler) {
        if (vblank_start_disable == 0) {
            DisableIntc(2);
        }
        vblank_start_disable++;
    }
}


void NuIntAddVBlankS(NuVBlankHandler *handler)
{
    NuVBlankHandler *prev;
    NuVBlankHandler *cur;

    DisableIntc(2);
    prev = 0;
    cur = vblank_start_list;
    while (cur && cur->priority <= handler->priority) {
        prev = cur;
        cur = cur->next;
    }
    if (prev) {
        handler->next = prev->next;
        handler->prev = prev;
        prev->next = handler;
        if (handler->next) {
            handler->next->prev = handler;
        }
    } else {
        if (vblank_start_list) {
            vblank_start_list->prev = handler;
        }
        handler->prev = 0;
        handler->next = vblank_start_list;
        vblank_start_list = handler;
    }
    EnableIntc(2);
}


void NuIntRemVBlankS(NuVBlankHandler *handler)
{
    DisableIntc(2);
    if (handler->prev) {
        handler->prev->next = handler->next;
    } else {
        vblank_start_list = handler->next;
    }
    if (handler->next) {
        handler->next->prev = handler->prev;
    }
    EnableIntc(2);
}


void NuEnableVBlank(void)
{
    if (vblank_start_handler) {
        if (--vblank_start_disable == 0) {
            EnableIntc(2);
        }
    }
    if (vblank_end_handler) {
        if (--vblank_end_disable == 0) {
            EnableIntc(3);
        }
    }
}


void NuDisableVBlank(void)
{
    if (vblank_end_handler) {
        if (vblank_end_disable == 0) {
            DisableIntc(3);
        }
        vblank_end_disable++;
    }
    if (vblank_start_handler) {
        if (vblank_start_disable == 0) {
            DisableIntc(2);
        }
        vblank_start_disable++;
    }
}


void ps2videoGetFreeMem(int *pages, int *bytes)
{
    int used;

    used = (PS2_REZ_W * PS2_REZ_H * 3 + 63) / 64;
    if (pages) {
        *pages = used;
    }
    if (bytes) {
        *bytes = 0x400000 - used * 256;
    }
}


void *NuPs2CopyCurrentBackBuffer(void *buf)
{
    if (NuPs2FrameCount & 1) {
        memcpy(buf, &NuPs2FrameReg0, sizeof(struct NuPs2GsReg));
    } else {
        memcpy(buf, &NuPs2FrameReg1, sizeof(struct NuPs2GsReg));
    }
    return (char *)buf + sizeof(struct NuPs2GsReg);
}


void *NuPs2CopyCurrentFrontBuffer(void *buf)
{
    if (NuPs2FrameCount & 1) {
        memcpy(buf, &NuPs2FrameReg1, sizeof(struct NuPs2GsReg));
    } else {
        memcpy(buf, &NuPs2FrameReg0, sizeof(struct NuPs2GsReg));
    }
    return (char *)buf + sizeof(struct NuPs2GsReg);
}


void *NuPs2CopyCurrentZBuffer(void *buf)
{
    if (NuPs2FrameCount & 1) {
        memcpy(buf, &NuPs2ZBufReg0, sizeof(struct NuPs2GsReg));
    } else {
        memcpy(buf, &NuPs2ZBufReg1, sizeof(struct NuPs2GsReg));
    }
    return (char *)buf + sizeof(struct NuPs2GsReg);
}


void NuPs2GetCurrentBackBuffer(u64 *buf)
{
    if (NuPs2FrameCount & 1) {
        *buf = NuPs2FrameReg0.data;
    } else {
        *buf = NuPs2FrameReg1.data;
    }
}


void NuPs2GetCurrentFrontBuffer(u64 *buf)
{
    if (NuPs2FrameCount & 1) {
        *buf = NuPs2FrameReg1.data;
    } else {
        *buf = NuPs2FrameReg0.data;
    }
}


void NuPs2GetCurrentZBuffer(u64 *buf)
{
    if (NuPs2FrameCount & 1) {
        *buf = NuPs2ZBufReg0.data;
    } else {
        *buf = NuPs2ZBufReg1.data;
    }
}


float NuPs2GetRenderRange(void)
{
    return 4096.0f;
}


float NuPs2GetRenderXCentre(void)
{
    return (float)PS2_VCNTR_X;
}


float NuPs2GetRenderYCentre(void)
{
    return (float)PS2_VCNTR_Y;
}


void timeUserReset(void)
{
    EE_T0_MODE = 0x82;
    EE_T0_COUNT = 0;
}


unsigned int timeUserRead(void)
{
    return EE_T0_COUNT;
}


void NuIntAddAllHandlers(void)
{
    ps2videoAddAllHandlers();
}


void NuIntRemoveAllHandlers(void)
{
    if (handlers_installed) {
        vblank_end_handler = 0;
        vblank_start_handler = 0;
        DisableDmac(2);
        RemoveDmacHandler(2, gs_dmac_id);
        DisableDmac(1);
        RemoveDmacHandler(1, vif1_dmac_id);
        sceGsSyncVCallback(0);
        DisableIntc(3);
        RemoveIntcHandler(3, vblank_end_intc_id);
        DisableIntc(5);
        RemoveIntcHandler(5, vif1_intc_id);
        vblank_end_list = 0;
        vblank_start_list = 0;
        handlers_installed = 0;
    }
}


int ifnVif1(int ca)
{
    ExitHandler();
    return 0;
}


int ifnVBlankE(int ca)
{
    NuVBlankHandler *handler;

    in_vblank_end = 1;
    if (auto_screen_swap) {
        NuPs2ScreenSwap(1);
    }
    for (handler = vblank_end_list; handler; handler = handler->next) {
        handler->fn();
    }
    in_vblank_end = 0;
    ExitHandler();
    return 1;
}


int ifnVBlankS(int ca)
{
    NuVBlankHandler *handler;

    in_vblank_end = 1;
    nuvideo_global_vbcnt++;
    for (handler = vblank_start_list; handler; handler = handler->next) {
        handler->fn();
    }
    in_vblank_end = 0;
    ExitHandler();
    return 1;
}
