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
 * DrawPanel3DObject and NextLetter are public inline functions: gcc 2.95
 * expands them into every later caller and defers their out-of-line copies
 * to the end of the unit.
 */

#include "nu_libc.h"

#include "nu3d/nugscn.h"
#include "nu3d/nuvport.h"
#include "numath/numtx.h"
#include "numath/nutrig.h"
#include "game/gametypes.h"
#include "game/creature.h"

struct pdebinfo_s {
    s32 i_objtab;
    float duration;
    float gravity;
};

struct pdeb_s {
    struct pdebinfo_s *info;
    struct nuvec_s oldpos;
    struct nuvec_s pos;
    struct nuvec_s mom;
    float time;
    float oldscale;
    float scale;
    u16 xrot;
    u16 yrot;
    u16 zrot;
    s16 i_objtab;
    s8 type;
    s8 active;
    u8 _pad[2];
};

extern struct objtab_s ObjTab[201];
extern struct pdeb_s PDeb[32];
extern struct font3dtab_s Font3DTab[];
extern signed char Font3DRemap[];
/* Panel counter (count +0x0, draw +0x2, frame +0x4).  The complete type must
 * be visible here -- before UpdatePanelItem and UpdatePlayerStats -- so that
 * plr_wumpas is recognised as small-data (size 8 <= -G8) and addressed
 * $gp-relative; an incomplete forward decl would force a larger address load
 * and desync UpdatePlayerStats by 16 bytes. */
extern struct panelcount_s plr_wumpas;
extern struct panelcount_s plr_bonus_wumpas;
extern s32 MAXVPSIZEX, MAXVPSIZEY;
extern s32 MINVPSIZEX, MINVPSIZEY;
extern s32 screendump, save_paused, Paused, editor_active;
extern s32 new_mode, new_level;
extern u8 D_00591FDE[];
extern float PANEL3DMULX, PANEL3DMULY;
extern s32 temp_character;
extern struct anim_s TempAnim;
extern struct game_s Game;
extern char tbuf[];
extern char D_00631F58[];
extern char D_00631F60[];
extern char D_00631F68[];
#define tOUTOF_SPACED D_00631F58
#define tOUTOF D_00631F60
#define tINTFORMAT D_00631F68
extern float font3d_xleft, font3d_ytop, font3d_xright, font3d_ybottom;
extern struct wscr_s WScr[32];
extern u16 PANELWUMPAYROT;
extern f32 NuTrigTable[];
extern float FONT3D_JSCALEX;
extern struct gtimer_s GameTimer;

extern s32 NuRndrGScnObj(void *, struct numtx_s *);
extern void GameSfx(s32, s32);
extern s32 NuHGobjRndrMtxDwa(struct NUHGOBJ_s *, struct numtx_s *, s32,
                             short *, struct numtx_s *, float **);
extern void Text3D(char *txt, float x, float y, float z, float scalex, float scaley,
                   float scalez, s32 align, s32 colour);
extern void AddSpacesIntoText(char *, s32);

inline s32 DrawPanel3DObject(s32 object, float x, float y, float z, float scalex,
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
                              e->obj.scene, e->obj.special, 0);
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

inline s32 NextLetter(char *text, struct nuspecial_s **obj) {
    s32 c;

    if (text == 0) {
        return 1;
    }
    if (obj != 0) {
        obj[1] = 0;
        obj[0] = 0;
    }
    c = Font3DRemap[*text];
    if ((c != -1) && (obj != 0)) {
        obj[0] = Font3DTab[c].obj.special;
    }
    return 1;
}


void UpdatePanelItem(struct panelcount_s *item, s32 reset, s32 snap_allowed) {
    if (item->frame == 25) {
        if (item->wait != 0) {
            item->wait--;
        }
        if (reset != 0) {
            item->wait = 50;
        }
        if (item->delay != 0) {
            item->delay--;
        } else if (item->draw < item->count) {
            item->draw++;
            if ((item == (struct panelcount_s *)&plr_wumpas) || (item == &plr_bonus_wumpas)) {
                GameSfx(0x19, 0);
                item->delay = 5;
            }
            item->wait = 50;
        } else if (item->count < item->draw) {
            item->draw = item->count;
            item->wait = 50;
        }
        if ((item->draw == item->count) && (item->wait == 0)) {
            item->frame--;
        }
    } else if ((reset != 0) ||
               ((snap_allowed != 0) && (item->draw != item->count) &&
                (new_mode == -1) && (new_level == new_mode))) {
        if (item->frame < 25) {
            item->frame += 2;
            if (item->frame > 25) {
                item->frame = 25;
            }
        }
    } else {
        if (item->draw != item->count) {
            item->draw = item->count;
        }
        if (item->frame > 0) {
            item->frame--;
        }
    }
}


