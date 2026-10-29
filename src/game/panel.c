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
 * DrawPanel3DObject, NextLetter, DrawPanel3DTempCharacter, MaxVP and GameVP
 * are public inline functions: gcc 2.95 expands them into every later caller
 * and defers their out-of-line copies to the end of the unit.
 */

#include "nu_libc.h"
#include "nu_port.h"

#include "nu3d/nucamera.h"
#include "nu3d/nugscn.h"
#include "nu3d/nuvport.h"
#include "numath/numtx.h"
#include "numath/nutrig.h"
#include "nups2/ps2ctl.h"
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

struct pdebinfo_s PDebInfo[7] = {
    { 0, 0.25f, 0.0f },
    { 0, 2.0f, -0.0024f },
    { 17, 1.5f, 0.0f },
    { 17, 1.5f, 0.0f },
    { 17, 1.5f, 0.0f },
    { 17, 1.5f, 0.0f },
    { 132, 1.0f, -0.00024000001f },
};

struct pdeb_s PDeb[32] = { 0 };

u16 PANELCRUNCHXROT = 0xF555;
f32 PANELCRUNCHX = 1.01f;
f32 PANELCRUNCHY = -1.0f;
f32 PANELCRUNCHZ = 1.25f;
f32 PANELCRUNCHSCALE = 0.256f;
f32 PANELSINKX = 1.0f;
f32 PANELSINKY = -1.1f;
f32 PANELSINKZ = 1.25f;
f32 PANELSINKSCALE = 0.045f;
f32 PANELBLIMPX = 1.0f;
f32 PANELBLIMPY = -0.9f;
f32 PANELBLIMPZ = 1.25f;
f32 PANELBLIMPSCALE = 0.1f;
f32 PANELCANVASX = -0.5f;
f32 PANELCANVASY = -0.7f;
f32 PANELCANVASZ = 1.0f;
f32 PANELCANVASSCALE = 0.4f;
f32 WUMPAOBJSX = -0.8f;
f32 WUMPATXTSX = -0.7f;
f32 CRATEOBJSX = -0.29f;
f32 CRATETXTSX = -0.17f;
f32 LIVESOBJSX = 0.53f;
f32 LIVESTXTSX = 0.65f;
f32 PANELSY = 0.8f;
f32 BONUSWUMPAOBJSX = -0.8f;
f32 BONUSWUMPATXTSX = -0.73f;
f32 BONUSCRATEOBJSX = -0.13f;
f32 BONUSCRATETXTSX = -0.05f;
f32 BONUSLIVESOBJSX = 0.7f;
f32 BONUSLIVESTXTSX = 0.77f;
f32 BONUSPANELSY = -0.9f;
u16 PANELWUMPAYROT = 0;
u16 PANELCRATEYROT = 0;
f32 NEWLIFEOBJSX = 0.32999998f;
f32 NEWBONUSLIFEOBJSX = 0.575f;
f32 LIFESCALE = 0.2f;
f32 BONUSLIFESCALE = 0.125f;
s32 force_panel_wumpa_update = 0;
s32 force_panel_crate_update = 0;
s32 force_panel_lives_update = 0;
s32 force_panel_items_update = 0;
f32 DIVPANEL3D = 1200.0f;
f32 CHECKPOINT3DSCALE = 0.3f;
f32 CHECKPOINT3DSEPERATION = 0.55f;
f32 CHECKPOINT3DHEIGHT = 0.9f;

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
extern struct cursor_s Cursor;
extern float PANEL3DMULX, PANEL3DMULY;
extern s32 temp_character;
extern AnimPacket TempAnim;
extern struct game_s Game;
extern char tbuf[];
extern float font3d_xleft, font3d_ytop, font3d_xright, font3d_ybottom;
extern struct wscr_s WScr[32];
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
        m._30 = x * PANEL3DMULX;
        m._31 = y * PANEL3DMULY;
        m._32 = z;
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


