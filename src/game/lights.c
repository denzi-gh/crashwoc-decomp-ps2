/*
 * Unit: game/lights
 *
 * Functions:
 *   0x002445a8 LoadLights
 *   0x00244818 SaveLights
 *   0x00244a98 ResetLights
 *   0x00244c10 DrawHighLightBox
 *   0x00244f48 TempDrawLights
 *   0x002450c8 FindNearestLights
 *   0x00245c48 CalculateSingleLightProportion
 *   0x00246180 FindLightProportion
 *   0x00246778 LightEverythingInEditor
 *   0x00246d40 SetLevelLights
 *   0x002472a0 SetCreatureLights
 *   0x00247580 SetLights
 *   0x002475d0 UpdateGlobals
 *   0x00247698 GetLights
 *   0x002476d8 InitObjectLights
 *   0x002477d0 RotateDirectionalLight
 *   0x00247820 SetNearestLights
 *   0x00247888 ScaleColour
 *   0x00247908 edlightDrawCross
 *   0x00247a20 SortLights
 */

#include "creature.h"

extern s32 USELIGHTS;
extern s32 LIGHTCREATURES;
extern s32 LIGHTCOUNT;
extern s32 AMBIENTCOUNT;
extern s32 DIRECTCOUNT;
extern s32 POINTCOUNT;
extern s32 glb_dir_error_flag;
extern s32 glb_amb_error_flag;
extern f32 sf;
extern f32 sf2;
extern f32 size;
extern f32 size2;
extern struct nuvec_s v001;
extern char tbuf[];
extern char LevelFileName[];
extern char D_00632778[];

#define lgt_file_fmt D_00632778

struct glowlight_s {
    struct nucolour3_s HighColour;
    struct nucolour3_s LowColour;
    struct nucolour3_s CurColour;
    struct nucolour3_s ColDiff;
    u16 Step;
    u16 CurAngle;
};

struct lights_s {
    s32 type;                       /* 0x00 */
    struct nuvec_s pos;             /* 0x04 */
    struct nuvec_s radius_pos;      /* 0x10 */
    f32 radius;                     /* 0x1C */
    u8 r;                           /* 0x20 */
    u8 g;                           /* 0x21 */
    u8 b;                           /* 0x22 */
    u8 glow;                        /* 0x23 */
    struct nucolour3_s colour;      /* 0x24 */
    struct nuvec_s direction;       /* 0x30 */
    u8 globalflag;                  /* 0x3C */
    u8 blendtype;                   /* 0x3D */
    u8 brightness;                  /* 0x3E */
    struct glowlight_s *glowlight;  /* 0x40 */
    u8 pad2;                        /* 0x44 */
};                                  /* 0x48 */

extern struct lights_s Lights[];

/* Object light block filled by InitObjectLights (dir[3], col[3], AmbCol). */
struct objlights_s {
    struct nuvec_s dir[3];      /* 0x00 */
    struct nucolour3_s col[3];  /* 0x24 */
    struct nuvec_s AmbCol;      /* 0x48 */
};                              /* 0x54 */

struct tubinfo_s {
    struct creature_s *c;
    s8 track;
    s8 count;
    u8 wait;
    s8 laps;
    f32 time;
    f32 duration;
    f32 lap_position;
    s8 place;
    u8 finished;
    u8 finishframes;
    u8 stall;
    u16 old_xrot;
    u16 old_yrot;
    u16 old_zrot;
    u8 boost;
    s8 spark;
};

struct chaseevent_s {
    struct nugspline_s *spl;
    struct nuhspecial_s obj[24];
};

struct chase_s {
    struct nugspline_s *spl_START;          /* 0x0000 */
    f32 time;                               /* 0x0004 */
    f32 duration;                           /* 0x0008 */
    struct nugspline_s *spl_CHASER[6];      /* 0x000C */
    struct nuvec_s pos[6];                  /* 0x0024 */
    struct RPos_s RPos[6];                  /* 0x006C */
    struct anim_s anim[6];                  /* 0x0144 */
    struct nuhspecial_s obj[6];             /* 0x01EC */
    s16 character[6];                       /* 0x021C */
    s16 action[6];                          /* 0x0228 */
    u16 xrot[6];                            /* 0x0234 */
    u16 yrot[6];                            /* 0x0240 */
    u16 zrot[6];                            /* 0x024C */
    struct chaseevent_s event[6][24];       /* 0x0258 */
    struct nugspline_s *spl_MISC[6][4];     /* 0x7098 */
    struct Nearest_Light_s lights[6];       /* 0x70F8 */
    f32 scale[6];                           /* 0x7518 */
    u8 misc_phase[6];                       /* 0x7530 */
    s8 ok[6];                               /* 0x7536 */
    u8 cuboid;                              /* 0x753C */
    s8 i;                                   /* 0x753D */
    s8 i_last;                              /* 0x753E */
    s8 i_next;                              /* 0x753F */
    s8 status;                              /* 0x7540 */
    s8 iRAIL;                               /* 0x7541 */
    s16 iALONG;                             /* 0x7542 */
    f32 fALONG;                             /* 0x7544 */
};                                          /* 0x7548 */

extern struct tubinfo_s OppTub;
extern struct chase_s Chase[3];
extern CharacterData CData[191];
extern s32 Level;

struct gtimer_s {
    u32 frame;
    u32 itime;
    u32 isec;
    f32 ftime;
    f32 fsec;
};

extern struct gtimer_s GameTimer;
extern s32 GameMode;
extern s32 gamecut;
extern s32 cutmovie;
extern f32 NuTrigTable[];
extern struct nuvec_s lcutdir[3];
extern struct nucolour3_s lcutdircol[3];
extern struct nuvec_s lcutambcol;
extern struct nuvec_s titledir[3];
extern struct nucolour3_s titlergb;
extern struct nucolour3_s titlergb2;
extern struct nuvec_s creditsdir[3];
extern struct nucolour3_s creditsdircol[3];
extern struct nuvec_s creditsrgb;
extern struct nuvec_s ZVec;
extern f32 intensity;
extern s32 creditsang[3];
extern struct nucolour3_s lcol[3];
extern struct nuvec_s ldir[3];
extern struct nuvec_s acol;

