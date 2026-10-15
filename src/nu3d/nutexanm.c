/*
 * Unit: nu3d/nutexanm
 *
 * Functions:
 *   0x00159090 NuTexAnimProgAssembleEnd
 *   0x00159188 pftaXDef
 *   0x001592a8 pftaGoto
 *   0x001593d8 pftaXRef
 *   0x00159508 pftaBtex
 *   0x00159760 pftaGosub
 *   0x00159890 pftaUntiltex
 *   0x001599d0 NuTexAnimProgReadScript
 *   0x00159c78 NuTexAnimEnvProc
 *   0x0015a4a8 NuTexAnimProgSysInit
 *   0x0015a4f8 NuTexAnimProgRelease
 *   0x0015a500 NuTexAnimProgFind
 *   0x0015a548 NuTexAnimProgDestroy
 *   0x0015a5a8 NuTexAnimProgCreate
 *   0x0015a690 NuTexAnimProgWrite
 *   0x0015a6e8 NuTexAnimProgRead
 *   0x0015a7e8 NuTexAnimEnvDestroy
 *   0x0015a818 NuTexAnimEnvCreate
 *   0x0015a8f0 NuTexAnimEnvReset
 *   0x0015a920 NuTexAnimRestart
 *   0x0015a988 NuTexAnimSetSignals
 *   0x0015a9b0 NuTexAnimProcessList
 *   0x0015a9f8 NuTexAnimFind
 *   0x0015aa50 NuTexAnimDestroy
 *   0x0015aac0 NuTexAnimCreate
 *   0x0015ac30 NuTexAnimAddList
 *   0x0015ac88 NuTexAnimRemoveList
 *   0x0015ad20 NuTexAnimProcess
 *   0x0015ad90 NuTexAnimProgInit
 *   0x0015adf0 pftaTex
 *   0x0015ae50 pftaTexAdj
 *   0x0015af18 pftaWait
 *   0x0015afb0 pftaRate
 *   0x0015b048 pftaOn
 *   0x0015b090 pftaOff
 *   0x0015b0d8 pftaLabel
 *   0x0015b1e0 pftaRet
 *   0x0015b208 pftaRepeat
 *   0x0015b2a8 pftaRepend
 *   0x0015b2d0 pftaEnd
 *   0x0015b2f8 pftaScriptname
 *   0x0015b330 NuTexAnimResetList
 */

#include "nu_libc.h"
#include "nu_types.h"

#include "nucore/nuerror.h"
#include "numath/nurand.h"
#include "nu3d/numtl.h"
#include "nu3d/nutexanm.h"
#include "nucore/nucoretypes.h"
#include "nucore/nufpar.h"

struct nunrand_s;

enum {
    NTA_TEX = 0,
    NTA_TEXADJ = 1,
    NTA_WAIT = 2,
    NTA_RATE = 5,
    NTA_GOTO = 7,
    NTA_GOSUB = 8,
    NTA_BTEX = 9,
    NTA_RET = 10,
    NTA_REPEAT = 11,
    NTA_REPEND = 12,
    NTA_UNTILTEX = 13,
    NTA_END = 14,
    NTA_XREF = 15
};

struct nutexanimprog_s {
    struct nutexanimprog_s *next;
    struct nutexanimprog_s *prev;
    char name[32];
    int on_pc[32];
    int off_pc[32];
    unsigned int on_mask;
    unsigned int off_mask;
    short xdef_label[32];
    short xdef_pc[32];
    int num_xdefs;
    short num_codes;
    int dynalloc : 1;
    short code[2];
};

typedef struct NuTexAnimList {
    struct nutexanim_s *anims;
    struct NuTexAnimList *next;
    struct NuTexAnimList *prev;
} NuTexAnimList;


extern int nta_labels[64];
extern NuTexAnimList ntalsysbuff[64];
extern NuTexAnimList *ntal_first;
extern NuTexAnimList *ntal_free;

