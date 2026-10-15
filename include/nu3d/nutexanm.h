#ifndef NU3D_NUTEXANM_H
#define NU3D_NUTEXANM_H

struct numtl_s;
struct nutexanimenv_s;
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

struct NuFPar;
struct nutexanimprog_s;

void NuTexAnimProgAssembleEnd(struct nutexanimprog_s *prog);
int NuTexAnimLabelIndex(char *name);
int NuTexAnimXDefIndex(char *name);
void pftaXDef(struct NuFPar *parser);
void pftaGoto(struct NuFPar *parser);
void pftaXRef(struct NuFPar *parser);
void pftaBtex(struct NuFPar *parser);
void pftaGosub(struct NuFPar *parser);
void pftaUntiltex(struct NuFPar *parser);
void NuTexAnimProgInit(struct nutexanimprog_s *prog);
struct nutexanimprog_s *NuTexAnimProgReadScript(union variptr_u *buff, char *name);
int NuTexAnimCondition(int condition, int value, int ref);
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
void pftaTex(struct NuFPar *parser);
void pftaTexAdj(struct NuFPar *parser);
void pftaWait(struct NuFPar *parser);
void pftaRate(struct NuFPar *parser);
void pftaOn(struct NuFPar *parser);
void pftaOff(struct NuFPar *parser);
void pftaLabel(struct NuFPar *parser);
void pftaRet(struct NuFPar *parser);
void pftaRepeat(struct NuFPar *parser);
void pftaRepend(struct NuFPar *parser);
void pftaEnd(struct NuFPar *parser);
void pftaScriptname(struct NuFPar *parser);
void NuTexAnimResetList(struct nutexanim_s *anim);

#endif /* NU3D_NUTEXANM_H */
