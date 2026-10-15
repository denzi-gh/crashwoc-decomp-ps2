/*
 * Unit: gamelib/edbits
 *
 * Functions:
 *   0x001a6b68 edbitsSoundPlay
 *   0x001a6cd8 edqrand
 *   0x001a6cf8 edbitsRegisterLevel
 *   0x001a6d48 edbitsRegisterSfx
 *   0x001a6d60 edbitsDrawCube
 *   0x001a70c0 edbitsDrawDiagonalCross
 *   0x001a7228 edbitsLookupSoundFX
 *   0x001a7308 edbitsGetSoundName
 *   0x001a7340 edbitsDrawCircleXY
 *   0x001a7468 edbitsLookupInstance
 *   0x001a74f0 edbitsSetSoundFxVolume
 *   0x001a74f8 edbitsRegisterBaseScene
 */

#include "nu_libc.h"

#include "numath/numathtypes.h"
#include "numath/nuvec.h"
#include "nu3d/numtl.h"
#include "nu3d/nugobj.h"
#include "nu3d/nugscn.h"

struct edsfx_s {
    char name[16];
    unsigned short pitch;
    unsigned short volume;
    char pad14[0x30 - 0x14];
};

extern int debris_sfx;
extern int edqseed;
extern int edbitsSfxVol;
extern signed char edbits_what_game;
extern struct edsfx_s *edSfxGlobalTab;
extern struct edsfx_s *edSfxLevelTab;
extern int edSfxAllCount;
extern int edSfxGlobalCount;
extern struct nugscn_s *edbits_base_scene;
extern char edbits_level_filename[];

extern int gamesfx_effect_volume;
extern int gamesfx_pitch;
extern int gamesfx_edbits;

extern float NuTrigTable[];

extern void GameSfx(int sfx, struct nuvec_s *pos);
extern void NuSoundPlay3d(struct nuvec_s *pos, int sfx, int volL, int volR, int pitch);
extern void NuStrCpy(char *dst, char *src);
extern int NuRndrLine3d(struct nuvtx_tc1_s *line, struct numtl_s *mtl, struct numtx_s *mtx);


void edbitsSoundPlay(struct nuvec_s *pos, int sfx) {
    int index;

    if (edbits_what_game == 2) {
        if (debris_sfx ? sfx < edSfxGlobalCount - 20 : sfx < edSfxGlobalCount) {
            gamesfx_effect_volume = edSfxGlobalTab[sfx].volume;
            gamesfx_pitch = edSfxGlobalTab[sfx].pitch;
        } else {
            if (debris_sfx) {
                sfx += 20;
            }
            index = sfx - edSfxGlobalCount;
            gamesfx_effect_volume = edSfxLevelTab[index].volume;
            gamesfx_pitch = edSfxLevelTab[index].pitch;
        }
        gamesfx_edbits = 1;
        GameSfx(sfx, pos);
        debris_sfx = 0;
    } else if (sfx < edSfxGlobalCount) {
        NuSoundPlay3d(pos, sfx, edSfxGlobalTab[sfx].volume * edbitsSfxVol / 100,
                      edSfxGlobalTab[sfx].volume * edbitsSfxVol / 100, edSfxGlobalTab[sfx].pitch);
    } else {
        index = sfx - edSfxGlobalCount;
        NuSoundPlay3d(pos, sfx, edSfxLevelTab[index].volume * edbitsSfxVol / 100,
                      edSfxLevelTab[index].volume * edbitsSfxVol / 100, edSfxLevelTab[index].pitch);
    }
}


int edqrand(void) {
    edqseed = (edqseed * 0x24CD + 1) & 0xFFFF;
    return edqseed;
}


void edbitsRegisterLevel(char *filename, int game) {
    if (filename) {
        NuStrCpy(edbits_level_filename, filename);
    } else {
        edbits_level_filename[0] = 0;
    }
    edbits_what_game = game;
}


void edbitsRegisterSfx(struct edsfx_s *globaltab, struct edsfx_s *leveltab, int globalcount,
                       int allcount) {
    edSfxGlobalTab = globaltab;
    edSfxLevelTab = leveltab;
    edSfxGlobalCount = globalcount;
    edSfxAllCount = allcount;
}