extern struct nufpcomjmp_s D_002E9100[];
extern struct nutexanimprog_s *D_0062EF30;
extern int D_00633188;
extern int D_0063318C;
extern struct nutexanimprog_s *D_00633190;
extern unsigned int D_00633194;
extern unsigned int D_00633198;
extern unsigned int D_0063319C;
extern char D_00615900[];
extern char D_00615980[];
extern char D_006159A8[];
extern char D_006159D8[];
extern char D_00615960[];
extern char D_00615A30[];
extern char D_00615A68[];
extern char D_00615AA0[];
extern char D_00615AE0[];
extern char D_00615B18[];
extern char D_006AE528[64][21];
extern char D_006AEA68[256][21];

#define nta_commands D_002E9100
#define ntap_first D_0062EF30
#define nta_numlabels D_00633188
#define nta_numxdefs D_0063318C
#define nta_prog D_00633190
#define nta_signals_on D_00633194
#define nta_signals_off D_00633198
#define nta_signals D_0063319C
#define nutexanm_srcfile D_00615900
#define nta_unknown_condition_msg D_00615980
#define nta_too_many_labels_msg D_006159A8
#define nta_too_many_xdefs_msg D_006159D8
#define nta_bad_condition_msg D_00615960
#define nta_call_overflow_msg D_00615A30
#define nta_call_underflow_msg D_00615A68
#define nta_repeat_overflow_msg D_00615AA0
#define nta_repend_msg D_00615AE0
#define nta_untiltex_msg D_00615B18
#define nta_label_names D_006AE528
#define nta_xdef_names D_006AEA68

extern void *NuMemAllocFn(int size, char *file, int line);
extern void NuMemFreeFn(void *ptr, char *file, int line);
extern int NuFileOpen(char *name, int mode);
extern int NuFileOpenSize(int fh);
extern int NuFileRead(int fh, void *buf, int size);
extern int NuFileWrite(int fh, void *data, int size);
extern void NuFileClose(int fh);
extern void NuDisableVBlank(void);
extern void NuEnableVBlank(void);
extern void NuStrCpy(char *dst, char *src);


void NuTexAnimProgAssembleEnd(struct nutexanimprog_s *prog)
{
    short i;

    for (i = 0; i < prog->num_codes;) {
        switch (prog->code[i]) {
        case NTA_TEXADJ:
            i += 4;
            break;
        case NTA_WAIT:
            i += 3;
            break;
        case NTA_RATE:
            i += 3;
            break;
        case NTA_BTEX:
            prog->code[i + 3] = nta_labels[prog->code[i + 3]];
            i += 4;
            break;
        case NTA_GOTO:
        case NTA_GOSUB:
            prog->code[i + 1] = nta_labels[prog->code[i + 1]];
            i += 2;
            break;
        case NTA_RET:
            i += 1;
            break;
        case NTA_REPEAT:
            i += 3;
            break;
        case NTA_REPEND:
            i += 1;
            break;
        case NTA_UNTILTEX:
            i += 3;
            break;
        case NTA_END:
            i += 1;
            break;
        case NTA_TEX:
        case NTA_XREF:
            i += 2;
            break;
        }
    }
}


static inline int NuTexAnimLabelIndex(char *name)
{
    int i;

    if (strlen(name) > 20) {
        name[20] = 0;
    }
    for (i = 0; i < nta_numlabels; i++) {
        if (!strcasecmp(nta_label_names[i], name)) {
            return i;
        }
    }
    if (nta_numlabels >= 64) {
        NuErrorProlog(nutexanm_srcfile, 0x209)(nta_too_many_labels_msg);
    }
    NuStrCpy(nta_label_names[nta_numlabels++], name);
    return nta_numlabels - 1;
}


static inline int NuTexAnimXDefIndex(char *name)
{
    int i;

    if (strlen(name) > 20) {
        name[20] = 0;
    }
    for (i = 0; i < nta_numxdefs; i++) {
        if (!strcasecmp(nta_xdef_names[i], name)) {
            return i;
        }
    }
    if (nta_numxdefs >= 256) {
        NuErrorProlog(nutexanm_srcfile, 0x21E)(nta_too_many_xdefs_msg);
    }
    NuStrCpy(nta_xdef_names[nta_numxdefs++], name);
    return nta_numxdefs - 1;
}


