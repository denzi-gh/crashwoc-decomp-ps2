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

typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef short s16;
typedef int s32;

struct nuvec_s {
    float x, y, z;
};

struct numtx_s {
    float _00, _01, _02, _03;
    float _10, _11, _12, _13;
    float _20, _21, _22, _23;
    float _30, _31, _32, _33;
};

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

struct NUHGOBJ_s;
struct anim_s;

struct CharacterModel {
    struct NUHGOBJ_s *hobj;
    u8 _pad[0x984];
};

extern struct objtab_s ObjTab[201];
extern struct panel_debris_s PDeb[32];
extern struct font3dtab_s Font3DTab[];
extern signed char Font3DRemap[];
extern struct panel_item_s plr_wumpas, plr_bonus_wumpas;
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
