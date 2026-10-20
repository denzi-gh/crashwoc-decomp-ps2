/*
 * Unit: gamelib/edgra
 *
 * Functions:
 *   0x001b5e70 edgraInitAllClumps
 *   0x001b6180 edgracbInstanceMenu
 *   0x001b6318 edgracbClumpPropertiesMenu
 *   0x001b64b8 edgraDetermineNearestClump
 *   0x001b65f8 edgraClumpCreate
 *   0x001b66f8 edgraDoInput
 *   0x001b6a30 edgraDrawCursor
 *   0x001b6d60 edgraFileSave
 *   0x001b6eb0 edgraFileLoad
 *   0x001b7060 edgraRender
 *   0x001b7190 edgraInit
 *   0x001b73f0 edgraClumpsReset
 *   0x001b7418 edgracbSetInstanceType
 *   0x001b7460 edgracbSetClumpWind
 *   0x001b74a8 edgracbSetClumpHeight
 *   0x001b74f0 edgracbToggleClumpReactive
 *   0x001b7520 edgracbCancelOptMenu
 *   0x001b7528 edgracbCancelInstanceMenu
 *   0x001b7550 edgracbCancelClumpPropertiesMenu
 *   0x001b7578 edgraClose
 *   0x001b7598 edgraEnter
 *   0x001b75b8 edgraClumpPlace
 *   0x001b7608 edgraClumpDestroy
 *   0x001b7638 edgraProc
 *   0x001b7698 edgracbFileSave
 *   0x001b7770 edgracbFileLoad
 */


#include "nu_libc.h"

#include "numath/numathtypes.h"
#include "numath/nuvec.h"
#include "nu3d/numtl.h"
#include "nu3d/nugobj.h"
#include "nu3d/nurndr.h"
#include "nu3d/nugscn.h"
#include "nups2/ps2ctl.h"
#include "edtools/edui.h"
#include "edtools/edmain.h"

struct grassclump_s {
    int type;
    int size;
    struct nuvec_s pos;
    float radius;
    float wind;
    int reactive;
    float height;
};

struct eduimenu_s *edgra_active_menu = 0;
struct eduimenu_s *edgra_options_menu = 0;
struct eduimenu_s *edgra_instance_menu = 0;
struct eduimenu_s *edgra_clumpproperties_menu = 0;
extern float edgra_radius;
extern int edgra_nearest;
extern int edgra_instance_type;
extern int edgra_next_clump;
extern int edgra_clump_size;
extern int edgra_units_used;
extern int edgra_clumps_used;
extern char edbits_what_game;
extern char edbits_level_filename[];
extern struct nugscn_s *edbits_base_scene;
extern float edgra_cam_ax;
extern float edgra_cam_ay;
extern void *app_fnt;
extern struct numtl_s *edgra_mtl;
extern struct numtl_s *edgra_mtl_zoff;
extern struct nuvec_s edgra_cam_pos;
extern struct grassclump_s GrassClumps[64];

extern void eduiMenuDestroy(void *menu);
extern void *eduiItemSelCreate(int value, struct eduistyle_s *style, int on, int group, void *cb,
                               char *label);
extern void NuFntScale(int x, int y);
extern void edcamSet(void);
extern void edbitsDrawDiagonalCross(float x, float y, float z, float size, int colour, void *mtl);
extern void eduiMenuDetach(void *menu);
extern void edcamMove(void);
extern void NuWindInit(void);
extern void NuWindCreate(struct nuinstance_s *instance, struct nuvec4_s *points, short count,
                         float wind, float height, int reactive);