void pftaXDef(struct nufpar_s *parser)
{
    int xdef;

    NuFParGetWord(parser);
    xdef = NuTexAnimXDefIndex(parser->wbuff);
    nta_prog->xdef_label[nta_prog->num_xdefs] = xdef;
    nta_prog->xdef_pc[nta_prog->num_xdefs] = nta_prog->num_codes;
    nta_prog->num_xdefs++;
}


void pftaGoto(struct nufpar_s *parser)
{
    int label;

    NuFParGetWord(parser);
    label = NuTexAnimLabelIndex(parser->wbuff);
    nta_prog->code[nta_prog->num_codes++] = NTA_GOTO;
    nta_prog->code[nta_prog->num_codes++] = label;
}


void pftaXRef(struct nufpar_s *parser)
{
    int xdef;

    NuFParGetWord(parser);
    xdef = NuTexAnimXDefIndex(parser->wbuff);
    nta_prog->code[nta_prog->num_codes++] = NTA_XREF;
    nta_prog->code[nta_prog->num_codes++] = xdef;
}


void pftaBtex(struct nufpar_s *parser)
{
    int condition;
    int tex;
    int label;

    NuFParGetWord(parser);
    switch (parser->wbuff[0]) {
    case '=':
        condition = 0;
        break;
    case '<':
        switch (parser->wbuff[1]) {
        case '>':
            condition = 5;
            break;
        case '=':
            condition = 3;
            break;
        default:
            condition = 1;
            break;
        }
        break;
    case '>':
        if (parser->wbuff[1] == '=') {
            condition = 4;
        } else {
            condition = 2;
        }
        break;
    case '!':
        condition = 5;
        break;
    default:
        NuErrorProlog(nutexanm_srcfile, 0x1EC)(nta_unknown_condition_msg, parser->wbuff, parser->line_num);
        break;
    }
    tex = NuFParGetInt(parser);
    NuFParGetWord(parser);
    label = NuTexAnimLabelIndex(parser->wbuff);
    nta_prog->code[nta_prog->num_codes++] = NTA_BTEX;
    nta_prog->code[nta_prog->num_codes++] = condition;
    nta_prog->code[nta_prog->num_codes++] = tex;
    nta_prog->code[nta_prog->num_codes++] = label;
}


void pftaGosub(struct nufpar_s *parser)
{
    int label;

    NuFParGetWord(parser);
    label = NuTexAnimLabelIndex(parser->wbuff);
    nta_prog->code[nta_prog->num_codes++] = NTA_GOSUB;
    nta_prog->code[nta_prog->num_codes++] = label;
}


void pftaUntiltex(struct nufpar_s *parser)
{
    int condition;
    int tex;

    NuFParGetWord(parser);
    switch (parser->wbuff[0]) {
    case '=':
        condition = 0;
        break;
    case '<':
        switch (parser->wbuff[1]) {
        case '>':
            condition = 5;
            break;
        case '=':
            condition = 3;
            break;
        default:
            condition = 1;
            break;
        }
        break;
    case '!':
        condition = 5;
        break;
    case '>':
        if (parser->wbuff[1] == '=') {
            condition = 4;
        } else {
            condition = 2;
        }
        break;
    default:
        NuErrorProlog(nutexanm_srcfile, 0x1EC)(nta_unknown_condition_msg, parser->wbuff, parser->line_num);
        break;
    }
    tex = NuFParGetInt(parser);
    nta_prog->code[nta_prog->num_codes++] = NTA_UNTILTEX;
    nta_prog->code[nta_prog->num_codes++] = condition;
    nta_prog->code[nta_prog->num_codes++] = tex;
}


inline void NuTexAnimProgInit(struct nutexanimprog_s *prog)
{
    int i;

    if (prog) {
        for (i = 0; i < 32; i++) {
            prog->off_pc[i] = -1;
            prog->on_pc[i] = -1;
        }
        prog->num_codes = 0;
        prog->on_mask = 0;
        prog->off_mask = 0;
        prog->name[0] = 0;
        prog->num_xdefs = 0;
        prog->dynalloc = 0;
    }
}


