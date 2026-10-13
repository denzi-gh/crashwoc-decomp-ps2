/*
 * Unit: nu3d/nucvtskn
 *
 * Functions:
 *   0x00155408 ShareEdge
 *   0x001555f0 WriteWeights
 *   0x001558d8 AddPointDataToPacket
 *   0x00155e68 GenModData
 *   0x00156168 EndSkinPacket
 *   0x00156350 FindStripNext
 *   0x00156520 AddStripTri
 *   0x00156768 GenStrips
 *   0x00156938 NuPs2CreateSkinDma
 *   0x00156fa8 SortPrimdefs
 *   0x00157700 SetVtxSkinData
 *   0x00157920 NuPs2CreateSkinNorm
 *   0x00158108 NuPs2CreateSkinSmoothOld
 *   0x00158900 NuPs2CreateSkinSmooth
 *   0x00158cb8 NuPs2CreateSkin
 *   0x00158d38 NuPs2DestroySkin
 *   0x00158de8 CloseSkinning
 *   0x00158e18 InitSkinning
 *   0x00158e68 ResetSkinning
 *   0x00158e78 NuPs2SetTempSkinBuffer
 *   0x00158ec8 NuCvtSknInstFix
 *   0x00158f08 NuCvtSknInstUnFix
 *   0x00158f48 PrevPktIdx
 */

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef struct SkinDmaTag {
    u16 qwc;
    u8 mode;
    u8 id;
    u32 addr;
    u32 data[2];
} SkinDmaTag;

typedef union VifWord {
    u32 code;
    float weight;
} VifWord;

typedef struct SkinPrimDef {
    char vrts[3][0x48];
    int vid[3];
    int padE4[3];
    int adj[3];
    char padFC[0x90];
} SkinPrimDef;

typedef struct SkinV {
    void *pnts;
    void *nrms;
    void *pid;
    void *nid;
} SkinV;

typedef struct NuPrim {
    struct NuPrim *next;
    int type;
} NuPrim;

typedef struct NuGeom {
    struct NuGeom *next;
    char pad04[0x28];
    NuPrim *prim;
    char pad30[0x14];
    SkinV *skinv;
} NuGeom;

typedef struct NuGobj {
    char pad00[0x0C];
    NuGeom *geom;
} NuGobj;


extern void *NuMemAllocFn(int size, char *file, int line);
extern void NuMemFreeFn(void *ptr, char *file, int line);
extern void *memset(void *s, int c, unsigned int n);
extern void *NuStreamGetOutputBuffer(int stream);
extern void NuPs2CreateSkinNorm(NuGobj *gobj);
extern void NuPs2CreateSkinSmooth(NuGobj *gobj);

extern char D_006156B0[];
extern void *D_00633120;
extern void *D_00633124;
extern char *D_00633128;
extern int D_0063312C;
extern int D_00633134;
extern int D_00633154;
extern int D_00633160;
extern char *D_00633178;
extern char *D_00633180;
extern int D_00633184;
extern char *skinbuffer_free;
extern int D_0062EF1C;
extern int D_006AE320[];
extern SkinPrimDef *D_00633174;
extern int D_006AE518[];
extern int D_002E9040[3][2];

#define nucvtskn_srcfile D_006156B0
#define stream_buffer0 D_00633120
#define stream_buffer1 D_00633124
#define skinbuffer D_00633128
#define skinbuffer_size D_0063312C
#define pkt_count D_00633134
#define pkt_reuse_count D_00633154
#define pkt_reuse_total D_00633160
#define pkt_reuse_max D_0062EF1C
#define pkt_ids D_006AE320
#define primdefs D_00633174
#define strip_edge D_006AE518
#define tri_edges D_002E9040
#define saved_skinbuffer D_00633178
#define saved_skinbuffer_free D_00633180
#define saved_skinbuffer_size D_00633184


int ShareEdge(SkinPrimDef *defs, int a, int b) {
    SkinPrimDef *pa;
    SkinPrimDef *pb;
    int i;
    int j;

    pa = &defs[a];
    pb = &defs[b];
    for (i = 0; i < 3; i++) {
        if (pa->adj[i] != -1) {
            continue;
        }
        for (j = 0; j < 3; j++) {
            if (pb->adj[j] != -1) {
                continue;
            }
            if (pa->vid[tri_edges[i][0]] == pb->vid[tri_edges[j][0]] && pa->vid[tri_edges[i][1]] == pb->vid[tri_edges[j][1]]) {
                pa->adj[i] = b;
                pb->adj[j] = a;
                return 1;
            }
            if (pa->vid[tri_edges[i][0]] == pb->vid[tri_edges[j][1]] && pa->vid[tri_edges[i][1]] == pb->vid[tri_edges[j][0]]) {
                pa->adj[i] = b;
                pb->adj[j] = a;
                return 1;
            }
        }
    }
    return 0;
}


