#ifndef NU3D_NUTEXANM_H
#define NU3D_NUTEXANM_H

struct numtl_s;
struct nutexanimprog_s;
union variptr_u;

/* Texture animation: cycles a material through a list of texture ids. */
struct nutexanim_s {
    struct nutexanim_s *succ;         /* 0x00 */
    struct nutexanim_s *prev;         /* 0x04 */
    short *tids;                      /* 0x08 */
    short numtids;                    /* 0x0C */
    int dynalloc : 1;                 /* 0x0E */
    struct numtl_s *mtl;              /* 0x10 */
    struct nutexanimenv_s *env;       /* 0x14 */
    char *ntaname;                    /* 0x18 */
    char *scriptname;                 /* 0x1C */
}; /* 0x20 */

/* Execution state of one texture animation program. */
struct nutexanimenv_s {
    struct nutexanimprog_s *prog;     /* 0x00 */
    int pc;                           /* 0x04 */
    int rep_count[16];                /* 0x08 */
    int rep_start[16];                /* 0x48 */
    int rep_ix;                       /* 0x88 */
    int ra[16];                       /* 0x8C */
    int ra_ix;                        /* 0xCC */
    int pause;                        /* 0xD0 */
    int pause_r;                      /* 0xD4 */
    int pause_cnt;                    /* 0xD8 */
    struct numtl_s *mtl;              /* 0xDC */
    short *tids;                      /* 0xE0 */
    int tex_ix;                       /* 0xE4 */
    unsigned int dynalloc : 1;        /* 0xE8 */
}; /* 0xEC */

struct nufpar_s;

void NuTexAnimProgAssembleEnd(struct nutexanimprog_s *prog);
void pftaXDef(struct nufpar_s *parser);
void pftaGoto(struct nufpar_s *parser);
void pftaXRef(struct nufpar_s *parser);
void pftaBtex(struct nufpar_s *parser);
void pftaGosub(struct nufpar_s *parser);
void pftaUntiltex(struct nufpar_s *parser);
void NuTexAnimProgInit(struct nutexanimprog_s *prog);
struct nutexanimprog_s *NuTexAnimProgReadScript(union variptr_u *buff, char *name);
void NuTexAnimEnvProc(struct nutexanimenv_s *env);
void NuTexAnimProgSysInit(void);
void NuTexAnimProgRelease(void);
struct nutexanimprog_s *NuTexAnimProgFind(char *name);
void NuTexAnimProgDestroy(struct nutexanimprog_s *prog);
struct nutexanimprog_s *NuTexAnimProgCreate(union variptr_u *buff, int code_size, char *name);
void NuTexAnimProgWrite(char *name, struct nutexanimprog_s *prog);
struct nutexanimprog_s *NuTexAnimProgRead(union variptr_u *buff, char *name);
void NuTexAnimEnvDestroy(struct nutexanimenv_s *env);
void NuTexAnimEnvReset(struct nutexanimenv_s *env);
struct nutexanimenv_s *NuTexAnimEnvCreate(union variptr_u *buff, struct numtl_s *mtl, short *tids, struct nutexanimprog_s *prog);
void NuTexAnimRestart(void);
void NuTexAnimSetSignals(unsigned int signals);
void NuTexAnimProcessList(struct nutexanim_s *anim);
struct nutexanim_s *NuTexAnimFind(struct nutexanim_s *anim, char *name);
void NuTexAnimDestroy(struct nutexanim_s *anim);
struct nutexanim_s *NuTexAnimCreate(union variptr_u *buff, struct nutexanimprog_s *prog, struct numtl_s *mtl, short *tids, int num_tids);
void NuTexAnimAddList(struct nutexanim_s *anim);
void NuTexAnimRemoveList(struct nutexanim_s *anim);
void NuTexAnimProcess(void);
void pftaTex(struct nufpar_s *parser);
void pftaTexAdj(struct nufpar_s *parser);
void pftaWait(struct nufpar_s *parser);
void pftaRate(struct nufpar_s *parser);
void pftaOn(struct nufpar_s *parser);
void pftaOff(struct nufpar_s *parser);
void pftaLabel(struct nufpar_s *parser);
void pftaRet(struct nufpar_s *parser);
void pftaRepeat(struct nufpar_s *parser);
void pftaRepend(struct nufpar_s *parser);
void pftaEnd(struct nufpar_s *parser);
void pftaScriptname(struct nufpar_s *parser);
void NuTexAnimResetList(struct nutexanim_s *anim);

#endif /* NU3D_NUTEXANM_H */