struct nutexanimprog_s *NuTexAnimProgReadScript(union variptr_u *buff, char *name)
{
    struct nutexanimprog_s *mem;
    struct nutexanimprog_s *prog;
    struct nufpar_s *parser;
    int len;
    int label;
    short *end;

    if (buff) {
        mem = buff->voidptr;
        mem = (struct nutexanimprog_s *)(((int)mem + 3) & 0xFFFFFFFC);
    } else {
        mem = NuMemAllocFn(0x400, nutexanm_srcfile, 0x2FC);
    }
    memset(nta_label_names, 0, sizeof(nta_label_names));
    prog = 0;
    nta_numlabels = 0;
    parser = NuFParCreate(name);
    if (parser) {
        NuFParPushCom(parser, nta_commands);
        prog = mem;
        NuTexAnimProgInit(prog);
        nta_prog = mem;
        while (NuFParGetLine(parser)) {
            len = NuFParGetWord(parser);
            if (len && !NuFParInterpretWord(parser) && parser->wbuff[0] && parser->wbuff[len - 1] == ':') {
                parser->wbuff[len - 1] = 0;
                label = NuTexAnimLabelIndex(parser->wbuff);
                nta_labels[label] = nta_prog->num_codes;
            }
        }
        end = &mem->code[mem->num_codes];
        mem = (struct nutexanimprog_s *)end;
        if (buff) {
            buff->voidptr = end;
        }
        NuFParDestroy(parser);
        NuTexAnimProgAssembleEnd(prog);
        prog->next = ntap_first;
        if (ntap_first) {
            ntap_first->prev = prog;
        }
        prog->prev = 0;
        ntap_first = prog;
    }
    return prog;
}


static inline int NuTexAnimCondition(int condition, int value, int ref)
{
    switch (condition) {
    case 0:
        if (value == ref) {
            return 1;
        }
        break;
    case 1:
        if (value < ref) {
            return 1;
        }
        break;
    case 2:
        if (value > ref) {
            return 1;
        }
        break;
    case 3:
        if (value <= ref) {
            return 1;
        }
        break;
    case 4:
        if (value >= ref) {
            return 1;
        }
        break;
    case 5:
        if (value != ref) {
            return 1;
        }
        break;
    default:
        NuErrorProlog(nutexanm_srcfile, 0x1BE)(nta_bad_condition_msg, condition);
        break;
    }
    return 0;
}