s32 sprintf(char *buf, const char *fmt, ...);
s32 NuFileLoadBuffer(char *name, void *buf, s32 max);
void *NuMemFileOpen(void *buf, s32 size, s32 mode);
void NuFileClose(void *f);
f32 NuFileReadFloat(void *f);
s32 NuFileReadInt(void *f);
s32 NuFileReadChar(void *f);
void *NuFileOpen(char *name, s32 mode);
s32 NuFileWriteInt(void *f, s32 v);
s32 NuFileWriteFloat(void *f, f32 v);
s32 NuFileWriteChar(void *f, char v);
f32 FindLightProportion(struct nuvec_s *vec, struct Nearest_Light_s *nearest_light);
extern f32 NuVecDist(struct nuvec_s *a, struct nuvec_s *b, void *c);
extern void NuVecSub(struct nuvec_s *d, struct nuvec_s *a, struct nuvec_s *b);
extern f32 NuVecNorm(struct nuvec_s *d, struct nuvec_s *a);
extern void NuVecAdd(struct nuvec_s *d, struct nuvec_s *a, struct nuvec_s *b);
extern s32 NuRndrSetDirectionalLights(struct nuvec_s *d0, struct nucolour3_s *c0,
                                      struct nuvec_s *d1, struct nucolour3_s *c1,
                                      struct nuvec_s *d2, struct nucolour3_s *c2);
extern s32 NuRndrSetAmbientLight(struct nuvec_s *amb);
extern void NuRndrSetSpecularLight(struct nuvec_s *dir, s32 enable);
extern void NuVecRotateX(struct nuvec_s *out, struct nuvec_s *in, s32 ang);
extern void NuVecRotateY(struct nuvec_s *out, struct nuvec_s *in, s32 ang);
void NuRndrLine3dDbg(u32 colour, f32 x0, f32 y0, f32 z0, f32 x1, f32 y1, f32 z1);
void ResetLights(struct Nearest_Light_s *nl);
void GetLights(struct nuvec_s *pos, struct Nearest_Light_s *nearest_lights,
               s32 SearchMode);
s32 FindNearestLights(struct nuvec_s *vec, struct Nearest_Light_s *nearest_light,
                      s32 SearchMode);

inline void SetLights(struct nucolour3_s *vCOL0, struct nuvec_s *vDIR0,
                      struct nucolour3_s *vCOL1, struct nuvec_s *vDIR1,
                      struct nucolour3_s *vCOL2, struct nuvec_s *vDIR2,
                      struct nuvec_s *vAMB) {
    NuRndrSetDirectionalLights(vDIR0, vCOL0, vDIR1, vCOL1, vDIR2, vCOL2);
    NuRndrSetAmbientLight(vAMB);
}

inline void UpdateGlobals(struct Nearest_Light_s *nl) {
    s32 i;
    s32 found_amb;
    s32 found_dir;

    glb_dir_error_flag = -1;
    glb_amb_error_flag = -1;
    found_amb = 0;
    found_dir = 0;
    nl->glbambindex = -1;
    nl->glbdirectional.Index = -1;
    for (i = 0; i < LIGHTCOUNT && (!found_amb || !found_dir); i++) {
        if (Lights[i].type == 0 && Lights[i].globalflag == 4) {
            nl->glbambindex = i;
            found_amb = 1;
            glb_amb_error_flag = 1;
        }
        if ((Lights[i].type == 1 || Lights[i].type == 2) &&
            Lights[i].globalflag == 4) {
            nl->glbdirectional.Index = i;
            found_dir = 1;
            glb_dir_error_flag = 1;
        }
    }
}

inline void RotateDirectionalLight(struct nuvec_s *v, s32 xrot, s32 yrot) {
    NuVecRotateX(v, &v001, xrot);
    NuVecRotateY(v, v, yrot);
}

inline void ScaleColour(struct nucolour3_s *colour, u8 r, u8 g, u8 b, u8 power) {
    if (power == 6) {
        colour->r = (s32)r * sf2;
        colour->g = (s32)g * sf2;
        colour->b = (s32)b * sf2;
    } else if (power == 7) {
        colour->r = (s32)r * sf;
        colour->g = (s32)g * sf;
        colour->b = (s32)b * sf;
    }
}

inline void edlightDrawCross(struct nuvec_s *pos, u8 r, u8 g, u8 b) {
    struct nuvec_s v[6];
    u32 colour;
    s32 i;

    for (i = 0; i < 6; i++) {
        v[i] = *pos;
    }
    v[0].x -= size2;
    v[1].x += size2;
    v[2].y -= size2;
    v[3].y += size2;
    v[4].z -= size2;
    v[5].z += size2;
    colour = (0xFF << 24) | (b << 16) | (g << 8) | r;
    NuRndrLine3dDbg(colour, v[0].x, v[0].y, v[0].z, v[1].x, v[1].y, v[1].z);
    NuRndrLine3dDbg(colour, v[2].x, v[2].y, v[2].z, v[3].x, v[3].y, v[3].z);
    NuRndrLine3dDbg(colour, v[4].x, v[4].y, v[4].z, v[5].x, v[5].y, v[5].z);
}

void LoadLights(void) {
    void *handle;
    s32 i;
    s32 filesize;

    LIGHTCOUNT = 0;
    AMBIENTCOUNT = 0;
    DIRECTCOUNT = 0;
    POINTCOUNT = 0;
    for (i = 0; i < 0x100; i++) {
        Lights[i].type = -1;
    }
    sprintf(tbuf, lgt_file_fmt, LevelFileName);
    filesize = NuFileLoadBuffer(tbuf, Chase, 0x7FFFFFFF);
    handle = NuMemFileOpen(Chase, filesize, 0);
    if (handle != 0) {
        LIGHTCOUNT = NuFileReadInt(handle);
        AMBIENTCOUNT = NuFileReadInt(handle);
        DIRECTCOUNT = NuFileReadInt(handle);
        POINTCOUNT = NuFileReadInt(handle);
        if (LIGHTCOUNT > 0x100) {
            LIGHTCOUNT = 0x100;
        }
        for (i = 0; i < LIGHTCOUNT;) {
            Lights[i].type = NuFileReadInt(handle);
            Lights[i].pos.x = NuFileReadFloat(handle);
            Lights[i].pos.y = NuFileReadFloat(handle);
            Lights[i].pos.z = NuFileReadFloat(handle);
            Lights[i].radius_pos.x = NuFileReadFloat(handle);
            Lights[i].radius_pos.y = NuFileReadFloat(handle);
            Lights[i].radius_pos.z = NuFileReadFloat(handle);
            Lights[i].radius = NuFileReadFloat(handle);
            Lights[i].r = NuFileReadChar(handle);
            Lights[i].g = NuFileReadChar(handle);
            Lights[i].b = NuFileReadChar(handle);
            Lights[i].colour.r = NuFileReadFloat(handle);
            Lights[i].colour.g = NuFileReadFloat(handle);
            Lights[i].colour.b = NuFileReadFloat(handle);
            if (Lights[i].type == 1 || Lights[i].type == 2) {
                Lights[i].direction.x = NuFileReadFloat(handle);
                Lights[i].direction.y = NuFileReadFloat(handle);
                Lights[i].direction.z = NuFileReadFloat(handle);
            }
            Lights[i].globalflag = NuFileReadInt(handle);
            Lights[i++].brightness = NuFileReadInt(handle);
        }
        NuFileClose(handle);
    }
}