extern int edqrand(void);
extern float NuFsqrt(float x);
extern float NuTrigTable[];
extern void NuRndrRect2di(int x, int y, int w, int h, int colour, void *mtl);
extern void NuRndrLine2di(int x0, int y0, int x1, int y1, int colour, void *mtl);
extern void edbitsDrawCircleXY(struct nuvec_s *pos, float radius, unsigned int colour, void *mtl);
extern void NuFntSet(void *fnt);
extern void NuFntSetPen(int colour);
extern void NuFntPrintEx(int x, int y, int flags, char *fmt, ...);
extern void edcamSetPos(struct nuvec_s *pos);
extern void edcamGetPosAng(struct nuvec_s *pos, float *ax, float *ay);
extern void eduiMenuProcess(void *menu, struct nupad_s *pad);
extern int NuFileSize(char *name);
extern int edbitsLookupInstance(char *name);
extern void EdFileSetMedia(int media);
extern int EdFileOpen(char *name, int mode);
extern int EdFileClose(void);
extern void EdFileWrite(void *data, int size);
extern void EdFileRead(void *data, int size);
extern void EdFileWriteInt(int data);
extern void EdFileWriteFloat(float data);
extern int EdFileReadInt(void);
extern float EdFileReadFloat(void);
extern struct eduimenu_s *eduiMenuCreate(int x, int y, int w, int h, void *font,
                                         void *cancel, char *title);
extern void *eduiMenuAddItem(struct eduimenu_s *menu, void *item);
extern int eduiMenuAttach(struct eduimenu_s *menu, struct eduimenu_s *child);
extern void *eduiItemCheckCreate(unsigned int value, struct eduistyle_s *style, int checked, int group,
                                 void *cb, char *label);
extern void *eduiItemToggleCreate(int value, struct eduistyle_s *style, int on, int group,
                                  void *cb, char *label);
extern void *eduiItemSliderCreate(void *data, struct eduistyle_s *style, int group, void *cb,
                                  float min, float max, float value, char *label);
extern void edgracbSetInstanceType(void *menu, struct eduiitem_s *item);
extern void edgracbSetClumpWind(void *menu, struct eduiitem_s *item);
extern void edgracbSetClumpHeight(void *menu, struct eduiitem_s *item);
extern void edgracbToggleClumpReactive(void *menu, struct eduiitem_s *item);
extern void edgracbCancelInstanceMenu(void);
extern void edgracbCancelClumpPropertiesMenu(void);
extern void edgracbCancelOptMenu(void);
extern void edgracbFileSave(void *menu);
extern void edgracbFileLoad(void *menu);

void edgraInit(void);
void edgraClose(void);
void edgraEnter(void);
int edgraProc(struct nupad_s *pad);
void edgraRender(void);

void edgraInitAllClumps(void)
{
    struct nuvec4_s points[256];
    float sway;
    float r;
    int angle;
    int i;
    int j;

    NuWindInit();
    edgra_units_used = 0;
    edgra_clumps_used = 0;
    for (i = 0; i < 64; i++) {
        if (GrassClumps[i].size != 0) {
            for (j = 0; j < GrassClumps[i].size; j++) {
                angle = edqrand();
                r = edqrand() * (GrassClumps[i].radius * (1.0f / 65536.0f));
                points[j].x = NuTrigTable[angle & 0xFFFF] * r;
                points[j].y = 0.0f;
                points[j].z = NuTrigTable[(angle + 0x4000) & 0xFFFF] * r;
                r = NuFsqrt(points[j].x * points[j].x + points[j].z * points[j].z);
                points[j].x *= r + 1.5f;
                points[j].z *= r + 1.5f;
                r *= 1.25f;
                sway = (edqrand() - 0x4000) * (1.0f / 65536.0f);
                points[j].w = 1.2f - r + sway * sway;
                if (points[j].w < 0.2f) {
                    points[j].w = 0.2f;
                }
                points[j].x += GrassClumps[i].pos.x;
                points[j].y += GrassClumps[i].pos.y;
                points[j].z += GrassClumps[i].pos.z;
            }
            NuWindCreate(edbits_base_scene->specials[GrassClumps[i].type].instance, points,
                         GrassClumps[i].size, GrassClumps[i].wind, GrassClumps[i].height,
                         GrassClumps[i].reactive);
            edgra_units_used += GrassClumps[i].size;
            edgra_clumps_used++;
        }
    }
}