void NuTexAnimEnvProc(struct nutexanimenv_s *env)
{
    struct nutexanimprog_s *prog;
    NuTexAnimList *list;
    struct nutexanim_s *anim;
    struct nutexanimenv_s *other;
    struct nutexanimprog_s *other_prog;
    int done;
    int i;
    int signal;
    int tex;
    int xdef;

    done = 0;
    prog = env->prog;
    if (!prog) {
        return;
    }
    if (nta_signals_off & prog->off_mask) {
        for (signal = 0; signal < 32; signal++) {
            if (nta_signals_off & (1 << signal) & prog->off_mask) {
                env->pc = prog->off_pc[signal];
                env->pause_cnt = 0;
                env->ra_ix = 0;
                env->rep_ix = 0;
                break;
            }
        }
    }
    if (nta_signals_on & prog->on_mask) {
        for (signal = 0; signal < 32; signal++) {
            if (nta_signals_on & (1 << signal) & prog->on_mask) {
                env->pc = prog->on_pc[signal];
                env->pause_cnt = 0;
                env->ra_ix = 0;
                env->rep_ix = 0;
                break;
            }
        }
    }
    if (env->pause_cnt) {
        env->pause_cnt--;
        return;
    }
    do {
        switch (prog->code[env->pc]) {
        case NTA_TEX:
            env->tex_ix = prog->code[env->pc + 1];
            env->mtl->tid = env->tids[env->tex_ix];
            env->pc += 2;
            env->pause_cnt = env->pause;
            if (env->pause_r) {
                env->pause_cnt += NuRand(0) % env->pause_r;
            }
            done = 1;
            break;
        case NTA_TEXADJ:
            tex = env->tex_ix + prog->code[env->pc + 1];
            if (tex < prog->code[env->pc + 2]) {
                tex = prog->code[env->pc + 2];
            }
            if (tex > prog->code[env->pc + 3]) {
                tex = prog->code[env->pc + 3];
            }
            env->tex_ix = tex;
            env->mtl->tid = env->tids[tex];
            env->pc += 4;
            env->pause_cnt = env->pause;
            if (env->pause_r) {
                env->pause_cnt += NuRand(0) % env->pause_r;
            }
            done = 1;
            break;
        case NTA_WAIT:
            env->pause_cnt = prog->code[env->pc + 1];
            if (prog->code[env->pc + 2]) {
                env->pause_cnt += NuRand(0) % prog->code[env->pc + 2];
            }
            done = 1;
            env->pc += 3;
            break;
        case NTA_RATE:
            env->pause = prog->code[env->pc + 1];
            env->pause_r = prog->code[env->pc + 2];
            env->pc += 3;
            break;
        case NTA_GOTO:
            env->pc = prog->code[env->pc + 1];
            break;
        case NTA_BTEX:
            if (NuTexAnimCondition(prog->code[env->pc + 1], env->tex_ix, prog->code[env->pc + 2])) {
                env->pc = prog->code[env->pc + 3];
            } else {
                env->pc += 4;
            }
            break;
        case NTA_GOSUB:
            if (env->ra_ix >= 16) {
                NuErrorProlog(nutexanm_srcfile, 0x3C8)(nta_call_overflow_msg, env->pc);
            }
            env->ra[env->ra_ix++] = env->pc + 2;
            env->pc = prog->code[env->pc + 1];
            break;
        case NTA_RET:
            if (env->ra_ix == 0) {
                NuErrorProlog(nutexanm_srcfile, 0x3D0)(nta_call_underflow_msg, env->pc);
            }
            env->pc = env->ra[--env->ra_ix];
            break;
        case NTA_REPEAT:
            if (env->rep_ix >= 16) {
                NuErrorProlog(nutexanm_srcfile, 0x3D8)(nta_repeat_overflow_msg, env->pc);
            }
            env->rep_count[env->rep_ix] = prog->code[env->pc + 1];
            if (prog->code[env->pc + 2]) {
                env->rep_count[env->rep_ix] += NuRand(0) % prog->code[env->pc + 2];
            }
            env->pc += 3;
            env->rep_start[env->rep_ix++] = env->pc;
            break;
        case NTA_REPEND:
            if (env->rep_ix == 0) {
                NuErrorProlog(nutexanm_srcfile, 0x3E2)(nta_repend_msg, env->pc);
            }
            if (!env->rep_count[env->rep_ix - 1]) {
                env->rep_ix--;
                env->pc++;
            } else {
                env->pc = env->rep_start[env->rep_ix - 1];
                env->rep_count[env->rep_ix - 1]--;
            }
            break;
        case NTA_UNTILTEX:
            if (env->rep_ix == 0) {
                NuErrorProlog(nutexanm_srcfile, 0x3F0)(nta_untiltex_msg, env->pc);
            }
            if (NuTexAnimCondition(prog->code[env->pc + 1], env->tex_ix, prog->code[env->pc + 2])
                || !env->rep_count[env->rep_ix - 1]) {
                env->pc += 3;
                env->rep_ix--;
            } else {
                env->pc = env->rep_start[env->rep_ix - 1];
                env->rep_count[env->rep_ix - 1]--;
            }
            break;
        case NTA_END:
            done = 1;
            break;
        case NTA_XREF:
            xdef = prog->code[env->pc + 1];
            for (list = ntal_first; list; list = list->next) {
                for (anim = list->anims; anim; anim = anim->succ) {
                    other = anim->env;
                    if (other && other != env && other->prog && other->prog->num_xdefs) {
                        other_prog = other->prog;
                        for (i = 0; i < other_prog->num_xdefs; i++) {
                            if (other_prog->xdef_label[i] == xdef) {
                                other->pc = other_prog->xdef_pc[i];
                                other->pause_cnt = 0;
                                other->ra_ix = 0;
                                other->rep_ix = 0;
                                break;
                            }
                        }
                    }
                }
            }
            env->pc += 2;
            break;
        }
    } while (!done);
}


