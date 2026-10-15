/*
 * Unit: game/visi
 *
 * Functions:
 *   0x00201450 ApplyVisiTable
 *   0x00201668 VSAddObjs
 *   0x00201868 BuildVisiTable
 *   0x00201a00 ReleaseVisiTable
 */

#include "nu_libc.h"

#include "numath/numathtypes.h"
#include "numath/nuvec.h"
#include "nu3d/numtl.h"
#include "nu3d/nugobj.h"
#include "nu3d/nugscn.h"

#include "creature.h"

extern void NuMemFreeFn(void *ptr, char *file, int line);

/* Retail-owned (no Data-from-C yet). */
extern char D_0061F2E8[];                /* source file name */
extern int D_0063326C;
extern char *D_00633270;
#define visi_srcfile D_0061F2E8
#define vscnt D_0063326C
#define vsdata D_00633270

/* One visibility zone: the instances to show while inside spline sp. */
struct visidat_s {
    struct nugspline_s *sp;
    int numinstances;
    struct nuinstance_s *i[1];
};

extern struct visidat_s *D_006B74B8[50];
#define visidat D_006B74B8

extern int NuPlnLineVU0(struct nuplane_s *p, struct nuvec_s *a, struct nuvec_s *b, struct nuvec_s *hit);

/* Even-odd test of wpos against the closed spline outline in the xy plane. */
static inline int PtInsideSpline(struct nuvec_s *wpos, struct nugspline_s *sp)
{
    struct nuvec_s *curr;
    struct nuvec_s *prev;
    struct nuvec_s *pts;
    struct nuvec_s ipnt;
    struct nuplane_s p;
    int n;
    int icnt;

    pts = (struct nuvec_s *)sp->pts;
    prev = &pts[sp->len - 1];
    curr = pts;

    p.a = 0.0f;
    p.b = 0.0f;
    p.c = 1.0f;
    p.d = -(p.a * wpos->x + p.b * wpos->y + p.c * wpos->z);
    for (icnt = 0, n = 0; icnt < sp->len; icnt++, curr++) {
        if (NuPlnLineVU0(&p, prev, curr, &ipnt) != 0 && ipnt.x >= wpos->x) {
            n++;
        }
        prev = curr;
    }
    return n & 1;
}

/* Counts the instances with a gobj centred inside spline sp, storing them in
   op when it is given. */
int VSAddObjs(struct nuinstance_s **op, struct nugscn_s *gsc, struct nugspline_s *sp)
{
    struct nugobj_s *gobj;
    struct nuvec_s wpos;
    int n;
    int m;
    int gcnt;
    int icnt;

    for (n = 0, icnt = 0; n < gsc->numinstance; n++) {
        gobj = gsc->gobjs[gsc->instances[n].objid];
        gcnt = gobj->ngobjs;
        for (m = 0; m < gcnt; m++) {
            NuVecMtxTransform(&wpos, &gobj->cntr, &gsc->instances[n].mtx);
            if (PtInsideSpline(&wpos, sp) != 0) {
                if (op != 0) {
                    op[icnt] = &gsc->instances[n];
                }
                icnt++;
                break;
            }
            gobj++;
        }
    }
    return icnt;
}

void ApplyVisiTable(struct nugscn_s *sc, struct nuvec_s *pos)
{
    struct visidat_s *vd;
    int n;
    int m;

    if (vscnt != 0) {
        if (pos == 0) {
            for (m = 0, n = 0; m < sc->numinstance; m++, n++) {
                sc->instances[n].flags.visitest = 1;
            }
        } else {
            for (m = 0, n = 0; m < sc->numinstance; m++, n++) {
                sc->instances[n].flags.visitest = 0;
            }
            for (m = 0; m < vscnt; m++) {
                vd = visidat[m];
                if (PtInsideSpline(pos, vd->sp) != 0) {
                    for (n = 0; n < vd->numinstances; n++) {
                        vd->i[n]->flags.visitest = 1;
                    }
                }
            }
        }
    }
}

extern char D_00631318[];               /* "vis" */
extern void *NuMemAllocFn(int size, char *file, int line);

/* Builds a visidat_s for every spline whose name contains "vis", listing the
   instances inside it, all in one allocation. */
void BuildVisiTable(struct nugscn_s *gsc)
{
    struct visidat_s *vd;
    int n;
    int ocnt;

    vscnt = 0;
    for (n = 0, ocnt = 0; n < gsc->numsplines; n++) {
        if (strstr(gsc->splines[n].name, D_00631318) != 0) {
            vscnt++;
            ocnt += VSAddObjs(0, gsc, &gsc->splines[n]);
        }
    }

    vsdata = 0;
    if (ocnt != 0) {
        vsdata = NuMemAllocFn(vscnt * sizeof(struct visidat_s)
                              + ocnt * sizeof(struct nuinstance_s *),
                              visi_srcfile, 0xAA);
        if (vsdata != 0) {
            vd = (struct visidat_s *)vsdata;
            vscnt = 0;
            for (n = 0; n < gsc->numsplines; n++) {
                if (strstr(gsc->splines[n].name, D_00631318) != 0) {
                    visidat[vscnt] = vd;
                    vscnt++;
                    ocnt = VSAddObjs(vd->i, gsc, &gsc->splines[n]);
                    vd->sp = &gsc->splines[n];
                    vd->numinstances = ocnt;
                    vd = (struct visidat_s *)((char *)vd + sizeof(struct visidat_s)
                                              + ocnt * sizeof(struct nuinstance_s *));
                }
            }
        }
    }
}

void ReleaseVisiTable(void) {
    if (vsdata != 0) {
        NuMemFreeFn(vsdata, visi_srcfile, 0xC0);
    }
    vscnt = 0;
    vsdata = 0;
}