void SaveLights(void) {
    void *handle;
    s32 i;
    s32 ambient;
    s32 directional;
    s32 point;

    ambient = 0;
    directional = 0;
    point = 0;
    for (i = 0; i < LIGHTCOUNT; i++) {
        if (Lights[i].type == 0) {
            ambient++;
        } else if (Lights[i].type == 1) {
            directional++;
        } else if (Lights[i].type == 2) {
            point++;
        }
        AMBIENTCOUNT = ambient;
        DIRECTCOUNT = directional;
        POINTCOUNT = point;
    }
    sprintf(tbuf, lgt_file_fmt, LevelFileName);
    handle = NuFileOpen(tbuf, 1);
    if (handle != 0) {
        NuFileWriteInt(handle, LIGHTCOUNT);
        NuFileWriteInt(handle, ambient);
        NuFileWriteInt(handle, directional);
        NuFileWriteInt(handle, point);
        for (i = 0; i < LIGHTCOUNT; i++) {
            NuFileWriteInt(handle, Lights[i].type);
            NuFileWriteFloat(handle, Lights[i].pos.x);
            NuFileWriteFloat(handle, Lights[i].pos.y);
            NuFileWriteFloat(handle, Lights[i].pos.z);
            NuFileWriteFloat(handle, Lights[i].radius_pos.x);
            NuFileWriteFloat(handle, Lights[i].radius_pos.y);
            NuFileWriteFloat(handle, Lights[i].radius_pos.z);
            NuFileWriteFloat(handle, Lights[i].radius);
            NuFileWriteChar(handle, Lights[i].r);
            NuFileWriteChar(handle, Lights[i].g);
            NuFileWriteChar(handle, Lights[i].b);
            NuFileWriteFloat(handle, Lights[i].colour.r);
            NuFileWriteFloat(handle, Lights[i].colour.g);
            NuFileWriteFloat(handle, Lights[i].colour.b);
            if (Lights[i].type == 1 || Lights[i].type == 2) {
                NuFileWriteFloat(handle, Lights[i].direction.x);
                NuFileWriteFloat(handle, Lights[i].direction.y);
                NuFileWriteFloat(handle, Lights[i].direction.z);
            }
            NuFileWriteInt(handle, Lights[i].globalflag);
            NuFileWriteInt(handle, Lights[i].brightness);
        }
        NuFileClose(handle);
    }
}

void ResetLights(struct Nearest_Light_s *nl) {
    nl->pDir1st = &nl->dir1;
    nl->pDir2nd = &nl->dir2;
    nl->pDir3rd = &nl->dir3;
    nl->ambientdist = 8000.0f;
    nl->dir1.Distance = 8000.0f;
    nl->dir2.Distance = 8000.0f;
    nl->dir3.Distance = 8000.0f;
    nl->AmbIndex = -1;
    nl->AmbCol.x = 0.0f;
    nl->AmbCol.y = 0.0f;
    nl->AmbCol.z = 0.0f;
    nl->dir1.Index = -1;
    nl->dir2.Index = -1;
    nl->dir3.Index = -1;
    nl->pDir1st->Direction.x = 0.0f;
    nl->pDir1st->Direction.y = 0.0f;
    nl->pDir1st->Direction.z = 0.0f;
    nl->pDir3rd->Direction.x = 0.0f;
    nl->pDir2nd->Direction.x = 0.0f;
    nl->pDir2nd->Direction.y = 0.0f;
    nl->pDir2nd->Direction.z = 0.0f;
    nl->pDir3rd->Direction.x = 0.0f;
    nl->pDir3rd->Direction.x = 0.0f;
    nl->pDir3rd->Direction.y = 0.0f;
    nl->pDir3rd->Direction.z = 0.0f;
    nl->pDir3rd->Direction.x = 0.0f;
    nl->negativeindex = -1;
    nl->negativedist = 8000.0f;
    UpdateGlobals(nl);
}

void InitObjectLights(struct nuvec_s *pos, struct objlights_s *out) {
    struct Nearest_Light_s nl;

    ResetLights(&nl);
    GetLights(pos, &nl, 0);
    out->dir[0] = nl.pDir1st->Direction;
    out->dir[1] = nl.pDir2nd->Direction;
    out->dir[2] = nl.pDir3rd->Direction;
    out->col[0] = nl.pDir1st->Colour;
    out->col[1] = nl.pDir2nd->Colour;
    out->col[2] = nl.pDir3rd->Colour;
    out->AmbCol = nl.AmbCol;
}

inline void GetLights(struct nuvec_s *pos, struct Nearest_Light_s *nearest_lights,
                      s32 SearchMode) {
    FindNearestLights(pos, nearest_lights, SearchMode);
    FindLightProportion(pos, nearest_lights);
}

void SortLights(struct Nearest_Light_s *nearLgt) {
    struct pdir_s *tptr;
    struct pdir_s *tptr2;

    tptr = nearLgt->pDir1st;
    tptr2 = nearLgt->pDir2nd;
    if (tptr->Distance > tptr2->Distance) {
        nearLgt->pDir1st = tptr2;
        nearLgt->pDir2nd = tptr;
    }
    tptr = nearLgt->pDir2nd;
    tptr2 = nearLgt->pDir3rd;
    if (tptr->Distance > tptr2->Distance) {
        nearLgt->pDir2nd = tptr2;
        nearLgt->pDir3rd = tptr;
    }
    tptr2 = nearLgt->pDir1st;
    tptr = nearLgt->pDir2nd;
    if (tptr2->Distance > tptr->Distance) {
        nearLgt->pDir1st = tptr;
        nearLgt->pDir2nd = tptr2;
    }
}