void edgracbInstanceMenu(struct eduimenu_s *menu)
{
    struct eduistyle_s style = {{0x80000000, 0x80FF0000}, {0x80808080, 0x80404040}};
    int checked;
    int i;

    edgra_instance_menu = eduiMenuCreate(70, 70, 180, 250, app_fnt, edgracbCancelInstanceMenu,
                                         "Instance Select");
    if (edgra_instance_menu != 0 && edbits_base_scene != 0) {
        eduiMenuAddItem(edgra_instance_menu,
                        eduiItemCheckCreate(-1, &style, edgra_instance_type == -1, 1,
                                            edgracbSetInstanceType, "None"));
        for (i = 0; i < edbits_base_scene->numspecial; i++) {
            if (edgra_instance_type == i) {
                checked = 1;
            } else {
                checked = 0;
            }
            eduiMenuAddItem(edgra_instance_menu,
                            eduiItemCheckCreate(i, &style, checked, 1, edgracbSetInstanceType,
                                                edbits_base_scene->specials[i].name));
            if (checked == 1) {
                edgra_instance_menu->current = edui_last_item;
            }
        }
        eduiMenuAttach(menu, edgra_instance_menu);
        edgra_instance_menu->x = menu->x + 10;
        edgra_instance_menu->y = menu->y + 40;
    }
}


void edgracbClumpPropertiesMenu(struct eduimenu_s *menu)
{
    struct eduistyle_s style = {{0x80000000, 0x80FF0000}, {0x80808080, 0x80404040}};

    if (edgra_nearest != -1 && GrassClumps[edgra_nearest].size != 0) {
        edgra_clumpproperties_menu = eduiMenuCreate(70, 70, 180, 250, app_fnt,
                                                    edgracbCancelClumpPropertiesMenu,
                                                    "Clump Properties");
        if (edgra_clumpproperties_menu != 0) {
            eduiMenuAddItem(edgra_clumpproperties_menu,
                            eduiItemSliderCreate(0, &style, 0, edgracbSetClumpWind, 0.01f, 1.99f,
                                                 GrassClumps[edgra_nearest].wind,
                                                 "Wind"));
            eduiMenuAddItem(edgra_clumpproperties_menu,
                            eduiItemSliderCreate(0, &style, 0, edgracbSetClumpHeight, 0.1f, 9.9f,
                                                 GrassClumps[edgra_nearest].height,
                                                 "Model Height"));
            eduiMenuAddItem(edgra_clumpproperties_menu,
                            eduiItemToggleCreate(0, &style, GrassClumps[edgra_nearest].reactive, 1,
                                                 edgracbToggleClumpReactive,
                                                 "React to Crash"));
            eduiMenuAttach(menu, edgra_clumpproperties_menu);
            edgra_clumpproperties_menu->x = menu->x + 10;
            edgra_clumpproperties_menu->y = menu->y + 40;
        }
    }
}


void edgraDetermineNearestClump(float range)
{
    struct nuvec_s d;
    float dist;
    int i;

    if (edgra_nearest != -1) {
        NuVecSub(&d, &edgra_cam_pos, &GrassClumps[edgra_nearest].pos);
        if (d.x * d.x + d.y * d.y + d.z * d.z == 0.0) {
            return;
        }
    }
    edgra_nearest = -1;
    for (i = 0; i < 64; i++) {
        if (GrassClumps[i].size != 0) {
            NuVecSub(&d, &edgra_cam_pos, &GrassClumps[i].pos);
            dist = d.x * d.x + d.y * d.y + d.z * d.z;
            if (range < 0.0f || dist < range) {
                edgra_nearest = i;
                range = dist;
            }
        }
    }
}


inline void edgraClumpPlace(int clump, struct nuvec_s *pos)
{
    GrassClumps[clump].pos = *pos;
    GrassClumps[clump].radius = edgra_radius;
    edgraInitAllClumps();
}


int edgraClumpCreate(struct nuvec_s *pos)
{
    int clump;

    if (edgra_units_used + edgra_clump_size > 0x400) {
        return -1;
    }
    clump = edgra_next_clump;
    do {
        if (!GrassClumps[edgra_next_clump].size) {
            break;
        }
        edgra_next_clump++;
        if (edgra_next_clump > 64) {
            edgra_next_clump = 0;
        }
        if (edgra_next_clump == clump) {
            return -1;
        }
    } while (1);
    GrassClumps[edgra_next_clump].type = edgra_instance_type;
    GrassClumps[edgra_next_clump].size = edgra_clump_size;
    GrassClumps[edgra_next_clump].wind = 0.2f;
    GrassClumps[edgra_next_clump].reactive = 1;
    GrassClumps[edgra_next_clump].height = 1.0f;
    edgraClumpPlace(edgra_next_clump, pos);
    clump = edgra_next_clump++;
    return clump;
}


