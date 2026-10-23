/*
 * Unit: game/visidat
 *
 * Functions:
 *   0x0024d0b0 visiSetSplineKnot
 *   0x0024d238 visiLoadData
 *   0x0024dc20 visiReset
 */

#include "nu_libc.h"
#include "nu_types.h"
#include "game/visidat.h"

#include "nucore/nuerror.h"
#include "numath/numathtypes.h"
#include "numath/nuvec.h"
#include "nu3d/numtl.h"
#include "nu3d/nugobj.h"
#include "nu3d/nugscn.h"
#include "nucore/nucoretypes.h"

struct visbndassoc_s {
    int visspline;
    int bndspline;
    int visstart_ix;
    int visend_ix;
};

struct visnames_s {
    u32 count;
    char names[4];
};

extern void *NuFileLoad(char *name);
extern void *NuMemAllocFn(int size, char *file, int line);
extern void NuMemFreeFn(void *ptr, char *file, int line);
extern int NuPlnLineVU0(struct nuplane_s *pln, struct nuvec_s *p0, struct nuvec_s *p1, struct nuvec_s *hit);

extern char D_00625208[];
extern char D_00625218[];
extern char D_00625238[];

#define visidat_srcfile D_00625208
#define err_alloc D_00625218
#define msg_visidata_used D_00625238


void visiSetSplineKnot(struct visidata_s *vd, int spix, int knotix) {
    int i;
    int ix;
    int n;
    u64 mask;

    if (vd == 0) {
        return;
    }
    if (spix == vd->curspline && knotix == vd->curknot) {
        return;
    }
    if (spix < 0) {
        return;
    }
    if (spix >= vd->sc->numsplines) {
        return;
    }
    if (vd->vspline[spix] == 0) {
        return;
    }
    if (knotix < 0) {
        return;
    }
    if (knotix >= vd->sc->splines[spix].len) {
        return;
    }
    vd->curspline = spix;
    vd->curknot = knotix;

    for (; knotix >= 0; knotix--) {
        if (vd->vspline[spix][knotix] != 0) {
            i = 0;
            for (ix = 0; ix < vd->binfosize; ix++) {
                mask = vd->vspline[spix][knotix][ix];
                for (n = 0; n < 64; n++) {
                    vd->sc->instances[i++].flags.visitest = mask & 1;
                    mask >>= 1;
                    if (i >= vd->sc->numinstance) {
                        break;
                    }
                }
                if (i >= vd->sc->numinstance) {
                    return;
                }
            }
            return;
        }
    }
}


static inline int PtInsideFBound(struct nuvec_s *pt, int npts, struct nuvec_s *pts) {
    struct nuvec_s hit;
    struct nuplane_s pln;
    struct nuvec_s *prev;
    int crossings;
    int i;

    prev = &pts[npts - 1];
    pln.a = 0.0f;
    pln.b = 0.0f;
    pln.c = 1.0f;
    pln.d = -(pln.a * pt->x + pln.b * pt->y + pln.c * pt->z);
    crossings = 0;
    for (i = 0; i < npts; i++) {
        if (NuPlnLineVU0(&pln, prev, pts, &hit) && hit.x >= pt->x) {
            crossings++;
        }
        prev = pts;
        pts++;
    }
    return crossings & 1;
}


struct visidata_s *visiLoadData(char *fname, struct nugscn_s *sc, union variptr_u *buff) {
    struct visidata_s *vd;
    union variptr_u fdatabase;
    union variptr_u fdata;
    int numassoc;
    int numbnd;
    int splen;
    struct nugobj_s *g;
    struct nuvec_s wpos;
    int gcnt;
    int n;
    int m;
    int o;
    int p;
    int binfix;
    int tmpsize;
    int spline_used;
    struct visbndassoc_s *assocs;
    u64 *binfo;
    u64 *tmpalloc;
    u64 *tmpdelta;
    int binfosize;
    int ver;

