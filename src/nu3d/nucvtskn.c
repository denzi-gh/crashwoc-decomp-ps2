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

#include "nu_libc.h"
#include "nu_types.h"

#include "nu3d/nustream.h"
#include "nu3d/nugobj.h"
#include "nucore/nuerror.h"

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
    u8 tag;
} VifWord;

typedef struct SkinPrimDef {
    struct nuvtx_tc1_s vrts[3];
    char pad6C[0x6C];
    int vid[3];
    int padE4[3];
    int adj[3];
    float weights[3][8];
    int mtxid[8];
    int nmtx;
    int merged;
    int group;
    int id;
} SkinPrimDef;

typedef struct SkinVtxInfo {
    float weights[3];
    u8 mtxid[4];
} SkinVtxInfo;

typedef struct SkinV {
    void *pnts;
    void *nrms;
    void *pid;
    void *nid;
} SkinV;


extern void *NuMemAllocFn(int size, char *file, int line);
extern void NuMemFreeFn(void *ptr, char *file, int line);
extern void NuPs2CreateSkinNorm(struct nugobj_s *gobj);
extern void NuPs2CreateSkinSmooth(struct nugobj_s *gobj);
extern SkinDmaTag *NuPs2CreateSkinDma(char **buffer, struct nugeom_s *geom, SkinPrimDef *defs, int cnt);
extern int SortPrimdefs(SkinPrimDef *defs, int cnt);

extern char D_006156B0[];
extern char D_006156F8[];
extern char D_00615710[];
extern char D_00615728[];
extern char D_00615748[];
extern char D_00615760[];
extern char D_00615778[];
extern char D_00615798[];
extern char D_006157D0[];
extern char D_006157F8[];
extern char D_00615838[];
extern char D_00615868[];
extern char D_006158A0[];
extern char D_006158C8[];
extern SkinPrimDef *D_00633120;
extern SkinPrimDef *D_00633124;
extern char *D_00633128;
extern u32 D_0063312C;
extern int D_00633130;
extern int D_00633134;
extern int D_00633138;
extern int D_0063313C;
extern u16 *D_00633140;
extern u16 *D_00633148;
extern u32 *D_00633150;
extern int D_00633154;
extern int D_00633158;
extern int D_0063315C;
extern int D_00633160;
extern int D_00633164;
extern int D_00633168;
extern char *D_00633178;
extern char *D_00633180;
extern int D_00633184;
extern char *skinbuffer_free;
extern int D_0062EF18;
extern int D_0062EF1C;
extern int D_006ADD18[8];
extern float D_006ADD38[][8];
extern int D_006AE278[];
extern int D_006AE320[];
extern SkinPrimDef *D_006AE3C8[];
extern int D_006AE470[];
extern float *D_00633170;
extern SkinPrimDef *D_00633174;
extern int D_006AE518[];
extern int D_002E9040[3][2];
extern int packet_unique[];

#define nucvtskn_srcfile D_006156B0
#define point_idx_msg D_006156F8
#define blend_offset_msg D_00615710
#define blend_count_msg D_00615728
#define more_unused_msg D_00615748
#define all_used_msg D_00615760
#define mtx_overflow_msg D_00615778
#define no_skin_msg D_00615798
#define collapsed_msg D_006157D0
#define banner_msg D_006157F8
#define dupes_msg D_00615838
#define totals_msg D_00615868
#define overflow_msg D_006158A0
#define peak_msg D_006158C8
#define primdefs D_00633120
#define primdefs_sorted D_00633124
#define skinbuffer D_00633128
#define skinbuffer_size D_0063312C
#define pktcnt D_00633130
#define pkttot D_00633134
#define pktvtxcolour D_00633138
#define pktcolourshift D_0063313C
#define pktgt D_00633140
#define pkthdr D_00633148
#define pktupaddr D_00633150
#define pktdupes D_00633154
#define tritot D_00633158
#define stritot D_0063315C
#define prevvidcnt D_00633160
#define totaldupes D_00633164
#define totalpts D_00633168
#define allocid D_0062EF18
#define prevvidlimit D_0062EF1C
#define stats D_006ADD18
#define pktweights D_006ADD38
#define pktwcnts D_006AE278
#define pktvid D_006AE320
#define pktprim D_006AE3C8
#define pktvrtix D_006AE470
#define stripptr D_00633170
#define stripprimdefs D_00633174
#define ptcache D_006AE518
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