inline s32 DrawPanel3DTempCharacter(u16 xrot, u16 yrot, u16 zrot, s32 rot, float x,
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
    index = CRemap[temp_character];
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


inline void MaxVP(void) {
    NuVpSetSize((float)(MAXVPSIZEX << 4), (float)(MAXVPSIZEY << 3));
    NW_WIDE_ONLY(nw_wide_vp(0);)
}


inline void GameVP(void) {
    s32 paused;
    s32 x, y;

    if (screendump != 0) {
        paused = save_paused;
    } else {
        paused = Paused;
    }

    if ((editor_active != 0) ||
        ((paused != 0) && ((Cursor.menu < 3) || (Cursor.menu > 8)))) {
        NuVpSetSize((float)(MAXVPSIZEX << 4), (float)(MAXVPSIZEY << 3));
        NW_WIDE_ONLY(nw_wide_vp(0);)
    } else {
        x = (MAXVPSIZEX << 4) + (((MINVPSIZEX << 4) - (MAXVPSIZEX << 4)) * paused) / NW_HZ(0x19, 0x1E);
        y = (MAXVPSIZEY << 3) + (((MINVPSIZEY << 3) - (MAXVPSIZEY << 3)) * paused) / NW_HZ(0x19, 0x1E);
        NuVpSetSize((float)x, (float)y);
        NW_WIDE_ONLY(nw_wide_vp(paused);)
    }
}


void UpdatePanelItem(struct panelcount_s *item, s32 reset, s32 snap_allowed) {
    if (item->frame == NW_HZ(25, 30)) {
        if (item->wait != 0) {
            item->wait--;
        }
        if (reset != 0) {
            item->wait = NW_HZ(50, 60);
        }
        if (item->delay != 0) {
            item->delay--;
        } else if (item->draw < item->count) {
            item->draw++;
            if ((item == (struct panelcount_s *)&plr_wumpas) || (item == &plr_bonus_wumpas)) {
                GameSfx(0x19, 0);
                item->delay = NW_HZ(5, 6);
            }
            item->wait = NW_HZ(50, 60);
        } else if (item->count < item->draw) {
            item->draw = item->count;
            item->wait = NW_HZ(50, 60);
        }
        if ((item->draw == item->count) && (item->wait == 0)) {
            item->frame--;
        }
    } else if ((reset != 0) ||
               ((snap_allowed != 0) && (item->draw != item->count) &&
                (new_mode == -1) && (new_level == new_mode))) {
        if (item->frame < NW_HZ(25, 30)) {
            item->frame += 2;
            if (item->frame > NW_HZ(25, 30)) {
                item->frame = NW_HZ(25, 30);
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
        txt = "/ ";
    } else {
        txt = "/";
    }
    Text3D(txt, x, y, 1.0f, scale + scale, scale + scale, scale + scale, 0,
           0);
    x0 = font3d_xleft;
    y0 = font3d_ytop;
    x1 = font3d_xright;
    y1 = font3d_ybottom;
    sprintf(tbuf, "%i", value);
    if (Game.language == 0x63) {
        AddSpacesIntoText(tbuf, 1);
    }
    Text3D(tbuf, x + (x0 - x) * 0.5f, y + (y0 - y) * 0.5f, 1.0f, scale, scale,
           scale, 8, 0);
    sprintf(tbuf, "%i", total);
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
        ang = ((GameTimer.frame % NW_HZ(150, 180)) << 16) / NW_HZ(150, 180);
        if ((check_time < check_duration) && (check_time >= CHECKWAIT)) {
            time = check_time - CHECKWAIT;
            txt = tCHECK[Game.language];
            for (i = 0; i < nCheckLetters; i++, ang += NW_HZ(0x176A, 0x1383)) {
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
            for (i = 0; i < nPointLetters; i++, ang += NW_HZ(0x176A, 0x1383)) {
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

    frame = (len - 1) * NW_HZ(16, 20);
    frames[0] = frame;
    frames[1] = frame + NW_HZ(0x19, 0x1E);
    frames[2] = frame + NW_HZ(0x3E, 0x4B);
    frames[3] = frame + NW_HZ(0x70, 0x87);

    x = (frames[2] + message_frame) % frames[3];

    i = -1;
    if (x < frames[0]) {
        if (x % NW_HZ(16, 20) < NW_HZ(11, 13)) {
            i = x / NW_HZ(16, 20) + 1;
        }
    } else if (x < frames[1]) {
        if ((x - frames[0]) < NW_HZ(18, 21)) {
            i = len;
        }
    } else if (x < frames[2]) {
        if ((x - frames[1]) < NW_HZ(31, 37)) {
            i = len;
        }
    } else if (((x - frames[2]) % NW_HZ(50, 60)) < NW_HZ(43, 52)) {
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


extern struct gtimer_s GlobalTimer;
extern s32 SWIDTH;
extern s32 SHEIGHT;
extern s32 PLAYERCOUNT;
extern s32 PANELOFF;
extern s32 pause_rndr_on;
extern s32 cutmovie;
extern s32 ShowPlayerCoordinate;
extern struct nucamera_s *pNuCam;
extern struct cammtx_s *pCam;
extern struct nuvec_s vTEMP;
extern struct numtx_s numtx_identity;
extern struct nupad_s *Pad[];
extern s32 Demo;
extern s32 GameMode;
extern s32 Hub;
extern float hubleveltext_pos;
extern s32 hubleveltext_level;
extern s32 hubleveltext_open;
extern s32 hubleveltext_i;
extern s32 InvincibilityCHEAT;
extern s32 pause_dir;
extern s32 pausestats_frame;
extern s32 ExtraMoves;
extern struct powerdata_s PData[6];
extern u8 CLetter[191];
extern s32 sapphire_relics;
extern s32 gold_relics;
extern s32 platinum_relics;
extern u8 bonus_wumpa_delay;
extern u8 bonus_life_delay;
extern f32 bonus_wumpa_wait;
extern f32 bonus_lives_wait;
extern float bonus_panel_wait;
extern struct nugscn_s *crate_scene;
extern struct crateeditor_s crate_list[];
extern s32 bonus_frame;
extern s32 DESTRUCTIBLECRATECOUNT;
extern s32 DESTRUCTIBLEBONUSCRATECOUNT;
extern struct panelcount_s plr_crystal;
extern struct panelcount_s plr_crategem;
extern struct panelcount_s plr_bonusgem;
extern struct newwumpa_s NewWumpa[];
extern s32 TimeTrial;
extern u32 finish_frame;
extern u32 finish_frames;
extern s32 newleveltime_slot;
extern u16 new_lev_flags;
extern float tt_sx;
extern float tt_sy;
extern struct gtimer_s TimeTrialTimer;
extern float TT_RELICX;
extern float TT_RELICY;
extern s32 tt_flash;
extern u32 timetrial_frame;
extern struct nuvec_s TTScrPos;
extern f32 TimeTrialWait;
extern s32 VEHICLECONTROL;
extern s32 SmokeyCountDownValue;
extern s32 SmokeyPosition;
extern struct tubinfo_s PlrTub;
extern s32 MAXLAPS;
extern s32 i_ring;
extern s32 carpet_panelpos;
extern s32 carpet_place;
extern s32 RINGCOUNT;
extern s32 GATECOUNT;
extern s32 plr_gates;
extern s32 in_finish_range;

extern char *tINSERTCONTROLLERINPORT1[6][2];
extern char *LevelName[44][6];
extern char *PlaceName[5][6];
extern char *tRELICSREQUIRED[6];
extern char *tCRYSTALS[6];
extern char *tPOWERS[6];
extern char *tGEMS[6];
extern char *tRELICS[6];
extern char *tTIMES[6];
extern char *tBONUS[6];
extern char *tDEMO[6];
extern char *tENTERINGTIMETRIAL[6];
extern char *tBESTTIME[6];
extern char *t2NDBESTTIME[6];
extern char *t3RDBESTTIME[6];
extern char *tNONEWTIME[6];
extern char *tGO[6];

extern float TT_SCALE;

char *test_font3d[2] = {
    "t?\200\232\216\323\330\231_\245\307\345\266\322\327\342\352-o",
    "s+>\220\265\340\351\326\355,\341\267\324\336\343\353=<x",
};
f32 PAUSEPANELZ = 1.15f;
f32 PAUSEPANELX = 0.605f;
f32 PAUSENAMEY = 0.81f;
f32 PAUSEINFOY = 0.1f;
f32 PAUSEPERCENTY = -0.77f;
f32 PAUSEHMUL = 1.0f;
f32 PAUSEVMUL = 1.25f;
f32 TT_MESSAGEY = 0.8f;
f32 TT_TIMESDY = -0.2f;
f32 TT_INPUTY = -0.6f;
f32 RUMBOSSBARX = 0.65f;
f32 RUMBOSSBARY = -0.85f;
f32 RUMBOSSBARZ = 1.0f;
f32 RUMBOSSBARSCALEX = -0.1f;
f32 RUMBOSSBARSCALEY = 0.1f;
f32 ATMBOSSBARX = 0.65f;
f32 ATMBOSSBARY = -0.85f;
f32 ATMBOSSBARZ = 1.0f;
f32 ATMBOSSBARSCALEX = -0.1f;
f32 ATMBOSSBARSCALEY = 0.1f;
f32 FIREBOSSBARX = 0.65f;
f32 FIREBOSSBARY = -0.85f;
f32 FIREBOSSBARZ = 1.0f;
f32 FIREBOSSBARSCALEX = -0.1f;
f32 FIREBOSSBARSCALEY = 0.1f;
f32 PANELRINGX = 0.8f;
f32 PANELRINGY = -0.8f;
f32 PANELRINGSIZE = 0.05f;


extern void NuCameraSet(struct nucamera_s *cam);
extern void NuStrCpy(char *dst, char *src);
extern void DrawMenu(struct cursor_s *cursor, s32 paused);
extern char *MakeEditText(char *txt);
extern void BossBar(f32 x, f32 y, f32 z, f32 xs, f32 ys, s32 health,
                    s32 maxhealth);
extern s32 GetGliderHealthPercentage(struct creature_s *c);
extern s32 GetRumblePlayerHealthPercentage();
extern void RumbleHeadUpDisplay(void);
extern void DrawPanel3DArrow(void);
extern s32 GetCurrentLevelObjectives(void);
extern s32 GetMaxLevelObjectives(void);
extern s32 bonus_crates_destroyed;
extern s32 save_bonus_crates_destroyed;
extern s32 Bonus;
extern s32 bonus_finish_frame;
extern f32 bonus_crates_wait;
extern struct panelcount_s plr_crates;
extern struct panelcount_s plr_lives;
extern struct ldata_s *LDATA;
extern u16 panel_head_xrot;
extern u16 panel_head_yrot;
extern s32 bonus_lives;
extern void DrawPanelDebris(void);

void DrawPanel(void) {
    struct creature_s *plr;
    s32 paused;
    s32 i;
    s32 j;
    s32 frame;
    s32 t;
    u16 yrot;
    u16 a;
    short wobble_x;
    short wobble_y;
    float f;
    float xs;
    float ys;
    float y;
    float scale;
    float size;
    char txt[32];

    plr = (PLAYERCOUNT != 0) ? player : 0;

    wobble_x = NuTrigTable[(((GlobalTimer.frame % NW_HZ(250, 300)) << 16) / NW_HZ(250, 300)) & 0xFFFF] *
               2731.0f + 1820.0f;
    wobble_y = NuTrigTable[((((GlobalTimer.frame % NW_HZ(200, 240)) << 16) / NW_HZ(200, 240)) + 0x4000) & 0xFFFF] *
               2731.0f;

    PANEL3DMULX = (float)SWIDTH / (DIVPANEL3D * ((float)SWIDTH / 640.0f));
    PANEL3DMULY = (float)SHEIGHT / (DIVPANEL3D * ((float)SHEIGHT / 448.0f));

    if (editor_active != 0) {
        return;
    }

    if (screendump != 0) {
        paused = save_paused;
    } else {
        paused = Paused;
    }

    if ((paused == 0) && ((LDATA->flags & 1) != 0) && (PANELOFF != 0)) {
        return;
    }

    yrot = -(((GlobalTimer.frame % NW_HZ(100, 120)) << 16) / NW_HZ(100, 120));

    if ((pause_rndr_on == 0) && (cutmovie == -1)) {
        Draw3DCheckpointLetters();
    }

    MaxVP();

    if ((ShowPlayerCoordinate != 0) && (plr != 0)) {
        NuCameraTransformScreenClip(&vTEMP, &plr->obj.pos, 1, 0);
    }

    pNuCam->mtx = numtx_identity;
    NuCameraSet(pNuCam);

    if ((Pad[0] != 0) && !Pad[0]->valid && (Demo == 0) && (Cursor.menu != 0) &&
        (Cursor.menu != 0x20) && (GameMode != 1) && (cutmovie == -1)) {
        Text3D(tINSERTCONTROLLERINPORT1[Game.language][0], 0.0f, 0.0f, 0.5f, 0.3f,
               0.3f, 0.3f, 4, 1);
        Text3D(tINSERTCONTROLLERINPORT1[Game.language][1], 0.0f, 0.0f, 0.5f, 0.3f,
               0.3f, 0.3f, 1, 1);
        for (i = 2; i > 0; i--) {
            DrawPanel3DObject(0x81, 0.0f, 0.0f, 0.75f, 1.0f, 0.125f, 0.09f, 0xC000, 0,
                              0, ObjTab[0x81].obj.scene, ObjTab[0x81].obj.special, 0);
        }
    }

    DrawMenu(&Cursor, paused);

    if ((Cursor.menu == 0x11) || (cutmovie != -1)) {
        goto exit;
    }

    if (Level == 0x23) {
    } else if ((Level == 0x25) && (Hub != -1) && (paused == 0) &&
               (hubleveltext_pos > 0.0f) && (hubleveltext_level != -1)) {
        f = (0.333f - hubleveltext_pos) / 0.333f;
        if (hubleveltext_open != 0) {
            Text3D(LevelName[hubleveltext_level][Game.language], 0.0f, f * 0.4f + 0.8f,
                   1.0f, 0.6f, 0.6f, 0.6f, 0, 0);
        }
        if ((u32)hubleveltext_i < 5) {
            if (hubleveltext_open != 0) {
                if (hubleveltext_i == 2) {
                    xs = 0.0f;
                    ys = -0.7f - f * 0.5f;
                } else {
                    ys = -0.7f;
                    if (hubleveltext_i > 2) {
                        xs = -0.6f - f * 0.8f;
                    } else {
                        xs = f * 0.8f + 0.6f;
                    }
                }
                DrawTimeTrialTimes(hubleveltext_level, xs, ys);
            } else if (Hub == 5) {
                sprintf(tbuf, tRELICSREQUIRED[Game.language], (hubleveltext_i + 1) * 5);
                if (Game.language == 0x63) {
                    AddSpacesIntoText(tbuf, 2);
                }
                Text3D(tbuf, 0.0f, f * 0.3f + 0.85f, 1.0f, 0.6f, 0.6f, 0.6f, 0, 3);
            }
        }
    }

    if ((LDATA->flags & 1) == 0) {
        goto exit;
    }

    if (plr != 0) {
        if (ShowPlayerCoordinate != 0) {
            sprintf(tbuf, "X:%.2f Y:%.2f Z:%.2f", plr->obj.pos.x, plr->obj.pos.y, plr->obj.pos.z);
            if (Game.language == 0x63) {
                AddSpacesIntoText(tbuf, 1);
            }
            Text3D(tbuf, vTEMP.x, vTEMP.y, 1.0f, 0.4f, 0.5f, 0.5f, 0, 3);
        }
        if ((InvincibilityCHEAT != 0) && ((GlobalTimer.frame % NW_HZ(25, 30)) < NW_HZ(12, 15))) {
            Text3D((Game.language == 0x63) ? "I N V I N C I B L E " : "INVINCIBLE", 0.0f,
                   1.0f, 1.0f, 0.4f, 0.4f, 0.4f, 1, 4);
        }
    }

    if (paused != 0) {
        if ((Cursor.menu >= 3) && (Cursor.menu <= 8)) {
            ys = 0.0f;
            if (pause_dir != 0) {
                f = (NW_HZ(25, 30) - paused) * (PAUSEVMUL / NW_HZ(25.0f, 30.0f));
                ys -= f;
            }
            DrawPanel3DObject(99, 0.0f, ys, PAUSEPANELZ, 0.1f, 0.1f, f * 0.1f, 0, 0, 0,
                              ObjTab[99].obj.scene, ObjTab[99].obj.special, 1);
            xs = 0.0f;
            if (pause_dir != 0) {
                f = (NW_HZ(25, 30) - paused) * (PAUSEHMUL / NW_HZ(25.0f, 30.0f));
                xs += f;
            }
            DrawPanel3DObject(98, xs, 0.0f, PAUSEPANELZ, 0.1f, 0.1f, f * 0.1f, 0, 0, 0,
                              ObjTab[98].obj.scene, ObjTab[98].obj.special, 1);
            if (paused == NW_HZ(25, 30)) {
                Text3D(Game.name, PAUSEPANELX, PAUSENAMEY, 1.0f, 0.6f, 0.6f, 0.6f, 0, 0);
                sprintf(tbuf, "%i%%", Game.percent);
                if (Game.language == 0x63) {
                    AddSpacesIntoText(tbuf, 1);
                }
                Text3D(tbuf, PAUSEPANELX, PAUSEPERCENTY - (NW_HZ(25, 30) - paused), 1.0f, 1.0f,
                       1.0f, 1.0f, 0, 0);
                if (LDATA->hub != -1) {
                    j = 5;
                } else {
                    j = 4;
                }
                i = (pausestats_frame % (j * NW_HZ(150U, 180U))) / NW_HZ(150U, 180U);
                xs = PAUSEPANELX;
                ys = PAUSEINFOY;
                switch (i) {
                    case 0:
                        Text3D(tCRYSTALS[Game.language], xs, ys + 0.35f, 1.0f, 0.6f,
                               0.6f, 0.6f, 0, 0);
                        BigOutOf(xs - 0.1f, ys - 0.125f, 0.6f, Game.crystals, 25);
                        DrawPanel3DCharacter(0x75, xs + 0.15f, ys - 0.175f, 1.0f, 0.2f,
                                             0.2f, 0.2f, 0, yrot, 0, -1, 1.0f, 1);
                        break;
                    case 1:
                        Text3D(tPOWERS[Game.language], xs, ys + 0.35f, 1.0f, 0.6f, 0.6f,
                               0.6f, 0, 0);
                        if (Game.language == 0x63) {
                            tbuf[1] = ' ';
                            tbuf[2] = 0;
                        } else {
                            tbuf[1] = 0;
                        }
                        if ((ExtraMoves != 0) || ((Game.powerbits & 1) != 0)) {
                            tbuf[0] = PData[0].font3d_letter;
                            Text3D(tbuf, xs - 0.16f, ys + 0.05f, 1.0f, 1.4f, 1.4f, 1.4f,
                                   0, 0);
                        }
                        if ((ExtraMoves != 0) || ((Game.powerbits & 2) != 0)) {
                            tbuf[0] = PData[1].font3d_letter;
                            Text3D(tbuf, xs, ys + 0.05f, 1.0f, 1.4f, 1.4f, 1.4f, 0, 0);
                        }
                        if ((ExtraMoves != 0) || ((Game.powerbits & 4) != 0)) {
                            tbuf[0] = PData[2].font3d_letter;
                            Text3D(tbuf, xs + 0.16f, ys + 0.05f, 1.0f, 1.4f, 1.4f, 1.4f,
                                   0, 0);
                        }
                        if ((ExtraMoves != 0) || ((Game.powerbits & 8) != 0)) {
                            tbuf[0] = PData[3].font3d_letter;
                            Text3D(tbuf, xs - 0.16f, ys - 0.2f, 1.0f, 1.4f, 1.4f, 1.4f,
                                   0, 0);
                        }
                        if ((ExtraMoves != 0) || ((Game.powerbits & 0x10) != 0)) {
                            tbuf[0] = PData[4].font3d_letter;
                            Text3D(tbuf, xs, ys - 0.2f, 1.0f, 1.4f, 1.4f, 1.4f, 0, 0);
                        }
                        if ((ExtraMoves != 0) || ((Game.powerbits & 0x20) != 0)) {
                            tbuf[0] = PData[5].font3d_letter;
                            Text3D(tbuf, xs + 0.16f, ys - 0.2f, 1.0f, 1.4f, 1.4f, 1.4f,
                                   0, 0);
                        }
                        break;
                    case 2:
                        Text3D(tGEMS[Game.language], xs, ys + 0.35f, 1.0f, 0.6f, 0.6f,
                               0.6f, 0, 0);
                        f = 0.0333f;
                        if ((Game.gembits & 2) != 0) {
                            f = 0.1f;
                        }
                        DrawPanel3DCharacter(0x7A, xs - 0.08f, ys + 0.2f, 1.0f, f,
                                             f, f, 0, yrot, 0, -1, 1.0f, 1);
                        f = 0.0333f;
                        if ((Game.gembits & 4) != 0) {
                            f = 0.1f;
                        }
                        DrawPanel3DCharacter(0x7B, xs + 0.08f, ys + 0.2f, 1.0f, f,
                                             f, f, 0, yrot, 0, -1, 1.0f, 1);
                        f = 0.0333f;
                        if ((Game.gembits & 1) != 0) {
                            f = 0.1f;
                        }
                        DrawPanel3DCharacter(0x79, xs - 0.15f, ys + 0.06f, 1.0f, f,
                                             f, f, 0, yrot, 0, -1, 1.0f, 1);
                        f = 0.0333f;
                        if ((Game.gembits & 0x10) != 0) {
                            f = 0.1f;
                        }
                        DrawPanel3DCharacter(0x7D, xs, ys + 0.06f, 1.0f, f, f,
                                             f, 0, yrot, 0, -1, 1.0f, 1);
                        f = 0.0333f;
                        if ((Game.gembits & 8) != 0) {
                            f = 0.1f;
                        }
                        DrawPanel3DCharacter(0x7C, xs + 0.15f, ys + 0.06f, 1.0f, f,
                                             f, f, 0, yrot, 0, -1, 1.0f, 1);
                        sprintf(tbuf, "+%i %c =", Game.crate_gems + Game.bonus_gems,
                                (u8)CLetter[119]);
                        if (Game.language == 0x63) {
                            AddSpacesIntoText(tbuf, 1);
                        }
                        Text3D(tbuf, xs, 0.0f, 1.0f, 0.6f, 0.6f, 0.6f, 0, 0);
                        BigOutOf(xs - 0.1f, ys - 0.3f, 0.6f, Game.gems, 46);
                        DrawPanel3DCharacter(0x78, xs + 0.15f, ys - 0.275f, 1.0f, 0.175f,
                                             0.175f, 0.175f, 0, yrot, 0, -1, 1.0f, 1);
                        break;
                    case 3:
                        DrawPanel3DObject(3, xs - 0.15f, ys + 0.12f, 1.0f, 0.08f, 0.08f,
                                          0.08f, 0, 0, 0, ObjTab[3].obj.scene,
                                          ObjTab[3].obj.special, 1);
                        sprintf(tbuf, "%i", sapphire_relics);
                        if (Game.language == 0x63) {
                            AddSpacesIntoText(tbuf, 1);
                        }
                        Text3D(tbuf, xs - 0.15f, ys - 0.03f, 1.0f, 0.6f, 0.6f, 0.6f, 0,
                               0);
                        DrawPanel3DObject(2, xs, ys + 0.12f, 1.0f, 0.08f, 0.08f, 0.08f,
                                          0, 0, 0, ObjTab[2].obj.scene,
                                          ObjTab[2].obj.special, 1);
                        sprintf(tbuf, "%i", gold_relics);
                        if (Game.language == 0x63) {
                            AddSpacesIntoText(tbuf, 1);
                        }
                        Text3D(tbuf, xs, ys - 0.03f, 1.0f, 0.6f, 0.6f, 0.6f, 0, 0);
                        DrawPanel3DObject(1, xs + 0.15f, ys + 0.12f, 1.0f, 0.08f, 0.08f,
                                          0.08f, 0, 0, 0, ObjTab[1].obj.scene,
                                          ObjTab[1].obj.special, 1);
                        sprintf(tbuf, "%i", platinum_relics);
                        if (Game.language == 0x63) {
                            AddSpacesIntoText(tbuf, 1);
                        }
                        Text3D(tbuf, xs + 0.15f, ys - 0.03f, 1.0f, 0.6f, 0.6f, 0.6f, 0,
                               0);
                        Text3D(tRELICS[Game.language], xs, ys + 0.35f, 1.0f, 0.6f, 0.6f,
                               0.6f, 0, 0);
                        BigOutOf(xs - 0.1f, ys - 0.3f, 0.6f, Game.relics, 30);
                        DrawPanel3DObject(3, xs + 0.15f, ys - 0.325f, 1.0f, 0.125f,
                                          0.125f, 0.125f, 0, yrot, 0, ObjTab[3].obj.scene,
                                          ObjTab[3].obj.special, 1);
                        break;
                    case 4:
                        Text3D(tTIMES[Game.language], xs, ys + 0.35f, 1.0f, 0.6f, 0.6f,
                               0.6f, 0, 0);
                        DrawTimeTrialTimes(Level, xs, ys - 0.1f);
                        break;
                }
            }
        }
        goto exit;
    }

    if (plr != 0) {
        i = 0;
        if ((plr_bonus_wumpas.count != 0) || (bonus_wumpa_delay != 0) ||
            (bonus_wumpa_wait > 0.0f)) {
            i = 1;
        }
        if (((Bonus == 2) && ((plr->obj.dead == 0) || (i != 0))) ||
            (((Bonus == 3) || (Bonus == 4)) && (i != 0))) {
            if (bonus_panel_wait > 0.0f) {
                f = bonus_panel_wait;
            } else if (bonus_wumpa_wait > 0.0f) {
                f = 0.5f - bonus_wumpa_wait;
            } else {
                f = 0.0f;
            }
            if (f > 0.0f) {
                ys = BONUSPANELSY + (-1.2f - BONUSPANELSY) * f * 2.0f;
            } else {
                ys = BONUSPANELSY;
            }
            sprintf(tbuf, "%i", plr_bonus_wumpas.draw);
            if (Game.language == 0x63) {
                AddSpacesIntoText(tbuf, 1);
            }
            Text3D(tbuf, BONUSWUMPATXTSX, ys, 1.0f, 0.6f, 0.6f, 0.6f, 2, 1);
            DrawPanel3DObject(0, BONUSWUMPAOBJSX, ys, 1.0f, 0.125f, 0.125f, 0.125f, 0,
                              PANELWUMPAYROT, 0, ObjTab[0].obj.scene,
                              ObjTab[0].obj.special, 1);
        }

        i = 0;
        if ((bonus_finish_frame < save_bonus_crates_destroyed * NW_HZ(5, 6) + NW_HZ(5, 6)) ||
            (bonus_crates_wait > 0.0f)) {
            i = 1;
        }
        if (((Bonus == 2) && ((plr->obj.dead == 0) || (i != 0))) ||
            (((Bonus == 3) || (Bonus == 4)) && (i != 0))) {
            if (bonus_panel_wait > 0.0f) {
                f = bonus_panel_wait;
            } else if (bonus_crates_wait > 0.0f) {
                f = 0.5f - bonus_crates_wait;
            } else {
                f = 0.0f;
            }
            if (f > 0.0f) {
                ys = BONUSPANELSY + (-1.2f - BONUSPANELSY) * f * 2.0f;
            } else {
                ys = BONUSPANELSY;
            }
            sprintf(tbuf, "%i/%i", bonus_crates_destroyed,
                    DESTRUCTIBLEBONUSCRATECOUNT);
            if (Game.language == 0x63) {
                AddSpacesIntoText(tbuf, 1);
            }
            Text3D(tbuf, BONUSCRATETXTSX, ys, 1.0f, 0.6f, 0.6f, 0.6f, 2, 2);
            DrawPanel3DObject(-1, BONUSCRATEOBJSX, ys, 1.0f, 0.09375f, 0.09375f,
                              0.09375f, 0, PANELCRATEYROT, 0, crate_scene,
                              crate_list[5].obj.special, 1);
        }

        i = 0;
        if ((bonus_lives != 0) || (bonus_life_delay != 0) || (bonus_lives_wait > 0.0f)) {
            i = 1;
        }
        if (((Bonus == 2) && ((plr->obj.dead == 0) || (i != 0))) ||
            (((Bonus == 3) || (Bonus == 4)) && (i != 0))) {
            if (bonus_panel_wait > 0.0f) {
                f = bonus_panel_wait;
            } else if (bonus_lives_wait > 0.0f) {
                f = 0.5f - bonus_lives_wait;
            } else {
                f = 0.0f;
            }
            if (f > 0.0f) {
                ys = BONUSPANELSY + (-1.2f - BONUSPANELSY) * f * 2.0f;
            } else {
                ys = BONUSPANELSY;
            }
            sprintf(tbuf, "%i", bonus_lives);
            if (Game.language == 0x63) {
                AddSpacesIntoText(tbuf, 1);
            }
            Text3D(tbuf, BONUSLIVESTXTSX, ys, 1.0f, 0.6f, 0.6f, 0.6f, 2, 0);
            i = 0x11;
            if (LDATA->character == 1) {
                i = 0x12;
            }
            DrawPanel3DObject(i, BONUSLIVESOBJSX, ys, 1.0f, BONUSLIFESCALE,
                              BONUSLIFESCALE, BONUSLIFESCALE, panel_head_xrot,
                              panel_head_yrot + 0x2000, 0, ObjTab[i].obj.scene,
                              ObjTab[i].obj.special, 1);
        }

        if ((Bonus == 2) && (plr->obj.dead == 0)) {
            DrawGameMessage(tBONUS[Game.language], bonus_frame, 0.5f);
        }
        if (Demo != 0) {
            DrawGameMessage(tDEMO[Game.language], GameTimer.frame, 0.5f);
        }

        if (plr != 0) {
            if (plr_wumpas.frame != 0) {
                f = PANELSY + (NW_HZ(25, 30) - plr_wumpas.frame) * NW_HZ(0.04f, 0.033333335f) * 0.4f;
                DrawPanel3DObject(0, WUMPAOBJSX, f, 1.0f, 0.2f, 0.2f, 0.2f, 0xE800,
                                  PANELWUMPAYROT, 0, ObjTab[0].obj.scene,
                                  ObjTab[0].obj.special, 1);
                sprintf(tbuf, "%i", plr_wumpas.draw);
                if (Game.language == 0x63) {
                    AddSpacesIntoText(tbuf, 1);
                }
                Text3D(tbuf, WUMPATXTSX, f, 1.0f, 1.2f, 1.2f, 1.2f, 2, 1);
            }
            if ((plr_crates.frame != 0) && (LDATA->hub != -1) &&
                ((LDATA->flags & 2) == 0)) {
                f = PANELSY + (NW_HZ(25, 30) - plr_crates.frame) * NW_HZ(0.04f, 0.033333335f) * 0.4f;
                DrawPanel3DObject(-1, CRATEOBJSX, f, 1.0f, 0.15f, 0.15f, 0.15f, 0xE800,
                                  PANELCRATEYROT, 0, crate_scene,
                                  crate_list[5].obj.special, 1);
                sprintf(tbuf, "%i", plr_crates.draw);
                if (Game.language == 0x63) {
                    AddSpacesIntoText(tbuf, 1);
                }
                Text3D(tbuf, CRATETXTSX, f, 1.0f, 1.2f, 1.2f, 1.2f, 2, 2);
                sprintf(tbuf, "/%i", DESTRUCTIBLECRATECOUNT);
                if (Game.language == 0x63) {
                    AddSpacesIntoText(tbuf, 1);
                }
                Text3D(tbuf, font3d_xright, font3d_ybottom, 1.0f, 0.6f, 0.6f, 0.6f, 6,
                       2);
            }
            if (plr_lives.frame != 0) {
                f = PANELSY + (NW_HZ(25, 30) - plr_lives.frame) * NW_HZ(0.04f, 0.033333335f) * 0.4f;
                i = 0x11;
                if (LDATA->character == 1) {
                    i = 0x12;
                }
                DrawPanel3DObject(i, LIVESOBJSX, f, 1.0f, LIFESCALE, LIFESCALE,
                                  LIFESCALE, panel_head_xrot - 0x1000,
                                  panel_head_yrot + 0x2000, 0, ObjTab[i].obj.scene,
                                  ObjTab[i].obj.special, 1);
                sprintf(tbuf, "%i", plr_lives.draw);
                if (Game.language == 0x63) {
                    AddSpacesIntoText(tbuf, 1);
                }
                Text3D(tbuf, LIVESTXTSX, f, 1.0f, 1.2f, 1.2f, 1.2f, 2, 0);
            }

            a = ((GameTimer.frame % NW_HZ(100, 120)) << 16) / NW_HZ(100, 120);

            if ((plr_crystal.count != 0) && (plr_crystal.frame != 0) &&
                (LDATA->hub != -1)) {
                f = -0.7f - (NW_HZ(25, 30) - plr_crystal.frame) * NW_HZ(0.04f, 0.033333335f) * 0.6f;
                DrawPanel3DCharacter(0x75, 0.0f, f, 1.0f, 0.15f, 0.15f, 0.15f, 0, a, 0,
                                     -1, 0.0f, 1);
            }
            if ((plr_crategem.count != 0) && (plr_crategem.frame != 0) &&
                (LDATA->hub != -1)) {
                f = -0.7f - (NW_HZ(25, 30) - plr_crategem.frame) * NW_HZ(0.04f, 0.033333335f) * 0.6f;
                DrawPanel3DCharacter(0x77, -0.2f, f, 1.0f, 0.15f, 0.15f, 0.15f, 0, a, 0,
                                     -1, 0.0f, 1);
            }
            if ((plr_bonusgem.count != 0) && (plr_bonusgem.frame != 0) &&
                (LDATA->hub != -1)) {
                f = -0.7f - (NW_HZ(25, 30) - plr_bonusgem.frame) * NW_HZ(0.04f, 0.033333335f) * 0.6f;
                if (plr_bonusgem.item == 8) {
                    i = 0x79;
                } else if (plr_bonusgem.item == 0x20) {
                    i = 0x7A;
                } else if (plr_bonusgem.item == 0x10) {
                    i = 0x7B;
                } else if (plr_bonusgem.item == 0x40) {
                    i = 0x7C;
                } else {
                    i = (plr_bonusgem.item == 0x80) ? 0x7D : 0x78;
                }
                DrawPanel3DCharacter(i, 0.2f, f, 1.0f, 0.15f, 0.15f, 0.15f, 0, a, 0, -1,
                                     0.0f, 1);
            }

            DrawWorldToPanelWumpa();

            for (i = 0; i < 32; i++) {
                if ((NewWumpa[i].active != 0) && (NewWumpa[i].count != 0)) {
                    DrawPanel3DObject(0, NewWumpa[i].screen.x, NewWumpa[i].screen.y, 1.0f,
                                      NewWumpa[i].scale, NewWumpa[i].scale,
                                      NewWumpa[i].scale,
                                      (NewWumpa[i].bonus == 0) ? 0xE800 : 0,
                                      PANELWUMPAYROT, 0, ObjTab[0].obj.scene,
                                      ObjTab[0].obj.special, 1);
                }
            }

            if (TimeTrial != 0) {
                switch (plr->obj.finished) {
                    case 1:
                        frame = finish_frame - (finish_frames - NW_HZ(50, 60));
                        if (frame < 0) {
                            frame = 0;
                        }
                        if (newleveltime_slot < 3) {
                            if ((new_lev_flags & 7) != 0) {
                                ys = 0.35f;
                            } else {
                                ys = 0.2f;
                            }
                            i = 3;
                        } else {
                            i = 4;
                            ys = 0.1f;
                        }
                        xs = 0.0f;
                        ys -= (i - 1) * TT_TIMESDY * 0.5f;
                        y = ys + newleveltime_slot * TT_TIMESDY;
                        scale = 0.9f;
                        if (frame < NW_HZ(25, 30)) {
                            if ((frame == 0) || ((frame % NW_HZ(10, 12)) > NW_HZ(4, 5))) {
                                xs = tt_sx;
                                y = tt_sy;
                                scale = TT_SCALE;
                            } else {
                                scale = xs;
                            }
                        } else if (frame < NW_HZ(50, 60)) {
                            f = (frame - NW_HZ(25, 30)) / NW_HZ(25.0f, 30.0f);
                            xs = tt_sx + (xs - tt_sx) * f;
                            y = tt_sy + (y - tt_sy) * f;
                            scale = TT_SCALE + (scale - TT_SCALE) * f;
                        }
                        if (((newleveltime_slot == 3) || (frame < NW_HZ(50, 60))) && (scale > 0.0f)) {
                            MakeTimeI(TimeTrialTimer.itime, 0, txt);
                            if (Game.language == 0x63) {
                                if (finish_frame < finish_frames - NW_HZ(50, 60)) {
                                    sprintf(tbuf, "    %c   %s", (u8)CLetter[118],
                                            txt);
                                } else {
                                    sprintf(tbuf, (newleveltime_slot < 3) ? "_ _ _   %s"
                                                                          : "        %s",
                                            txt);
                                }
                            } else if (finish_frame < finish_frames - NW_HZ(50, 60)) {
                                sprintf(tbuf, "  %c %s", (u8)CLetter[118], txt);
                            } else {
                                sprintf(tbuf, (newleveltime_slot < 3) ? "___ %s" : "    %s",
                                        txt);
                            }
                            Text3D(tbuf, xs, y, 1.0f, scale, scale, scale, 0, 4);
                        }
                        for (i = 0; i < 3; i++) {
                            if (frame < NW_HZ(50, 60)) {
                                return;
                            }
                            MakeLevelTimeString(&Game.level[Level].time[i], tbuf);
                            if (i == newleveltime_slot) {
                                Text3D(MakeEditText(tbuf), xs, y, 1.0f, scale, scale,
                                       scale, 0, 4);
                            } else {
                                Text3D(tbuf, 0.0f, ys, 1.0f, 0.8f, 0.8f, 0.8f, 0, 0);
                            }
                            if (i == newleveltime_slot) {
                                if ((new_lev_flags & 4) != 0) {
                                    j = 1;
                                } else if ((new_lev_flags & 2) != 0) {
                                    j = 2;
                                } else {
                                    j = ((new_lev_flags & 1) != 0) ? 3 : -1;
                                }
                                if (j != -1) {
                                    DrawPanel3DObject(j, TT_RELICX, TT_RELICY, 1.0f, 0.125f,
                                                      0.125f, 0.125f, 0, 0, 0,
                                                      ObjTab[j].obj.scene,
                                                      ObjTab[j].obj.special, 1);
                                }
                            }
                            ys += TT_TIMESDY;
                        }
                        if ((tt_flash != 0) && (frame >= NW_HZ(50, 60)) &&
                            ((Cursor.menu != 0xE) ||
                             ((newleveltime_slot == 3) && (Cursor.menu_frame < NW_HZ(250, 300))))) {
                            switch (newleveltime_slot) {
                                case 0:
                                    NuStrCpy(tbuf, tBESTTIME[Game.language]);
                                    break;
                                case 1:
                                    NuStrCpy(tbuf, t2NDBESTTIME[Game.language]);
                                    break;
                                case 2:
                                    NuStrCpy(tbuf, t3RDBESTTIME[Game.language]);
                                    break;
                                default:
                                    NuStrCpy(tbuf, tNONEWTIME[Game.language]);
                                    break;
                            }
                            Text3D(tbuf, 0.0f, TT_MESSAGEY, 1.0f, 0.8f, 0.8f, 0.8f, 0, 3);
                        }
                        break;
                    case 0:
                        if ((timetrial_frame - NW_HZ(50, 60) < NW_HZ(150, 180)) &&
                            ((timetrial_frame % NW_HZ(50, 60)) < NW_HZ(40, 48))) {
                            Text3D(tENTERINGTIMETRIAL[Game.language], 0.0f, 0.0f, 1.0f,
                                   0.6f, 0.6f, 0.6f, 0, 4);
                        }
                        t = TimeTrialTimer.itime;
                        if ((s32)TimeTrialTimer.itime > 0x2BF1D) {
                            t = 0x2BF1D;
                        }
                        MakeTimeI(t, 0, txt);
                        sprintf(tbuf, (Game.language == 0x63) ? "    %c   %s" : "  %c %s",
                                (u8)CLetter[118], txt);
                        xs = tt_sx;
                        ys = tt_sy;
                        if (timetrial_frame < NW_HZ(37, 45)) {
                            if ((timetrial_frame % NW_HZ(12, 15)) < NW_HZ(6, 7)) {
                                scale = 0.5f;
                                xs = TTScrPos.x;
                                ys = TTScrPos.y;
                            } else {
                                scale = 0.0f;
                            }
                        } else if (timetrial_frame < NW_HZ(50, 60)) {
                            f = (timetrial_frame - NW_HZ(37, 45)) / NW_HZ(12.0f, 15.0f);
                            xs += (TTScrPos.x - xs) * (1.0f - f);
                            ys += (TTScrPos.y - ys) * (1.0f - f);
                            scale = f * 0.5f + 0.5f;
                        } else {
                            scale = 1.0f;
                        }
                        if (scale > 0.0f) {
                            Text3D(tbuf, xs, ys, 1.0f, TT_SCALE * scale, TT_SCALE * scale,
                                   TT_SCALE * scale, 0, (TimeTrialWait > 0.0f) ? 3 : 0);
                        }
                        break;
                }
            }

            if (((LDATA->flags & 0x200) != 0) && (VEHICLECONTROL == 1) &&
                (plr->obj.finished == 0)) {
                i = 0x11;
                if (LDATA->character == 1) {
                    i = 0x12;
                }
                DrawPanel3DObject(i, -0.8f, -0.8f, 1.0f, 0.3f, 0.3f, 0.3f, wobble_x,
                                  wobble_y + 0xE000, 0, ObjTab[i].obj.scene,
                                  ObjTab[i].obj.special, 1);
                sprintf(tbuf, "%.3i%%", GetGliderHealthPercentage(plr));
                if (tbuf[0] == '0') {
                    if (tbuf[1] == '0') {
                        tbuf[0] = tbuf[1] = ' ';
                    } else {
                        tbuf[0] = ' ';
                    }
                }
                if (Game.language == 0x63) {
                    AddSpacesIntoText(tbuf, 1);
                }
                Text3D(tbuf, -0.7f, -0.85f, 1.0f, 0.8f, 0.8f, 0.8f, 2, 0);
                if (Level == 0x18) {
                    BossBar(ATMBOSSBARX, ATMBOSSBARY, ATMBOSSBARZ, ATMBOSSBARSCALEX,
                            ATMBOSSBARSCALEY, GetCurrentLevelObjectives(),
                            GetMaxLevelObjectives());
                } else {
                    i = GetCurrentLevelObjectives();
                    j = GetMaxLevelObjectives();
                    sprintf(tbuf, "%i/%i", j - i, j);
                    if (Game.language == 0x63) {
                        AddSpacesIntoText(tbuf, 1);
                    }
                    Text3D(tbuf, 0.7f, -0.85f, 1.0f, 0.8f, 0.8f, 0.8f, 8, 0);
                }
                if (Level == 0x12) {
                    DrawPanel3DTempCharacter(panel_head_xrot,
                                             panel_head_yrot +
                                                 ((GameTimer.frame % NW_HZ(100, 120)) << 16) / NW_HZ(100, 120),
                                             0, 0, PANELSINKX, PANELSINKY, PANELSINKZ,
                                             PANELSINKSCALE);
                } else if (Level == 0x18) {
                    DrawPanel3DTempCharacter(wobble_x + (s16)PANELCRUNCHXROT, wobble_y + 0x2000,
                                             0, 0, PANELCRUNCHX, PANELCRUNCHY,
                                             PANELCRUNCHZ, PANELCRUNCHSCALE);
                } else {
                    DrawPanel3DTempCharacter(panel_head_xrot,
                                             panel_head_yrot +
                                                 ((GameTimer.frame % NW_HZ(100, 120)) << 16) / NW_HZ(100, 120),
                                             0, 0, PANELBLIMPX, PANELBLIMPY, PANELBLIMPZ,
                                             PANELBLIMPSCALE);
                }
            } else if ((Level == 0x15) && (plr->obj.finished == 0)) {
                DrawPanel3DObject(0x11, -0.8f, -0.8f, 1.0f, 0.3f, 0.3f, 0.3f, wobble_x,
                                  wobble_y + 0xE000, 0, ObjTab[0x11].obj.scene,
                                  ObjTab[0x11].obj.special, 1);
                sprintf(tbuf, "%.3i%%", GetRumblePlayerHealthPercentage(plr));
                if (tbuf[0] == '0') {
                    if (tbuf[1] == '0') {
                        tbuf[0] = tbuf[1] = ' ';
                    } else {
                        tbuf[0] = ' ';
                    }
                }
                if (Game.language == 0x63) {
                    AddSpacesIntoText(tbuf, 1);
                }
                Text3D(tbuf, -0.7f, -0.85f, 1.0f, 0.8f, 0.8f, 0.8f, 2, 0);
                RumbleHeadUpDisplay();
                BossBar(RUMBOSSBARX, RUMBOSSBARY, RUMBOSSBARZ, RUMBOSSBARSCALEX,
                        RUMBOSSBARSCALEY, GetCurrentLevelObjectives(),
                        GetMaxLevelObjectives());
                DrawPanel3DTempCharacter(wobble_x + (s16)PANELCRUNCHXROT, wobble_y + 0x2000, 0, 0,
                                         PANELCRUNCHX, PANELCRUNCHY, PANELCRUNCHZ,
                                         PANELCRUNCHSCALE);
            } else if ((Level == 0x16) && (plr->obj.finished == 0)) {
                BossBar(FIREBOSSBARX, FIREBOSSBARY, FIREBOSSBARZ, FIREBOSSBARSCALEX,
                        FIREBOSSBARSCALEY, GetCurrentLevelObjectives(),
                        GetMaxLevelObjectives());
                DrawPanel3DTempCharacter(wobble_x + (s16)PANELCRUNCHXROT, -wobble_y + 0x2000, 0, 0,
                                         PANELCRUNCHX, PANELCRUNCHY, PANELCRUNCHZ,
                                         PANELCRUNCHSCALE);
            } else if ((Level == 3) && (plr->obj.finished == 0)) {
                if (SmokeyCountDownValue > 0) {
                    tbuf[0] = (char)SmokeyCountDownValue + '0';
                    if (Game.language == 0x63) {
                        tbuf[1] = ' ';
                        tbuf[2] = 0;
                    } else {
                        tbuf[1] = 0;
                    }
                    Text3D(tbuf, 0.0f, 0.0f, 1.0f, 1.5f, 1.5f, 1.5f, 0, 0);
                } else if (SmokeyCountDownValue == -1) {
                    size = 2.5f;
                    if (Game.language == 4) {
                        size = 2.25f;
                    }
                    Text3D(tGO[Game.language], 0.0f, 0.0f, 1.0f, size, size, size, 0, 4);
                }
                if (in_finish_range != 0) {
                    goto debris;
                }
                i = 0x11;
                if (LDATA->character == 1) {
                    i = 0x12;
                }
                DrawPanel3DObject(i, -0.8f, -0.8f, 1.0f, 0.3f, 0.3f, 0.3f,
                                  panel_head_xrot, panel_head_yrot + 0xE000, 0,
                                  ObjTab[i].obj.scene, ObjTab[i].obj.special, 1);
                Text3D(PlaceName[SmokeyPosition - 1][Game.language], -0.7f, -0.85f, 1.0f,
                       0.8f, 0.8f, 0.8f, 2, (SmokeyPosition == 1) ? 4 : 0);
            } else if (Level == 0x1C) {
                if (GameTimer.frame < NW_HZ(150, 180)) {
                    tbuf[0] = '3' - GameTimer.frame / NW_HZ(50, 60);
                    if (Game.language == 0x63) {
                        tbuf[1] = ' ';
                        tbuf[2] = 0;
                    } else {
                        tbuf[1] = 0;
                    }
                    i = 0;
                    f = 1.5f;
                } else if (GameTimer.frame < NW_HZ(200, 240)) {
                    NuStrCpy(tbuf, tGO[Game.language]);
                    f = 2.5f;
                    if (Game.language == 4) {
                        f = 2.25f;
                    }
                    i = 4;
                } else {
                    tbuf[0] = 0;
                }
                if (tbuf[0] != 0) {
                    Text3D(tbuf, 0.0f, 0.0f, 1.0f, f, f, f, 0, i);
                }
                if (in_finish_range != 0) {
                    goto debris;
                }
                DrawPanel3DObject(0x11, -0.8f, -0.8f, 1.0f, 0.3f, 0.3f, 0.3f,
                                  panel_head_xrot, panel_head_yrot + 0xE000, 0,
                                  ObjTab[0x11].obj.scene, ObjTab[0x11].obj.special, 1);
                Text3D(PlaceName[PlrTub.place - 1][Game.language], -0.7f, -0.85f, 1.0f,
                       0.8f, 0.8f, 0.8f, 2, (PlrTub.place == 1) ? 4 : 0);
                i = (PlrTub.laps + 1 > MAXLAPS) ? MAXLAPS : PlrTub.laps + 1;
                sprintf(tbuf, "%i/%i", i, MAXLAPS);
                if (Game.language == 0x63) {
                    AddSpacesIntoText(tbuf, 1);
                }
                Text3D(tbuf, 0.7f, -0.85f, 1.0f, 0.8f, 0.8f, 0.8f, 8, (i > 0) ? 4 : 0);
            } else if (Level == 0x1D) {
                if (GameTimer.frame < NW_HZ(150, 180)) {
                    f = 1.5f;
                    tbuf[0] = '3' - GameTimer.frame / NW_HZ(50, 60);
                    tbuf[1] = 0;
                    i = 0;
                } else if (GameTimer.frame < NW_HZ(200, 240)) {
                    NuStrCpy(tbuf, tGO[Game.language]);
                    f = 2.5f;
                    if (Game.language == 4) {
                        f = 2.25f;
                    }
                    i = 4;
                } else {
                    tbuf[0] = 0;
                }
                if (tbuf[0] != 0) {
                    Text3D(tbuf, 0.0f, 0.0f, 1.0f, f, f, f, 0, i);
                }
                if (in_finish_range != 0) {
                    goto debris;
                }
                if (i_ring > 0) {
                    f = (NW_HZ(25, 30) - carpet_panelpos) / NW_HZ(25.0f, 30.0f) * 0.4f;
                    DrawPanel3DObject(0x11, -0.8f, -0.8f - f, 1.0f, 0.3f, 0.3f, 0.3f,
                                      panel_head_xrot, panel_head_yrot + 0xE000, 0,
                                      ObjTab[0x11].obj.scene, ObjTab[0x11].obj.special, 1);
                    Text3D((carpet_place == 1) ? PlaceName[1][Game.language]
                                               : PlaceName[0][Game.language],
                           -0.7f, -0.85f - f, 1.0f, 0.8f, 0.8f, 0.8f, 2,
                           (carpet_place != 1) ? 4 : 0);
                }
                sprintf(tbuf, "%i/%i", i_ring, RINGCOUNT);
                if (Game.language == 0x63) {
                    AddSpacesIntoText(tbuf, 1);
                }
                Text3D(tbuf, 0.7f, -0.85f, 1.0f, 0.8f, 0.8f, 0.8f, 8, (i_ring > 0) ? 4 : 0);
                DrawPanel3DObject(0x68, PANELRINGX, PANELRINGY, 1.0f, PANELRINGSIZE,
                                  PANELRINGSIZE, PANELRINGSIZE, 0, 0, 0,
                                  ObjTab[0x68].obj.scene, ObjTab[0x68].obj.special, 1);
                if (ObjTab[0x6C].obj.special != 0) {
                    DrawPanel3DArrow();
                }
            }

            if ((in_finish_range == 0) && (TimeTrial == 0) && (GATECOUNT != 0) &&
                (plr_gates != 0)) {
                sprintf(tbuf, "%i/%i", plr_gates, GATECOUNT);
                if (Game.language == 0x63) {
                    AddSpacesIntoText(tbuf, 1);
                }
                Text3D(tbuf, -0.9f, -0.85f, 1.0f, 0.8f, 0.8f, 0.8f, 2, 4);
            }
        }
    }
debris:
    DrawPanelDebris();
exit:
    pNuCam->mtx = pCam->m;
    NuCameraSet(pNuCam);
    GameVP();
}


s32 i_pdeb = 0;
s32 bonus_frame = 0;
struct panelcount_s plr_wumpas = { 0 };
struct panelcount_s plr_crates = { 0 };
struct panelcount_s plr_lives = { 0 };
struct panelcount_s plr_crystal = { 0 };
struct panelcount_s plr_crategem = { 0 };
struct panelcount_s plr_bonusgem = { 0 };
struct panelcount_s plr_bonus_wumpas = { 0 };
f32 PANEL3DMULX = 0.0f;
f32 PANEL3DMULY = 0.0f;
u16 panel_head_xrot = 0;
u16 panel_head_yrot = 0;
s32 MAXVPSIZEX = 0;
s32 MAXVPSIZEY = 0;
s32 MINVPSIZEX = 0;
s32 MINVPSIZEY = 0;
s32 old_bonus_crates = 0;

/* --- UpdatePlayerStats externs ------------------------------------- */

extern struct cratecube_s Crate[];
extern s32 CRATECOUNT;
extern int crate_wumpa;
extern s32 crates_destroyed;
extern s32 mask_crates;
extern s32 newmask_advise;
extern struct nuvec_s vNEWMASK;

extern void AddPanelDebris(f32 x, f32 y, s32 type, f32 scale, s32 count);
extern void NewMask(MaskInfo *mask, struct nuvec_s *pos);
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
            x = bonus_finish_frame / NW_HZ(5, 6);
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
        if (bonus_finish_frame == save_bonus_crates_destroyed * NW_HZ(5, 6) + NW_HZ(4, 5)) {
            bonus_crates_wait = 0.5f;
        } else if ((bonus_finish_frame >= save_bonus_crates_destroyed * NW_HZ(5, 6) + NW_HZ(5, 6)) &&
                   (bonus_crates_wait -= NW_HZ(0.02f, 0.016666668f),
                    bonus_crates_wait <= 0.0f)) {
            bonus_crates_wait = -NW_HZ(0.02f, 0.016666668f);
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
        force_panel_lives_update = NW_HZ(0x32, 0x3C);
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
                        deb->mom.x = qrand() * 1.5259022e-05f * 0.5f * NW_HZ(0.02f, 0.016666668f);
                        deb->mom.y = (qrand() * 1.5259022e-05f + 2.0f) * NW_HZ(0.02f, 0.016666668f);
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
                        f = qrand() * 1.5259022e-05f * NW_HZ(0.02f, 0.016666668f);
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
            deb->time += NW_HZ(0.02f, 0.016666668f);
            if (deb->time < deb->info->duration) {
                tmul = deb->time / deb->info->duration;
                switch (deb->type) {
                    case 0:
                        deb->pos.x = BONUSWUMPAOBJSX +
                                     (WUMPAOBJSX - BONUSWUMPAOBJSX) * 0.9f * tmul;
                        deb->pos.y = BONUSPANELSY +
                                     (PANELSY - BONUSPANELSY) * 0.9f * tmul;
                        deb->xrot = tmul * -5529.5996f;
                        deb->yrot = -(((GameTimer.frame % NW_HZ(33, 40)) << 16) / NW_HZ(33, 40));
                        deb->scale = tmul * 0.075f + 0.125f;
                        break;
                    case 1:
                        deb->pos.x += deb->mom.x;
                        deb->mom.y += deb->info->gravity;
                        deb->pos.y += deb->mom.y;
                        deb->xrot = 0;
                        deb->yrot = -(((GameTimer.frame % NW_HZ(33, 40)) << 16) / NW_HZ(33, 40));
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
                            if ((deb->time - NW_HZ(0.02f, 0.016666668f)) < 0.25f) {
                                force_panel_lives_update = NW_HZ(50, 60);
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