void DrawHighLightBox(struct nuvec_s *pos, u8 r, u8 g, u8 b) {
    struct nuvec_s v[8];
    u32 colour;
    s32 i;

    for (i = 0; i < 8; i++) {
        v[i] = *pos;
    }
    v[0].y += size;
    v[0].x -= size;
    v[0].z += size;
    v[1].y += size;
    v[1].x += size;
    v[1].z += size;
    v[2].y += size;
    v[2].x += size;
    v[2].z -= size;
    v[3].y += size;
    v[3].x -= size;
    v[3].z -= size;
    v[4].y -= size;
    v[4].x -= size;
    v[4].z += size;
    v[5].y -= size;
    v[5].x += size;
    v[5].z += size;
    v[6].y -= size;
    v[6].x += size;
    v[6].z -= size;
    v[7].y -= size;
    v[7].x -= size;
    v[7].z -= size;
    colour = (b << 16) | (g << 8) | r;
    NuRndrLine3dDbg(colour, v[0].x, v[0].y, v[0].z, v[1].x, v[1].y, v[1].z);
    NuRndrLine3dDbg(colour, v[1].x, v[1].y, v[1].z, v[2].x, v[2].y, v[2].z);
    NuRndrLine3dDbg(colour, v[2].x, v[2].y, v[2].z, v[3].x, v[3].y, v[3].z);
    NuRndrLine3dDbg(colour, v[3].x, v[3].y, v[3].z, v[0].x, v[0].y, v[0].z);
    NuRndrLine3dDbg(colour, v[4].x, v[4].y, v[4].z, v[5].x, v[5].y, v[5].z);
    NuRndrLine3dDbg(colour, v[5].x, v[5].y, v[5].z, v[6].x, v[6].y, v[6].z);
    NuRndrLine3dDbg(colour, v[6].x, v[6].y, v[6].z, v[7].x, v[7].y, v[7].z);
    NuRndrLine3dDbg(colour, v[7].x, v[7].y, v[7].z, v[4].x, v[4].y, v[4].z);
    NuRndrLine3dDbg(0xFFFFFF, v[4].x, v[4].y, v[4].z, v[0].x, v[0].y, v[0].z);
    NuRndrLine3dDbg(0xFFFFFF, v[5].x, v[5].y, v[5].z, v[1].x, v[1].y, v[1].z);
    NuRndrLine3dDbg(0xFFFFFF, v[6].x, v[6].y, v[6].z, v[2].x, v[2].y, v[2].z);
    NuRndrLine3dDbg(0xFFFFFF, v[7].x, v[7].y, v[7].z, v[3].x, v[3].y, v[3].z);
}

void TempDrawLights(void) {
    s32 i;

    for (i = 0; i < LIGHTCOUNT; i++) {
        edlightDrawCross(&Lights[i].pos, Lights[i].r, Lights[i].g, Lights[i].b);
    }
}