inline int PrevPktIdx(int id) {
    int i;

    if (prevvidcnt < prevvidlimit && pkttot) {
        for (i = 0; i < pkttot; i++) {
            if (pktvid[i] == id) {
                pktdupes++;
                prevvidcnt++;
                return i;
            }
        }
    }
    return -1;
}


static inline void *CloseUnpack(void *pkt) {
    int first;
    int qwc;

    if (pktcnt) {
        first = *pktupaddr;
        qwc = (pktvtxcolour ? first * 5 : first * 4) + 3;
        *pktupaddr = (qwc | 0x8000) | (pktvtxcolour ? pktcnt * 3 << 16 : pktcnt << 17) | 0x6C000000;
    } else {
        pkt = pktupaddr;
    }
    return pkt;
}


float *AddPointDataToPacket(float *pkt, SkinPrimDef *defs, int prim, int vtx, int last) {
    float *start;
    VifWord *hdr;
    VifWord weights[8];
    VifWord w;
    u8 *colour;
    u32 *up;
    u16 *h;
    float weight;
    float maxweight;
    int maxix;
    int qwc;
    int cnt;
    int i;

    start = pkt;
    if (vtx < 0 || vtx >= 3) {
        NuErrorProlog(nucvtskn_srcfile, 0x142)(point_idx_msg);
    }
    ptcache[2] = ptcache[1];
    ptcache[1] = ptcache[0];
    ptcache[0] = defs[prim].vid[vtx];
    maxweight = 0.0f;
    maxix = 0;
    cnt = 0;
    for (i = 0; i < 8; i++) {
        if (defs[prim].mtxid[i] != -1) {
            weight = defs[prim].weights[vtx][i];
            w.weight = weight;
            w.tag &= 0xF8;
            w.tag |= i;
            weights[cnt] = w;
            cnt++;
            if (maxweight < weight) {
                maxweight = weight;
                maxix = i;
            }
        }
    }
    pktvid[pkttot] = defs[prim].vid[vtx];
    pktprim[pkttot] = &defs[prim];
    pktvrtix[pkttot] = vtx;
    *pkt++ = defs[prim].vrts[vtx].pnt.x;
    *pkt++ = defs[prim].vrts[vtx].pnt.y;
    *pkt++ = defs[prim].vrts[vtx].pnt.z;
    *pkt = defs[prim].vrts[vtx].nrm.x;
    hdr = (VifWord *)pkt;
    hdr[1].weight = defs[prim].vrts[vtx].tc[0];
    hdr[2].weight = defs[prim].vrts[vtx].tc[1];
    hdr[3].weight = defs[prim].vrts[vtx].nrm.y;
    hdr[4].weight = defs[prim].vrts[vtx].nrm.z;
    hdr[0].tag = PrevPktIdx(defs[prim].vid[vtx]) + 1;
    if (hdr[0].tag) {
        cnt = 0;
    } else {
        packet_unique[pkttot] = defs[prim].vid[vtx];
    }
    hdr[1].tag = maxix;
    hdr[2].tag = cnt < 5 ? cnt : 4;
    hdr[3].tag = cnt - 4 > -1 ? cnt - 4 : 0;
    hdr[4].tag = last != 0;
    pkt += 5;
    if (pktvtxcolour) {
        colour = (u8 *)&defs[prim].vrts->diffuse;
        colour += vtx * sizeof(struct nuvtx_tc1_s);
        *pkt++ = colour[0] >> pktcolourshift;
        *pkt++ = colour[1] >> pktcolourshift;
        *pkt++ = colour[2] >> pktcolourshift;
        *pkt++ = colour[3];
    }
    pktweights[pkttot][0] = weights[0].weight;
    pktweights[pkttot][1] = weights[1].weight;
    pktweights[pkttot][2] = weights[2].weight;
    pktweights[pkttot][3] = weights[3].weight;
    pktweights[pkttot][4] = weights[4].weight;
    pktweights[pkttot][5] = weights[5].weight;
    pktweights[pkttot][6] = weights[6].weight;
    pktweights[pkttot][7] = weights[7].weight;
    pktwcnts[pkttot] = cnt;
    if (!cnt) {
        up = CloseUnpack(start);
        qwc = (pktvtxcolour ? pkttot * 5 : pkttot * 4) + 3;
        up[0] = qwc | 0x61028000;
        pkt = (float *)(up + 1);
        h = (u16 *)pkt;
        *h++ = PrevPktIdx(defs[prim].vid[vtx]) + 1;
        *h++ = last != 0;
        pkt = (float *)h;
        pktupaddr = (u32 *)pkt;
        pkttot++;
        *(u32 *)pkt = pkttot;
        pktcnt = 0;
        pkt++;
    } else {
        pkttot++;
        pktcnt++;
    }
    return pkt;
}


