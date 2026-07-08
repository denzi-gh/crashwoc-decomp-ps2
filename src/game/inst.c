/*
 * Unit: game/inst
 *
 * Functions:
 *   0x001d5a98 InstSceneLoad
 *   0x001d5b58 InstSceneDestroy
 *   0x001d5bd8 InstAnimDataLoad
 *   0x001d5ca0 InstAnimDataDestroy
 *   0x001d5d08 InstShadDataLoad
 *   0x001d5dd8 InstShadDataDestroy
 *   0x001d5e58 InstClose
 *   0x001d5f38 InstInit
 */

union variptr_u;
struct nulsthdr_s;
struct nulnkhdr_s;
struct nuscene_s;
struct nuanimdata_s;
struct shaddata_s;

struct sceneinst_s {
    struct nuscene_s *scene;
    char name[0x100];
    int inst_cnt;
};

struct animdatainst_s {
    struct nuanimdata_s *ad;
    char name[0x100];
    int inst_cnt;
};

struct shadinst_s {
    struct shaddata_s *shad;
    char name[0x100];
    int inst_cnt;
};

extern struct nulsthdr_s *D_006309fc;
extern struct nulsthdr_s *D_00630a00;
extern struct nulsthdr_s *D_00630a04;

#define sceneinst_pool D_006309fc
#define animdatainst_pool D_00630a00
#define shaddatainst_pool D_00630a04

extern union variptr_u superbuffer_ptr;
extern union variptr_u superbuffer_end;

extern struct nulsthdr_s *NuLstCreate(int num, int size);
extern void NuLstDestroy(struct nulsthdr_s *hdr);
extern struct nulnkhdr_s *NuLstGetNext(struct nulsthdr_s *hdr, struct nulnkhdr_s *lnk);
extern struct nulnkhdr_s *NuLstAlloc(struct nulsthdr_s *hdr);
extern void NuLstFree(struct nulnkhdr_s *lnk);
extern struct nuscene_s *NuSceneLoad(char *name);
extern void NuSceneDestroy(struct nuscene_s *scene);
extern struct nuanimdata_s *NuAnimDataLoadBuff(char *name, union variptr_u *buff, union variptr_u *endbuff);
extern void NuAnimDataDestroy(struct nuanimdata_s *animdata);
extern struct shaddata_s *ShadDataLoad(char *name);
extern void ShadDataDestroy(struct shaddata_s *shad);
extern int strcasecmp(const char *s1, const char *s2);
extern char *strcpy(char *dest, const char *src);

struct nuscene_s *InstSceneLoad(char *name) {
    struct sceneinst_s *sc;

    sc = (struct sceneinst_s *)NuLstGetNext(sceneinst_pool, 0);
    while (sc != 0) {
        if (strcasecmp(name, sc->name) == 0) {
            sc->inst_cnt++;
            return sc->scene;
        }
        sc = (struct sceneinst_s *)NuLstGetNext(sceneinst_pool, (struct nulnkhdr_s *)sc);
    }

    sc = (struct sceneinst_s *)NuLstAlloc(sceneinst_pool);
    if (sc != 0) {
        sc->scene = NuSceneLoad(name);
        if (sc->scene != 0) {
            strcpy(sc->name, name);
            sc->inst_cnt = 1;
            return sc->scene;
        }
        NuLstFree((struct nulnkhdr_s *)sc);
    }

    return 0;
}

void InstSceneDestroy(struct nuscene_s *scene) {
    struct sceneinst_s *sc;

    sc = (struct sceneinst_s *)NuLstGetNext(sceneinst_pool, 0);
    while (sc != 0) {
        if (sc->scene == scene) {
            sc->inst_cnt--;
            if (sc->inst_cnt == 0) {
                NuSceneDestroy(sc->scene);
                NuLstFree((struct nulnkhdr_s *)sc);
            }
            return;
        }
        sc = (struct sceneinst_s *)NuLstGetNext(sceneinst_pool, (struct nulnkhdr_s *)sc);
    }
}

struct nuanimdata_s *InstAnimDataLoad(char *name) {
    struct animdatainst_s *lst;
    struct nuanimdata_s *adat;

    lst = (struct animdatainst_s *)NuLstGetNext(animdatainst_pool, 0);
    while (lst != 0) {
        if (strcasecmp(name, lst->name) == 0) {
            lst->inst_cnt++;
            return lst->ad;
        }
        lst = (struct animdatainst_s *)NuLstGetNext(animdatainst_pool, (struct nulnkhdr_s *)lst);
    }