s32 FindNearestLights(struct nuvec_s *vec, struct Nearest_Light_s *nearest_light,
                      s32 SearchMode) {
    u8 i;
    u8 loop;
    u8 PrevIndex;
    f32 distance;
    s32 scount;
    struct nuvec_s direction;
    f32 sfactor;
    struct pdir_s *tptr;
    struct pdir_s *tptr2;

    PrevIndex = nearest_light->AmbIndex;
    loop = nearest_light->CurLoopIndex;
    scount = 0x10;
    if (LIGHTCOUNT == 0) {
        return 0;
    }
    if (SearchMode == 0 || LIGHTCOUNT < 0x10) {
        scount = LIGHTCOUNT;
    }
    if (nearest_light->pDir1st->Index != -1) {
        nearest_light->pDir1st->Distance =
            NuVecDist(&Lights[nearest_light->pDir1st->Index].pos, vec, 0);
    }
    if (nearest_light->pDir2nd->Index != -1) {
        nearest_light->pDir2nd->Distance =
            NuVecDist(&Lights[nearest_light->pDir2nd->Index].pos, vec, 0);
    }
    if (nearest_light->pDir3rd->Index != -1) {
        nearest_light->pDir3rd->Distance =
            NuVecDist(&Lights[nearest_light->pDir3rd->Index].pos, vec, 0);
    }
    if (nearest_light->negativeindex != -1) {
        nearest_light->negativedist =
            NuVecDist(&Lights[nearest_light->negativeindex].pos, vec, 0);
    }
    if (nearest_light->AmbIndex != -1) {
        nearest_light->ambientdist =
            NuVecDist(&Lights[nearest_light->AmbIndex].pos, vec, 0);
    }
    tptr = nearest_light->pDir1st;
    tptr2 = nearest_light->pDir2nd;
    if (tptr->Distance > tptr2->Distance) {
        nearest_light->pDir1st = tptr2;
        nearest_light->pDir2nd = tptr;
    }
    tptr = nearest_light->pDir2nd;
    tptr2 = nearest_light->pDir3rd;
    if (tptr->Distance > tptr2->Distance) {
        nearest_light->pDir2nd = tptr2;
        nearest_light->pDir3rd = tptr;
    }
    tptr = nearest_light->pDir1st;
    tptr2 = nearest_light->pDir2nd;
    if (tptr->Distance > tptr2->Distance) {
        nearest_light->pDir1st = tptr2;
        nearest_light->pDir2nd = tptr;
    }
    for (i = 0; i < scount; i++, loop++) {
        if (loop == LIGHTCOUNT) {
            loop = 0;
        }
        distance = NuVecDist(&Lights[loop].pos, vec, 0);
        if (Lights[loop].type == 3) {
            if (distance < nearest_light->negativedist) {
                nearest_light->negativedist = distance;
                nearest_light->negativeindex = loop;
            }
        } else if (Lights[loop].type == 0) {
            if (loop != nearest_light->glbambindex &&
                distance < nearest_light->ambientdist) {
                nearest_light->ambientdist = distance;
                nearest_light->AmbIndex = loop;
            }
        } else if ((Lights[loop].type == 1 || Lights[loop].type == 2) &&
                   loop != nearest_light->pDir1st->Index) {
            if (loop != nearest_light->pDir2nd->Index &&
                loop != nearest_light->pDir3rd->Index &&
                loop != nearest_light->glbdirectional.Index) {
                if (distance < nearest_light->pDir1st->Distance) {
                    nearest_light->pDir3rd->Index = nearest_light->pDir2nd->Index;
                    nearest_light->pDir2nd->Index = nearest_light->pDir1st->Index;
                    nearest_light->pDir1st->Index = loop;
                    nearest_light->pDir3rd->Distance =
                        nearest_light->pDir2nd->Distance;
                    nearest_light->pDir2nd->Distance =
                        nearest_light->pDir1st->Distance;
                    nearest_light->pDir1st->Distance = distance;
                } else if (distance < nearest_light->pDir2nd->Distance) {
                    nearest_light->pDir3rd->Index = nearest_light->pDir2nd->Index;
                    nearest_light->pDir2nd->Index = loop;
                    nearest_light->pDir3rd->Distance =
                        nearest_light->pDir2nd->Distance;
                    nearest_light->pDir2nd->Distance = distance;
                } else if (distance < nearest_light->pDir3rd->Distance) {
                    nearest_light->pDir3rd->Index = loop;
                    nearest_light->pDir3rd->Distance = distance;
                }
            }
        }
    }
    nearest_light->CurLoopIndex = loop;
    if (nearest_light->AmbIndex == PrevIndex) {
        nearest_light->ambientdist =
            NuVecDist(&Lights[nearest_light->AmbIndex].pos, vec, 0);
    }
    if (nearest_light->AmbIndex != -1) {
        if (Lights[nearest_light->AmbIndex].brightness == 6) {
            sfactor = sf2;
        } else if (Lights[nearest_light->AmbIndex].brightness == 7) {
            sfactor = sf;
        }
        nearest_light->AmbCol.x = sfactor * Lights[nearest_light->AmbIndex].r;
        nearest_light->AmbCol.y = sfactor * Lights[nearest_light->AmbIndex].g;
        nearest_light->AmbCol.z = sfactor * Lights[nearest_light->AmbIndex].b;
    } else if (nearest_light->glbambindex != -1) {
        nearest_light->AmbCol.x = sf * Lights[nearest_light->glbambindex].r;
        nearest_light->AmbCol.y = sf * Lights[nearest_light->glbambindex].g;
        nearest_light->AmbCol.z = sf * Lights[nearest_light->glbambindex].b;
    }
    if (nearest_light->pDir1st->Index != -1) {
        if (Lights[nearest_light->pDir1st->Index].type == 2) {
            NuVecSub(&direction, &Lights[nearest_light->pDir1st->Index].pos, vec);
            NuVecNorm(&direction, &direction);
            nearest_light->pDir1st->Direction = direction;
        } else {
            nearest_light->pDir1st->Direction =
                Lights[nearest_light->pDir1st->Index].direction;
        }
        ScaleColour(&nearest_light->pDir1st->Colour,
                    Lights[nearest_light->pDir1st->Index].r,
                    Lights[nearest_light->pDir1st->Index].g,
                    Lights[nearest_light->pDir1st->Index].b,
                    Lights[nearest_light->pDir1st->Index].brightness);
    } else if (nearest_light->glbdirectional.Index != -1 &&
               (Lights[nearest_light->glbdirectional.Index].type == 1 ||
                Lights[nearest_light->glbdirectional.Index].type == 2)) {
        if (Lights[nearest_light->pDir1st->Index].brightness == 6) {
            sfactor = sf2;
        } else if (Lights[nearest_light->pDir1st->Index].brightness == 7) {
            sfactor = sf;
        }
        nearest_light->pDir1st->Colour.r =
            sfactor * Lights[nearest_light->glbdirectional.Index].r;
        nearest_light->pDir1st->Colour.g =
            sfactor * Lights[nearest_light->glbdirectional.Index].g;
        nearest_light->pDir1st->Colour.b =
            sfactor * Lights[nearest_light->glbdirectional.Index].b;
        nearest_light->pDir1st->Distance = 8000.0f;
    } else {
        nearest_light->pDir1st->Colour.r = 0.0f;
        nearest_light->pDir1st->Colour.g = 0.0f;
        nearest_light->pDir1st->Colour.b = 0.0f;
        nearest_light->pDir1st->Distance = 8000.0f;
    }
    if (nearest_light->pDir2nd->Index != -1) {
        if (Lights[nearest_light->pDir2nd->Index].type == 2) {
            NuVecSub(&direction, &Lights[nearest_light->pDir2nd->Index].pos, vec);
            NuVecNorm(&direction, &direction);
            nearest_light->pDir2nd->Direction = direction;
        } else {
            nearest_light->pDir2nd->Direction =
                Lights[nearest_light->pDir2nd->Index].direction;
        }
        ScaleColour(&nearest_light->pDir2nd->Colour,
                    Lights[nearest_light->pDir2nd->Index].r,
                    Lights[nearest_light->pDir2nd->Index].g,
                    Lights[nearest_light->pDir2nd->Index].b,
                    Lights[nearest_light->pDir2nd->Index].brightness);
    } else if (nearest_light->glbdirectional.Index != -1 &&
               (Lights[nearest_light->glbdirectional.Index].type == 1 ||
                Lights[nearest_light->glbdirectional.Index].type == 2)) {
        if (Lights[nearest_light->pDir2nd->Index].brightness == 6) {
            sfactor = sf2;
        } else if (Lights[nearest_light->pDir2nd->Index].brightness == 7) {
            sfactor = sf;
        }
        nearest_light->pDir2nd->Colour.r =
            sfactor * Lights[nearest_light->glbdirectional.Index].r;
        nearest_light->pDir2nd->Colour.g =
            sfactor * Lights[nearest_light->glbdirectional.Index].g;
        nearest_light->pDir2nd->Colour.b =
            sfactor * Lights[nearest_light->glbdirectional.Index].b;
        nearest_light->pDir2nd->Distance = 8000.0f;
    } else {
        nearest_light->pDir2nd->Colour.r = 0.0f;
        nearest_light->pDir2nd->Colour.g = 0.0f;
        nearest_light->pDir2nd->Colour.b = 0.0f;
        nearest_light->pDir2nd->Distance = 8000.0f;
    }
    if (nearest_light->pDir3rd->Index != -1) {
        if (Lights[nearest_light->pDir3rd->Index].type == 2) {
            NuVecSub(&direction, &Lights[nearest_light->pDir3rd->Index].pos, vec);
            NuVecNorm(&direction, &direction);
            nearest_light->pDir3rd->Direction = direction;
        } else {
            nearest_light->pDir3rd->Direction =
                Lights[nearest_light->pDir3rd->Index].direction;
        }
        ScaleColour(&nearest_light->pDir3rd->Colour,
                    Lights[nearest_light->pDir3rd->Index].r,
                    Lights[nearest_light->pDir3rd->Index].g,
                    Lights[nearest_light->pDir3rd->Index].b,
                    Lights[nearest_light->pDir3rd->Index].brightness);
    } else if (nearest_light->glbdirectional.Index != -1 &&
               (Lights[nearest_light->glbdirectional.Index].type == 1 ||
                Lights[nearest_light->glbdirectional.Index].type == 2)) {
        if (Lights[nearest_light->pDir3rd->Index].brightness == 6) {
            sfactor = sf2;
        } else if (Lights[nearest_light->pDir3rd->Index].brightness == 7) {
            sfactor = sf;
        }
        nearest_light->pDir3rd->Colour.r =
            sfactor * Lights[nearest_light->glbdirectional.Index].r;
        nearest_light->pDir3rd->Colour.g =
            sfactor * Lights[nearest_light->glbdirectional.Index].g;
        nearest_light->pDir3rd->Colour.b =
            sfactor * Lights[nearest_light->glbdirectional.Index].b;
        nearest_light->pDir3rd->Distance = 8000.0f;
    } else {
        nearest_light->pDir3rd->Colour.r = 0.0f;
        nearest_light->pDir3rd->Colour.g = 0.0f;
        nearest_light->pDir3rd->Colour.b = 0.0f;
        nearest_light->pDir3rd->Distance = 8000.0f;
    }
    return 1;
}

