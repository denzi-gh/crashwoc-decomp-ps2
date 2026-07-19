/*
 * Unit: game/font3d
 *
 * Functions:
 *   0x00237d58 RemapAccentedCharacter
 *   0x00237ec8 Reset3DFontObjects
 *   0x00237ff0 Update3DFontObjects
 *   0x002380c8 InitFont3D
 *   0x00238280 Text3D
 *   0x00238b48 CombinationCharacterBD
 *   0x00238ba0 CombinationCharacterBC
 */

#include "creature.h"

/* Font3DObjTab entry (objtemp_s): i@0, flags@2, action@3, anim_time@4. */
typedef struct {
    short i;          /* 0x0 */
    u8 flags;         /* 0x2 */
    s8 action;        /* 0x3 */
    float anim_time;  /* 0x4 */
    float scale;      /* 0x8 */
} objtemp_s;          /* 0xC */

extern objtemp_s Font3DObjTab[];
extern f32 D_0062E350;
extern f32 D_0062E354;
extern char D_005CD7B0[];   /* j_bd */
extern char D_005CD808[];   /* j_bc */

extern s32 qrand(void);

/* --- Font3D scene / material types (offsets verified from InitFont3D) --- */

struct numtl_s {
    u8 pad_1c6[0x1C6];
    u8 special_id;          /* +0x1C6 verified in InitFont3D (lbu 0x1C6) */
};

struct nugscn_s {
    u8 pad_08[0x8];
    struct numtl_s **mtls;  /* +0x8  verified in InitFont3D (lw 0x8) */
    s32 nummtl;             /* +0xC  verified in InitFont3D (lw 0xC)  */
};

/* NuSpecialFind out-parameter (obj): scene@0, special@4. */
struct objinfo_s {
    void *scene;            /* 0x0 */
    void *special;          /* 0x4 */
};

/* Font3DTab entry: ascii@0, obj{scene@4,special@8}, name@0xC; stride 0x10.
 * All offsets verified in InitFont3D. */
struct font3dtab_s {
    s32 ascii;              /* 0x0 */
    struct objinfo_s obj;   /* 0x4 scene, 0x8 special */
    char *name;             /* 0xC */
};                          /* 0x10 */

/* Font3DAccentTab entry: obj{scene@0,special@4}, name@8; stride 0xC.
 * Offsets verified in InitFont3D. */
struct font3djchar_s {
    struct objinfo_s obj;   /* 0x0 scene, 0x4 special */
    char *name;             /* 0x8 */
};                          /* 0xC */

extern struct font3dtab_s Font3DTab[];
extern struct font3djchar_s Font3DAccentTab[];
extern signed char Font3DRemap[];
extern struct numtl_s *Font3DMtlTab[5][2];
extern s32 font3d_initialised;

extern void *NuSpecialFind(struct nugscn_s *scn, struct objinfo_s *obj,
                           char *name);

/* --- Text3D externs --- */

/* ObjTab entry: scene@0, special@4; stride 0x20 (sll 5 in Text3D). */
struct objtab_s {
    void *scene;            /* 0x0 */
    void *special;          /* 0x4 */
    u8 pad_08[0x18];
};                          /* 0x20 */

struct game_s {
    u8 pad_00[0xF];
    u8 language;            /* 0xF */
};

extern struct objtab_s ObjTab[201];
extern struct game_s Game;
extern void *font3d_scene;
extern f32 font3d_dx, font3d_dy;
extern f32 font3d_xleft, font3d_xright, font3d_xmid;
extern f32 font3d_ytop, font3d_ybottom, font3d_ymid;
extern f32 FONT3D_JSCALEDX, FONT3DYMUL, FONT3DSIZE;
extern f32 D_0062E358, D_0062E35C, D_0062E360;
extern struct numtl_s **nurndr_forced_mtl_table;

extern int strlen(const char *s);
extern void DrawPanel3DObject(s32 i, f32 x, f32 y, f32 z, f32 sx, f32 sy,
                              f32 sz, s32 a1, s32 a2, s32 rot, void *scene,
                              void *special, s32 last);
extern void DrawPanel3DCharacter(s32 i, f32 x, f32 y, f32 z, f32 sx, f32 sy,
                                 f32 sz, s32 a1, s32 a2, s32 a3, s32 action,
                                 f32 anim_time, s32 last);


