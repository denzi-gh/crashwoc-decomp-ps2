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
#include "nu3d/nurndr.h"
#include "nu3d/nutex.h"
#include "nu3d/nuvport.h"
#include "nups2/nups2types.h"
#include "nups2/ps2video.h"

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
extern void sceGsResetGraph(short mode, short inter, short omode, short ffmode);
extern void sceGsSetDefDBuffDc(GsDispEnv *db, short psm, short w, short h, short ztest, short zpsm, short clear);
extern void NuFntSet(void *font);
extern void NuFntPrintEx(int x, int y, int flags, char *fmt, ...);
extern int NuRndrQuad2d(struct nuvtx2d_s *vtx, struct numtl_s *mtl);

#define ExitHandler() __asm__ volatile ("sync\n\tei")

extern int gs_dma_status;
extern int vu1_dma_status;
extern int nutex_use_path3;
extern int nurndr_half_offset;
extern void *system_font;
extern int NuFntHeight;
extern int NuFntBase;

/* GS register pairs of one draw environment (libgraph sceGsDrawEnv1/2). */
struct GsDrawEnv {
    struct NuPs2GsReg frame;      /* 0x00 */
    struct NuPs2GsReg zbuf;       /* 0x10 */
    struct NuPs2GsReg xyoffset;   /* 0x20 */
    struct NuPs2GsReg scissor;    /* 0x30 */
    struct NuPs2GsReg prmodecont; /* 0x40 */
    struct NuPs2GsReg colclamp;   /* 0x50 */
    struct NuPs2GsReg dthe;       /* 0x60 */
    struct NuPs2GsReg test;       /* 0x70 */
}; /* 0x80 */

/* libgraph sceGsClear. */
struct GsClear {
    struct NuPs2GsReg testa;      /* 0x00 */
    struct NuPs2GsReg prim;       /* 0x10 */
    struct NuPs2GsReg rgbaq;      /* 0x20 */
    struct NuPs2GsReg xyz2a;      /* 0x30 */
    struct NuPs2GsReg xyz2b;      /* 0x40 */
    struct NuPs2GsReg testb;      /* 0x50 */
}; /* 0x60 */

/* Double-buffered display and draw environments (libgraph sceGsDBuffDc). */
struct GsDBuffDc {
    GsDispEnv disp[2];            /* 0x000 */
    u128 giftag0;                 /* 0x050 */
    struct GsDrawEnv draw01;      /* 0x060 */
    struct GsDrawEnv draw02;      /* 0x0E0 */
    struct GsClear clear0;        /* 0x160 */
    u128 giftag1;                 /* 0x1C0 */
    struct GsDrawEnv draw11;      /* 0x1D0 */
    struct GsDrawEnv draw12;      /* 0x250 */
    struct GsClear clear1;        /* 0x2D0 */
}; /* 0x330 */

static u32 def_cols[16] = {
    0x00000080, 0x44444480, 0x00008880, 0x0000FF80,
    0x88000080, 0xFF000080, 0x88008880, 0xFF00FF80,
    0x00880080, 0x00FF0080, 0x00888880, 0x00FFFF80,
    0x88880080, 0xFFFF0080, 0x88888880, 0xFFFFFF80,
};
static int videocfg[6] = { 0 };
struct GsDBuffDc ps2video_display_cfg = { 0 };

sceDmaChan *dmachan_vif1 = 0;
volatile int nuvideo_global_vbcnt = 0;
int nuvideo_global_framerate = 60;
static TbSet *sys_tb = 0;
int tb_peak_reset = 0;
static volatile int framestarttime = 0;
static unsigned int frameendtime = 0;
static unsigned int frametime = 0;
int PS2_REZ_W = 0;
int PS2_REZ_H = 0;
int PS2_VCNTR_X = 0;
int PS2_VCNTR_Y = 0;
static int auto_screen_swap_enabled = 0;
int ps2video_novsync = 0;

extern int nuvideo_progressive_scan;
extern int ps2_video_fmt;
extern int ps2_zb_fmt;
extern unsigned int ps2_video_size;

static int odev;
static int frame;
static int threadid;
static int invbi;
static int nu_intc_vif1;
static int nu_intc_vblank_e;
static int nu_intc_vblank_s;
static int nu_dmac_vif1;
static int nu_dmac_gif;
static int displayx[2];
static int displayy[2];
static int elapsed_time;
static float gamma_table[256];


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

    elapsed = EE_T1_COUNT - framestarttime;
    elapsed_time = elapsed;
    if (elapsed < 0) {
        elapsed_time = elapsed + 0x10000;
    }
    return elapsed_time;
}