void CalculateSingleLightProportion(struct nuvec_s *vec, struct pdir_s *light,
                                    s32 global) {
    f32 DistFromLight;
    f32 glb_proportion;
    f32 loc_proportion;
    struct nuvec_s blend_vec;
    struct nuvec_s loc_vec;
    struct nuvec_s glob_vec;
    f32 sfactor;
    struct glowlight_s *glight;

    glb_proportion = 0.0f;
    if (light->Index != -1) {
        DistFromLight = NuVecDist(&Lights[light->Index].pos, vec, 0);
        if (DistFromLight <= Lights[light->Index].radius) {
            loc_proportion =
                1.0 - (1.0 / Lights[light->Index].radius) * DistFromLight;
            if (global != -1) {
                glb_proportion = 1.0 - loc_proportion;
                if (Lights[global].brightness == 6) {
                    glb_proportion = glb_proportion + glb_proportion;
                }
                if (Lights[light->Index].brightness == 6) {
                    loc_proportion = loc_proportion + loc_proportion;
                }
                if (Lights[light->Index].glow != 0) {
                    glight = Lights[light->Index].glowlight;
                    light->Colour.r = Lights[global].r * sf * glb_proportion +
                                      glight->CurColour.r * loc_proportion;
                    light->Colour.g = Lights[global].g * sf * glb_proportion +
                                      glight->CurColour.g * loc_proportion;
                    light->Colour.b = Lights[global].b * sf * glb_proportion +
                                      glight->CurColour.b * loc_proportion;
                } else {
                    light->Colour.r = Lights[global].r * sf * glb_proportion +
                                      light->Colour.r * loc_proportion;
                    light->Colour.g = Lights[global].g * sf * glb_proportion +
                                      light->Colour.g * loc_proportion;
                    light->Colour.b = Lights[global].b * sf * glb_proportion +
                                      light->Colour.b * loc_proportion;
                }
                loc_vec = light->Direction;
                loc_vec.x *= loc_proportion;
                loc_vec.y *= loc_proportion;
                loc_vec.z *= loc_proportion;
                glob_vec = Lights[global].direction;
                glob_vec.x *= glb_proportion;
                glob_vec.y *= glb_proportion;
                glob_vec.z *= glb_proportion;
                NuVecAdd(&blend_vec, &loc_vec, &glob_vec);
                if (NuVecNorm(&blend_vec, &blend_vec) > 0.0f) {
                    light->Direction = blend_vec;
                }
            } else if (Lights[light->Index].glow != 0) {
                glight = Lights[light->Index].glowlight;
                light->Colour.r = glight->CurColour.r * loc_proportion;
                light->Colour.g = glight->CurColour.g * loc_proportion;
                light->Colour.b = glight->CurColour.b * loc_proportion;
            } else {
                light->Colour.r = light->Colour.r * loc_proportion;
                light->Colour.g = light->Colour.g * loc_proportion;
                light->Colour.b = light->Colour.b * loc_proportion;
            }
        } else if (global != -1) {
            if (Lights[global].brightness == 6) {
                sfactor = sf2;
            } else if (Lights[global].brightness == 7) {
                sfactor = sf;
            }
            light->Colour.r = Lights[global].r * sfactor;
            light->Colour.g = Lights[global].g * sfactor;
            light->Colour.b = Lights[global].b * sfactor;
            light->Direction = Lights[global].direction;
        } else {
            light->Colour.r = glb_proportion;
            light->Colour.g = glb_proportion;
            light->Colour.b = glb_proportion;
        }
    } else if (global != -1) {
        if (Lights[global].brightness == 6) {
            sfactor = sf2;
        } else if (Lights[global].brightness == 7) {
            sfactor = sf;
        }
        light->Colour.r = Lights[global].r * 0.33f * sfactor;
        light->Colour.g = Lights[global].g * 0.33f * sfactor;
        light->Colour.b = Lights[global].b * 0.33f * sfactor;
        light->Direction = Lights[global].direction;
    }
}

