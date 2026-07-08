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

struct panel_debris_s {
    u8 _pad[0x3D];
    u8 active;
    u8 _pad2[2];
};

struct font3dtab_s {
    u8 _pad[8];
    s32 value;
    u8 _pad2[4];
};

struct panel_item_s {
    s16 target;
    s16 value;
    s8 timer;
    s8 delay;
    u8 sfx_delay;
    u8 _pad;
};

struct screen_wumpa_s {
    float x;
    float y;
    float _pad8;
    float timer;
    float target_x;
    float target_y;
    float scale;
    s8 spin;
    u8 _pad1d[3];
};

struct game_s {
    u8 _pad[0xF];
    u8 language;
};

extern struct objtab_s ObjTab[201];
extern struct panel_debris_s PDeb[32];
extern struct font3dtab_s Font3DTab[];
extern signed char Font3DRemap[];
/* Panel counter (count +0x0, draw +0x2, frame +0x4).  The complete type must
 * be visible here -- before UpdatePanelItem and UpdatePlayerStats -- so that
 * plr_wumpas is recognised as small-data (size 8 <= -G8) and addressed
 * $gp-relative; an incomplete forward decl would force a larger address load
 * and desync UpdatePlayerStats by 16 bytes. */
struct panelcount_s {
    short count;
    short draw;
    signed char frame;
    u8 pad[3];
};
extern struct panelcount_s plr_wumpas;
extern struct panel_item_s plr_bonus_wumpas;
extern s32 MAXVPSIZEX, MAXVPSIZEY;
extern s32 MINVPSIZEX, MINVPSIZEY;
extern s32 screendump, save_paused, Paused, editor_active;
extern s32 new_mode, new_level;
extern u8 D_00591FDE[];
extern float PANEL3DMULX, PANEL3DMULY;
extern s32 temp_character;
extern s8 CRemap[];
extern struct CharacterModel CModel[];
extern struct anim_s TempAnim;
extern struct game_s Game;
extern char tbuf[];
extern char D_00631F58[];
extern char D_00631F60[];
extern char D_00631F68[];
extern float font3d_xleft, font3d_ytop, font3d_xright, font3d_ybottom;
extern struct screen_wumpa_s WScr[32];
extern float D_0062E3A4;
extern u16 PANELWUMPAYROT;

extern void NuMtxSetScale(struct numtx_s *, struct nuvec_s *);
extern void NuMtxRotateX(struct numtx_s *, u16);
extern void NuMtxRotateY(struct numtx_s *, u16);
extern void NuMtxRotateZ(struct numtx_s *, u16);
extern s32 NuRndrGScnObj(void *, struct numtx_s *);
extern void NuVpSetSize(float, float);
extern void GameSfx(s32, s32);
extern void EvalModelAnim(struct CharacterModel *, struct anim_s *,
                          struct numtx_s *, struct numtx_s *, float ***,
                          struct numtx_s *);
extern s32 NuHGobjRndrMtxDwa(struct NUHGOBJ_s *, struct numtx_s *, s32,
                             short *, struct numtx_s *, float **);
extern void Text3D(char *, s32, s32, float, float, float, float, float, float);
extern s32 sprintf(char *, char *, ...);
extern void AddSpacesIntoText(char *, s32);

void UpdatePanelItem(struct panel_item_s *item, s32 reset, s32 snap_allowed) {
    s32 timer;
    u32 timer_u;
    s32 next_timer;
    s32 value;
    s32 target;

    timer = item->timer;
    timer_u = (u8)item->timer;
    if (timer == 0x19) {
        if (item->delay != 0) {
            item->delay = (u8)item->delay - 1;
        }
        if (reset != 0) {
            item->delay = 0x32;
        }
        if (item->sfx_delay != 0) {
            item->sfx_delay--;
        } else {
            value = item->value;
            target = item->target;
            if (value < target) {
                item->value = value + 1;
                if ((item == &plr_wumpas) || (item == &plr_bonus_wumpas)) {
                    GameSfx(0x19, 0);
                    item->sfx_delay = 5;
                }
                item->delay = 0x32;
            } else if (target < value) {
                item->value = target;
                item->delay = 0x32;
            }
        }
        if ((item->value == item->target) && (item->delay == 0)) {
            item->timer--;
        }
    } else {
        if (reset == 0) {
            value = item->value;
            target = item->target;
            if ((snap_allowed == 0) || (value != target)) {
                if ((snap_allowed == 0) || (new_mode != -1) ||
                    (new_level != new_mode)) {
                    if (value != target) {
                        item->value = target;
                    }
                    if (item->timer > 0) {
                        item->timer--;
                    }
                    return;
                }
            } else {
                if (item->timer > 0) {
                    item->timer--;
                }
                return;
            }
        }
        if (timer < 0x19) {
            next_timer = timer_u + 2;
            item->timer = next_timer;
            if ((s8)next_timer >= 0x1A) {
                item->timer = 0x19;
            }
        }
    }
}