void NuTexAnimProgSysInit(void)
{
    int i;

    ntap_first = 0;
    for (i = 0; i < 63; i++) {
        ntalsysbuff[i].next = &ntalsysbuff[i + 1];
    }
    ntal_first = 0;
    ntalsysbuff[63].next = 0;
    ntal_free = ntalsysbuff;
    nta_numxdefs = 0;
    nta_signals = 0;
    nta_signals_off = 0;
    nta_signals_on = 0;
}


void NuTexAnimProgRelease(void)
{
    ntap_first = 0;
}


struct nutexanimprog_s *NuTexAnimProgFind(char *name)
{
    struct nutexanimprog_s *prog;

    for (prog = ntap_first; prog; prog = prog->next) {
        if (!strcasecmp(name, prog->name)) {
            break;
        }
    }
    return prog;
}


void NuTexAnimProgDestroy(struct nutexanimprog_s *prog)
{
    if (prog->prev) {
        prog->prev->next = prog->next;
    } else {
        ntap_first = prog->next;
    }
    if (prog->next) {
        prog->next->prev = prog->prev;
    }
    if (prog->dynalloc) {
        NuMemFreeFn(prog, nutexanm_srcfile, 0xAD);
    }
}


struct nutexanimprog_s *NuTexAnimProgCreate(union variptr_u *buff, int code_size, char *name)
{
    struct nutexanimprog_s *prog;

    if (buff) {
        prog = buff->voidptr;
        buff->u8 += sizeof(struct nutexanimprog_s);
        buff->u8 += code_size;
    } else {
        prog = NuMemAllocFn(sizeof(struct nutexanimprog_s) + code_size, nutexanm_srcfile, 0xDD);
    }
    if (prog) {
        NuTexAnimProgInit(prog);
        if (!buff) {
            prog->dynalloc = 1;
        }
        if (name) {
            NuStrCpy(prog->name, name);
        }
    }
    return prog;
}


void NuTexAnimProgWrite(char *name, struct nutexanimprog_s *prog)
{
    int fh;

    fh = NuFileOpen(name, 1);
    if (fh) {
        NuFileWrite(fh, prog, sizeof(struct nutexanimprog_s) + prog->num_codes * sizeof(short));
        NuFileClose(fh);
    }
}


struct nutexanimprog_s *NuTexAnimProgRead(union variptr_u *buff, char *name)
{
    struct nutexanimprog_s *prog;
    int fh;
    int size;

    prog = 0;
    fh = NuFileOpen(name, 0);
    if (fh) {
        size = NuFileOpenSize(fh);
        if (buff) {
            prog = (struct nutexanimprog_s *)((buff->intaddr + 3) & 0xFFFFFFFC);
            buff->voidptr = (char *)prog + size;
        } else {
            prog = NuMemAllocFn(size, nutexanm_srcfile, 0x117);
        }
        NuFileRead(fh, prog, size);
        NuFileClose(fh);
        prog->next = ntap_first;
        if (ntap_first) {
            ntap_first->prev = prog;
        }
        prog->prev = 0;
        ntap_first = prog;
        if (buff) {
            prog->dynalloc = 0;
        } else {
            prog->dynalloc = 1;
        }
    }
    return prog;
}


inline void NuTexAnimEnvDestroy(struct nutexanimenv_s *env)
{
    if (env->dynalloc) {
        NuMemFreeFn(env, nutexanm_srcfile, 0x161);
    }
}


inline void NuTexAnimEnvReset(struct nutexanimenv_s *env)
{
    if (env) {
        env->pc = 0;
        env->rep_ix = 0;
        env->ra_ix = 0;
        env->pause = 0;
        env->pause_r = 0;
        env->pause_cnt = 0;
        env->tex_ix = 0;
    }
}