void InitFont3D(struct nugscn_s *gscn) {
    struct numtl_s *mtl;
    s32 i;

    for (i = 0; Font3DTab[i].ascii != 0; i++) {
        Font3DTab[i].obj.scene = 0;
        Font3DTab[i].obj.special = 0;
    }
    for (i = 0; i < 256; i++) {
        Font3DRemap[i] = -1;
    }
    for (i = 0; i < 7; i++) {
        Font3DAccentTab[i].obj.scene = 0;
        Font3DAccentTab[i].obj.special = 0;
    }
    if (gscn != 0) {
        for (i = 0; Font3DTab[i].ascii != 0; i++) {
            if (NuSpecialFind(gscn, &Font3DTab[i].obj, Font3DTab[i].name) != 0) {
                Font3DRemap[Font3DTab[i].ascii] = i;
            }
        }
        for (i = 0; i <= 6; i++) {
            NuSpecialFind(gscn, &Font3DAccentTab[i].obj, Font3DAccentTab[i].name);
        }
        for (i = 0; i < gscn->nummtl; i++) {
            mtl = gscn->mtls[i];
            if (mtl->special_id == 1 || mtl->special_id == 2 ||
                mtl->special_id == 3 || mtl->special_id == 4 ||
                mtl->special_id == 5) {
                Font3DMtlTab[mtl->special_id - 1][1] = mtl;
            }
        }
    }
    font3d_initialised = 1;
}


s32 RemapAccentedCharacter(char *c) {
    s32 accent;

    switch (*c) {
        default:
            accent = -1;
            break;
        case -0x1F:
            *c = '\0';
            accent = 6;
            break;
        case -0x79:
        case -0x80:
            *c = 'C';
            accent = 0;
            break;
        case -0x24:
        case -0x66:
        case -0x7F:
            *c = 'U';
            accent = 1;
            break;
        case -0x3c:
        case -0x72:
        case -0x7C:
            *c = 'A';
            accent = 1;
            break;
        case -0x77:
        case -0x2D:
            *c = 'E';
            accent = 1;
            break;
        case -0x28:
        case -0x75:
            *c = 'I';
            accent = 1;
            break;
        case -0x67:
        case -0x6C:
            *c = 'O';
            accent = 1;
            break;
        case -0x2F:
        case -0x5B:
        case -0x5C:
            *c = 'N';
            accent = 2;
            break;
        case -0x39:
        case -0x3A:
            *c = 'A';
            accent = 2;
            break;
        case -0x1B:
        case -0x1C:
            *c = 'O';
            accent = 2;
            break;
        case -0x4A:
        case -0x7D:
            *c = 'A';
            accent = 3;
            break;
        case -0x2E:
        case -0x36:
        case -0x78:
            *c = 'E';
            accent = 3;
            break;
        case -0x29:
        case -0x74:
            *c = 'I';
            accent = 3;
            break;
        case -0x1E:
        case -0x6D:
            *c = 'O';
            accent = 3;
            break;
        case -0x16:
        case -0x6A:
            *c = 'U';
            accent = 3;
            break;
        case -0x37:
        case -0x70:
        case -0x7E:
            *c = 'E';
            accent = 4;
            break;
        case -0x3F:
        case -0x4B:
        case -0x60:
            *c = 'A';
            accent = 4;
            break;
        case -0x20:
        case -0x5E:
            *c = 'O';
            accent = 4;
            break;
        case -0x17:
        case -0x5D:
            *c = 'U';
            accent = 4;
            break;
        case -0x2A:
        case -0x5F:
            *c = 'I';
            accent = 4;
            break;
        case -0x13:
        case -0x14:
            *c = 'Y';
            accent = 4;
            break;
        case -0x40:
        case -0x49:
        case -0x7B:
            *c = 'A';
            accent = 5;
            break;
        case -0x38:
        case -0x2C:
        case -0x76:
            *c = 'E';
            accent = 5;
            break;
        case -0x22:
        case -0x73:
            *c = 'I';
            accent = 5;
            break;
        case -0x6B:
        case -0x1D:
            *c = 'O';
            accent = 5;
            break;
        case -0x69:
        case -0x15:
            *c = 'U';
            accent = 5;
            break;
    }
    return accent;
}

void Reset3DFontObjects(void) {
    objtemp_s *tab;
    struct CharacterModel *model;
    s32 j;
    s32 i;
    f32 one;
    f32 rate;

    one = 1.0f;
    rate = D_0062E350;
    tab = Font3DObjTab;
    for (i = 0; i < 0x1a; i++, tab++) {
        tab->anim_time = one;
        if ((tab->flags & 1) != 0) {
            if (tab->i != -1) {
                if ((u8)tab->action < 0x76) {
                    j = CRemap[tab->i];
                    if (j != -1) {
                        model = &CModel[j];
                        if (model->anmdata[tab->action] != 0) {
                            tab->anim_time = (f32)qrand() * rate *
                                (model->anmdata[tab->action]->time - one) +
                                one;
                        }
                    }
                }
            }
        }
    }
}

