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

struct nulsthdr_s;
struct nulnkhdr_s;
struct nuscene_s;

struct sceneinst_s {
    struct nuscene_s *scene;
    char name[0x100];
    int inst_cnt;
};

static struct nulsthdr_s *sceneinst_pool;
static struct nulsthdr_s *animdatainst_pool;
static struct nulsthdr_s *shaddatainst_pool;

extern struct nulnkhdr_s *NuLstGetNext(struct nulsthdr_s *hdr, struct nulnkhdr_s *lnk);
extern struct nulnkhdr_s *NuLstAlloc(struct nulsthdr_s *hdr);
extern void NuLstFree(struct nulnkhdr_s *lnk);
extern struct nuscene_s *NuSceneLoad(char *name);
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