extern struct ldata_s LData[];
extern s32 Level;
extern s32 HubFromLevel(s32 level);
extern void MakeTimeI(s32 time, s32 mode, char *txt);
extern void MakeLevelTimeString(struct time_s *time, char *txt);

void DrawTimeTrialTimes(s32 level, float x, float y) {
    float dy;
    s32 i;
    s32 j;
    float t;

    i = -1;
    if (((Game.level[level].flags & 8) != 0) || (HubFromLevel(level) == 5)) {
        if ((Game.level[level].flags & 1) == 0) {
            i = 3;
            t = LData[level].time[0];
        } else if ((Game.level[level].flags & 2) == 0) {
            i = 2;
            t = LData[level].time[1];
        } else if ((Game.level[level].flags & 4) == 0) {
            i = 1;
            t = LData[level].time[2];
        }
    }

    if (Level == 0x25) {
        DrawPanel3DObject(0x81, x, y - 0.03f, 1.01f, 0.15f, 0.13f,
                          (i != -1) ? 0.11f : 0.08f, 0xC000, 0, 0,
                          ObjTab[129].obj.scene, ObjTab[129].obj.special, 0);
    }

    dy = -0.125f;
    y -= dy;
    if (i != -1) {
        if (Game.language == 0x63) {
            tbuf[0] = tbuf[1] = tbuf[2] = tbuf[3] = ' ';
            tbuf[4] = ObjTab[i].font3d_letter;
            tbuf[5] = tbuf[6] = tbuf[7] = ' ';
        } else {
            tbuf[0] = ' ';
            tbuf[1] = ' ';
            tbuf[2] = ObjTab[i].font3d_letter;
            tbuf[3] = ' ';
        }

        y -= dy * 1.25f * 0.5f;
        MakeTimeI(t, 0, (Game.language == 0x63) ? tbuf + 8 : tbuf + 4);
        Text3D(tbuf, x, y, 1.0f, (Game.language == 0x63) ? 0.55f : 0.6f,
               0.6f, 0.6f, 0, 3);
        y += dy * 1.25f;
    }

    for (j = 0; j < 3; j++) {
        MakeLevelTimeString(&Game.level[level].time[j], tbuf);
        Text3D(tbuf, x, y, 1.0f, 0.6f, 0.6f, 0.6f, 0, 0);
        y += dy;
    }
}


void BigOutOf(float x, float y, float scale, s32 value, s32 total) {
    char *txt;
    float x0;
    float y0;
    float x1;
    float y1;

    if (Game.language == 0x63) {
        txt = tOUTOF_SPACED;
    } else {
        txt = tOUTOF;
    }
    Text3D(txt, x, y, 1.0f, scale + scale, scale + scale, scale + scale, 0,
           0);
    x0 = font3d_xleft;
    y0 = font3d_ytop;
    x1 = font3d_xright;
    y1 = font3d_ybottom;
    sprintf(tbuf, tINTFORMAT, value);
    if (Game.language == 0x63) {
        AddSpacesIntoText(tbuf, 1);
    }
    Text3D(tbuf, x + (x0 - x) * 0.5f, y + (y0 - y) * 0.5f, 1.0f, scale, scale,
           scale, 8, 0);
    sprintf(tbuf, tINTFORMAT, total);
    if (Game.language == 0x63) {
        AddSpacesIntoText(tbuf, 1);
    }
    Text3D(tbuf, x + (x1 - x) * 0.5f, y + (y1 - y) * 0.5f, 1.0f, scale, scale,
           scale, 2, 0);
}


extern void NuHGobjEvalAnim(struct NUHGOBJ_s *hobj, struct nuanimdata_s *data,
                            f32 time, s32 nJ, void *pJ,
                            struct numtx_s *tmtx);
extern void NuHGobjEval(struct NUHGOBJ_s *hobj, s32 nJ, void *pJ,
                        struct numtx_s *tmtx);