    lst = (struct animdatainst_s *)NuLstAlloc(animdatainst_pool);
    if (lst != 0) {
        adat = NuAnimDataLoadBuff(name, &superbuffer_ptr, &superbuffer_end);
        lst->ad = adat;
        if (adat != 0) {
            strcpy(lst->name, name);
            lst->inst_cnt = 1;
            return lst->ad;
        }
        NuLstFree((struct nulnkhdr_s *)lst);
    }

    return 0;
}

void InstAnimDataDestroy(struct nuanimdata_s *animdata) {
    struct animdatainst_s *lst;

    lst = (struct animdatainst_s *)NuLstGetNext(animdatainst_pool, 0);
    while (lst != 0) {
        if (lst->ad == animdata) {
            lst->inst_cnt--;
            if (lst->inst_cnt == 0) {
                NuLstFree((struct nulnkhdr_s *)lst);
                NuAnimDataDestroy(animdata);
            }
            return;
        }
        lst = (struct animdatainst_s *)NuLstGetNext(animdatainst_pool, (struct nulnkhdr_s *)lst);
    }
}

struct shaddata_s *InstShadDataLoad(char *name) {
    struct shadinst_s *sdi;
    struct shaddata_s *shad;

    sdi = (struct shadinst_s *)NuLstGetNext(shaddatainst_pool, 0);
    while (sdi != 0) {
        if (strcasecmp(name, sdi->name) == 0) {
            sdi->inst_cnt++;
            return sdi->shad;
        }
        sdi = (struct shadinst_s *)NuLstGetNext(shaddatainst_pool, (struct nulnkhdr_s *)sdi);
    }

    sdi = (struct shadinst_s *)NuLstAlloc(shaddatainst_pool);
    if (sdi != 0) {
        shad = ShadDataLoad(name);
        sdi->shad = shad;
        if (shad != 0) {
            strcpy(sdi->name, name);
            sdi->inst_cnt = 1;
            return sdi->shad;
        }
        NuLstFree((struct nulnkhdr_s *)sdi);
    }

    return 0;
}

void InstShadDataDestroy(struct shaddata_s *shad) {
    struct shadinst_s *sdi;

    sdi = (struct shadinst_s *)NuLstGetNext(shaddatainst_pool, 0);
    while (sdi != 0) {
        if (sdi->shad == shad) {
            sdi->inst_cnt--;
            if (sdi->inst_cnt == 0) {
                ShadDataDestroy(sdi->shad);
                NuLstFree((struct nulnkhdr_s *)sdi);
            }
            return;
        }
        sdi = (struct shadinst_s *)NuLstGetNext(shaddatainst_pool, (struct nulnkhdr_s *)sdi);
    }
}

void InstClose(void) {
    struct shadinst_s *sdi;
    struct animdatainst_s *adi;
    struct sceneinst_s *si;

    if (shaddatainst_pool != 0) {
        sdi = (struct shadinst_s *)NuLstGetNext(shaddatainst_pool, 0);
        while (sdi != 0) {
            ShadDataDestroy(sdi->shad);
            sdi = (struct shadinst_s *)NuLstGetNext(shaddatainst_pool, (struct nulnkhdr_s *)sdi);
        }
        NuLstDestroy(shaddatainst_pool);
        shaddatainst_pool = 0;
    }

    if (animdatainst_pool != 0) {
        adi = (struct animdatainst_s *)NuLstGetNext(animdatainst_pool, 0);
        while (adi != 0) {
            adi = (struct animdatainst_s *)NuLstGetNext(animdatainst_pool, (struct nulnkhdr_s *)adi);
        }
        NuLstDestroy(animdatainst_pool);
        animdatainst_pool = 0;
    }

    if (sceneinst_pool != 0) {
        si = (struct sceneinst_s *)NuLstGetNext(sceneinst_pool, 0);
        while (si != 0) {
            NuSceneDestroy(si->scene);
            si = (struct sceneinst_s *)NuLstGetNext(sceneinst_pool, (struct nulnkhdr_s *)si);
        }
        NuLstDestroy(sceneinst_pool);
        sceneinst_pool = 0;
    }
}

void InstInit(void) {
    sceneinst_pool = NuLstCreate(0x10, 0x108);
    animdatainst_pool = NuLstCreate(0xc0, 0x108);
    shaddatainst_pool = NuLstCreate(0x80, 0x108);
}