inline struct nutexanimenv_s *NuTexAnimEnvCreate(union variptr_u *buff, struct numtl_s *mtl, short *tids, struct nutexanimprog_s *prog)
{
    struct nutexanimenv_s *env;

    if (buff) {
        env = (struct nutexanimenv_s *)((buff->intaddr + 3) & 0xFFFFFFFC);
        buff->voidptr = (char *)env + sizeof(struct nutexanimenv_s);
    } else {
        env = NuMemAllocFn(sizeof(struct nutexanimenv_s), nutexanm_srcfile, 0x17D);
    }
    if (env) {
        env->prog = prog;
        env->mtl = mtl;
        env->tids = tids;
        NuTexAnimEnvReset(env);
        if (buff) {
            env->dynalloc = 0;
        } else {
            env->dynalloc = 1;
        }
    }
    return env;
}


void NuTexAnimRestart(void)
{
    NuTexAnimList *list;
    struct nutexanim_s *anim;

    for (list = ntal_first; list; list = list->next) {
        for (anim = list->anims; anim; anim = anim->succ) {
            NuTexAnimEnvReset(anim->env);
        }
    }
}


void NuTexAnimSetSignals(unsigned int signals)
{
    unsigned int changed;

    changed = signals ^ nta_signals;
    nta_signals = signals;
    nta_signals_on = changed & signals;
    nta_signals_off = changed & ~signals;
}


void NuTexAnimProcessList(struct nutexanim_s *anim)
{
    for (; anim; anim = anim->succ) {
        if (anim->env) {
            NuTexAnimEnvProc(anim->env);
        }
    }
}


struct nutexanim_s *NuTexAnimFind(struct nutexanim_s *anim, char *name)
{
    for (; anim; anim = anim->succ) {
        if (!strcasecmp(anim->ntaname, name)) {
            return anim;
        }
    }
    return 0;
}


void NuTexAnimDestroy(struct nutexanim_s *anim)
{
    if (anim->env) {
        NuTexAnimEnvDestroy(anim->env);
    }
    if (anim->dynalloc) {
        NuMemFreeFn(anim, nutexanm_srcfile, 0x4AE);
    }
}


struct nutexanim_s *NuTexAnimCreate(union variptr_u *buff, struct nutexanimprog_s *prog, struct numtl_s *mtl, short *tids, int num_tids)
{
    struct nutexanim_s *anim;

    if (buff) {
        anim = (struct nutexanim_s *)((buff->intaddr + 3) & 0xFFFFFFFC);
        buff->voidptr = (char *)anim + sizeof(struct nutexanim_s);
        buff->u8 += num_tids * sizeof(short);
    } else {
        anim = NuMemAllocFn(sizeof(struct nutexanim_s) + num_tids * sizeof(short), nutexanm_srcfile, 0x4CC);
    }
    anim->prev = 0;
    anim->succ = 0;
    anim->numtids = num_tids;
    anim->mtl = mtl;
    anim->ntaname = 0;
    anim->tids = (short *)(anim + 1);
    memcpy(anim->tids, tids, num_tids * sizeof(short));
    anim->env = NuTexAnimEnvCreate(buff, mtl, anim->tids, prog);
    return anim;
}


void NuTexAnimAddList(struct nutexanim_s *anim)
{
    NuTexAnimList *list;

    NuDisableVBlank();
    list = ntal_free;
    if (list) {
        ntal_free = list->next;
        list->anims = anim;
        list->next = ntal_first;
        list->prev = 0;
        if (ntal_first) {
            ntal_first->prev = list;
        }
        ntal_first = list;
    }
    NuEnableVBlank();
}


void NuTexAnimRemoveList(struct nutexanim_s *anim)
{
    NuTexAnimList *list;

    NuDisableVBlank();
    for (list = ntal_first; list; list = list->next) {
        if (list->anims == anim) {
            if (list->next) {
                list->next->prev = list->prev;
            }
            if (list->prev) {
                list->prev->next = list->next;
            } else {
                ntal_first = list->next;
            }
            list->next = ntal_free;
            ntal_free = list;
            NuEnableVBlank();
            return;
        }
    }
    NuEnableVBlank();
}