VifWord *WriteWeights(VifWord *pkt, int addr, int cnt, float *weights) {
    int i;

    switch (cnt) {
    case 0:
        break;
    case 1:
        (pkt++)->code = addr | 0x60018000;
        for (i = 0; i < cnt; i++) {
            (pkt++)->weight = weights[i];
        }
        break;
    case 2:
        (pkt++)->code = addr | 0x64018000;
        for (i = 0; i < cnt; i++) {
            (pkt++)->weight = weights[i];
        }
        break;
    case 3:
        (pkt++)->code = addr | 0x68018000;
        for (i = 0; i < cnt; i++) {
            (pkt++)->weight = weights[i];
        }
        break;
    case 4:
        (pkt++)->code = addr | 0x6C018000;
        for (i = 0; i < cnt; i++) {
            (pkt++)->weight = weights[i];
        }
        break;
    case 5:
        (pkt++)->code = addr | 0x6C018000;
        for (i = 0; i < 4; i++) {
            (pkt++)->weight = weights[i];
        }
        (pkt++)->code = (addr + 1) | 0x60018000;
        for (i = 4; i < cnt; i++) {
            (pkt++)->weight = weights[i];
        }
        break;
    case 6:
        (pkt++)->code = addr | 0x6C018000;
        for (i = 0; i < 4; i++) {
            (pkt++)->weight = weights[i];
        }
        (pkt++)->code = (addr + 1) | 0x64018000;
        for (i = 4; i < cnt; i++) {
            (pkt++)->weight = weights[i];
        }
        break;
    case 7:
        (pkt++)->code = addr | 0x6C018000;
        for (i = 0; i < 4; i++) {
            (pkt++)->weight = weights[i];
        }
        (pkt++)->code = (addr + 1) | 0x68018000;
        for (i = 4; i < cnt; i++) {
            (pkt++)->weight = weights[i];
        }
        break;
    case 8:
        (pkt++)->code = addr | 0x6C028000;
        for (i = 0; i < 8; i++) {
            (pkt++)->weight = weights[i];
        }
        break;
    }
    return pkt;
}


int FindStripNext(int cur, int base, int cnt, int mode, int *used) {
    int best;
    int bestcount;
    int i;
    int j;
    int k;
    int next;
    int count;
    int found;
    int m;

    best = -1;
    bestcount = 0;
    if (mode == 1) {
        for (i = 0; i < 3; i++) {
            next = primdefs[cur].adj[i];
            if (next != -1 && used[next - base] == 0) {
                count = 0;
                for (j = 0; j < 3; j++) {
                    if (primdefs[next].adj[j] != -1 && used[primdefs[next].adj[j] - base] <= 1) {
                        count++;
                    }
                }
                if (count != 0 && bestcount < count) {
                    best = next;
                    bestcount = count;
                }
            }
        }
        return best;
    }
    found = -1;
    for (i = 0; i < cnt; i++) {
        if (i == cur - base || used[i] > 0) {
            continue;
        }
        k = i + base;
        for (j = 0; j < 3; j++) {
            if (primdefs[k].vid[j] == strip_edge[0]) {
                for (m = 0; m < 3; m++) {
                    if (primdefs[k].vid[m] != strip_edge[1]) {
                        continue;
                    }
                    found = k;
                    goto done;
                }
            }
        }
        found = -1;
    done:
        if (found != -1) {
            break;
        }
    }
    return found;
}


void NuPs2CreateSkin(NuGobj *gobj) {
    stream_buffer0 = NuStreamGetOutputBuffer(0);
    stream_buffer1 = NuStreamGetOutputBuffer(1);
    if (gobj->geom) {
        if (gobj->geom->prim->type == 5) {
            NuPs2CreateSkinNorm(gobj);
        }
        if (gobj->geom->prim->type == 8) {
            NuPs2CreateSkinSmooth(gobj);
        }
    }
}


void NuPs2DestroySkin(NuGobj *gobj) {
    NuGeom *geom;

    for (geom = gobj->geom; geom; geom = geom->next) {
        if (geom->skinv) {
            if (geom->skinv->pnts) {
                NuMemFreeFn(geom->skinv->pnts, nucvtskn_srcfile, 0x67A);
            }
            if (geom->skinv->nrms) {
                NuMemFreeFn(geom->skinv->nrms, nucvtskn_srcfile, 0x67C);
            }
            if (geom->skinv->pid) {
                NuMemFreeFn(geom->skinv->pid, nucvtskn_srcfile, 0x67E);
            }
            NuMemFreeFn(geom->skinv, nucvtskn_srcfile, 0x680);
        }
    }
}


void CloseSkinning(void) {
    if (skinbuffer) {
        NuMemFreeFn(skinbuffer, nucvtskn_srcfile, 0x55);
    }
    skinbuffer = 0;
}


void InitSkinning(int size) {
    skinbuffer = NuMemAllocFn(size, nucvtskn_srcfile, 0x4A);
    skinbuffer_free = skinbuffer;
    skinbuffer_size = size;
    memset(skinbuffer, 0, size);
}


void ResetSkinning(void) {
    skinbuffer_free = skinbuffer;
}


void NuPs2SetTempSkinBuffer(char *buffer, int size) {
    if (buffer) {
        saved_skinbuffer = skinbuffer;
        saved_skinbuffer_free = skinbuffer_free;
        saved_skinbuffer_size = skinbuffer_size;
        skinbuffer = buffer;
        skinbuffer_free = buffer;
        skinbuffer_size = size;
    } else {
        skinbuffer = saved_skinbuffer;
        skinbuffer_free = saved_skinbuffer_free;
        skinbuffer_size = saved_skinbuffer_size;
    }
}


void NuCvtSknInstFix(SkinDmaTag *tag) {
    u32 base;

    base = (u32)tag;
    for (;; tag++) {
        switch (tag->id) {
        case 0x60:
            return;
        case 0x50:
            tag->addr += base;
            break;
        }
    }
}


void NuCvtSknInstUnFix(SkinDmaTag *tag) {
    u32 base;

    base = (u32)tag;
    for (;; tag++) {
        switch (tag->id) {
        case 0x60:
            return;
        case 0x50:
            tag->addr -= base;
            break;
        }
    }
}


int PrevPktIdx(int id) {
    int i;

    if (pkt_reuse_total < pkt_reuse_max && pkt_count) {
        for (i = 0; i < pkt_count; i++) {
            if (pkt_ids[i] == id) {
                pkt_reuse_count++;
                pkt_reuse_total++;
                return i;
            }
        }
    }
    return -1;
}