static inline int tbslotEnd(TbSet *set, int slot)
{
    unsigned int elapsed;

    set = set != 0 ? set : sys_tb;
    slot &= 0xF;
    elapsed = EE_T1_COUNT - set->begin[slot];
    if (elapsed > 0xFFFF) {
        elapsed += 0xFFFF;
    }
    set->values[slot][1 - set->current[slot]] += elapsed;
    return set->values[slot][1 - set->current[slot]];
}


static inline void tbslotFlip(TbSet *set, int slot)
{
    int current;

    set = set != 0 ? set : sys_tb;
    slot &= 0xF;
    current = set->current[slot] ^ 1;
    set->current[slot] = current;
    set->values[slot][current ? 0 : 1] = 0;
}


static inline TbSet *tbsetAlloc(int *colours)
{
    TbSet *set;
    int i;

    set = NuMemAllocFn(sizeof(TbSet), "..\\nu2crash.ps2\\nups2\\ps2video.c", 0x1CF);
    if (set) {
        memset(set, 0, sizeof(TbSet));
        if (colours) {
            for (i = 0; i < 16; i++) {
                set->colour[i] = colours[i];
            }
        } else {
            memcpy(set->colour, def_cols, sizeof(set->colour));
        }
    }
    return set;
}


void tbRender(TbSet *set)
{
    struct nuvtx2d_s vtx[4];
    float scale;
    float top;
    float screen_h;
    int reset;
    int value;
    int x;
    int y;
    int i;

    reset = 1;
    if (set == 0) {
        set = sys_tb;
        reset = 0;
    }
    if (system_font) {
        NuFntSet(system_font);
    }
    scale = 1.0f / PS2_REZ_H;
    top = 0.05f;
    for (i = 0; i < 16; i++) {
        if (set->values[i][set->current[i]] != 0) {
            if (set->peak[i] < set->values[i][set->current[i]]) {
                set->peak[i] = set->values[i][set->current[i]];
            }
            vtx[0].x = (i + 1) * 0.025f * 3.0f;
            vtx[0].y = top;
            vtx[0].z = 0.0f;
            vtx[0].rhw = 1.0f;
            vtx[0].colour = set->colour[i];
            vtx[1] = vtx[2] = vtx[3] = vtx[0];
            vtx[3].x = vtx[1].x = vtx[1].x + 0.025f;
            vtx[3].y = vtx[2].y = vtx[2].y + set->values[i][set->current[i]] * scale;
            NuRndrQuad2d(vtx, 0);
            screen_h = PS2_REZ_H;
            if (system_font) {
                x = (int)(PS2_REZ_W * 0.025f) * (i + 1) * 3;
                x += PS2_REZ_W * 0.025f * 0.5f;
                value = set->values[i][set->current[i]];
                if (value != 0) {
                    y = (int)(screen_h * 0.05f) * 16;
                    y += NuFntHeight * 4;
                    y -= NuFntHeight;
                    NuFntPrintEx(x * 16, y, 2, "%d", value % 10);
                    y -= NuFntHeight;
                    value /= 10;
                    NuFntPrintEx(x * 16, y, 2, "%d", value % 10);
                    y -= NuFntHeight;
                    value /= 10;
                    NuFntPrintEx(x * 16, y, 2, "%d", value % 10);
                }
                if (set->peak[i] != 0) {
                    x = (int)(PS2_REZ_W * 0.025f) * (i + 1) * 3;
                    x += PS2_REZ_W * 0.025f * 0.5f;
                    y = PS2_REZ_H * 16 - NuFntHeight * 2;
                    NuFntPrintEx(x * 16, y + NuFntBase, 2, "%d", set->peak[i]);
                }
                if (set->name[i] != 0) {
                    x = (int)(PS2_REZ_W * 0.025f) * (i + 1) * 3;
                    x += PS2_REZ_W * 0.025f * 0.5f;
                    y = PS2_REZ_H * 16 - NuFntHeight;
                    NuFntPrintEx(x * 16, y + NuFntBase, 2, set->name[i]);
                }
            }
        }
        if (reset) {
            tbslotFlip(set, i);
        }
        if (tb_peak_reset) {
            set->peak[i] = 0;
        }
    }
    tb_peak_reset = 0;
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
    threadid = GetThreadId();
    EE_T1_MODE = 0x83;
    sys_tb = tbsetAlloc(0);
}