void Update3DFontObjects(void) {
    objtemp_s *tab;
    struct CharacterModel *model;
    struct nuanimdata_s *ad;
    struct animlist *al;
    s32 j;
    s32 i;
    f32 rate;
    f32 one;

    rate = D_0062E354;
    one = 1.0f;
    tab = Font3DObjTab;
    for (i = 0; i < 0x1a; i++, tab++) {
        if ((tab->flags & 1) != 0) {
            if ((u8)tab->action < 0x76) {
                j = CRemap[tab->i];
                if (j != -1) {
                    model = &CModel[j];
                    ad = model->anmdata[tab->action];
                    if (ad != 0) {
                        al = model->animlist[tab->action];
                        tab->anim_time = tab->anim_time + al->speed * rate;
                        if (ad->time < tab->anim_time) {
                            if ((al->flags & 1) != 0) {
                                tab->anim_time =
                                    tab->anim_time - (ad->time - one);
                            } else {
                                tab->anim_time = ad->time;
                            }
                        }
                    }
                }
            }
        }
    }
}

s32 CombinationCharacterBD(char c0, char c1) {
    char *p;

    for (p = D_005CD7B0; *p != '\0'; p += 2) {
        if (c0 == *p && c1 == p[1]) {
            return 1;
        }
    }
    return 0;
}

s32 CombinationCharacterBC(char c0, char c1) {
    char *p;

    for (p = D_005CD808; *p != '\0'; p += 2) {
        if (c0 == *p && c1 == p[1]) {
            return 1;
        }
    }
    return 0;
}