inline void edgraClumpDestroy(int clump)
{
    GrassClumps[clump].size = 0;
    edgraInitAllClumps();
}


void edgraDoInput(struct nupad_s *pad)
{
    int i;

    if (!(pad->paddata & 0x100)) {
        edcamMove();
    }
    if (pad->paddata & 0x100) {
        if (edgra_nearest != -1) {
            if (pad->paddata_db & 0x8) {
                i = edgra_nearest;
                do {
                    i++;
                    if (i == 64) {
                        i %= 64;
                    }
                } while (GrassClumps[i].size == 0);
                edgra_nearest = i;
            }
            if (pad->paddata_db & 0x2) {
                do {
                    edgra_nearest--;
                    if (edgra_nearest == -1) {
                        edgra_nearest = 63;
                    }
                } while (GrassClumps[edgra_nearest].size == 0);
            }
        } else {
            edgraDetermineNearestClump(-1.0f);
        }
        if (edgra_nearest != -1) {
            edcamSetPos(&GrassClumps[edgra_nearest].pos);
            edgra_radius = GrassClumps[edgra_nearest].radius;
            edgra_clump_size = GrassClumps[edgra_nearest].size;
        }
    }
    edcamGetPosAng(&edgra_cam_pos, &edgra_cam_ax, &edgra_cam_ay);
    if (!(pad->paddata & 0x100)) {
        if (pad->paddata_db & 0x80) {
            edgra_active_menu = edgra_options_menu;
        }
        if ((pad->paddata_db & 0x40) && edgra_instance_type != -1) {
            edgraClumpCreate(&edgra_cam_pos);
        }
        if ((pad->paddata & 0x20) && edgra_nearest != -1) {
            edgraClumpPlace(edgra_nearest, &edgra_cam_pos);
        }
        if (pad->paddata_db & 0x10) {
            if (edgra_nearest != -1) {
                edgraClumpDestroy(edgra_nearest);
            }
            edgra_nearest = -1;
        }
    }
    edgra_radius += pad->rdata.l_algpad_u / 5000.0f;
    edgra_radius -= pad->rdata.l_algpad_d / 5000.0f;
    if (edgra_radius < 0.1f) {
        edgra_radius = 0.1f;
    }
    if (edgra_radius > 10.0f) {
        edgra_radius = 10.0f;
    }
    if (pad->rdata.l_algpad_l == 0xFF || (pad->paddata_db & 0x8000)) {
        edgra_clump_size--;
    }
    if (pad->rdata.l_algpad_r == 0xFF || (pad->paddata_db & 0x2000)) {
        edgra_clump_size++;
    }
    if (edgra_clump_size < 1) {
        edgra_clump_size = 1;
    }
    if (edgra_clump_size > 256) {
        edgra_clump_size = 256;
    }
}