u8 *GenModData(u8 *pkt, struct nugeom_s *geom, int *end, int stride) {
    struct nuvec_s *offsets;
    u8 *hdr;
    u8 *unpack;
    int blended;
    int cnt;
    int vid;
    int ix;
    int i;

    blended = 0;
    if (geom->blendgeom && geom->vtxptr) {
        *(u32 *)pkt = 0x01000101;
        pkt += 4;
        *(u32 *)pkt = 0x13000000;
        pkt += 4;
        for (ix = 0; ix < geom->blendgeom->nblends; ix++) {
            hdr = pkt;
            *(u32 *)pkt = 0x6E010038;
            pkt += 4;
            cnt = 0;
            *pkt++ = 0;
            *pkt++ = 0;
            *pkt++ = 0;
            *pkt++ = 0;
            unpack = pkt;
            pkt += 4;
            offsets = geom->blendgeom->blend_offsets[ix];
            if (offsets) {
                for (i = 0; i < 42; i++) {
                    vid = packet_unique[i];
                    if (vid >= 0) {
                        if (offsets[vid].x != 0.0f || offsets[vid].y != 0.0f || offsets[vid].z != 0.0f) {
                            cnt++;
                            *(float *)pkt = offsets[vid].x;
                            pkt += 4;
                            *(float *)pkt = offsets[vid].y;
                            pkt += 4;
                            *(float *)pkt = offsets[vid].z;
                            pkt += 4;
                            *(u32 *)pkt = i * stride + 3;
                            pkt += 4;
                            NuDebugMsgProlog(nucvtskn_srcfile, 0x231)(blend_offset_msg, vid, offsets[vid].x, offsets[vid].y, offsets[vid].z);
                        }
                    }
                }
            }
            blended |= cnt;
            if (cnt) {
                *(u32 *)unpack = cnt << 16 | 0x6C000039;
                NuDebugMsgProlog(nucvtskn_srcfile, 0x23B)(blend_count_msg, ix, cnt);
                hdr[4] = ix;
                hdr[5] = cnt + 0x3A;
                *(u32 *)pkt = 0x1400000C;
                pkt += 4;
                *(u32 *)pkt = 0x17000000;
                pkt += 4;
            } else {
                pkt = hdr;
            }
        }
        if (blended) {
            *end = 0;
        }
    }
    return pkt;
}