void Text3D(char *txt, f32 x, f32 y, f32 z, f32 scalex, f32 scaley, f32 scalez,
            s32 align, s32 colour) {
    s32 i;
    s32 l;
    f32 w;
    f32 f;
    f32 dx;
    f32 dy;
    f32 x0;
    f32 y0;
    f32 xpulse;
    objtemp_s *obj;
    s32 idx;
    s32 accent;
    char c;
    char c1;

    if (font3d_initialised == 0) {
        return;
    }
    if (font3d_scene == 0) {
        return;
    }
    font3d_dx = 0.0f;
    font3d_dy = 0.0f;
    font3d_xright = x;
    font3d_xmid = x;
    font3d_xleft = x;
    font3d_ybottom = y;
    font3d_ymid = y;
    font3d_ytop = y;
    if (txt == 0) {
        return;
    }
    l = strlen(txt);
    if (l <= 0) {
        return;
    }

    w = 0.0f;
    for (i = 0; txt[i] != 0; i++) {
        c = txt[i];
        if (c == '#') {
            if (txt[i + 1] != 0) {
                i++;
            }
        } else if (Game.language == 0x63) {
            c1 = txt[i + 1];
            if (c1 != 0) {
                f = 1.0f;
                if (c1 == ' ') {
                    if ((c == ':') || (c == '.')) {
                        f = 0.5f;
                    } else if (c == 'p') {
                        f = 12.0f;
                    }
                } else if (((c == '8' || c == '9') || (c >= 'A' && c <= 'F')) &&
                           ((c1 >= '0' && c1 <= '9') ||
                            (c1 >= 'A' && c1 <= 'F'))) {
                    f = FONT3D_JSCALEDX;
                    if (txt[i + 2] == 'B' &&
                        ((txt[i + 3] == 'D' && CombinationCharacterBD(c, c1)) ||
                         (txt[i + 3] == 'C' && CombinationCharacterBC(c, c1)))) {
                        i = i + 2;
                    }
                }
                w += f;
                i++;
            }
        } else {
            if ((c == ':') || (c == '.')) {
                w += 0.5f;
            } else if (c == 'p') {
                w += 12.0f;
            } else {
                w += 1.0f;
            }
        }
    }

    dx = scalex * D_0062E358;
    font3d_dx = dx;
    dy = (scaley * D_0062E358) * FONT3DYMUL;
    font3d_dy = dy;
    if ((align & 5) == 4) {
        y = y + dy * 0.5f;
    } else if ((align & 5) == 1) {
        y = y - dy * 0.5f;
    }
    if ((align & 0xA) == 8) {
        x = x - (dx * w - dx * 0.5f);
    } else if ((align & 0xA) == 2) {
        x = x + dx * 0.5f;
    } else {
        x = x - (dx * w * 0.5f - dx * 0.5f);
    }
    idx = ((u32)colour < 5) ? colour : 0;
    nurndr_forced_mtl_table = &Font3DMtlTab[idx][0];
    font3d_xleft = x - dx * 0.5f;

    for (i = 0; i < l; i++) {
        c = txt[i];
        xpulse = 1.0f;
        if (Game.language == 0x63) {
            c1 = txt[i + 1];
            if (c1 == 0) {
                break;
            }
            if ((c != '#') && (c1 != ' ')) {
                goto advance;
            }
        }
        if ((c >= 0) || (c == (char)0xF8) || (c == (char)0xFE)) {
            if (c == '#') {
                if (txt[i + 1] != 0) {
                    switch (txt[i + 1]) {
                        case 'o': idx = 0; break;
                        case 'w': idx = 1; break;
                        case 'c': idx = 2; break;
                        case 'b': idx = 3; break;
                        case 'g': idx = 4; break;
                        default: idx = -1; break;
                    }
                    if (idx != -1) {
                        nurndr_forced_mtl_table = &Font3DMtlTab[idx][0];
                    }
                }
                i++;
                continue;
            } else if ((c >= 'a' && c <= 'z') && (Font3DRemap[c] == -1)) {
                obj = &Font3DObjTab[c - 'a'];
                if (obj->i != -1) {
                    if ((obj->flags & 2) != 0) {
                        if (ObjTab[obj->i].special != 0) {
                            DrawPanel3DObject(obj->i, x, y, z,
                                obj->scale * scalex, obj->scale * scaley,
                                obj->scale * scalez, 0, 0, 0,
                                ObjTab[obj->i].scene, ObjTab[obj->i].special, 1);
                            goto advance;
                        }
                    } else if ((obj->flags & 1) != 0) {
                        DrawPanel3DCharacter(obj->i, x, y, z,
                            obj->scale * scalex, obj->scale * scaley,
                            obj->scale * scalez, 0, 0, 0, obj->action,
                            obj->anim_time, 1);
                    }
                }
                goto advance;
            } else {
                idx = Font3DRemap[c];
                if (idx == -1) {
                    goto advance;
                }
                if (c == 'x' || c == 't' || c == 'o' || c == 's') {
                    f = 1.0f;
                } else {
                    f = 4.0f;
                }
                if ((c == ':') || (c == '.')) {
                    x0 = x - dx * 0.25f;
                } else if (c == 'p') {
                    x0 = x + dx * 5.5f;
                } else {
                    x0 = x;
                }
                if (c == (char)0xFE) {
                    x0 = x0 - dx * D_0062E35C;
                    y0 = y + dy * D_0062E360;
                } else {
                    y0 = y;
                }
                DrawPanel3DObject(-1, x0, y0, z, FONT3DSIZE * scalex,
                    FONT3DSIZE * scaley, (FONT3DSIZE * scalez) * f, 0, 0, 0,
                    font3d_scene, Font3DTab[idx].obj.special, 1);
                goto advance;
            }
        } else if (c < 0) {
            accent = RemapAccentedCharacter(&c);
            if (accent == -1) {
                goto advance;
            }
            if (Font3DRemap[c] != -1) {
                DrawPanel3DObject(-1, x, y, z, FONT3DSIZE * scalex,
                    FONT3DSIZE * scaley, (FONT3DSIZE * scalez) * 4.0f, 0, 0, 0,
                    font3d_scene, Font3DTab[Font3DRemap[c]].obj.special, 1);
            }
            DrawPanel3DObject(-1, x, y, z, FONT3DSIZE * scalex,
                FONT3DSIZE * scaley, (FONT3DSIZE * scalez) * 6.0f, 0, 0, 0,
                font3d_scene, Font3DAccentTab[accent].obj.special, 1);
        }
    advance:
        if ((c == ':') || (c == '.')) {
            x = x + (dx * 0.5f) * xpulse;
        } else if (c == 'p') {
            x = x + (dx * 12.0f) * xpulse;
        } else {
            x = x + dx * xpulse;
        }
        if (Game.language == 0x63) {
            i++;
        }
    }

    nurndr_forced_mtl_table = 0;
    y0 = dy * 0.5f;
    font3d_ybottom = y - y0;
    font3d_ytop = y + y0;
    x0 = x - dx * 0.5f;
    font3d_xright = x0;
    font3d_xmid = (font3d_xleft + font3d_xright) * 0.5f;
    font3d_ymid = (font3d_ytop + font3d_ybottom) * 0.5f;
}