void edgraDrawCursor(void)
{
    struct nuvtx_tc1_s line[2];

    line[0].pnt.x = edgra_cam_pos.x - 0.5f;
    line[0].pnt.y = edgra_cam_pos.y;
    line[0].pnt.z = edgra_cam_pos.z;
    line[1].pnt.x = edgra_cam_pos.x + 0.5f;
    line[1].pnt.y = edgra_cam_pos.y;
    line[1].pnt.z = edgra_cam_pos.z;
    line[0].diffuse = -1;
    line[1].diffuse = -1;
    NuRndrLine3d(line, edgra_mtl, 0);
    line[0].pnt.x = edgra_cam_pos.x;
    line[0].pnt.y = edgra_cam_pos.y - 0.5f;
    line[0].pnt.z = edgra_cam_pos.z;
    line[1].pnt.x = edgra_cam_pos.x;
    line[1].pnt.y = edgra_cam_pos.y + 0.5f;
    line[1].pnt.z = edgra_cam_pos.z;
    line[0].diffuse = -1;
    line[1].diffuse = -1;
    NuRndrLine3d(line, edgra_mtl, 0);
    line[0].pnt.x = edgra_cam_pos.x;
    line[0].pnt.y = edgra_cam_pos.y;
    line[0].pnt.z = edgra_cam_pos.z - 0.5f;
    line[1].pnt.x = edgra_cam_pos.x;
    line[1].pnt.y = edgra_cam_pos.y;
    line[1].pnt.z = edgra_cam_pos.z + 0.5f;
    line[0].diffuse = -1;
    line[1].diffuse = -1;
    NuRndrLine3d(line, edgra_mtl, 0);
    edbitsDrawCircleXY(&edgra_cam_pos, edgra_radius, 0xFFFFFFFF, edgra_mtl);
    NuRndrRect2di(0x1720, 0xA50, 0xDC0, 0x370, 0x80808080, edgra_mtl_zoff);
    NuRndrRect2di(0x1710, 0x990, 0xDE0, 0xC0, 0x80000000, edgra_mtl_zoff);
    NuRndrLine2di(0x1710, 0xA50, 0x1710, 0xDC8, 0x80000000, edgra_mtl_zoff);
    NuRndrLine2di(0x24F0, 0xA50, 0x24F0, 0xDC8, 0x80000000, edgra_mtl_zoff);
    NuRndrLine2di(0x1710, 0xDC8, 0x24F0, 0xDC8, 0x80000000, edgra_mtl_zoff);
    NuFntSet(app_fnt);
    NuFntSetPen(0x80808080);
    NuFntPrintEx(0x17C0, 0xA28, 0, "Info Box");
    NuFntSetPen(0x80000000);
    if (edgra_instance_type == -1) {
        NuFntPrintEx(0x17C0, 0xAF0, 0, "Curr Inst: <none>");
    } else {
        NuFntPrintEx(0x17C0, 0xAF0, 0, "Curr Inst: %s",
                     edbits_base_scene->specials[edgra_instance_type].name);
    }
    NuFntPrintEx(0x17C0, 0xB90, 0, "Base Radius: %0.2f", edgra_radius);
    NuFntPrintEx(0x17C0, 0xC30, 0, "Clump Size: %d", edgra_clump_size);
    NuFntPrintEx(0x17C0, 0xCD0, 0, "Used: %d/%d,%d/%d", edgra_units_used, 0x400, edgra_clumps_used, 64);
    NuFntPrintEx(0x17C0, 0xD70, 0, "%5.2f", edgra_cam_pos.x);
    NuFntPrintEx(0x1C20, 0xD70, 0, "%5.2f", edgra_cam_pos.y);
    NuFntPrintEx(0x2080, 0xD70, 0, "%5.2f", edgra_cam_pos.z);
}


int edgraFileSave(char *filename)
{
    char name[32];
    int count;
    int i;

    count = 0;
    for (i = 0; i < 64; i++) {
        if (GrassClumps[i].size != 0) {
            count++;
        }
    }
    EdFileSetMedia(1);
    if (EdFileOpen(filename, 1)) {
        EdFileWriteInt(3);
        EdFileWriteInt(count);
        for (i = 0; i < 64; i++) {
            if (GrassClumps[i].size != 0) {
                strncpy(name, edbits_base_scene->specials[GrassClumps[i].type].name, 20);
                name[20] = 0;
                EdFileWrite(name, 20);
                EdFileWriteInt(GrassClumps[i].size);
                EdFileWrite(&GrassClumps[i].pos, 12);
                EdFileWriteFloat(GrassClumps[i].radius);
                EdFileWriteFloat(GrassClumps[i].wind);
                EdFileWriteInt(GrassClumps[i].reactive);
                EdFileWriteFloat(GrassClumps[i].height);
            }
        }
        EdFileClose();
        return 1;
    }
    return 0;
}


int edgraFileLoad(char *filename)
{
    char name[32];
    int latest = 3;
    int version;
    int count;
    int i;

    EdFileSetMedia(1);
    if (!EdFileOpen(filename, 0)) {
        return 0;
    }
    version = EdFileReadInt();
    if (latest < version) {
        EdFileClose();
        return 0;
    }
    count = EdFileReadInt();
    for (i = 0; i < count; i++) {
        EdFileRead(name, 20);
        GrassClumps[i].type = edbitsLookupInstance(name);
        GrassClumps[i].size = EdFileReadInt();
        if (GrassClumps[i].type == -1) {
            GrassClumps[i].size = 0;
        }
        EdFileRead(&GrassClumps[i].pos, 12);
        GrassClumps[i].radius = EdFileReadFloat();
        GrassClumps[i].wind = EdFileReadFloat();
        if (version >= 2) {
            GrassClumps[i].reactive = EdFileReadInt();
        } else {
            GrassClumps[i].reactive = 1;
        }
        if (version >= 3) {
            GrassClumps[i].height = EdFileReadFloat();
        } else {
            GrassClumps[i].height = 1.0f;
        }
    }
    EdFileClose();
    edgraInitAllClumps();
    edgra_nearest = -1;
    edgra_next_clump = count;
    edgraDetermineNearestClump(1.0f);
    return 1;
}