f32 FindLightProportion(struct nuvec_s *vec, struct Nearest_Light_s *nearest_light) {
    f32 glb_proportion;
    f32 proportion;
    f32 loc_proportion;
    f32 DistFromLight;
    f32 sfactor;
    struct glowlight_s *glight;

    proportion = 0.0f;
    loc_proportion = 0.0f;
    if (nearest_light->AmbIndex != -1) {
        DistFromLight = NuVecDist(&Lights[nearest_light->AmbIndex].pos, vec, 0);
        if (DistFromLight <= Lights[nearest_light->AmbIndex].radius) {
            loc_proportion = 1.0 - (1.0 / Lights[nearest_light->AmbIndex].radius) *
                                       DistFromLight;
            if (nearest_light->glbambindex != -1) {
                glb_proportion = 1.0 - loc_proportion;
                if (Lights[nearest_light->glbambindex].brightness == 6) {
                    glb_proportion = glb_proportion + glb_proportion;
                }
                if (Lights[nearest_light->AmbIndex].brightness == 6) {
                    loc_proportion = loc_proportion + loc_proportion;
                }
                if (Lights[nearest_light->AmbIndex].glow != 0) {
                    glight = Lights[nearest_light->AmbIndex].glowlight;
                    nearest_light->AmbCol.x = glight->CurColour.r * sf * glb_proportion +
                                              glight->CurColour.r * loc_proportion;
                    nearest_light->AmbCol.y = glight->CurColour.g * sf * glb_proportion +
                                              glight->CurColour.g * loc_proportion;
                    nearest_light->AmbCol.z = glight->CurColour.b * sf * glb_proportion +
                                              glight->CurColour.b * loc_proportion;
                } else {
                    nearest_light->AmbCol.x =
                        Lights[nearest_light->glbambindex].r * sf * glb_proportion +
                        nearest_light->AmbCol.x * loc_proportion;
                    nearest_light->AmbCol.y =
                        Lights[nearest_light->glbambindex].g * sf * glb_proportion +
                        nearest_light->AmbCol.y * loc_proportion;
                    nearest_light->AmbCol.z =
                        Lights[nearest_light->glbambindex].b * sf * glb_proportion +
                        nearest_light->AmbCol.z * loc_proportion;
                }
            } else if (Lights[nearest_light->AmbIndex].glow != 0) {
                glight = Lights[nearest_light->AmbIndex].glowlight;
                nearest_light->AmbCol.x = glight->CurColour.r * loc_proportion;
                nearest_light->AmbCol.y = glight->CurColour.g * loc_proportion;
                nearest_light->AmbCol.z = glight->CurColour.b * loc_proportion;
            } else {
                nearest_light->AmbCol.x = nearest_light->AmbCol.x * loc_proportion;
                nearest_light->AmbCol.y = nearest_light->AmbCol.y * loc_proportion;
                nearest_light->AmbCol.z = nearest_light->AmbCol.z * loc_proportion;
            }
        } else if (nearest_light->glbambindex != -1) {
            if (Lights[nearest_light->glbambindex].brightness == 6) {
                sfactor = sf2;
            } else if (Lights[nearest_light->glbambindex].brightness == 7) {
                sfactor = sf;
            }
            nearest_light->AmbCol.x = Lights[nearest_light->glbambindex].r * sfactor;
            nearest_light->AmbCol.y = Lights[nearest_light->glbambindex].g * sfactor;
            nearest_light->AmbCol.z = Lights[nearest_light->glbambindex].b * sfactor;
        } else {
            nearest_light->AmbCol.x = loc_proportion;
            nearest_light->AmbCol.y = loc_proportion;
            nearest_light->AmbCol.z = loc_proportion;
        }
    } else if (nearest_light->glbambindex != -1) {
        if (Lights[nearest_light->glbambindex].brightness == 6) {
            sfactor = sf2;
        } else if (Lights[nearest_light->glbambindex].brightness == 7) {
            sfactor = sf;
        }
        nearest_light->AmbCol.x = Lights[nearest_light->glbambindex].r * sfactor;
        nearest_light->AmbCol.y = Lights[nearest_light->glbambindex].g * sfactor;
        nearest_light->AmbCol.z = Lights[nearest_light->glbambindex].b * sfactor;
    }
    CalculateSingleLightProportion(vec, nearest_light->pDir1st,
                                   nearest_light->glbdirectional.Index);
    CalculateSingleLightProportion(vec, nearest_light->pDir2nd,
                                   nearest_light->glbdirectional.Index);
    CalculateSingleLightProportion(vec, nearest_light->pDir3rd,
                                   nearest_light->glbdirectional.Index);
    if (nearest_light->negativeindex != -1) {
        DistFromLight = NuVecDist(&Lights[nearest_light->negativeindex].pos, vec, 0);
        if (DistFromLight <= Lights[nearest_light->negativeindex].radius) {
            loc_proportion =
                (1.0 / Lights[nearest_light->negativeindex].radius) * DistFromLight;
            if (nearest_light->AmbIndex != -1) {
                nearest_light->AmbCol.x = nearest_light->AmbCol.x * loc_proportion;
                nearest_light->AmbCol.y = nearest_light->AmbCol.y * loc_proportion;
                nearest_light->AmbCol.z = nearest_light->AmbCol.z * loc_proportion;
            } else {
                nearest_light->AmbCol.x =
                    Lights[nearest_light->glbambindex].r * sf * loc_proportion;
                nearest_light->AmbCol.y =
                    Lights[nearest_light->glbambindex].g * sf * loc_proportion;
                nearest_light->AmbCol.z =
                    Lights[nearest_light->glbambindex].b * sf * loc_proportion;
            }
            if (nearest_light->pDir1st->Index != -1) {
                nearest_light->pDir1st->Colour.r *= loc_proportion;
                nearest_light->pDir1st->Colour.g *= loc_proportion;
                nearest_light->pDir1st->Colour.b *= loc_proportion;
            }
            if (nearest_light->pDir2nd->Index != -1) {
                nearest_light->pDir2nd->Colour.r *= loc_proportion;
                nearest_light->pDir2nd->Colour.g *= loc_proportion;
                nearest_light->pDir2nd->Colour.b *= loc_proportion;
            }
            if (nearest_light->pDir3rd->Index != -1) {
                nearest_light->pDir3rd->Colour.r *= loc_proportion;
                nearest_light->pDir3rd->Colour.g *= loc_proportion;
                nearest_light->pDir3rd->Colour.b *= loc_proportion;
            }
        }
    }
    return proportion;
}

void LightEverythingInEditor(void) {
    struct obj_s obj;
    struct nuvec_s pos;
    struct creature_s *c;
    struct chase_s *chase;
    s32 i;
    s32 j;
    s32 character;

    c = Character;
    for (i = 0; i < 9; i++, c++) {
        if (c->on != 0) {
            UpdateGlobals(&c->lights);
            pos.x = c->obj.pos.x;
            pos.y = c->obj.pos.y + (c->obj.bot + c->obj.top) * c->obj.SCALE * 0.5f;
            pos.z = c->obj.pos.z;
            GetLights(&pos, &c->lights, 0);
        }
    }
    if (Level == 0x1C) {
        UpdateGlobals(&OppTub.c->lights);
        pos.x = OppTub.c->obj.pos.x;
        pos.y = OppTub.c->obj.pos.y +
                (OppTub.c->obj.bot + OppTub.c->obj.top) * OppTub.c->obj.SCALE * 0.5f;
        pos.z = OppTub.c->obj.pos.z;
        GetLights(&pos, &OppTub.c->lights, 0);
    }
    c = player;
    if (c->obj.mask != 0 && c->obj.mask->active != 0) {
        UpdateGlobals(&c->obj.mask->lights);
        GetLights(&c->obj.mask->pos, &c->obj.mask->lights, 0);
    }
    chase = Chase;
    for (i = 0; i < 3; i++, chase++) {
        if (chase->status == 2) {
            for (j = 0; j < 6; j++) {
                if (chase->ok[j] != 0) {
                    character = chase->character[j];
                    if (character != -1 && character < 999) {
                        UpdateGlobals(&chase->lights[j]);
                        obj.pos.y = chase->pos[j].y;
                        obj.bot = CData[character].min.y;
                        obj.top = CData[character].max.y;
                        obj.SCALE = CData[character].scale;
                        pos.x = chase->pos[j].x;
                        pos.y = obj.pos.y + (obj.bot + obj.top) * obj.SCALE * 0.5f;
                        pos.z = chase->pos[j].z;
                        GetLights(&pos, &chase->lights[j], 0);
                    }
                }
            }
        }
    }
}

