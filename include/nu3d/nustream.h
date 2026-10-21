#ifndef NU3D_NUSTREAM_H
#define NU3D_NUSTREAM_H

#include "nu_types.h"
#include "nucore/nucoretypes.h"
#include "numath/numathtypes.h"

struct numtl_s;

/* A render packet stream: DMA packets are written at ptr. */
struct rndrstream_s {
    union variptr_u base;           /* 0x00 */
    union variptr_u ptr;            /* 0x04 */
    union variptr_u t_base;         /* 0x08 texture DMA chain */
    union variptr_u t_ptr;          /* 0x0C */
    unsigned int gs_ptr;            /* 0x10 GS address */
    struct numtl_s *mtl;            /* 0x14 */
    unsigned char *mpg;             /* 0x18 */
    unsigned int drawpri;           /* 0x1C */
    s32 local_mtx_id;               /* 0x20 */
    short view_mtx_id;              /* 0x24 */
    short scissor_id;               /* 0x26 */
    short xyoffset_id;              /* 0x28 */
    short fx_mtx_id;                /* 0x2A */
    union variptr_u misc_addr;      /* 0x2C */
    unsigned int attr;              /* 0x30 */
    s32 tid;                        /* 0x34 */
}; /* 0x38 */

struct nucolour4_s;

void NuRndrInitStreams(void);
void NuRndrStreamInit(int size);
struct rndrstream_s *NuRndrStreamAllocStream(unsigned int sortkey, void *mpgtag, unsigned int flags);
void NuRndrStreamFlush(void);
void NuRndrStreamSetMisc(struct rndrstream_s *stream);
int NuRndrStreamSetLocalMtx(struct rndrstream_s *stream);
int NuRndrSetViewMtx(struct numtx_s *vm, struct numtx_s *pm, struct numtx_s *clip_pm);
int NuRndrSetSpecularLight(struct nuvec_s *dir, struct nucolour4_s *col);
void *NuStreamGetOutputBuffer(int index);
void NuRndrSwapStreamBuffers(void);
void FaceYDirStream(int angle);
int NuRndrSetLocalMtx(struct numtx_s *mtx, int clip);
void NuRndrSetXYOffset(int x, int y);
void NuRndrSetScissor(int x0, int y0, int x1, int y1);
int NuRndrSetZFix(int zfix);
int NuRndrSetDirectionalLights(struct nuvec_s *d0, struct nucolour3_s *c0, struct nuvec_s *d1, struct nucolour3_s *c1, struct nuvec_s *d2, struct nucolour3_s *c2);
int NuRndrSetDirectionalLights4(struct nuvec_s *d0, struct nucolour4_s *c0, struct nuvec_s *d1, struct nucolour4_s *c1, struct nuvec_s *d2, struct nucolour4_s *c2);
int NuRndrSetAmbientLight(struct nucolour3_s *colour);
int NuRndrSetGlobalAlpha(float alpha);
int NuRndrSetFxMtx(struct numtx_s *mtx);
u64 *NuRndrStreamGetScissor(void);
u64 *NuRndrStreamGetOffset(void);
void NuRndrStreamResetStream(struct rndrstream_s *stream, int flag);
void NuRndrStreamResetStreams(int flag);
void NuRndrVu1Draw(void);
void NuRndrStreamFxMPG(int mode);
void *NuRndrStreamAlloc(int qwords);
int NuRndrStreamAddMtl(struct rndrstream_s *stream, struct numtl_s *mtl);
u64 NuRndrStreamAddTex(struct rndrstream_s *stream, int tid, signed char mode);
int NuRndrStreamSetDirectionalLights(struct rndrstream_s *stream, struct nuvec_s *d0, struct nucolour3_s *c0, struct nuvec_s *d1, struct nucolour3_s *c1, struct nuvec_s *d2, struct nucolour3_s *c2);
int NuRndrStreamSetAmbientLight(struct rndrstream_s *stream, struct nucolour3_s *colour);
int NuRndrStreamSetViewMtx(struct rndrstream_s *stream);
int NuRndrStreamSetViewMtxScissorClip(struct rndrstream_s *stream);
int NuRndrStreamSetLocalMtxEx(struct rndrstream_s *stream, int clip);
void *NuRndrStreamPrependMPG(struct rndrstream_s *stream, void *tag);
void NuRndrStreamClearZBState(void);
void NuRndrStreamSetZBState(unsigned int zbuf, unsigned int psm, int zmask);
void *NuRndrStreamAddZBState(void *stream, int index);
void CheckRndrStream(void);

#endif /* NU3D_NUSTREAM_H */