void edgraRender(void)
{
    struct nuvec_s pos;

    edcamSet();
    edgraDrawCursor();
    if (edgra_nearest != -1) {
        edbitsDrawDiagonalCross(GrassClumps[edgra_nearest].pos.x, GrassClumps[edgra_nearest].pos.y,
                                GrassClumps[edgra_nearest].pos.z, 0.125f, 0xFF80FFFF,
                                edgra_mtl_zoff);
        edbitsDrawCircleXY(&GrassClumps[edgra_nearest].pos, GrassClumps[edgra_nearest].radius,
                           0xFF80FFFF, edgra_mtl_zoff);
        pos = GrassClumps[edgra_nearest].pos;
        pos.y += GrassClumps[edgra_nearest].height;
        edbitsDrawCircleXY(&pos, GrassClumps[edgra_nearest].radius, 0xFF80FFFF, edgra_mtl_zoff);
    }
    if (edgra_active_menu != 0) {
        NuFntScale(12, 12);
        eduiMenuRender(edgra_active_menu);
    }
}


inline void edgraClumpsReset(void)
{
    int i;

    for (i = 0; i < 64; i++) {
        GrassClumps[i].size = 0;
    }
    edgra_next_clump = 0;
}


/* The inline callbacks' strings sit here in retail, their code is deferred to the end. */
inline void edgracbFileSave(void *menu)
{
    char filename[256];

    if (edbits_what_game == 1) {
        sprintf(filename, "%s\\Grass.dat", edbits_level_filename);
        if (edgraFileSave(filename)) {
            eduiCreateMessageMenu(menu, "Saved OK", 1);
        } else {
            eduiCreateMessageMenu(menu, "File Save Error", 0);
        }
    } else if (edbits_what_game == 2) {
        sprintf(filename, "%s.gra", edbits_level_filename);
        if (edgraFileSave(filename)) {
            eduiCreateMessageMenu(menu, "Saved OK", 1);
        } else {
            eduiCreateMessageMenu(menu, "File Save Error", 0);
        }
    } else {
        eduiCreateMessageMenu(menu, "File Save Error", 0);
    }
}


inline void edgracbFileLoad(void *menu)
{
    char filename[256];

    edgraClumpsReset();
    if (edbits_what_game == 1) {
        sprintf(filename, "%s\\Grass.dat", edbits_level_filename);
        if (NuFileSize(filename) > 0) {
            edgraFileLoad(filename);
        }
        eduiCreateMessageMenu(menu, "Loaded OK", 1);
    } else if (edbits_what_game == 2) {
        sprintf(filename, "%s.gra", edbits_level_filename);
        if (NuFileSize(filename) > 0) {
            edgraFileLoad(filename);
        }
        eduiCreateMessageMenu(menu, "Loaded OK", 1);
    } else {
        eduiCreateMessageMenu(menu, "File Load Error", 0);
    }
}