void BigOutOf(s32 outof, s32 max, float x, float y, float scale) {
    char *text;
    s32 out;
    s32 maximum;
    char *fmt;
    float xleft;
    float half;
    float xsave;
    float ysave;
    float scale_save;
    float one;
    float ytop;
    float xright;
    float ybottom;

    xsave = x;
    ysave = y;
    scale_save = scale;
    out = outof;
    maximum = max;
    if (Game.language == 0x63) {
        text = D_00631F58;
    } else {
        text = D_00631F60;
    }
    one = 1.0f;
    Text3D(text, 0, 0, xsave, ysave, one, scale_save + scale_save,
           scale_save + scale_save, scale_save + scale_save);

    xleft = font3d_xleft;
    ytop = font3d_ytop;
    xright = font3d_xright;
    ybottom = font3d_ybottom;
    fmt = D_00631F68;
    sprintf(tbuf, fmt, out);
    if (Game.language == 0x63) {
        AddSpacesIntoText(tbuf, 1);
    }
    half = 0.5f;
    Text3D(tbuf, 8, 0, xsave + ((xleft - xsave) * half),
           ysave + ((ytop - ysave) * half), one, scale_save, scale_save,
           scale_save);

    sprintf(tbuf, fmt, maximum);
    if (Game.language == 0x63) {
        AddSpacesIntoText(tbuf, 1);
    }
    Text3D(tbuf, 2, 0, xsave + ((xright - xsave) * half),
           ysave + ((ybottom - ysave) * half), one, scale_save, scale_save,
           scale_save);
}

void DrawWorldToPanelWumpa(void) {
    struct numtx_s m;
    struct nuvec_s s;
    struct screen_wumpa_s *base;
    struct screen_wumpa_s *entry;
    struct screen_wumpa_s *w;
    struct objtab_s *objtab;
    struct nugscn_s *scn;
    struct nuspecial_s *obj;
    float t;
    float x;
    float y;
    float scale;
    float scale_target;
    u16 xrot;
    s32 offset;

    base = WScr;
    w = base;
    offset = 0;
    do {
        entry = (struct screen_wumpa_s *)((u8 *)base + offset);
        if (entry->timer > 0.0f) {
            t = (0.25f - entry->timer) * 4.0f;
            if (w->spin != 0) {
                scale_target = 0.125f;
            } else {
                scale_target = D_0062E3A4;
            }
            scale = w->scale + ((scale_target - w->scale) * t);
            x = w->x + ((entry->target_x - w->x) * t);
            y = entry->y + ((entry->target_y - entry->y) * t);
            xrot = 0;
            if (w->spin == 0) {
                xrot = 0xE800;
            }
            objtab = ObjTab;
            scn = objtab->scene;
            obj = objtab->special;
            if (((scn != 0) && (obj != 0)) && (scale != 0.0f)) {
                s.x = scale;
                s.y = scale;
                s.z = scale;
                NuMtxSetScale(&m, &s);
                if (PANELWUMPAYROT != 0) {
                    NuMtxRotateY(&m, PANELWUMPAYROT);
                }
                if (xrot != 0) {
                    NuMtxRotateX(&m, xrot);
                }
                m._32 = 1.0f;
                m._30 = x * PANEL3DMULX;
                m._31 = y * PANEL3DMULY;
                NuRndrGScnObj(scn->gobjs[obj->instance->objid], &m);
            }
        }
        w++;
        offset += sizeof(struct screen_wumpa_s);
    } while (w < &base[32]);
}

/* --- UpdatePlayerStats externs ------------------------------------- */

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

s32 DrawPanel3DTempCharacter(u16 xrot, u16 yrot, u16 zrot, s32 rot, float x,
                             float y, float z, float scale) {
    struct numtx_s m;
    struct numtx_s tmtx[0x100];
    struct nuvec_s s;
    float **dwa;
    struct CharacterModel *model;
    s32 character;
    s32 index;

    character = temp_character;
    if ((character != 0x62) && (character != 0xB8) && (character != 0xB9) &&
        (character != 0xBA) && (character != 0xBC)) {
        return 0;
    }
    index = CRemap[character];
    if (index == -1) {
        return 0;
    }
    model = &CModel[index];
    s.z = scale;
    s.y = scale;
    s.x = scale;
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
                NuMtxRotateY(&m, zrot);
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
                NuMtxRotateX(&m, zrot);
            }
            break;
    }
    m._32 = z;
    m._30 = x * PANEL3DMULX;
    m._31 = y * PANEL3DMULY;
    EvalModelAnim(model, &TempAnim, &m, tmtx, &dwa, 0);
    return NuHGobjRndrMtxDwa(model->hobj, &m, 1, 0, tmtx, dwa);
}

void MaxVP(void) {
    NuVpSetSize((float)(MAXVPSIZEX << 4), (float)(MAXVPSIZEY << 3));
}

void GameVP(void) {
    s32 paused;
    s32 maxx;
    s32 maxy;
    s32 x;
    s32 y;

    if (screendump != 0) {
        paused = save_paused;
    } else {
        paused = Paused;
    }

    if ((editor_active != 0) ||
        ((paused != 0) && ((u32)(D_00591FDE[0] - 3) >= 6))) {
        NuVpSetSize((float)(MAXVPSIZEX << 4), (float)(MAXVPSIZEY << 3));
    } else {
        maxx = MAXVPSIZEX << 4;
        maxy = MAXVPSIZEY << 3;
        x = maxx + (((MINVPSIZEX << 4) - maxx) * paused) / 0x19;
        y = maxy + (((MINVPSIZEY << 3) - maxy) * paused) / 0x19;
        NuVpSetSize((float)x, (float)y);
    }
}

void ResetPanelDebris(void) {
    struct panel_debris_s *deb;
    s32 i;

    deb = PDeb;
    i = 0x1F;
    do {
        deb->active = 0;
        i--;
        deb++;
    } while (i >= 0);
}

s32 NextLetter(char *text, s32 *out) {
    s32 c;

    if (text == 0) {
        return 1;
    }
    if (out != 0) {
        out[1] = 0;
        out[0] = 0;
    }
    c = Font3DRemap[*text];
    if ((c != -1) && (out != 0)) {
        out[0] = Font3DTab[c].value;
    }
    return 1;
}