void NuTexAnimProcess(void)
{
    NuTexAnimList *list;
    struct nutexanim_s *anim;

    for (list = ntal_first; list; list = list->next) {
        for (anim = list->anims; anim; anim = anim->succ) {
            if (anim->env) {
                NuTexAnimEnvProc(anim->env);
            }
        }
    }
}


void pftaTex(struct nufpar_s *parser)
{
    int tex;

    tex = NuFParGetInt(parser);
    nta_prog->code[nta_prog->num_codes++] = NTA_TEX;
    nta_prog->code[nta_prog->num_codes++] = tex;
}


void pftaTexAdj(struct nufpar_s *parser)
{
    int tex;
    int adjust;
    int count;

    tex = NuFParGetInt(parser);
    adjust = NuFParGetInt(parser);
    count = NuFParGetInt(parser);
    nta_prog->code[nta_prog->num_codes++] = NTA_TEXADJ;
    nta_prog->code[nta_prog->num_codes++] = tex;
    nta_prog->code[nta_prog->num_codes++] = adjust;
    nta_prog->code[nta_prog->num_codes++] = count;
}


void pftaWait(struct nufpar_s *parser)
{
    int base;
    int random;

    base = NuFParGetInt(parser);
    random = NuFParGetInt(parser);
    nta_prog->code[nta_prog->num_codes++] = NTA_WAIT;
    nta_prog->code[nta_prog->num_codes++] = base;
    nta_prog->code[nta_prog->num_codes++] = random;
}


void pftaRate(struct nufpar_s *parser)
{
    int base;
    int random;

    base = NuFParGetInt(parser);
    random = NuFParGetInt(parser);
    nta_prog->code[nta_prog->num_codes++] = NTA_RATE;
    nta_prog->code[nta_prog->num_codes++] = base;
    nta_prog->code[nta_prog->num_codes++] = random;
}


void pftaOn(struct nufpar_s *parser)
{
    int signal;

    signal = NuFParGetInt(parser);
    nta_prog->on_pc[signal] = nta_prog->num_codes;
    nta_prog->on_mask |= 1 << signal;
}


void pftaOff(struct nufpar_s *parser)
{
    int signal;

    signal = NuFParGetInt(parser);
    nta_prog->off_pc[signal] = nta_prog->num_codes;
    nta_prog->off_mask |= 1 << signal;
}


void pftaLabel(struct nufpar_s *parser)
{
    int label;

    NuFParGetWord(parser);
    label = NuTexAnimLabelIndex(parser->wbuff);
    nta_labels[label] = nta_prog->num_codes;
}


void pftaRet(struct nufpar_s *parser)
{
    nta_prog->code[nta_prog->num_codes++] = NTA_RET;
}


void pftaRepeat(struct nufpar_s *parser)
{
    int count;
    int label;

    count = NuFParGetInt(parser);
    if (count == 0) {
        count = 0x7FFFFFFF;
    }
    label = NuFParGetInt(parser);
    nta_prog->code[nta_prog->num_codes++] = NTA_REPEAT;
    nta_prog->code[nta_prog->num_codes++] = count;
    nta_prog->code[nta_prog->num_codes++] = label;
}


void pftaRepend(struct nufpar_s *parser)
{
    nta_prog->code[nta_prog->num_codes++] = NTA_REPEND;
}


void pftaEnd(struct nufpar_s *parser)
{
    nta_prog->code[nta_prog->num_codes++] = NTA_END;
}


void pftaScriptname(struct nufpar_s *parser)
{
    NuFParGetWord(parser);
    parser->wbuff[32] = 0;
    strcpy(nta_prog->name, parser->wbuff);
}


void NuTexAnimResetList(struct nutexanim_s *anim)
{
    for (; anim; anim = anim->succ) {
        NuTexAnimEnvReset(anim->env);
    }
}