VifWord *EndSkinPacket(VifWord *pkt, struct nugeom_s *geom, int end) {
    VifWord *hdr;
    int addr;
    int i;

    if (pkttot) {
        hdr = CloseUnpack(pkt);
        *pkthdr = pktvtxcolour ? pkttot * 5 : pkttot * 4;
        *pktgt = pkttot | 0x8000;
        hdr->code = 0x01000101;
        pkt = hdr + 1;
        addr = pktvtxcolour ? 6 : 5;
        for (i = 0; i < pkttot; i++) {
            pkt = WriteWeights(pkt, addr, pktwcnts[i], pktweights[i]);
            addr += pktvtxcolour ? 5 : 4;
        }
        pkt = (VifWord *)GenModData((u8 *)pkt, geom, &end, pktvtxcolour ? 5 : 4);
        if (end) {
            (pkt++)->code = 0x17000000;
        } else {
            (pkt++)->code = 0x14000302;
        }
        totaldupes += pktdupes;
        totalpts += pkttot;
    }
    pkttot = 0;
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
    int m;
    int vtx;

    best = -1;
    bestcount = 0;
    if (mode == 1) {
        for (i = 0; i < 3; i++) {
            next = stripprimdefs[cur].adj[i];
            if (next != -1 && used[next - base] == 0) {
                count = 0;
                for (j = 0; j < 3; j++) {
                    if (stripprimdefs[next].adj[j] != -1 && used[stripprimdefs[next].adj[j] - base] <= 1) {
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
    next = -1;
    for (i = 0; i < cnt; i++) {
        if (i == cur - base || used[i] > 0) {
            continue;
        }
        k = i + base;
        for (vtx = 0; vtx < 3; vtx++) {
            if (stripprimdefs[k].vid[vtx] == ptcache[0]) {
                for (m = 0; m < 3; m++) {
                    if (stripprimdefs[k].vid[m] != ptcache[1]) {
                        continue;
                    }
                    next = k;
                    goto done;
                }
            }
        }
        next = -1;
    done:
        if (next != -1) {
            break;
        }
    }
    return next;
}


void AddStripTri(int prim, int prev, int first) {
    int vtx0;
    int vtx1;
    int vtx2;
    int unused;
    int found;
    int i;
    int j;

    if (first == 1) {
        for (vtx0 = 0; vtx0 < 3; vtx0++) {
            found = 0;
            for (j = 0; j < 3; j++) {
                if (stripprimdefs[prev].vid[vtx0] == stripprimdefs[prim].vid[j]) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                goto unshared;
            }
        }
        vtx0 = -1;
    unshared:
        switch (vtx0) {
        default:
            vtx0 = 0;
        case 0:
            vtx1 = 1;
            vtx2 = 2;
            break;
        case 1:
            vtx1 = 0;
            vtx2 = 2;
            break;
        case 2:
            vtx1 = 0;
            vtx2 = 1;
            break;
        }
        stripptr = AddPointDataToPacket(stripptr, stripprimdefs, prev, vtx0, 1);
        stripptr = AddPointDataToPacket(stripptr, stripprimdefs, prev, vtx1, 1);
        stripptr = AddPointDataToPacket(stripptr, stripprimdefs, prev, vtx2, 0);
    }
    unused = -1;
    for (i = 0; i < 3; i++) {
        if (stripprimdefs[prim].vid[i] == ptcache[0] || stripprimdefs[prim].vid[i] == ptcache[1]) {
            continue;
        }
        if (unused != -1) {
            NuErrorProlog(nucvtskn_srcfile, 0x2E8)(more_unused_msg);
        }
        unused = i;
    }
    if (unused == -1) {
        NuErrorProlog(nucvtskn_srcfile, 0x2ED)(all_used_msg);
    }
    stripptr = AddPointDataToPacket(stripptr, stripprimdefs, prim, unused, 0);
}


int SortPrimdefs(SkinPrimDef *defs, int cnt) {
    char mtxused[256];
    SkinPrimDef tmp;
    int group;
    int out;
    int diff;
    int total;
    int i;
    int j;
    int k;
    int m;
    int n;

    for (i = 0; i < cnt; i++) {
        for (j = i + 1; j < cnt; j++) {
            if (defs[i].nmtx < defs[j].nmtx) {
                tmp = defs[i];
                defs[i] = defs[j];
                defs[j] = tmp;
            }
        }
    }
    out = 0;
    for (i = 0; i < cnt; i++) {
        if (defs[i].merged) {
            continue;
        }
        group = out;
        defs[i].group = out;
        primdefs_sorted[out] = defs[i];
        out++;
        memset(mtxused, 0, sizeof(mtxused));
        for (j = 0; j < defs[i].nmtx; j++) {
            mtxused[defs[i].mtxid[j]] = 1;
        }
        for (j = i + 1; j < cnt; j++) {
            if (defs[j].merged || defs[i].nmtx < defs[j].nmtx) {
                continue;
            }
            diff = 0;
            for (n = 0; n < defs[j].nmtx; n++) {
                diff += abs(mtxused[defs[j].mtxid[n]] - 1);
            }
            if (diff) {
                continue;
            }
            defs[j].group = group;
            primdefs_sorted[out] = defs[j];
            defs[j].merged = 1;
            for (k = 0; k < 8; k++) {
                primdefs_sorted[out].mtxid[k] = -1;
                primdefs_sorted[out].weights[0][k] = 0.0f;
                primdefs_sorted[out].weights[1][k] = 0.0f;
                primdefs_sorted[out].weights[2][k] = 0.0f;
            }
            for (k = 0; k < defs[i].nmtx; k++) {
                for (m = 0; m < defs[j].nmtx; m++) {
                    if (defs[i].mtxid[k] == defs[j].mtxid[m]) {
                        primdefs_sorted[out].weights[0][k] = defs[j].weights[0][m];
                        primdefs_sorted[out].weights[1][k] = defs[j].weights[1][m];
                        primdefs_sorted[out].weights[2][k] = defs[j].weights[2][m];
                        primdefs_sorted[out].mtxid[k] = defs[i].mtxid[k];
                    }
                }
            }
            out++;
        }
    }
    group = -1;
    total = 0;
    for (i = 0; i < out; i++) {
        if (primdefs_sorted[i].group != group) {
            group = primdefs_sorted[i].group;
            total += primdefs_sorted[i].nmtx;
        }
    }
    return total;
}


static inline int AddMtxToPrimDef(SkinPrimDef *pd, int mtx) {
    int i;

    for (i = 0; i < pd->nmtx; i++) {
        if (pd->mtxid[i] == mtx) {
            return i;
        }
    }
    if (i >= 8) {
        NuDebugMsgProlog(nucvtskn_srcfile, 0x504)(mtx_overflow_msg);
        return 0;
    }
    pd->mtxid[i] = mtx;
    pd->nmtx++;
    return i;
}


void SetVtxSkinData(SkinPrimDef *pd, int vtx, struct nuvtx_tc1_s *verts, int vid, struct nugeom_s *geom) {
    struct nuskin_s *skin;
    float *weights;
    int offset;
    int i;

    pd->vrts[vtx] = verts[vid];
    pd->vid[vtx] = vid;
    if (vtx < 3) {
        pd->adj[vtx] = -1;
        for (skin = geom->skins; skin; skin = skin->next) {
            offset = vid - skin->vtxoffset;
            if (offset >= 0 && offset < skin->vtxcnt) {
                weights = &skin->weights[skin->mtxcnt * offset];
                for (i = 0; i < skin->mtxcnt; i++) {
                    pd->weights[vtx][AddMtxToPrimDef(pd, skin->mtxid[i])] = weights[i];
                }
                return;
            }
        }
        NuDebugMsgProlog(nucvtskn_srcfile, 0x529)(no_skin_msg, vid);
    }
}


static inline void RelocateSkinDma(SkinDmaTag *tag) {
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


static inline void SetVtxSkinInfo(SkinPrimDef *pd, int vtx, struct nuvtx_tc1_s *verts, int vid, struct nugeom_s *geom) {
    SkinVtxInfo *infos;
    int i;

    pd->vrts[vtx] = verts[vid];
    pd->vid[vtx] = vid;
    pd->adj[vtx] = -1;
    infos = geom->vtxskininfo;
    for (i = 0; i != 3 && infos[vid].weights[i] != 0.0f; i++) {
        pd->weights[vtx][AddMtxToPrimDef(pd, infos[vid].mtxid[i])] = infos[vid].weights[i];
    }
}


void NuPs2CreateSkinNorm(struct nugobj_s *gobj) {
    struct nugeom_s *geom;
    struct nuprim_s *prim;
    struct nuvtx_tc1_s *verts;
    SkinPrimDef *pd;
    char **buffer;
    int cnt;
    int total;
    int dupes;
    char *start;
    int common;
    int peak;
    int i;

    start = skinbuffer_free;
    dupes = 0;
    total = 0;
    memset(stats, 0, sizeof(stats));
    totaldupes = 0;
    totalpts = 0;
    tritot = 0;
    stritot = 0;
    buffer = &skinbuffer_free;
    for (geom = gobj->geom; geom; geom = geom->next) {
        if (!geom->skins && !geom->vtxskininfo) {
            continue;
        }
        prim = geom->prim;
        cnt = 0;
        pd = primdefs;
        verts = geom->vtxptr;
        for (i = 0; i < prim->cnt; i += 3) {
            memset(pd, 0, sizeof(SkinPrimDef));
            memset(pd->mtxid, -1, sizeof(pd->mtxid));
            if (geom->vtxskininfo) {
                SetVtxSkinInfo(pd, 0, verts, prim->vid[i], geom);
                SetVtxSkinInfo(pd, 1, verts, prim->vid[i + 1], geom);
                SetVtxSkinInfo(pd, 2, verts, prim->vid[i + 2], geom);
            } else {
                SetVtxSkinData(pd, 0, verts, prim->vid[i], geom);
                SetVtxSkinData(pd, 1, verts, prim->vid[i + 1], geom);
                SetVtxSkinData(pd, 2, verts, prim->vid[i + 2], geom);
            }
            if (pd->vid[0] == pd->vid[1] || pd->vid[1] == pd->vid[2] || pd->vid[0] == pd->vid[2]) {
                NuDebugMsgProlog(nucvtskn_srcfile, 0x580)(collapsed_msg);
                pd--;
                cnt--;
            }
            pd->id = allocid++;
            stats[pd->nmtx]++;
            pd++;
            cnt++;
        }
        dupes += SortPrimdefs(primdefs, cnt);
        common = 0;
        peak = 0;
        for (i = 0; i < 8; i++) {
            if (stats[i] && peak < i) {
                peak = i;
            }
            if (stats[i] >= stats[common]) {
                common = i;
            }
        }
        geom->skindma = NuPs2CreateSkinDma(buffer, geom, primdefs_sorted, cnt);
        geom->dmasize = *buffer - (char *)geom->skindma;
        RelocateSkinDma(geom->skindma);
        total += cnt;
    }
    if (dupes) {
        NuDebugMsgProlog(nucvtskn_srcfile, 0x5A5)(banner_msg);
        NuDebugMsgProlog(nucvtskn_srcfile, 0x5A6)(dupes_msg, dupes, total, 0);
        NuDebugMsgProlog(nucvtskn_srcfile, 0x5A7)(totals_msg, tritot, stritot, totalpts, totaldupes, skinbuffer_free - start);
        NuDebugMsgProlog(nucvtskn_srcfile, 0x5A8)(banner_msg);
    }
    if (skinbuffer_free - skinbuffer > skinbuffer_size) {
        NuDebugMsgProlog(nucvtskn_srcfile, 0x5AB)(overflow_msg);
    }
}


void NuPs2CreateSkinSmoothOld(struct nugobj_s *gobj) {
    int i;
    struct nugeom_s *geom;
    struct nuprim_s *prim;
    struct nuvtx_tc1_s *verts;
    SkinPrimDef *pd;
    char **buffer;
    int total;
    int dupes;
    char *start;
    int cnt;
    int common;
    int peak;

    start = skinbuffer_free;
    dupes = 0;
    total = 0;
    memset(stats, 0, sizeof(stats));
    totaldupes = 0;
    totalpts = 0;
    tritot = 0;
    stritot = 0;
    buffer = &skinbuffer_free;
    for (geom = gobj->geom; geom; geom = geom->next) {
        if (!geom->skins && !geom->vtxskininfo) {
            continue;
        }
        prim = geom->prim;
        cnt = 0;
        pd = primdefs;
        verts = geom->vtxptr;
        for (i = 0; i < prim->cnt; i += 6) {
            memset(pd, 0, sizeof(SkinPrimDef));
            memset(pd->mtxid, -1, sizeof(pd->mtxid));
            if (geom->vtxskininfo) {
                SetVtxSkinInfo(pd, 0, verts, prim->vid[i], geom);
                SetVtxSkinInfo(pd, 1, verts, prim->vid[i + 3], geom);
                SetVtxSkinInfo(pd, 2, verts, prim->vid[i + 5], geom);
            } else {
                SetVtxSkinData(pd, 0, verts, prim->vid[i], geom);
                SetVtxSkinData(pd, 1, verts, prim->vid[i + 3], geom);
                SetVtxSkinData(pd, 2, verts, prim->vid[i + 5], geom);
            }
            stats[pd->nmtx]++;
            pd++;
            cnt++;
        }
        dupes += SortPrimdefs(primdefs, cnt);
        common = 0;
        peak = 0;
        for (i = 0; i < 8; i++) {
            if (stats[i] && peak < i) {
                peak = i;
            }
            if (stats[i] >= stats[common]) {
                common = i;
            }
        }
        NuDebugMsgProlog(nucvtskn_srcfile, 0x5F9)(peak_msg, peak, common);
        geom->skindma = NuPs2CreateSkinDma(buffer, geom, primdefs_sorted, cnt);
        geom->dmasize = *buffer - (char *)geom->skindma;
        RelocateSkinDma(geom->skindma);
        total += cnt;
    }
    if (dupes) {
        NuDebugMsgProlog(nucvtskn_srcfile, 0x604)(banner_msg);
        NuDebugMsgProlog(nucvtskn_srcfile, 0x605)(dupes_msg, dupes, total, 0);
        NuDebugMsgProlog(nucvtskn_srcfile, 0x606)(totals_msg, tritot, stritot, totalpts, totaldupes, skinbuffer_free - start);
        NuDebugMsgProlog(nucvtskn_srcfile, 0x607)(banner_msg);
    }
    if (skinbuffer_free - skinbuffer > skinbuffer_size) {
        NuDebugMsgProlog(nucvtskn_srcfile, 0x60A)(overflow_msg);
    }
}


void NuPs2CreateSkinSmooth(struct nugobj_s *gobj) {
    struct nugeom_s *geom;
    struct nuprim_s *prim;
    struct nuvtx_tc1_s *verts;
    SkinPrimDef *pd;
    char **buffer;
    int dupes;
    char *start;
    int total;
    int cnt;
    int common;
    int peak;
    int i;

    start = skinbuffer_free;
    dupes = 0;
    total = 0;
    memset(stats, 0, sizeof(stats));
    totaldupes = 0;
    totalpts = 0;
    tritot = 0;
    stritot = 0;
    buffer = &skinbuffer_free;
    for (geom = gobj->geom; geom; geom = geom->next) {
        if (!geom->skins && !geom->vtxskininfo) {
            continue;
        }
        prim = geom->prim;
        cnt = 0;
        pd = primdefs;
        verts = geom->vtxptr;
        for (i = 0; i < prim->cnt; i += 6) {
            memset(pd, 0, sizeof(SkinPrimDef));
            memset(pd->mtxid, -1, sizeof(pd->mtxid));
            cnt++;
            SetVtxSkinData(pd, 0, verts, prim->vid[i], geom);
            SetVtxSkinData(pd, 1, verts, prim->vid[i + 3], geom);
            SetVtxSkinData(pd, 2, verts, prim->vid[i + 5], geom);
            SetVtxSkinData(pd, 3, verts, prim->vid[i + 1], geom);
            SetVtxSkinData(pd, 4, verts, prim->vid[i + 2], geom);
            SetVtxSkinData(pd, 5, verts, prim->vid[i + 4], geom);
            stats[pd->nmtx]++;
            pd++;
        }
        dupes += SortPrimdefs(primdefs, cnt);
        common = 0;
        peak = 0;
        for (i = 0; i < 8; i++) {
            if (stats[i] && peak < i) {
                peak = i;
            }
            if (stats[i] >= stats[common]) {
                common = i;
            }
        }
        NuDebugMsgProlog(nucvtskn_srcfile, 0x64C)(peak_msg, peak, common);
        geom->skindma = NuPs2CreateSkinDma(buffer, geom, primdefs_sorted, cnt);
        geom->dmasize = *buffer - (char *)geom->skindma;
        RelocateSkinDma(geom->skindma);
        total += cnt;
    }
    if (dupes) {
        NuDebugMsgProlog(nucvtskn_srcfile, 0x657)(banner_msg);
        NuDebugMsgProlog(nucvtskn_srcfile, 0x658)(dupes_msg, dupes, total, 0);
        NuDebugMsgProlog(nucvtskn_srcfile, 0x659)(totals_msg, tritot, stritot, totalpts, totaldupes, skinbuffer_free - start);
        NuDebugMsgProlog(nucvtskn_srcfile, 0x65A)(banner_msg);
    }
    if (skinbuffer_free - skinbuffer > skinbuffer_size) {
        NuDebugMsgProlog(nucvtskn_srcfile, 0x65D)(overflow_msg);
    }
}


void NuPs2CreateSkin(struct nugobj_s *gobj) {
    primdefs = NuStreamGetOutputBuffer(0);
    primdefs_sorted = NuStreamGetOutputBuffer(1);
    if (gobj->geom) {
        if (gobj->geom->prim->type == 5) {
            NuPs2CreateSkinNorm(gobj);
        }
        if (gobj->geom->prim->type == 8) {
            NuPs2CreateSkinSmooth(gobj);
        }
    }
}


void NuPs2DestroySkin(struct nugobj_s *gobj) {
    struct nugeom_s *geom;

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