void edgraInit(void)
{
    struct eduistyle_s style = {{0x80000000, 0x80FF0000}, {0x80808080, 0x80404040}};
    struct numtl_s *mtl;
    struct numtl_s *zoff;

    edgraClumpsReset();
    mtl = NuMtlCreate(1);
    edgra_mtl = mtl;
    mtl->attrib.cull = 2;
    mtl->attrib.zmode = 0;
    mtl->diffuse.r = 0.5f;
    mtl->diffuse.g = 0.5f;
    mtl->diffuse.b = 0.5f;
    mtl->alpha = 1.0f;
    mtl->attrib.alpha = 0;
    NuMtlUpdate(mtl);
    zoff = NuMtlCreate(1);
    edgra_mtl_zoff = zoff;
    zoff->attrib.cull = 2;
    zoff->attrib.zmode = 3;
    zoff->diffuse.r = 0.5f;
    zoff->diffuse.g = 0.5f;
    zoff->diffuse.b = 0.5f;
    zoff->alpha = 1.0f;
    zoff->attrib.alpha = 0;
    NuMtlUpdate(zoff);
    edgra_instance_type = -1;
    edgra_options_menu = eduiMenuCreate(70, 70, 220, 300, app_fnt, edgracbCancelOptMenu,
                                        "Options");
    if (edgra_options_menu != 0) {
        eduiMenuAddItem(edgra_options_menu,
                        eduiItemSelCreate(1, &style, 0, 0, edgracbInstanceMenu,
                                          "Instance Select"));
        eduiMenuAddItem(edgra_options_menu,
                        eduiItemSelCreate(1, &style, 0, 0, edgracbClumpPropertiesMenu,
                                          "Clump Properties..."));
        eduiMenuAddItem(edgra_options_menu,
                        eduiItemSelCreate(1, &style, 0, 0, edgracbFileSave,
                                          "Save Grass"));
        eduiMenuAddItem(edgra_options_menu,
                        eduiItemSelCreate(1, &style, 0, 0, edgracbFileLoad,
                                          "Load Grass"));
    }
}


struct edmainreg_s edgradesc = {
    0, 0, "Grass Editor", edgraInit, edgraClose, edgraEnter, {0}, 0,
    EDMAIN_BLOCKID('g', 'r', 'a', 's'), edgraProc, edgraRender
};
struct nuvec_s edgra_cam_pos = { 0.0f, 0.0f, 0.0f };
struct grassclump_s GrassClumps[64] = { { 0 } };
struct numtl_s *edgra_mtl = 0;
struct numtl_s *edgra_mtl_zoff = 0;
float edgra_cam_dist = 0.0f;
float edgra_cam_ax = 0.0f;
float edgra_cam_ay = 0.0f;
float edgra_radius = 0.0f;
int edgra_nearest = 0;
int edgra_instance_type = 0;
int edgra_next_clump = 0;
int edgra_units_used = 0;
int edgra_clumps_used = 0;
int edgra_clump_size = 0;


void edgracbSetInstanceType(void *menu, struct eduiitem_s *item)
{
    eduiMenuDetach(menu);
    eduiMenuDestroy(menu);
    edgra_instance_menu = 0;
    edgra_instance_type = item->value;
    edgra_active_menu = 0;
}


void edgracbSetClumpWind(void *menu, struct eduiitem_s *item)
{
    if (edgra_nearest != -1) {
        GrassClumps[edgra_nearest].wind = item->u.slider.val;
    }
    edgraInitAllClumps();
}


void edgracbSetClumpHeight(void *menu, struct eduiitem_s *item)
{
    if (edgra_nearest != -1) {
        GrassClumps[edgra_nearest].height = item->u.slider.val;
    }
    edgraInitAllClumps();
}


void edgracbToggleClumpReactive(void *menu, struct eduiitem_s *item)
{
    if (edgra_nearest != -1) {
        GrassClumps[edgra_nearest].reactive = item->on;
    }
}


void edgracbCancelOptMenu(void)
{
    edgra_active_menu = 0;
}


void edgracbCancelInstanceMenu(void)
{
    eduiMenuDestroy(edgra_instance_menu);
    edgra_instance_menu = 0;
}


void edgracbCancelClumpPropertiesMenu(void)
{
    eduiMenuDestroy(edgra_clumpproperties_menu);
    edgra_clumpproperties_menu = 0;
}


void edgraClose(void)
{
    eduiMenuDestroy(edgra_options_menu);
}


void edgraEnter(void)
{
    edgra_nearest = -1;
    edgra_radius = 0.5f;
    edgra_clump_size = 64;
}


int edgraProc(struct nupad_s *pad)
{
    if (edgra_active_menu != 0) {
        eduiMenuProcess(edgra_active_menu, pad);
        return 0;
    }
    edgraDoInput(pad);
    edgraDetermineNearestClump(1.0f);
    if (pad->paddata_db & 0x800) {
        return 1;
    }
    return 0;
}
