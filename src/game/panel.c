/*
 * Unit: game/panel
 *
 * Functions:
 *   0x00239d50 UpdatePanelItem
 *   0x00239ec0 DrawPanel3DCharacter
 *   0x0023a2b8 Draw3DCheckpointLetters
 *   0x0023a9b0 DrawGameMessage
 *   0x0023acc0 DrawTimeTrialTimes
 *   0x0023b078 BigOutOf
 *   0x0023b258 DrawWorldToPanelWumpa
 *   0x0023b450 DrawPanel
 *   0x0023f7a0 UpdatePlayerStats
 *   0x0023fa60 AddPanelDebris
 *   0x0023fd38 UpdatePanelDebris
 *   0x00240150 DrawPanelDebris
 *   0x00240400 DrawPanel3DObject
 *   0x00240648 DrawPanel3DTempCharacter
 *   0x00240810 MaxVP
 *   0x00240858 GameVP
 *   0x00240958 ResetPanelDebris
 *   0x00240980 NextLetter
 *
 * DrawPanel3DObject reconstructed from the PS2 disassembly: null/scale guard,
 * object-id dispatch (1->0x85, 2->0x86, 3->0x87) that recurses on the resolved
 * ObjTab entry (then falls through to render), scale/switch(rot)/render.
 * WORK IN PROGRESS attempt #2.
 */

#include "creature.h"

struct nuinstance_s {
    struct numtx_s matrix;
    s32 objid;
    /* remaining fields unused here */
};

struct nugscn_s {
    short *tids;
    s32 numtid;
    void *mtls;
    s32 nummtl;
    s32 numgobj;
    void **gobjs;
    /* remaining fields unused here */
};

struct nuspecial_s {
    struct numtx_s mtx;
    struct nuinstance_s *instance;
    char *name;
    /* remaining fields unused here */
};

struct objtab_s {
    struct nugscn_s *scene;
    struct nuspecial_s *special;
    u8 _pad[24];
};

extern struct objtab_s ObjTab[201];
extern float PANEL3DMULX, PANEL3DMULY;

extern void NuMtxSetScale(struct numtx_s *, struct nuvec_s *);
extern void NuMtxRotateX(struct numtx_s *, u16);
extern void NuMtxRotateY(struct numtx_s *, u16);
extern void NuMtxRotateZ(struct numtx_s *, u16);
extern s32 NuRndrGScnObj(void *, struct numtx_s *);

/* --- UpdatePlayerStats externs ------------------------------------- */

/* Panel counter (count +0x0, draw +0x2, frame +0x4). */
struct panelcount_s {
    short count;
    short draw;
    signed char frame;
    u8 pad[3];
};

/* A crate cube (stride 0x90); only the fields UpdatePlayerStats reads are
 * typed. */
typedef struct {
    u8 _pad00[0x30];
    s8 on;             /* 0x30 */
    u8 _pad31[0x07];
    u16 flags;         /* 0x38 */
    u8 _pad3a[0x04];
    s8 newtype;        /* 0x3E */
    u8 _pad3f[0x02];
    s8 metal_count;    /* 0x41 */
    u8 _pad42[0x4E];
} CrateCube;           /* 0x90 */

extern CrateCube Crate[];
extern s32 CRATECOUNT;
extern s32 crates_destroyed;
extern s32 bonus_crates_destroyed;
extern s32 old_bonus_crates;
extern s32 save_bonus_crates_destroyed;
extern s32 Bonus;
extern s32 bonus_finish_frame;
extern f32 bonus_crates_wait;
extern f32 D_0062E640;              /* 1/50 wait decrement (.sdata) */
extern f32 D_0062E644;              /* -1/50 wait floor (.sdata) */
extern u16 crate_wumpa;
extern struct panelcount_s plr_crates;
extern struct panelcount_s plr_wumpas;
extern struct panelcount_s plr_lives;
extern s32 force_panel_lives_update;
extern s32 mask_crates;
extern s32 newmask_advise;
extern f32 WUMPAOBJSX;
extern f32 PANELSY;
extern struct mask_s vNEWMASK;
extern u8 Cursor[];

extern void AddPanelDebris(f32 x, f32 y, s32 obj, f32 z, s32 flag);
extern void NewMask(struct mask_s *mask, struct mask_s *src);
extern void NewMenu(void *cursor, s32 a, s32 b, s32 c);