    vd = 0;
    fdatabase.voidptr = NuFileLoad(fname);
    if (fdatabase.voidptr != 0) {
        fdata.s32 = fdatabase.s32 + 1;
        vd = buff->voidptr;
        buff->intaddr += sizeof(struct visidata_s);
        memset(vd, 0, sizeof(struct visidata_s));
        vd->sc = sc;
        vd->curspline = -1;
        vd->curknot = -1;
        vd->vspline = buff->voidptr;
        buff->intaddr += sc->numsplines * 4;

        ver = fdatabase.s32[0];
        if (ver < 0) {
            numbnd = fdatabase.s32[1];
            fdata.s32 = fdatabase.s32 + 2;
        } else {
            numbnd = ver;
        }

        binfosize = (vd->sc->numinstance + 63) / 64;
        vd->binfosize = binfosize;
        tmpalloc = NuMemAllocFn(binfosize * numbnd * 8, visidat_srcfile, 0xC0);
        if (tmpalloc == 0) {
            NuErrorProlog(visidat_srcfile, 0xC2)(err_alloc, binfosize * numbnd * 8);
        }

        for (o = 0; o < numbnd; o++) {
            u64 mask;

            splen = *fdata.s32++;
            binfix = o * binfosize;
            tmpalloc[binfix] = 0;
            mask = 1;
            for (n = 0; n < vd->sc->numinstance; n++) {
                g = vd->sc->gobjs[vd->sc->instances[n].objid];
                gcnt = g->ngobjs;
                for (m = 0; m < gcnt; m++) {
                    NuVecMtxTransform(&wpos, &g->cntr, &vd->sc->instances[n].mtx);
                    if (PtInsideFBound(&wpos, splen, fdata.vec3)) {
                        tmpalloc[binfix] |= mask;
                    }
                    g++;
                }
                mask <<= 1;
                if (mask == 0) {
                    binfix++;
                    mask = 1;
                }
            }
            fdata.vec3 += splen;
        }

        numassoc = *fdata.s32++;
        assocs = fdata.voidptr;

        if (ver < 0) {
            union variptr_u splused;
            int *spix;
            char *spname;

            splused.voidptr = &assocs[numassoc];
            if ((*splused.u32) != 0) {
                spix = NuMemAllocFn((*splused.u32) * 4, visidat_srcfile, 0xEE);
                spname = splused.charptr + 4;
                for (o = 0; o < (*splused.u32); o++) {
                    spix[o] = -1;
                    for (n = 0; n < sc->numsplines; n++) {
                        if (strcmp(sc->splines[n].name, spname) == 0) {
                            spix[o] = n;
                            break;
                        }
                    }
                    while (*spname != 0) {
                        spname++;
                    }
                    spname++;
                }
                for (o = 0; o < numassoc; o++) {
                    assocs[o].visspline = spix[assocs[o].visspline];
                    if (assocs[o].visspline < 0) {
                        assocs[o].visspline = 0;
                        assocs[o].visstart_ix = 0;
                        assocs[o].visend_ix = 1;
                    }
                }
                NuMemFreeFn(spix, visidat_srcfile, 0x107);
            }
        }

        binfo = NuMemAllocFn(binfosize * 8, visidat_srcfile, 0x10E);
        tmpsize = 0;
        tmpdelta = 0;
        if (binfo == 0) {
            NuErrorProlog(visidat_srcfile, 0x110)(err_alloc, binfosize * 8);
        }

        for (o = 0; o < vd->sc->numsplines; o++) {
            spline_used = 0;
            for (n = 0; n < numassoc; n++) {
                if (assocs[n].visspline != o) {
                    continue;
                }
                if (spline_used == 0) {
                    if (tmpsize < vd->sc->splines[o].len * binfosize) {
                        if (tmpdelta != 0) {
                            NuMemFreeFn(tmpdelta, visidat_srcfile, 0x11D);
                        }
                        tmpsize = vd->sc->splines[o].len * binfosize + 1;
                        tmpdelta = NuMemAllocFn(tmpsize * 8, visidat_srcfile, 0x120);
                        if (tmpdelta == 0) {
                            NuErrorProlog(visidat_srcfile, 0x122)(err_alloc, tmpsize * 8);
                        }
                    }
                    memset(tmpdelta, 0, tmpsize * 8);
                }
                spline_used = 1;
                assocs[n].visstart_ix = (assocs[n].visstart_ix < vd->sc->splines[o].len)
                                            ? assocs[n].visstart_ix : vd->sc->splines[o].len;
                assocs[n].visend_ix = (assocs[n].visend_ix < vd->sc->splines[o].len)
                                          ? assocs[n].visend_ix : vd->sc->splines[o].len;
                for (m = assocs[n].visstart_ix; m <= assocs[n].visend_ix; m++) {
                    for (p = 0; p < binfosize; p++) {
                        tmpdelta[m * binfosize + p] |= tmpalloc[assocs[n].bndspline * binfosize + p];
                    }
                }
            }

            if (spline_used != 0) {
                vd->vspline[o] = buff->voidptr;
                buff->intaddr += vd->sc->splines[o].len * 4;
                memset(vd->vspline[o], 0, vd->sc->splines[o].len * 4);
                buff->intaddr = (buff->intaddr + 7) & ~7;
                for (n = 0; n < vd->sc->splines[o].len; n++) {
                    u64 deltatot = 0;
                    for (m = 0; m < binfosize; m++) {
                        if (n == 0) {
                            binfo[m] = tmpdelta[m];
                        } else {
                            binfo[m] = tmpdelta[(n - 1) * binfosize + m] ^ tmpdelta[n * binfosize + m];
                        }
                        deltatot |= binfo[m];
                    }
                    if (deltatot != 0) {
                        vd->vspline[o][n] = buff->u64;
                        for (m = 0; m < binfosize; m++) {
                            *buff->u64++ = tmpdelta[n * binfosize + m];
                        }
                    }
                }
            }
        }

        if (tmpdelta != 0) {
            NuMemFreeFn(tmpdelta, visidat_srcfile, 0x156);
        }
        if (binfo != 0) {
            NuMemFreeFn(binfo, visidat_srcfile, 0x158);
        }
        if (tmpalloc != 0) {
            NuMemFreeFn(tmpalloc, visidat_srcfile, 0x15A);
        }
        NuMemFreeFn(fdatabase.voidptr, visidat_srcfile, 0x15C);
        buff->intaddr = (buff->intaddr + 15) & ~15;
        NuDebugMsgProlog(visidat_srcfile, 0x15F)(msg_visidata_used, buff->intaddr - (nuintptr)vd);
    }
    return vd;
}


void visiReset(struct visidata_s *vd) {
    vd->curspline = -1;
    vd->curknot = -1;
}