void edbitsDrawCube(float x, float y, float z, float sizex, float sizey, float sizez,
                    int rotz0, int roty0, int rotz1, int roty1, int colour, struct numtl_s *mtl) {
    struct nuvtx_tc1_s line[2];
    struct nuvec_s a;
    struct nuvec_s b;
    float corners[60] = {
        -1.0f, -1.0f, 1.0f,   1.0f, -1.0f, 1.0f,   1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,    -1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,   -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,   -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, 1.0f,    -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,    1.0f, -1.0f, -1.0f,  1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, 1.0f,     1.0f, -1.0f, 1.0f,
    };
    int i;
    int j;

    for (i = 0; i < 60; i += 15) {
        for (j = 0; j < 12; j += 3) {
            a.x = sizex * corners[i + j];
            a.y = sizey * corners[i + j + 1];
            a.z = sizez * corners[i + j + 2];
            b.x = sizex * corners[i + j + 3];
            b.y = sizey * corners[i + j + 4];
            b.z = sizez * corners[i + j + 5];
            NuVecRotateZ(&a, &a, rotz0);
            NuVecRotateY(&a, &a, roty0);
            NuVecRotateZ(&a, &a, rotz1);
            NuVecRotateY(&a, &a, roty1);
            NuVecRotateZ(&b, &b, rotz0);
            NuVecRotateY(&b, &b, roty0);
            NuVecRotateZ(&b, &b, rotz1);
            NuVecRotateY(&b, &b, roty1);
            line[0].pnt.x = x + a.x;
            line[0].pnt.y = y + a.y;
            line[0].pnt.z = z + a.z;
            line[1].pnt.x = x + b.x;
            line[1].pnt.y = y + b.y;
            line[1].pnt.z = z + b.z;
            line[0].diffuse = colour;
            line[1].diffuse = colour;
            NuRndrLine3d(line, mtl, 0);
        }
    }
}


void edbitsDrawDiagonalCross(float x, float y, float z, float size, int colour,
                             struct numtl_s *mtl) {
    struct nuvtx_tc1_s line[2];
    float diagonals[12] = {
        1.0f, 1.0f, 1.0f,    -1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,   -1.0f, -1.0f, 1.0f,
    };
    int i;

    for (i = 0; i < 12; i += 3) {
        line[0].pnt.x = x + size * diagonals[i];
        line[0].pnt.y = y + size * diagonals[i + 1];
        line[0].pnt.z = z + size * diagonals[i + 2];
        line[0].diffuse = colour;
        line[1].pnt.x = x - size * diagonals[i];
        line[1].pnt.y = y - size * diagonals[i + 1];
        line[1].pnt.z = z - size * diagonals[i + 2];
        line[1].diffuse = colour;
        NuRndrLine3d(line, mtl, 0);
    }
}


int edbitsLookupSoundFX(char *name) {
    int i;

    if (edSfxLevelTab) {
        for (i = 0; i < edSfxAllCount - edSfxGlobalCount; i++) {
            if (!strncmp(edSfxLevelTab[i].name, name, 15)) {
                return i + edSfxGlobalCount;
            }
        }
    }
    if (edSfxGlobalTab) {
        for (i = 0; i < edSfxGlobalCount; i++) {
            if (!strncmp(edSfxGlobalTab[i].name, name, 15)) {
                return i;
            }
        }
    }
    return -1;
}


char *edbitsGetSoundName(int sfx) {
    if (sfx < edSfxGlobalCount) {
        return edSfxGlobalTab[sfx].name;
    }
    return edSfxLevelTab[sfx - edSfxGlobalCount].name;
}


void edbitsDrawCircleXY(struct nuvec_s *centre, float radius, int colour, struct numtl_s *mtl) {
    struct nuvtx_tc1_s line[2];
    int i;
    int angle;
    float *trig;

    line[1].pnt.x = centre->x;
    line[1].pnt.y = centre->y;
    line[1].pnt.z = centre->z + radius;
    line[0].diffuse = colour;
    line[1].diffuse = colour;
    for (i = 1; i < 11; i++) {
        line[0].pnt = line[1].pnt;
        line[1].pnt.x = centre->x;
        line[1].pnt.y = centre->y;
        line[1].pnt.z = centre->z;
        angle = (i << 16) / 10;
        trig = &NuTrigTable[angle & 0xFFFF];
        line[1].pnt.x += *trig * radius;
        trig = &NuTrigTable[(angle + 0x4000) & 0xFFFF];
        line[1].pnt.z += *trig * radius;
        NuRndrLine3d(line, mtl, 0);
    }
}


int edbitsLookupInstance(char *name) {
    int i;

    if (edbits_base_scene) {
        for (i = 0; i < edbits_base_scene->numspecial; i++) {
            if (!strncmp(edbits_base_scene->specials[i].name, name, 19)) {
                return i;
            }
        }
    }
    return -1;
}


void edbitsSetSoundFxVolume(int volume) {
    edbitsSfxVol = volume;
}


void edbitsRegisterBaseScene(struct nugscn_s *scene) {
    edbits_base_scene = scene;
}