int nusound_irx_loaded = 0;


void NuPs2InitIOP(void)
{
    static int initialised = 0;

    if (!initialised) {
        initialised = 1;

        if (NuFileSifLoadModule("SYS\\SIO2MAN.IRX", 0, 0) < 0
            && NuFileSifLoadModule("SYS\\SIO2MAN.IRX", 0, 0) < 0
            && NuFileSifLoadModule("/usr/local/sce/iop/modules/SIO2MAN.IRX", 0, 0) < 0
            && sceSifLoadModule("host0:/usr/local/sce/iop/modules/sio2man.irx", 0, 0) < 0) {
            printf("Can't load module sio2man\n");
            Exit(0);
        }
        printf("Loaded module sio2man\n");

        if (NuFileSifLoadModule("SYS\\PADMAN.IRX", 0, 0) < 0
            && NuFileSifLoadModule("/usr/local/sce/iop/modules/PADMAN.IRX", 0, 0) < 0
            && sceSifLoadModule("host0:/usr/local/sce/iop/modules/padman.irx", 0, 0) < 0) {
            printf("Can't load module padman\n");
            Exit(0);
        }
        printf("Loaded module padman\n");

        if (NuFileSifLoadModule("SYS\\MCMAN.IRX", 0, 0) < 0
            && NuFileSifLoadModule("/usr/local/sce/iop/modules/MCMAN.IRX", 0, 0) < 0
            && sceSifLoadModule("host0:/usr/local/sce/iop/modules/mcman.irx", 0, 0) < 0) {
            printf("Can't load module mcman\n");
            Exit(0);
        }
        printf("Loaded module mcman\n");

        if (NuFileSifLoadModule("SYS\\MCSERV.IRX", 0, 0) < 0
            && NuFileSifLoadModule("/usr/local/sce/iop/modules/mcserv.irx", 0, 0) < 0
            && sceSifLoadModule("host0:/usr/local/sce/iop/modules/mcserv.irx", 0, 0) < 0) {
            printf("Can't load module mcserv\n");
            Exit(0);
        }
        printf("Loaded module mcserv\n");

        if (NuFileSifLoadModule("SYS\\LIBSD.IRX", 0, 0) < 0
            && NuFileSifLoadModule("/usr/local/sce/iop/modules/libsd.irx", 0, 0) < 0
            && sceSifLoadModule("host0:/usr/local/sce/iop/modules/libsd.irx", 0, 0) < 0) {
            printf("Can't load module libsd\n");
            Exit(0);
        }
        printf("Loaded module libsd\n");

        nusound_irx_loaded = 1;
        if (NuFileSifLoadModule("SYS\\NUSOUND.IRX", 0, 0) < 0
            && NuFileSifLoadModule("/usr/local/sce/iop/modules/nusound.irx", 0, 0) < 0
            && sceSifLoadModule("host0:/usr/local/sce/iop/modules/nusound.irx", 0, 0) < 0) {
            printf("Can't load module nusound - sound services will be denied\n");
            nusound_irx_loaded = 0;
        }
        printf("Loaded module nusound\n");

        if (NuFileSifLoadModule("SYS\\SDRDRV.IRX", 0, 0) < 0
            && NuFileSifLoadModule("/usr/local/sce/iop/modules/sdrdrv.irx", 0, 0) < 0
            && sceSifLoadModule("host0:/usr/local/sce/iop/modules/sdrdrv.irx", 0, 0) < 0) {
            printf("Can't load module sdrdrv\n");
            Exit(0);
        }
        printf("Loaded module sdrdrv\n");

        printf("sceMcInit result %d\n", sceMcInit());
        scePadInit(0);
        sceSifInitIopHeap();
        sceSdRemoteInit();
    }
}


static int vbe_set = 0;
static struct NuVBlankHandler *vbe_chain = 0;
static int vbe_dis_cnt = 0;
static int vbs_set = 0;
static struct NuVBlankHandler *vbs_chain = 0;
static int vbs_dis_cnt = 0;
static int all_handlers_initialised = 0;
int hold_last_chain = 0;
static int channel1_waiting = 0;
static int channel2_waiting = 0;
static int ch2_ends = 0;
static int ch1_waits = 0;
static int gs_dma_countdownval = 4;
static int gs_dma_countdown = 4;


