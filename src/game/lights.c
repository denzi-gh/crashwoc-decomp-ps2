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

/* lights_s: type@0x0, globalflag@0x3C, stride 0x48 (verified in UpdateGlobals). */
struct lights_s {
    s32 type;          /* 0x00 */
    u8 unk_0x04[0x38];
    u8 globalflag;     /* 0x3C */
    u8 unk_0x3D[0xB];
};                     /* 0x48 */

extern struct lights_s Lights[];

extern void FindNearestLights(struct nuvec_s *pos, struct Nearest_Light_s *nl,
                              s32 mode);
extern void FindLightProportion(struct nuvec_s *pos, struct Nearest_Light_s *nl);
extern void NuRndrSetDirectionalLights(struct nuvec_s *d0, struct nucolour3_s *c0,
                                       struct nuvec_s *d1, struct nucolour3_s *c1,
                                       struct nuvec_s *d2, struct nucolour3_s *c2);
extern void NuRndrSetAmbientLight(struct nuvec_s *amb);
extern void NuVecRotateX(struct nuvec_s *out, struct nuvec_s *in, s32 ang);
extern void NuVecRotateY(struct nuvec_s *out, struct nuvec_s *in, s32 ang);


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

void UpdateGlobals(struct Nearest_Light_s *nl) {
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

void SetNearestLights(struct Nearest_Light_s *l) {
    if (USELIGHTS != 0 && LIGHTCREATURES != 0) {
        SetLights(&l->pDir1st->Colour, &l->pDir1st->Direction, &l->pDir2nd->Colour,
                  &l->pDir2nd->Direction, &l->pDir3rd->Colour, &l->pDir3rd->Direction,
                  &l->AmbCol);
    }
}

void ScaleColour(struct nucolour3_s *colour, u8 r, u8 g, u8 b, u8 power) {
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