void UpdatePlayerStats(struct creature_s *plr) {
    CrateCube *crate;
    s32 i;
    s32 dead;
    s32 x;

    crates_destroyed = 0;
    bonus_crates_destroyed = 0;
    crate = Crate;
    for (i = 0; i < CRATECOUNT; i++, crate++) {
        if (((crate->flags & 0x10) != 0) &&
            ((crate->on == 0) ||
             ((crate->newtype == 0xF) && (crate->metal_count != 0)))) {
            if ((crate->flags & 0x40) != 0) {
                bonus_crates_destroyed++;
            } else {
                crates_destroyed++;
            }
        }
    }

    if ((Bonus == 2) && (plr->obj.dead != 0)) {
        dead = 1;
    } else {
        dead = 0;
    }

    if (dead) {
        bonus_crates_destroyed = old_bonus_crates;
    } else {
        old_bonus_crates = bonus_crates_destroyed;
    }

    save_bonus_crates_destroyed = bonus_crates_destroyed;

    if ((Bonus == 4) || (Bonus == 3) || dead) {
        if (bonus_finish_frame < bonus_crates_destroyed * 5) {
            x = bonus_finish_frame / 5;
            if (!dead) {
                crates_destroyed += x;
            }
            bonus_crates_destroyed -= x;
        } else {
            if (!dead) {
                crates_destroyed += bonus_crates_destroyed;
            }
            bonus_crates_destroyed = 0;
        }
        if (bonus_finish_frame == save_bonus_crates_destroyed * 5 + 4) {
            bonus_crates_wait = 0.5f;
        } else if ((bonus_finish_frame >= save_bonus_crates_destroyed * 5 + 5) &&
                   (bonus_crates_wait -= D_0062E640,
                    bonus_crates_wait <= 0.0f)) {
            bonus_crates_wait = D_0062E644;
        }
    }

    plr_crates.count = crates_destroyed;
    plr_wumpas.count += crate_wumpa;
    while (99 < plr_wumpas.count) {
        plr_wumpas.count += -100;
        AddPanelDebris(WUMPAOBJSX, PANELSY, 5, 0.0f, 1);
    }

    if (99 < plr_lives.count) {
        plr_lives.count = 99;
        force_panel_lives_update = 0x32;
    }

    for (; mask_crates != 0; mask_crates--) {
        if (plr->obj.mask != 0) {
            NewMask(plr->obj.mask, &vNEWMASK);
            if (newmask_advise != 0) {
                if (plr->obj.mask->active < 3) {
                    NewMenu(Cursor, 0x21, -1, -1);
                    ResetAnimPacket(&plr->obj.mask->anim, 0x22);
                    player->obj.mom.x = 0;
                    player->obj.mom.z = 0;
                    player->slide = 0;
                }
                newmask_advise = 0;
            }
        }
    }
}

s32 DrawPanel3DObject(s32 object, float x, float y, float z, float scalex,
                      float scaley, float scalez, u16 xrot, u16 yrot, u16 zrot,
                      struct nugscn_s *scn, struct nuspecial_s *obj, s32 rot) {
    struct numtx_s m;
    struct nuvec_s s;
    struct objtab_s *e;
    s32 o;

    if (((scn != 0) && (obj != 0)) &&
        ((scalex != 0.0f) || ((scaley != 0.0f) || (scalez != 0.0f)))) {
        if (object == 1) {
            o = 0x85;
        } else if (object == 2) {
            o = 0x86;
        } else {
            o = (object == 3) ? 0x87 : -1;
        }
        if (o != -1) {
            e = &ObjTab[o];
            DrawPanel3DObject(o, x, y, z, scalex, scaley, scalez, 0, 0, 0,
                              e->scene, e->special, 0);
        }
        s.x = scalex;
        s.y = scaley;
        s.z = scalez;
        NuMtxSetScale(&m, &s);
        switch (rot) {
            case 0:
                if (xrot != 0) {
                    NuMtxRotateX(&m, xrot);
                }
                if (yrot != 0) {
                    NuMtxRotateY(&m, yrot);
                }
                if (zrot != 0) {
                    NuMtxRotateZ(&m, zrot);
                }
                break;
            case 1:
                if (yrot != 0) {
                    NuMtxRotateY(&m, yrot);
                }
                if (xrot != 0) {
                    NuMtxRotateX(&m, xrot);
                }
                if (zrot != 0) {
                    NuMtxRotateZ(&m, zrot);
                }
                break;
        }
        m._32 = z;
        m._30 = x * PANEL3DMULX;
        m._31 = y * PANEL3DMULY;
        return NuRndrGScnObj(scn->gobjs[obj->instance->objid], &m);
    }
    return 0;
}
