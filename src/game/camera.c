/*
 * Unit: game/camera
 *
 * Functions:
 *   0x001d5f80 InitRails
 *   0x001d6228 BestRailPosition
 *   0x001d6850 ComplexRailPosition
 *   0x001d6b08 MoveRailPosition
 *   0x001d6e08 RailInfo
 *   0x001d73b8 MoveGameCamera
 *   0x001daaf8 DrawCameraTarget
 *   0x001dac00 GetALONG
 *   0x001dac58 FurtherALONG
 *   0x001dace0 FurtherBEHIND
 *   0x001dad68 ResetGameCameras
 *   0x001dada0 JudderGameCamera
 *   0x001dae28 BlendGameCamera
 *   0x001dae68 InitCameraTargetMaterial
 *   0x001daf10 DrawMtlLine3D
 *   0x001daf58 LookUpDownRail
 *   0x001dafd0 InSplineArea
 *   0x001db078 AddCross
 */

#include "creature.h"

/* cammtx_s (game camera): offsets verified in ResetGameCameras/BlendGameCamera. */
struct cammtx_s {
    u8 unk_0x00[0xB0];
    struct nuvec_s oldpos;        /* 0xB0 */
    struct nuvec_s newpos;        /* 0xBC */
    u8 unk_0xC8[0x18];
    f32 distance;                 /* 0xE0 */
    f32 ahead;                    /* 0xE4 */
    f32 judder;                   /* 0xE8 */
    f32 blend_time;               /* 0xEC */
    f32 blend_duration;           /* 0xF0 */
    u8 unk_0xF4[0xC];
    u16 old_xrot;                 /* 0x100 */
    u16 new_xrot;                 /* 0x102 */
    u16 old_yrot;                 /* 0x104 */
    u16 new_yrot;                 /* 0x106 */
    u16 old_zrot;                 /* 0x108 */
    u16 new_zrot;                 /* 0x10A */
    u8 unk_0x10C[0x8];
    s8 mode;                      /* 0x114 */
    u8 unk_0x115[0x5];
    s8 vertical;                  /* 0x11A */
    u8 unk_0x11B[0x1];
};                                /* 0x11C */

/* Rail[i].type @0x26, stride 0x28 (mirrors src/game/ai.c and bug.c). */
struct rail_s {
    u8 unk_0x00[0x26];
    s8 type;
    u8 unk_0x27;
};

extern struct rail_s Rail[];
extern struct RPos_s TempRPos;
extern s32 nRAILS;
extern s32 temp_iRAIL;
extern s32 temp_iALONG;

extern void ComplexRailPosition(struct nuvec_s *pos, s32 iRAIL, s32 iALONG,
                                struct RPos_s *rpos, s32 a4);


void ResetGameCameras(struct cammtx_s *Gamecam, s32 n) {
    while (n > 0) {
        Gamecam->mode = -1;
        Gamecam->judder = 0.0f;
        Gamecam->blend_time = 0.0f;
        Gamecam->blend_duration = 0.0f;
        Gamecam->distance = 0.0f;
        Gamecam->ahead = 0.0f;
        Gamecam->vertical = '\0';
        Gamecam++;
        n--;
    }
}

void BlendGameCamera(struct cammtx_s *cam, f32 time) {
    cam->oldpos = cam->newpos;
    cam->old_xrot = cam->new_xrot;
    cam->old_yrot = cam->new_yrot;
    cam->old_zrot = cam->new_zrot;
    cam->blend_time = 0.0f;
    cam->blend_duration = time;
}

void GetALONG(struct nuvec_s *pos, struct RPos_s *rpos, s32 iRAIL, s32 iALONG,
              s32 info) {
    if (rpos == 0) {
        rpos = &TempRPos;
    }
    if (nRAILS != 0) {
        ComplexRailPosition(pos, iRAIL, iALONG, rpos, 0);
    } else {
        temp_iALONG = -1;
        temp_iRAIL = -1;
    }
}

s32 FurtherALONG(s32 iRAIL0, s32 iALONG0, f32 fALONG0, s32 iRAIL1, s32 iALONG1,
                 f32 fALONG1) {
    if (iRAIL0 == -1) {
        return 0;
    }
    if (iRAIL1 == -1) {
        return 0;
    }
    if ((Rail + iRAIL0)->type != (Rail + iRAIL1)->type) {
        return 0;
    }
    if (iRAIL0 < iRAIL1) {
        return 0;
    }
    if (iRAIL0 > iRAIL1) {
        return 1;
    }
    if (iALONG0 < iALONG1) {
        return 0;
    }
    if (iALONG0 > iALONG1) {
        return 1;
    }
    if (fALONG0 > fALONG1) {
        return 1;
    }
    return 0;
}

s32 FurtherBEHIND(s32 iRAIL0, s32 iALONG0, f32 fALONG0, s32 iRAIL1, s32 iALONG1,
                  f32 fALONG1) {
    if (iRAIL0 == -1) {
        return 0;
    }
    if (iRAIL1 == -1) {
        return 0;
    }
    if ((Rail + iRAIL0)->type != (Rail + iRAIL1)->type) {
        return 0;
    }
    if (iRAIL0 > iRAIL1) {
        return 0;
    }
    if (iRAIL0 < iRAIL1) {
        return 1;
    }
    if (iALONG0 > iALONG1) {
        return 0;
    }
    if (iALONG0 < iALONG1) {
        return 1;
    }
    if (fALONG0 < fALONG1) {
        return 1;
    }
    return 0;
}