extern s32 NuHGobjRndrMtx(struct NUHGOBJ_s *hobj, struct numtx_s *m,
                          s32 nlayers, s16 *layer, struct numtx_s *tmtx);

s32 DrawPanel3DCharacter(s32 character, float x, float y, float z,
                         float scalex, float scaley, float scalez, u16 xrot,
                         u16 yrot, u16 zrot, s32 action, float anim_time,
                         s32 rot) {
    struct numtx_s m;
    struct numtx_s tmtx[0x100];
    struct nuvec_s s;
    struct CharacterModel *model;
    s32 id;
    struct objtab_s *tab;

    if ((scalex == 0.0f) && (scaley == 0.0f) && (scalez == 0.0f)) {
        return 0;
    }
    if ((u32)character > 0xBE) {
        return 0;
    }
    id = CRemap[character];
    if (id == -1) {
        return 0;
    }
    model = &CModel[id];
    if (character == 0x75) {
        id = 0x84;
    } else if (character == 0x77) {
        id = 0x88;
    } else if (character == 0x78) {
        id = 0x89;
    } else if (character == 0x79) {
        id = 0x8A;
    } else if (character == 0x7A) {
        id = 0x8B;
    } else if (character == 0x7B) {
        id = 0x8C;
    } else if (character == 0x7C) {
        id = 0x8D;
    } else if (character == 0x7D) {
        id = 0x8E;
    } else {
        id = -1;
    }
    if (id != -1) {
        tab = &ObjTab[id];
        DrawPanel3DObject(id, x, y, z, scalex, scaley, scalez, 0, 0, 0,
                          tab->obj.scene, tab->obj.special, 0);
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
    if (((u32)action < 0x76) && (model->anmdata[action] != 0)) {
        NuHGobjEvalAnim(model->obj, model->anmdata[action], anim_time, 0, 0,
                        tmtx);
    } else {
        NuHGobjEval(model->obj, 0, 0, tmtx);
    }
    return NuHGobjRndrMtx(model->obj, &m, 1, 0, tmtx);
}


extern struct cammtx_s GameCam[];
extern struct nugscn_s *font3d_scene;
extern struct nuvec_s cpPOS;
extern char *tCHECK[];
extern char *tPOINT[];
extern s32 nCheckLetters;
extern s32 nPointLetters;
extern float check_time;
extern float check_duration;
extern float check_delay;
extern float point_time;
extern float point_duration;
extern float point_delay;
extern float CHECKWAIT;
extern float POINTWAIT;
extern float CPLTIME;
extern float CHECKPOINT3DSCALE;
extern float CHECKPOINT3DSEPERATION;
extern float CHECKPOINT3DHEIGHT;
extern s32 Draw3DObject(s32 object, struct nuvec_s *pos, u16 xrot, u16 yrot,
                        u16 zrot, f32 scalex, f32 scaley, f32 scalez,
                        struct nugscn_s *scn, struct nuspecial_s *obj, s32 rot);

void Draw3DCheckpointLetters(void) {
    s32 i;
    s32 j;
    u16 ang;
    float dx;
    float dz;
    float t;
    float f;
    float x;
    float time;
    struct nuvec_s pos;
    struct nuvec_s s;
    char *txt;
    struct nuspecial_s *obj[2];

    if (font3d_scene != 0) {
        dx = NuTrigTable[(GameCam->yrot + 0x4000) & 0xFFFF] *
             CHECKPOINT3DSEPERATION;
        dz = NuTrigTable[(GameCam->yrot - 0x8000) & 0xFFFF] *
             CHECKPOINT3DSEPERATION;
        ang = ((GameTimer.frame % 150) << 16) / 150;
        if ((check_time < check_duration) && (check_time >= CHECKWAIT)) {
            time = check_time - CHECKWAIT;
            txt = tCHECK[Game.language];
            for (i = 0; i < nCheckLetters; i++, ang += 0x176A) {
                t = CPLTIME * i;
                txt = &txt[NextLetter(txt, obj)];
                if ((obj[0] != 0) && (time >= t) && (time < t + check_delay)) {
                    f = time - t;
                    x = (nCheckLetters - 1) * CHECKPOINT3DSEPERATION * 0.5f * -1.0f +
                        i * CHECKPOINT3DSEPERATION;
                    pos.x = cpPOS.x;
                    pos.y = cpPOS.y + 0.25f + CHECKPOINT3DHEIGHT;
                    pos.y += NuTrigTable[ang] * 0.125f;
                    pos.z = cpPOS.z;
                    if (f < CPLTIME) {
                        pos.x += (pos.x + dx * x - pos.x) * f * (1.0f / CPLTIME);
                        pos.y = cpPOS.y + (pos.y - cpPOS.y) * f * (1.0f / CPLTIME);
                        pos.z += (pos.z + dz * x - pos.z) * f * (1.0f / CPLTIME);
                        s.x = CHECKPOINT3DSCALE * f * (1.0f / CPLTIME);
                    } else {
                        if (f >= check_delay - CPLTIME) {
                            x -= (f - (check_delay - CPLTIME)) / CPLTIME * 5.0f;
                        }
                        pos.x = cpPOS.x + dx * x;
                        pos.z = cpPOS.z + dz * x;
                        s.x = CHECKPOINT3DSCALE;
                    }
                    s.z = s.x;
                    s.y = s.x;
                    if (Game.language == 0x63) {
                        s.x *= FONT3D_JSCALEX;
                    }
                    for (j = 0; j < 2; j++) {
                        if (obj[j] != 0) {
                            Draw3DObject(-1, &pos, 0, GameCam->yrot, 0, s.x, s.y,
                                         (j == 1) ? s.z * 1.5f : s.z,
                                         font3d_scene, obj[j], 0);
                        }
                    }
                }
            }
        }
        if ((point_time < point_duration) && (point_time >= POINTWAIT)) {
            time = point_time - POINTWAIT;
            txt = tPOINT[Game.language];
            for (i = 0; i < nPointLetters; i++, ang += 0x176A) {
                t = CPLTIME * i;
                txt = &txt[NextLetter(txt, obj)];
                if ((obj[0] != 0) && (time >= t) && (time < t + point_delay)) {
                    f = time - t;
                    x = (nPointLetters - 1) * CHECKPOINT3DSEPERATION * 0.5f * -1.0f +
                        i * CHECKPOINT3DSEPERATION;
                    pos.x = cpPOS.x;
                    pos.y = cpPOS.y + 0.25f + CHECKPOINT3DHEIGHT;
                    pos.y += NuTrigTable[ang] * 0.125f;
                    pos.z = cpPOS.z;
                    if (f < CPLTIME) {
                        pos.x += (pos.x + dx * x - pos.x) * f * (1.0f / CPLTIME);
                        pos.y = cpPOS.y + (pos.y - cpPOS.y) * f * (1.0f / CPLTIME);
                        pos.z += (pos.z + dz * x - pos.z) * f * (1.0f / CPLTIME);
                        s.x = CHECKPOINT3DSCALE * f * (1.0f / CPLTIME);
                    } else {
                        if (f >= point_delay - CPLTIME) {
                            x -= (f - (point_delay - CPLTIME)) / CPLTIME * 5.0f;
                        }
                        pos.x = cpPOS.x + dx * x;
                        pos.z = cpPOS.z + dz * x;
                        s.x = CHECKPOINT3DSCALE;
                    }
                    s.z = s.x;
                    s.y = s.x;
                    if (Game.language == 0x63) {
                        s.x *= FONT3D_JSCALEX;
                    }
                    for (j = 0; j < 2; j++) {
                        if (obj[j] != 0) {
                            Draw3DObject(-1, &pos, 0, GameCam->yrot, 0, s.x, s.y,
                                         (j == 1) ? s.z * 1.5f : s.z,
                                         font3d_scene, obj[j], 0);
                        }
                    }
                }
            }
        }
    }
}


extern char *tLOADING[];
extern s32 JStrLen(char *txt);
extern s32 CombinationCharacterBD(char c0, char c1);
extern s32 CombinationCharacterBC(char c0, char c1);

void DrawGameMessage(char *txt, s32 message_frame, float ys) {
    s32 i;
    s32 j;
    s32 len;
    s32 frame;
    s32 frames[4];
    float xs;
    float size;
    float sx;
    char msg[5];
    char *p;
    s32 x;

    if (Game.language == 0x63) {
        len = JStrLen(txt);
    } else {
        len = strlen(txt);
    }

    frame = (len - 1) * 16;
    frames[0] = frame;
    frames[1] = frame + 0x19;
    frames[2] = frame + 0x3E;
    frames[3] = frame + 0x70;

    x = (frames[2] + message_frame) % frames[3];

    i = -1;
    if (x < frames[0]) {
        if (x % 16 < 11) {
            i = x / 16 + 1;
        }
    } else if (x < frames[1]) {
        if ((x - frames[0]) < 18) {
            i = len;
        }
    } else if (x < frames[2]) {
        if ((x - frames[1]) < 31) {
            i = len;
        }
    } else if (((x - frames[2]) % 50) < 43) {
        i = len;
    }

    if (i != -1) {
        size = 1.0f;
        if ((strcmp(txt, tLOADING[Game.language]) == 0) &&
            (Game.language == 3)) {
            size = 0.8f;
        }

        xs = -((len - 1) * 0.1f * size * 0.5f);
        sx = (Game.language != 0x63) ? 1.0f : FONT3D_JSCALEX;
        p = txt;
        for (j = 0; j < i; j++) {
            if (Game.language == 0x63) {
                msg[0] = *p++;
                msg[1] = *p++;
                if ((p[0] == 0x42) &&
                    (((p[1] == 0x44) &&
                      (CombinationCharacterBD(msg[0], msg[1]) != 0)) ||
                     ((p[1] == 0x43) &&
                      (CombinationCharacterBC(msg[0], msg[1]) != 0)))) {
                    msg[2] = *p++;
                    msg[3] = *p++;
                    msg[4] = 0;
                } else {
                    msg[2] = 0;
                }
            } else {
                msg[0] = *p++;
                msg[1] = 0;
            }
            Text3D(msg, xs, ys, 1.0f, size * sx, 1.0f, 1.0f, 0, 4);
            xs = size * 0.1f + xs;
        }
    }
}


void DrawWorldToPanelWumpa(void) {
    float xs;
    float ys;
    float f;
    float size;
    s32 i;
    float x;
    float y;

    for (i = 0; i < 32; i++) {
        if (WScr[i].timer > 0.0f) {
            x = WScr[i].pos.x;
            y = WScr[i].pos.y;
            f = (0.25f - WScr[i].timer) * 4.0f;
            size = WScr[i].scale +
                   ((WScr[i].bonus != 0 ? 0.125f : 0.2f) - WScr[i].scale) * f;
            xs = x + (WScr[i].xs - x) * f;
            ys = y + (WScr[i].ys - y) * f;
            DrawPanel3DObject(0, xs, ys, 1.0f, size, size, size,
                              WScr[i].bonus ? 0 : 0xE800, PANELWUMPAYROT, 0,
                              ObjTab[0].obj.scene, ObjTab[0].obj.special, 1);
        }
    }
}


/* --- UpdatePlayerStats externs ------------------------------------- */

extern struct cratecube_s Crate[];
extern s32 CRATECOUNT;
extern int crate_wumpa;
extern s32 crates_destroyed;
extern s32 bonus_crates_destroyed;
extern s32 old_bonus_crates;
extern s32 save_bonus_crates_destroyed;
extern s32 Bonus;
extern s32 bonus_finish_frame;
extern f32 bonus_crates_wait;
extern f32 D_0062E640;              /* 1/50 wait decrement (.sdata) */
extern f32 D_0062E644;              /* -1/50 wait floor (.sdata) */
extern struct panelcount_s plr_crates;
extern struct panelcount_s plr_wumpas;
extern struct panelcount_s plr_lives;
extern s32 force_panel_lives_update;
extern s32 mask_crates;
extern s32 newmask_advise;
extern f32 WUMPAOBJSX;
extern f32 PANELSY;
extern struct nuvec_s vNEWMASK;
extern struct cursor_s Cursor;

extern void AddPanelDebris(f32 x, f32 y, s32 type, f32 scale, s32 count);
extern void NewMask(struct mask_s *mask, struct nuvec_s *pos);
extern void NewMenu(void *cursor, s32 a, s32 b, s32 c);

void UpdatePlayerStats(struct creature_s *plr) {
    struct cratecube_s *crate;
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
                    NewMenu(&Cursor, 0x21, -1, -1);
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

extern struct pdebinfo_s PDebInfo[];
extern struct ldata_s *LDATA;
extern s32 i_pdeb;
extern s32 qrand(void);

void AddPanelDebris(f32 x, f32 y, s32 type, f32 scale, s32 count) {
    struct pdeb_s *deb;
    f32 f;
    s32 i;
    u32 i_objtab;

    i = count;
    do {
        if ((u32)type < 7) {
            deb = &PDeb[i_pdeb];
            deb->info = &PDebInfo[type];
            i_objtab = deb->info->i_objtab;
            if ((i_objtab == 0x11) && (LDATA->character == 1)) {
                i_objtab = 0x12;
            }
            if ((i_objtab < 0xC9) && ((ObjTab + i_objtab)->obj.special != 0)) {
                deb->type = type;
                deb->xrot = 0;
                deb->yrot = 0;
                deb->zrot = 0;
                switch ((s8)type) {
                    case 1:
                        deb->mom.x = qrand() * 1.5259022e-05f * 0.5f * 0.02f;
                        deb->mom.y = (qrand() * 1.5259022e-05f + 2.0f) * 0.02f;
                        deb->pos.x = x + (deb->mom.x + deb->mom.x);
                        deb->pos.y = (y + 0.05f) + deb->mom.y;
                        break;
                    case 2:
                    case 3:
                    case 4:
                    case 5:
                        deb->pos.x = x;
                        deb->pos.y = y;
                        deb->oldpos = deb->pos;
                        deb->oldscale = scale;
                        break;
                    case 6:
                        deb->zrot = qrand() / 2 + 0xC000;
                        f = qrand() * 1.5259022e-05f * 0.02f;
                        deb->mom.x = NuTrigTable[deb->zrot] * f;
                        deb->mom.y =
                            NuTrigTable[(deb->zrot + 0x4000) & 0xFFFF] * f;
                        deb->pos.x = x + (deb->mom.x + deb->mom.x);
                        deb->pos.y = y + (deb->mom.y + deb->mom.y);
                        break;
                    default:
                        deb->mom.x = 0.0f;
                        deb->mom.y = 0.0f;
                        deb->pos.x = x;
                        deb->pos.y = y;
                        break;
                }
                deb->i_objtab = i_objtab;
                deb->time = 0.0f;
                deb->active = 1;
                i_pdeb++;
                if (i_pdeb == 0x20) {
                    i_pdeb = 0;
                }
            }
        }
        i--;
    } while (i > 0);
}


extern f32 BONUSWUMPAOBJSX;
extern f32 BONUSPANELSY;
extern f32 NEWLIFEOBJSX;
extern f32 LIVESOBJSX;
extern f32 NEWBONUSLIFEOBJSX;
extern f32 BONUSLIVESOBJSX;
extern f32 LIFESCALE;
extern f32 BONUSLIFESCALE;
extern u16 panel_head_xrot;
extern u16 panel_head_yrot;
extern s32 bonus_lives;

void UpdatePanelDebris(void) {
    struct pdeb_s *deb;
    float tmul;
    float xs[2];
    float ys;
    float scale;
    float f;
    float old_time;
    s32 i;

    deb = PDeb;
    for (i = 0; i < 32; i++, deb++) {
        if (deb->active != 0) {
            old_time = deb->time;
            deb->time += 0.02f;
            if (deb->time < deb->info->duration) {
                tmul = deb->time / deb->info->duration;
                switch (deb->type) {
                    case 0:
                        deb->pos.x = BONUSWUMPAOBJSX +
                                     (WUMPAOBJSX - BONUSWUMPAOBJSX) * 0.9f * tmul;
                        deb->pos.y = BONUSPANELSY +
                                     (PANELSY - BONUSPANELSY) * 0.9f * tmul;
                        deb->xrot = tmul * -5529.5996f;
                        deb->yrot = -(((GameTimer.frame % 33) << 16) / 33);
                        deb->scale = tmul * 0.075f + 0.125f;
                        break;
                    case 1:
                        deb->pos.x += deb->mom.x;
                        deb->mom.y += deb->info->gravity;
                        deb->pos.y += deb->mom.y;
                        deb->xrot = 0;
                        deb->yrot = -(((GameTimer.frame % 33) << 16) / 33);
                        deb->scale = 0.125f;
                        break;
                    case 2:
                    case 3:
                    case 4:
                    case 5:
                        if (deb->type == 3) {
                            xs[0] = NEWBONUSLIFEOBJSX;
                            xs[1] = BONUSLIVESOBJSX;
                            ys = BONUSPANELSY;
                            scale = BONUSLIFESCALE;
                            deb->xrot = panel_head_xrot;
                        } else {
                            xs[0] = NEWLIFEOBJSX;
                            xs[1] = LIVESOBJSX;
                            ys = PANELSY;
                            scale = LIFESCALE;
                            if ((deb->type == 2) || (deb->type == 5)) {
                                deb->xrot = panel_head_xrot - 0x1000;
                            } else {
                                deb->xrot = panel_head_xrot - tmul * 4096.0f;
                            }
                        }
                        if (deb->time < 0.25f) {
                            f = deb->time * 4.0f;
                            deb->pos.x = deb->oldpos.x + (xs[0] - deb->oldpos.x) * f;
                            deb->pos.y = deb->oldpos.y + (ys - deb->oldpos.y) * f;
                            deb->scale = deb->oldscale + (scale - deb->oldscale) * f;
                        } else if (deb->time < 1.0f) {
                            if (old_time < 0.25f) {
                                GameSfx(0x2B, 0);
                            }
                            if ((deb->time - 0.02f) < 0.25f) {
                                force_panel_lives_update = 50;
                            }
                            deb->pos.x = xs[0];
                            deb->pos.y = ys;
                        } else {
                            f = (deb->time - 1.0f) * 2.0f;
                            deb->pos.x = xs[0] + (xs[1] - xs[0]) * f;
                        }
                        deb->yrot = panel_head_yrot + 0x2000;
                        break;
                    case 6:
                        deb->pos.x += deb->mom.x;
                        deb->mom.y += deb->info->gravity;
                        deb->pos.y = deb->pos.y + deb->mom.y;
                        if ((deb->mom.x != 0.0f) || (deb->mom.y != 0.0f)) {
                            deb->zrot = -NuAtan2D(deb->mom.x, deb->mom.y);
                        }
                        deb->scale = (1.0f - tmul) * 0.125f;
                        break;
                }
            } else {
                if ((deb->type == 2) || (deb->type == 4) || (deb->type == 5)) {
                    if (plr_lives.count < 99) {
                        plr_lives.count++;
                    }
                } else if (deb->type == 3) {
                    bonus_lives++;
                }
                deb->active = 0;
            }
        }
    }
}


void DrawPanelDebris(void) {
    struct pdeb_s *deb;
    s32 i;
    float f;
    struct objtab_s *tab;

    deb = PDeb;
    for (i = 0; i < 32; i++, deb++) {
        if (deb->active != 0) {
            if ((deb->type == 5) || (deb->type == 4) || (deb->type == 3) ||
                (deb->type == 2)) {
                if ((0.25f <= deb->time) && (deb->time < 1.0f)) {
                    f = fmod(deb->time - 0.25f, 0.25f);
                    if ((0.0625f <= f) && (f < 0.1875f)) {
                        continue;
                    }
                }
            }
            tab = &ObjTab[deb->i_objtab];
            DrawPanel3DObject(deb->i_objtab, deb->pos.x, deb->pos.y, 1.0f,
                              deb->scale, deb->scale, deb->scale, deb->xrot,
                              deb->yrot, deb->zrot,
                              tab->obj.scene,
                              tab->obj.special, 1);
        }
    }
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
    return NuHGobjRndrMtxDwa(model->obj, &m, 1, 0, tmtx, dwa);
}

void MaxVP(void) {
    NuVpSetSize((float)(MAXVPSIZEX << 4), (float)(MAXVPSIZEY << 3));
}

void GameVP(void) {
    s32 paused;
    s32 x, y;

    if (screendump != 0) {
        paused = save_paused;
    } else {
        paused = Paused;
    }

    if ((editor_active != 0) ||
        ((paused != 0) && ((u32)(D_00591FDE[0] - 3) >= 6))) {
        NuVpSetSize((float)(MAXVPSIZEX << 4), (float)(MAXVPSIZEY << 3));
    } else {
        x = (MAXVPSIZEX << 4) + (((MINVPSIZEX << 4) - (MAXVPSIZEX << 4)) * paused) / 0x19;
        y = (MAXVPSIZEY << 3) + (((MINVPSIZEY << 3) - (MAXVPSIZEY << 3)) * paused) / 0x19;
        NuVpSetSize((float)x, (float)y);
    }
}

void ResetPanelDebris(void) {
    struct pdeb_s *deb;
    s32 i;

    deb = PDeb;
    i = 0x1F;
    do {
        deb->active = 0;
        i--;
        deb++;
    } while (i >= 0);
}