void SetLevelLights(void) {
    struct nuvec_s specular = {0.0f, 0.0f, -1.0f};
    double trig;

    if (GameMode == 1 && gamecut != 0 && gamecut != 6 && gamecut != 11 &&
        gamecut != 16) {
        RotateDirectionalLight(&lcutdir[0], ((GameTimer.frame % 100) << 16) / 100,
                               ((GameTimer.frame % 100) << 16) / 100);
        RotateDirectionalLight(&lcutdir[1], ((GameTimer.frame % 100) << 16) / 100,
                               ((GameTimer.frame % 100) << 16) / 100);
        RotateDirectionalLight(&lcutdir[2], ((GameTimer.frame % 100) << 16) / 100,
                               ((GameTimer.frame % 100) << 16) / 100);
        SetLights(&lcutdircol[0], &lcutdir[0], &lcutdircol[1], &lcutdir[1],
                  &lcutdircol[2], &lcutdir[2], &lcutambcol);
    } else if (cutmovie == 0) {
        RotateDirectionalLight(&titledir[0], ((GameTimer.frame % 200) << 16) / 200,
                               ((GameTimer.frame % 325) << 16) / 325);
        RotateDirectionalLight(&titledir[1], ((GameTimer.frame % 275) << 16) / 275,
                               ((GameTimer.frame % 375) << 16) / 375);
        RotateDirectionalLight(&titledir[2], ((GameTimer.frame % 100) << 16) / 100,
                               ((GameTimer.frame % 125) << 16) / 125);
        SetLights(&titlergb, &titledir[0], &titlergb, &titledir[1], &titlergb2,
                  &titledir[2], (struct nuvec_s *)&titlergb);
        NuRndrSetSpecularLight(&specular, 0);
    } else if (Level == 43 && cutmovie == -1) {
        trig = NuTrigTable[creditsang[0] & 0xFFFF];
        if (trig < 0.0) {
            trig = -trig;
        }
        intensity = trig;
        NuVecRotateY(&creditsdir[0], &ZVec, creditsang[0]);
        creditsdircol[0].r = intensity;
        creditsang[0] += 256;
        trig = NuTrigTable[creditsang[1] & 0xFFFF];
        if (trig < 0.0) {
            trig = -trig;
        }
        intensity = trig;
        NuVecRotateY(&creditsdir[1], &ZVec, -creditsang[1]);
        creditsdircol[1].r = intensity;
        creditsang[1] += 256;
        trig = NuTrigTable[creditsang[2] & 0xFFFF];
        if (trig < 0.0) {
            trig = -trig;
        }
        intensity = trig;
        NuVecRotateX(&creditsdir[2], &ZVec, creditsang[2]);
        creditsang[2] += 256;
        creditsdircol[2].r = intensity;
        NuRndrSetDirectionalLights(&creditsdir[0], &creditsdircol[0], &creditsdir[1],
                                   &creditsdircol[1], &creditsdir[2], &creditsdircol[2]);
        NuRndrSetAmbientLight(&creditsrgb);
    } else {
        SetLights(&lcol[0], &ldir[0], &lcol[1], &ldir[1], &lcol[2], &ldir[2], &acol);
    }
}

void SetCreatureLights(struct creature_s *c) {
    struct nuvec_s ambcol;
    struct nuvec_s dir[3];
    struct nucolour3_s col[3];
    f32 f;

    if (USELIGHTS != 0 && LIGHTCREATURES != 0) {
        ambcol = c->lights.AmbCol;
        col[0] = c->lights.pDir1st->Colour;
        dir[0] = c->lights.pDir1st->Direction;
        col[1] = c->lights.pDir2nd->Colour;
        dir[1] = c->lights.pDir2nd->Direction;
        col[2] = c->lights.pDir3rd->Colour;
        dir[2] = c->lights.pDir3rd->Direction;
    } else {
        if (c->obj.dead != 0x11) {
            return;
        }
        ambcol = acol;
        col[0] = lcol[0];
        dir[0] = ldir[0];
        col[1] = lcol[1];
        dir[1] = ldir[1];
        col[2] = lcol[2];
        dir[2] = ldir[2];
    }
    if (c->obj.dead == 0x11) {
        if (c->obj.die_time < c->obj.die_duration) {
            f = 1.0f - c->obj.die_time / c->obj.die_duration;
        } else {
            f = 0.0f;
        }
    } else {
        f = 1.0f;
    }
    if (f != 1.0f) {
        ambcol.x = ambcol.x * f;
        ambcol.y = ambcol.y * f;
        ambcol.z = ambcol.z * f;
        col[0].r = col[0].r * f;
        col[0].g = col[0].g * f;
        col[0].b = col[0].b * f;
        col[1].r = col[1].r * f;
        col[1].g = col[1].g * f;
        col[1].b = col[1].b * f;
        col[2].r = col[2].r * f;
        col[2].g = col[2].g * f;
        col[2].b = col[2].b * f;
    }
    SetLights(&col[0], &dir[0], &col[1], &dir[1], &col[2], &dir[2], &ambcol);
}

void SetNearestLights(struct Nearest_Light_s *l) {
    if (USELIGHTS != 0 && LIGHTCREATURES != 0) {
        SetLights(&l->pDir1st->Colour, &l->pDir1st->Direction, &l->pDir2nd->Colour,
                  &l->pDir2nd->Direction, &l->pDir3rd->Colour, &l->pDir3rd->Direction,
                  &l->AmbCol);
    }
}