static inline void ps2videoAddAllHandlers(void)
{
    if (!all_handlers_initialised) {
        nu_intc_vif1 = AddIntcHandler(5, ifnVif1, 0);
        EnableIntc(5);
        vbe_chain = 0;
        nu_intc_vblank_e = AddIntcHandler(3, ifnVBlankE, -1);
        EnableIntc(3);
        vbe_set = 1;
        vbs_chain = 0;
        sceGsSyncVCallback(ifnVBlankS);
        nu_intc_vblank_s = 0;
        EnableIntc(2);
        vbs_set = 1;
        nu_dmac_vif1 = AddDmacHandler(1, idmacVif1, 0);
        EnableDmac(1);
        nu_dmac_gif = AddDmacHandler(2, idmacGs, 0);
        EnableDmac(2);
        all_handlers_initialised = 1;
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
            ch2_ends++;
            if (channel1_waiting) {
                D_ENABLEW = 0x10000;
                D2_CHCR |= 0x100;
                D1_CHCR |= 0x100;
                D_ENABLEW = 0;
                channel1_waiting = 0;
            } else {
                if (nutex_use_path3) {
                    addr = *(unsigned int *)addr | 0x20000000;
                    ((unsigned char *)addr)[3] &= 0x7F;
                }
                if (gs_dma_countdown) {
                    if (gs_dma_status) {
                        D_ENABLEW = 0x10000;
                        D2_CHCR |= 0x100;
                        D_ENABLEW = 0;
                    }
                    gs_dma_countdown--;
                } else {
                    channel2_waiting = 1;
                    gs_dma_countdown = gs_dma_countdownval;
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
            ch1_waits++;
            if (packet[-0xD] & 0x80) {
                channel1_waiting = channel;
            } else {
                D_ENABLEW = 0x10000;
                D1_CHCR |= 0x100;
                D_ENABLEW = 0;
            }
            if (channel2_waiting) {
                D_ENABLEW = 0x10000;
                D2_CHCR |= 0x100;
                D_ENABLEW = 0;
                channel2_waiting = 0;
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
    sceGsSetDefDBuffDc(ps2video_display_cfg.disp, psm, width, height, 3, zpsm, 0);
    odev = !sceGsSyncV(0);
    frame = 0;
    EE_T0_MODE = 0x9F;
    ps2videoAddAllHandlers();
    switch (PS2_REZ_W) {
    case 256:
        videocfg[2] = 9;
        break;
    case 320:
        videocfg[2] = 7;
        break;
    case 384:
        videocfg[2] = 6;
        break;
    case 512:
        videocfg[2] = 4;
        break;
    case 640:
        videocfg[2] = 3;
        break;
    }
    displayx[0] = (int)ps2video_display_cfg.disp[0].display_reg & 0xFFF;
    displayy[0] = (int)(ps2video_display_cfg.disp[0].display_reg >> 12) & 0x7FF;
    displayx[1] = (int)ps2video_display_cfg.disp[1].display_reg & 0xFFF;
    displayy[1] = (int)(ps2video_display_cfg.disp[1].display_reg >> 12) & 0x7FF;
    NuPs2FlushVRam(0, 0, 0);
    GIF_MODE = 2;
}


static inline void ps2videoDumpRow(int fh, u8 *line, u128 *pixels, int y)
{
    u8 *src;
    int x;

    src = (u8 *)(pixels + y * PS2_REZ_W / 4);
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
    u128 *pixels[2];
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
    pixels[0] = NuMemAllocFn(PS2_REZ_W * PS2_REZ_H * 4, "..\\nu2crash.ps2\\nups2\\ps2video.c", 0x861);
    pixels[1] = NuMemAllocFn(PS2_REZ_W * PS2_REZ_H * 4, "..\\nu2crash.ps2\\nups2\\ps2video.c", 0x862);
    while (VU1_DmaGetStatus()) {
    }
    sceGsSyncPath(0, 0);
    buffer = ~(frame ^ odev) & 1;
    sceGsSetDefStoreImage(&image, buffer ? 0 : PS2_REZ_W * PS2_REZ_H / 64, PS2_REZ_W / 64, 0,
                          0, 0, PS2_REZ_W, PS2_REZ_H);
    FlushCache(0);
    sceGsExecStoreImage(&image, pixels[0]);
    sceGsSyncPath(0, 0);
    if (lace) {
        sceGsSetDefStoreImage(&image, buffer ? PS2_REZ_W * PS2_REZ_H / 64 : 0, PS2_REZ_W / 64, 0,
                              0, 0, PS2_REZ_W, PS2_REZ_H);
        FlushCache(0);
        sceGsExecStoreImage(&image, pixels[1]);
        sceGsSyncPath(0, 0);
    }
    y = PS2_REZ_H;
    do {
        y--;
        if (lace) {
            ps2videoDumpRow(fh, line, pixels[1], y);
        }
        ps2videoDumpRow(fh, line, pixels[0], y);
    } while (y != 0);
    NuMemFreeFn(pixels[0], "..\\nu2crash.ps2\\nups2\\ps2video.c", 0x891);
    NuMemFreeFn(pixels[1], "..\\nu2crash.ps2\\nups2\\ps2video.c", 0x892);
    NuFileClose(fh);
}


int frame_start_wait = 0;
int crta = 1;
int crtb = 2;
int crtc = 1;
int ps2video_ignore_frameout = 0;
int nuvideo_progressive_scan = 0;
int ps2_video_fmt = 0;
int ps2_zb_fmt = 0;
unsigned int ps2_video_size = 0;


int NuPs2ScreenSwap(int vsync)
{
    int half_offset;

    if (!invbi) {
        while (VU1_DmaGetStatus()) {
        }
        tbslotEnd(0, 0);
        sceGsSyncPath(0, 0);
    }

    frameendtime = EE_T1_COUNT;
    frametime = frameendtime - framestarttime;
    if (frametime > 0xFFFF) {
        frametime += 0xFFFF;
    }
    if (ps2video_ignore_frameout || invbi) {
        frametime = 50;
    }

    if (vsync) {
        frame++;
    } else {
        NuDebugMsgProlog("..\\nu2crash.ps2\\nups2\\ps2video.c", 0x921)("swap not");
    }

    if (!invbi) {
        if (ps2video_novsync) {
            odev = 0;
        } else {
            odev = sceGsSyncV(0);
        }
    } else {
        odev = 1 - odev;
    }

    if (frametime <= ps2_video_size && !nuvideo_progressive_scan) {
        sceGsSetHalfOffset((frame & 1) ? &ps2video_display_cfg.draw11.frame : &ps2video_display_cfg.draw01.frame,
                           PS2_VCNTR_X, PS2_VCNTR_Y, odev);
        sceGsSetHalfOffset2((frame & 1) ? &ps2video_display_cfg.draw02.frame : &ps2video_display_cfg.draw12.frame,
                            PS2_VCNTR_X, PS2_VCNTR_Y, odev);
        half_offset = 1;
        nurndr_half_offset = odev * 8;
    } else {
        sceGsSetHalfOffset((frame & 1) ? &ps2video_display_cfg.draw11.frame : &ps2video_display_cfg.draw01.frame,
                           PS2_VCNTR_X, PS2_VCNTR_Y, 0);
        sceGsSetHalfOffset2((frame & 1) ? &ps2video_display_cfg.draw02.frame : &ps2video_display_cfg.draw12.frame,
                            PS2_VCNTR_X, PS2_VCNTR_Y, 0);
        half_offset = 0;
        nurndr_half_offset = 0;
    }

    if (!invbi) {
        FlushCache(0);
    } else {
        iFlushCache(0);
    }

    if (vsync) {
        sceGsSwapDBuffDc(ps2video_display_cfg.disp, frame & 1);
    } else {
        half_offset = 1;
    }

    framestarttime = EE_T1_COUNT;
    if (!invbi) {
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


struct TbSet *tbsetCreate(int *colours)
{
    return tbsetAlloc(colours);
}


void tbsetDestroy(TbSet *set)
{
    NuMemFreeFn(set, "..\\nu2crash.ps2\\nups2\\ps2video.c", 0x1E7);
}


void tbslotReset(TbSet *set, int slot)
{
    int current;

    set = set != 0 ? set : sys_tb;
    slot &= 0xF;
    current = set->current[slot] ^ 1;
    set->current[slot] = current;
    set->values[slot][current ? 0 : 1] = 0;
}


int tbslotLastValue(struct TbSet *set, int slot)
{
    set = set != 0 ? set : sys_tb;
    slot &= 0xF;
    return set->values[slot][set->current[slot]];
}


void tbslotBeginFn(TbSet *set, int slot)
{
    set = set != 0 ? set : sys_tb;
    slot &= 0xF;
    set->begin[slot] = EE_T1_COUNT;
    set->name[slot] = 0;
}


void tbslotBeginExFn(struct TbSet *set, int slot, char *name)
{
    set = set != 0 ? set : sys_tb;
    slot &= 0xF;
    set->begin[slot] = EE_T1_COUNT;
    set->name[slot] = name;
}


int tbslotEndFn(TbSet *set, int slot)
{
    unsigned int elapsed;
    int over;
    unsigned int lim = 0xFFFF;

    set = set != 0 ? set : sys_tb;
    slot &= 0xF;
    /* The narrow index preserves the retail compiler's address-copy sequence. */
    elapsed = EE_T1_COUNT - set->begin[(unsigned char)slot];
    over = elapsed > lim;
    elapsed += over * lim;
    set->values[slot][1 - set->current[slot]] += elapsed;
    return set->values[slot][1 - set->current[slot]];
}


void tbInit(void)
{
    EE_T1_MODE = 0x83;
    sys_tb = tbsetAlloc(0);
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
    ps2video_display_cfg.disp[0].display.dx = displayx[0] + x;
    ps2video_display_cfg.disp[0].display.dy = displayy[0] + y;
    ps2video_display_cfg.disp[1].display.dx = displayx[1] + x;
    ps2video_display_cfg.disp[1].display.dy = displayy[1] + y;
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
        dest = NuMemAllocFn(w * h * 4, "..\\nu2crash.ps2\\nups2\\ps2video.c", 0x8A8);
    }
    while (VU1_DmaGetStatus()) {
    }
    sceGsSyncPath(0, 0);
    buffer = (frame ^ odev) & 1;
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
    if (!auto_screen_swap_enabled) {
        while (VU1_DmaGetStatus()) {
        }
        sceGsSyncPath(0, 0);
        while (abs(GetElapsedTime()) < 50) {
        }
        auto_screen_swap_enabled = 1;
    }
}


void NuPs2DisableAutoScreenSwap(void)
{
    if (auto_screen_swap_enabled) {
        while (abs(GetElapsedTime()) >= 100) {
        }
        auto_screen_swap_enabled = 0;
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
    if (vbe_set) {
        if (--vbe_dis_cnt == 0) {
            EnableIntc(3);
        }
    }
}


void NuDisableVBlankE(void)
{
    if (vbe_set) {
        if (vbe_dis_cnt == 0) {
            DisableIntc(3);
        }
        vbe_dis_cnt++;
    }
}


void NuIntAddVBlankE(struct NuVBlankHandler *handler)
{
    struct NuVBlankHandler *prev;
    struct NuVBlankHandler *cur;

    DisableIntc(3);
    prev = 0;
    cur = vbe_chain;
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
        if (vbe_chain) {
            vbe_chain->prev = handler;
        }
        handler->prev = 0;
        handler->next = vbe_chain;
        vbe_chain = handler;
    }
    EnableIntc(3);
}


void NuIntRemVBlankE(struct NuVBlankHandler *handler)
{
    DisableIntc(3);
    if (handler->prev) {
        handler->prev->next = handler->next;
    } else {
        vbe_chain = handler->next;
    }
    if (handler->next) {
        handler->next->prev = handler->prev;
    }
    EnableIntc(3);
}


void NuEnableVBlankS(void)
{
    if (vbs_set) {
        if (--vbs_dis_cnt == 0) {
            EnableIntc(2);
        }
    }
}


void NuDisableVBlankS(void)
{
    if (vbs_set) {
        if (vbs_dis_cnt == 0) {
            DisableIntc(2);
        }
        vbs_dis_cnt++;
    }
}


void NuIntAddVBlankS(struct NuVBlankHandler *handler)
{
    struct NuVBlankHandler *prev;
    struct NuVBlankHandler *cur;

    DisableIntc(2);
    prev = 0;
    cur = vbs_chain;
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
        if (vbs_chain) {
            vbs_chain->prev = handler;
        }
        handler->prev = 0;
        handler->next = vbs_chain;
        vbs_chain = handler;
    }
    EnableIntc(2);
}


void NuIntRemVBlankS(struct NuVBlankHandler *handler)
{
    DisableIntc(2);
    if (handler->prev) {
        handler->prev->next = handler->next;
    } else {
        vbs_chain = handler->next;
    }
    if (handler->next) {
        handler->next->prev = handler->prev;
    }
    EnableIntc(2);
}


void NuEnableVBlank(void)
{
    if (vbs_set) {
        if (--vbs_dis_cnt == 0) {
            EnableIntc(2);
        }
    }
    if (vbe_set) {
        if (--vbe_dis_cnt == 0) {
            EnableIntc(3);
        }
    }
}


void NuDisableVBlank(void)
{
    if (vbe_set) {
        if (vbe_dis_cnt == 0) {
            DisableIntc(3);
        }
        vbe_dis_cnt++;
    }
    if (vbs_set) {
        if (vbs_dis_cnt == 0) {
            DisableIntc(2);
        }
        vbs_dis_cnt++;
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
    if (frame & 1) {
        memcpy(buf, &ps2video_display_cfg.draw01.frame, sizeof(struct NuPs2GsReg));
    } else {
        memcpy(buf, &ps2video_display_cfg.draw11.frame, sizeof(struct NuPs2GsReg));
    }
    return (char *)buf + sizeof(struct NuPs2GsReg);
}


void *NuPs2CopyCurrentFrontBuffer(void *buf)
{
    if (frame & 1) {
        memcpy(buf, &ps2video_display_cfg.draw11.frame, sizeof(struct NuPs2GsReg));
    } else {
        memcpy(buf, &ps2video_display_cfg.draw01.frame, sizeof(struct NuPs2GsReg));
    }
    return (char *)buf + sizeof(struct NuPs2GsReg);
}


void *NuPs2CopyCurrentZBuffer(void *buf)
{
    if (frame & 1) {
        memcpy(buf, &ps2video_display_cfg.draw01.zbuf, sizeof(struct NuPs2GsReg));
    } else {
        memcpy(buf, &ps2video_display_cfg.draw11.zbuf, sizeof(struct NuPs2GsReg));
    }
    return (char *)buf + sizeof(struct NuPs2GsReg);
}


void NuPs2GetCurrentBackBuffer(u64 *buf)
{
    if (frame & 1) {
        *buf = ps2video_display_cfg.draw01.frame.data;
    } else {
        *buf = ps2video_display_cfg.draw11.frame.data;
    }
}


void NuPs2GetCurrentFrontBuffer(u64 *buf)
{
    if (frame & 1) {
        *buf = ps2video_display_cfg.draw11.frame.data;
    } else {
        *buf = ps2video_display_cfg.draw01.frame.data;
    }
}


void NuPs2GetCurrentZBuffer(u64 *buf)
{
    if (frame & 1) {
        *buf = ps2video_display_cfg.draw01.zbuf.data;
    } else {
        *buf = ps2video_display_cfg.draw11.zbuf.data;
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
    if (all_handlers_initialised) {
        vbe_set = 0;
        vbs_set = 0;
        DisableDmac(2);
        RemoveDmacHandler(2, nu_dmac_gif);
        DisableDmac(1);
        RemoveDmacHandler(1, nu_dmac_vif1);
        sceGsSyncVCallback(0);
        DisableIntc(3);
        RemoveIntcHandler(3, nu_intc_vblank_e);
        DisableIntc(5);
        RemoveIntcHandler(5, nu_intc_vif1);
        vbe_chain = 0;
        vbs_chain = 0;
        all_handlers_initialised = 0;
    }
}


int ifnVif1(int ca)
{
    ExitHandler();
    return 0;
}


int ifnVBlankE(int ca)
{
    struct NuVBlankHandler *handler;

    invbi = 1;
    if (auto_screen_swap_enabled) {
        NuPs2ScreenSwap(1);
    }
    for (handler = vbe_chain; handler; handler = handler->next) {
        handler->fn();
    }
    invbi = 0;
    ExitHandler();
    return 1;
}


int ifnVBlankS(int ca)
{
    struct NuVBlankHandler *handler;

    invbi = 1;
    nuvideo_global_vbcnt++;
    for (handler = vbs_chain; handler; handler = handler->next) {
        handler->fn();
    }
    invbi = 0;
    ExitHandler();
    return 1;
}
