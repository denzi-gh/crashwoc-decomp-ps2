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
extern s32 glb_dir_error_flag;
extern s32 glb_amb_error_flag;
extern f32 sf;
extern f32 sf2;
extern struct nuvec_s v001;

/* lights_s: type@0x0, globalflag@0x3C, stride 0x48 (verified in UpdateGlobals).
 * pos@0x04, r/g/b@0x20..0x22, direction@0x30, brightness@0x3E all verified
 * against FindNearestLights. */
struct lights_s {
    s32 type;                    /* 0x00 */
    struct nuvec_s pos;          /* 0x04 */
    u8 unk_0x10[0x10];           /* 0x10 */
    u8 r;                        /* 0x20 */
    u8 g;                        /* 0x21 */
    u8 b;                        /* 0x22 */
    u8 unk_0x23[0xD];            /* 0x23 */
    struct nuvec_s direction;    /* 0x30 */
    u8 globalflag;               /* 0x3C */
    u8 unk_0x3D;                 /* 0x3D */
    u8 brightness;               /* 0x3E */
    u8 unk_0x3F[0x9];            /* 0x3F */
};                               /* 0x48 */

extern struct lights_s Lights[];

/* Object light block filled by InitObjectLights (dir[3], col[3], AmbCol). */
struct objlights_s {
    struct nuvec_s dir[3];      /* 0x00 */
    struct nucolour3_s col[3];  /* 0x24 */
    struct nuvec_s AmbCol;      /* 0x48 */
};                              /* 0x54 */

extern void FindLightProportion(struct nuvec_s *pos, struct Nearest_Light_s *nl);
extern f32 NuVecDist(struct nuvec_s *a, struct nuvec_s *b, void *c);
extern void NuVecSub(struct nuvec_s *d, struct nuvec_s *a, struct nuvec_s *b);
extern void NuVecNorm(struct nuvec_s *d, struct nuvec_s *a);
extern void NuRndrSetDirectionalLights(struct nuvec_s *d0, struct nucolour3_s *c0,
                                       struct nuvec_s *d1, struct nucolour3_s *c1,
                                       struct nuvec_s *d2, struct nucolour3_s *c2);
extern void NuRndrSetAmbientLight(struct nuvec_s *amb);
extern void NuVecRotateX(struct nuvec_s *out, struct nuvec_s *in, s32 ang);
extern void NuVecRotateY(struct nuvec_s *out, struct nuvec_s *in, s32 ang);
void ResetLights(struct Nearest_Light_s *nl);
s32 FindNearestLights(struct nuvec_s *vec, struct Nearest_Light_s *nearest_light,
                      s32 SearchMode);


inline void SetLights(struct nucolour3_s *vCOL0, struct nuvec_s *vDIR0,
                      struct nucolour3_s *vCOL1, struct nuvec_s *vDIR1,
                      struct nucolour3_s *vCOL2, struct nuvec_s *vDIR2,
                      struct nuvec_s *vAMB) {
    NuRndrSetDirectionalLights(vDIR0, vCOL0, vDIR1, vCOL1, vDIR2, vCOL2);
    NuRndrSetAmbientLight(vAMB);
}

void GetLights(struct nuvec_s *pos, struct Nearest_Light_s *nearest_lights,
               s32 SearchMode) {
    FindNearestLights(pos, nearest_lights, SearchMode);
    FindLightProportion(pos, nearest_lights);
}

void RotateDirectionalLight(struct nuvec_s *v, s32 xrot, s32 yrot) {
    NuVecRotateX(v, &v001, xrot);
    NuVecRotateY(v, v, yrot);
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

void SetNearestLights(struct Nearest_Light_s *l) {
    if (USELIGHTS != 0 && LIGHTCREATURES != 0) {
        SetLights(&l->pDir1st->Colour, &l->pDir1st->Direction, &l->pDir2nd->Colour,
                  &l->pDir2nd->Direction, &l->pDir3rd->Colour, &l->pDir3rd->Direction,
                  &l->AmbCol);
    }
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
