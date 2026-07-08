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
typedef unsigned short u16;
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

extern struct objtab_s ObjTab[201];
extern float PANEL3DMULX, PANEL3DMULY;

extern void NuMtxSetScale(struct numtx_s *, struct nuvec_s *);
extern void NuMtxRotateX(struct numtx_s *, u16);
extern void NuMtxRotateY(struct numtx_s *, u16);
extern void NuMtxRotateZ(struct numtx_s *, u16);
extern s32 NuRndrGScnObj(void *, struct numtx_s *);

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
