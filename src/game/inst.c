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

extern struct nulsthdr_s *D_006309fc;
extern struct nulsthdr_s *D_00630a00;

#define sceneinst_pool D_006309fc
#define animdatainst_pool D_00630a00

extern union variptr_u superbuffer_ptr;
extern union variptr_u superbuffer_end;

extern struct nulnkhdr_s *NuLstGetNext(struct nulsthdr_s *hdr, struct nulnkhdr_s *lnk);
extern struct nulnkhdr_s *NuLstAlloc(struct nulsthdr_s *hdr);
extern void NuLstFree(struct nulnkhdr_s *lnk);
extern struct nuscene_s *NuSceneLoad(char *name);
extern void NuSceneDestroy(struct nuscene_s *scene);
extern struct nuanimdata_s *NuAnimDataLoadBuff(char *name, union variptr_u *buff, union variptr_u *endbuff);
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

    lst = 0;
    while (1) {
        lst = (struct animdatainst_s *)NuLstGetNext(animdatainst_pool, (struct nulnkhdr_s *)lst);
        if (lst == 0) {
            break;
        }
        if (strcasecmp(name, lst->name) == 0) {
            lst->inst_cnt++;
            return lst->ad;
        }
    }

    lst = (struct animdatainst_s *)NuLstAlloc(animdatainst_pool);
    if (lst != 0) {
        if ((lst->ad = NuAnimDataLoadBuff(name, &superbuffer_ptr, &superbuffer_end)) != 0) {
            strcpy(lst->name, name);
            lst->inst_cnt = 1;
            return lst->ad;
        }
        NuLstFree((struct nulnkhdr_s *)lst);
    }

    return 0;
}
