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

typedef struct NuFPar {
    int handle;
    char buffer[0x1000];
    char line[0x101];
    char word[0x101];
    int line_num;
} NuFPar;

struct numtl_s {
    char pad000[0x1A4];
    int tid;
};

struct nunrand_s;

typedef void (*NuErrorFunc)(const char *fmt, ...);

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

typedef struct NuFParCommand {
    char *name;
    void (*func)(NuFPar *parser);
} NuFParCommand;

typedef struct NuTexAnimProg {
    struct NuTexAnimProg *next;
    struct NuTexAnimProg *prev;
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
} NuTexAnimProg;

typedef struct NuTexAnimEnv {
    NuTexAnimProg *prog;
    int pc;
    int repeat_count[16];
    int repeat_pc[16];
    int repeat_depth;
    int call_stack[16];
    int call_depth;
    int wait_base;
    int wait_random;
    int wait;
    struct numtl_s *mtl;
    short *tids;
    int tex;
    unsigned int dynalloc : 1;
} NuTexAnimEnv;

typedef struct NuTexAnim {
    struct NuTexAnim *next;
    struct NuTexAnim *prev;
    short *tids;
    short num_tids;
    int dynalloc : 1;
    struct numtl_s *mtl;
    NuTexAnimEnv *env;
    char *name;
    char *scriptname;
} NuTexAnim;

typedef struct NuTexAnimList {
    NuTexAnim *anims;
    struct NuTexAnimList *next;
    struct NuTexAnimList *prev;
} NuTexAnimList;


extern int nta_labels[64];
extern NuTexAnimList ntalsysbuff[64];
extern NuTexAnimList *ntal_first;
extern NuTexAnimList *ntal_free;

extern NuFParCommand D_002E9100[];
extern NuTexAnimProg *D_0062EF30;
extern int D_00633188;
extern int D_0063318C;
extern NuTexAnimProg *D_00633190;
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
extern int NuFParGetWord(NuFPar *parser);
extern int NuFParGetInt(NuFPar *parser);
extern NuFPar *NuFParCreate(char *name);
extern void NuFParDestroy(NuFPar *parser);
extern int NuFParGetLine(NuFPar *parser);
extern int NuFParPushCom(NuFPar *parser, NuFParCommand *commands);
extern int NuFParInterpretWord(NuFPar *parser);
extern void NuDisableVBlank(void);
extern void NuEnableVBlank(void);
extern long NuRand(struct nunrand_s *rand);
extern NuErrorFunc NuErrorProlog(const char *file, int line);
extern void NuTexAnimEnvProc(NuTexAnimEnv *env);
extern int strcasecmp(char *a, char *b);
extern unsigned int strlen(const char *s);
extern void *memset(void *dst, int value, unsigned int size);
extern void *memcpy(void *dst, const void *src, unsigned int size);
extern char *strcpy(char *dst, const char *src);
extern void NuStrCpy(char *dst, char *src);


void NuTexAnimProgAssembleEnd(NuTexAnimProg *prog)
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


void pftaXDef(NuFPar *parser)
{
    int xdef;

    NuFParGetWord(parser);
    xdef = NuTexAnimXDefIndex(parser->word);
    nta_prog->xdef_label[nta_prog->num_xdefs] = xdef;
    nta_prog->xdef_pc[nta_prog->num_xdefs] = nta_prog->num_codes;
    nta_prog->num_xdefs++;
}


void pftaGoto(NuFPar *parser)
{
    int label;

    NuFParGetWord(parser);
    label = NuTexAnimLabelIndex(parser->word);
    nta_prog->code[nta_prog->num_codes++] = NTA_GOTO;
    nta_prog->code[nta_prog->num_codes++] = label;
}


void pftaXRef(NuFPar *parser)
{
    int xdef;

    NuFParGetWord(parser);
    xdef = NuTexAnimXDefIndex(parser->word);
    nta_prog->code[nta_prog->num_codes++] = NTA_XREF;
    nta_prog->code[nta_prog->num_codes++] = xdef;
}


void pftaBtex(NuFPar *parser)
{
    int condition;
    int tex;
    int label;

    NuFParGetWord(parser);
    switch (parser->word[0]) {
    case '=':
        condition = 0;
        break;
    case '<':
        switch (parser->word[1]) {
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
        if (parser->word[1] == '=') {
            condition = 4;
        } else {
            condition = 2;
        }
        break;
    case '!':
        condition = 5;
        break;
    default:
        NuErrorProlog(nutexanm_srcfile, 0x1EC)(nta_unknown_condition_msg, parser->word, parser->line_num);
        break;
    }
    tex = NuFParGetInt(parser);
    NuFParGetWord(parser);
    label = NuTexAnimLabelIndex(parser->word);
    nta_prog->code[nta_prog->num_codes++] = NTA_BTEX;
    nta_prog->code[nta_prog->num_codes++] = condition;
    nta_prog->code[nta_prog->num_codes++] = tex;
    nta_prog->code[nta_prog->num_codes++] = label;
}


void pftaGosub(NuFPar *parser)
{
    int label;

    NuFParGetWord(parser);
    label = NuTexAnimLabelIndex(parser->word);
    nta_prog->code[nta_prog->num_codes++] = NTA_GOSUB;
    nta_prog->code[nta_prog->num_codes++] = label;
}


void pftaUntiltex(NuFPar *parser)
{
    int condition;
    int tex;

    NuFParGetWord(parser);
    switch (parser->word[0]) {
    case '=':
        condition = 0;
        break;
    case '<':
        switch (parser->word[1]) {
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
        if (parser->word[1] == '=') {
            condition = 4;
        } else {
            condition = 2;
        }
        break;
    default:
        NuErrorProlog(nutexanm_srcfile, 0x1EC)(nta_unknown_condition_msg, parser->word, parser->line_num);
        break;
    }
    tex = NuFParGetInt(parser);
    nta_prog->code[nta_prog->num_codes++] = NTA_UNTILTEX;
    nta_prog->code[nta_prog->num_codes++] = condition;
    nta_prog->code[nta_prog->num_codes++] = tex;
}


inline void NuTexAnimProgInit(NuTexAnimProg *prog)
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


NuTexAnimProg *NuTexAnimProgReadScript(char **buff, char *name)
{
    NuTexAnimProg *mem;
    NuTexAnimProg *prog;
    NuFPar *parser;
    int len;
    int label;
    short *end;

    if (buff) {
        mem = (NuTexAnimProg *)*buff;
        mem = (NuTexAnimProg *)(((int)mem + 3) & 0xFFFFFFFC);
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
            if (len && !NuFParInterpretWord(parser) && parser->word[0] && parser->word[len - 1] == ':') {
                parser->word[len - 1] = 0;
                label = NuTexAnimLabelIndex(parser->word);
                nta_labels[label] = nta_prog->num_codes;
            }
        }
        end = &mem->code[mem->num_codes];
        mem = (NuTexAnimProg *)end;
        if (buff) {
            *buff = (char *)end;
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


void NuTexAnimEnvProc(NuTexAnimEnv *env)
{
    NuTexAnimProg *prog;
    NuTexAnimList *list;
    NuTexAnim *anim;
    NuTexAnimEnv *other;
    NuTexAnimProg *other_prog;
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
                env->wait = 0;
                env->call_depth = 0;
                env->repeat_depth = 0;
                break;
            }
        }
    }
    if (nta_signals_on & prog->on_mask) {
        for (signal = 0; signal < 32; signal++) {
            if (nta_signals_on & (1 << signal) & prog->on_mask) {
                env->pc = prog->on_pc[signal];
                env->wait = 0;
                env->call_depth = 0;
                env->repeat_depth = 0;
                break;
            }
        }
    }
    if (env->wait) {
        env->wait--;
        return;
    }
    do {
        switch (prog->code[env->pc]) {
        case NTA_TEX:
            env->tex = prog->code[env->pc + 1];
            env->mtl->tid = env->tids[env->tex];
            env->pc += 2;
            env->wait = env->wait_base;
            if (env->wait_random) {
                env->wait += NuRand(0) % env->wait_random;
            }
            done = 1;
            break;
        case NTA_TEXADJ:
            tex = env->tex + prog->code[env->pc + 1];
            if (tex < prog->code[env->pc + 2]) {
                tex = prog->code[env->pc + 2];
            }
            if (tex > prog->code[env->pc + 3]) {
                tex = prog->code[env->pc + 3];
            }
            env->tex = tex;
            env->mtl->tid = env->tids[tex];
            env->pc += 4;
            env->wait = env->wait_base;
            if (env->wait_random) {
                env->wait += NuRand(0) % env->wait_random;
            }
            done = 1;
            break;
        case NTA_WAIT:
            env->wait = prog->code[env->pc + 1];
            if (prog->code[env->pc + 2]) {
                env->wait += NuRand(0) % prog->code[env->pc + 2];
            }
            done = 1;
            env->pc += 3;
            break;
        case NTA_RATE:
            env->wait_base = prog->code[env->pc + 1];
            env->wait_random = prog->code[env->pc + 2];
            env->pc += 3;
            break;
        case NTA_GOTO:
            env->pc = prog->code[env->pc + 1];
            break;
        case NTA_BTEX:
            if (NuTexAnimCondition(prog->code[env->pc + 1], env->tex, prog->code[env->pc + 2])) {
                env->pc = prog->code[env->pc + 3];
            } else {
                env->pc += 4;
            }
            break;
        case NTA_GOSUB:
            if (env->call_depth >= 16) {
                NuErrorProlog(nutexanm_srcfile, 0x3C8)(nta_call_overflow_msg, env->pc);
            }
            env->call_stack[env->call_depth++] = env->pc + 2;
            env->pc = prog->code[env->pc + 1];
            break;
        case NTA_RET:
            if (env->call_depth == 0) {
                NuErrorProlog(nutexanm_srcfile, 0x3D0)(nta_call_underflow_msg, env->pc);
            }
            env->pc = env->call_stack[--env->call_depth];
            break;
        case NTA_REPEAT:
            if (env->repeat_depth >= 16) {
                NuErrorProlog(nutexanm_srcfile, 0x3D8)(nta_repeat_overflow_msg, env->pc);
            }
            env->repeat_count[env->repeat_depth] = prog->code[env->pc + 1];
            if (prog->code[env->pc + 2]) {
                env->repeat_count[env->repeat_depth] += NuRand(0) % prog->code[env->pc + 2];
            }
            env->pc += 3;
            env->repeat_pc[env->repeat_depth++] = env->pc;
            break;
        case NTA_REPEND:
            if (env->repeat_depth == 0) {
                NuErrorProlog(nutexanm_srcfile, 0x3E2)(nta_repend_msg, env->pc);
            }
            if (!env->repeat_count[env->repeat_depth - 1]) {
                env->repeat_depth--;
                env->pc++;
            } else {
                env->pc = env->repeat_pc[env->repeat_depth - 1];
                env->repeat_count[env->repeat_depth - 1]--;
            }
            break;
        case NTA_UNTILTEX:
            if (env->repeat_depth == 0) {
                NuErrorProlog(nutexanm_srcfile, 0x3F0)(nta_untiltex_msg, env->pc);
            }
            if (NuTexAnimCondition(prog->code[env->pc + 1], env->tex, prog->code[env->pc + 2])
                || !env->repeat_count[env->repeat_depth - 1]) {
                env->pc += 3;
                env->repeat_depth--;
            } else {
                env->pc = env->repeat_pc[env->repeat_depth - 1];
                env->repeat_count[env->repeat_depth - 1]--;
            }
            break;
        case NTA_END:
            done = 1;
            break;
        case NTA_XREF:
            xdef = prog->code[env->pc + 1];
            for (list = ntal_first; list; list = list->next) {
                for (anim = list->anims; anim; anim = anim->next) {
                    other = anim->env;
                    if (other && other != env && other->prog && other->prog->num_xdefs) {
                        other_prog = other->prog;
                        for (i = 0; i < other_prog->num_xdefs; i++) {
                            if (other_prog->xdef_label[i] == xdef) {
                                other->pc = other_prog->xdef_pc[i];
                                other->wait = 0;
                                other->call_depth = 0;
                                other->repeat_depth = 0;
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


NuTexAnimProg *NuTexAnimProgFind(char *name)
{
    NuTexAnimProg *prog;

    for (prog = ntap_first; prog; prog = prog->next) {
        if (!strcasecmp(name, prog->name)) {
            break;
        }
    }
    return prog;
}


void NuTexAnimProgDestroy(NuTexAnimProg *prog)
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


NuTexAnimProg *NuTexAnimProgCreate(char **buff, int code_size, char *name)
{
    NuTexAnimProg *prog;

    if (buff) {
        prog = (NuTexAnimProg *)*buff;
        *buff += sizeof(NuTexAnimProg);
        *buff += code_size;
    } else {
        prog = NuMemAllocFn(sizeof(NuTexAnimProg) + code_size, nutexanm_srcfile, 0xDD);
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


void NuTexAnimProgWrite(char *name, NuTexAnimProg *prog)
{
    int fh;

    fh = NuFileOpen(name, 1);
    if (fh) {
        NuFileWrite(fh, prog, sizeof(NuTexAnimProg) + prog->num_codes * sizeof(short));
        NuFileClose(fh);
    }
}


NuTexAnimProg *NuTexAnimProgRead(char **buff, char *name)
{
    NuTexAnimProg *prog;
    int fh;
    int size;

    prog = 0;
    fh = NuFileOpen(name, 0);
    if (fh) {
        size = NuFileOpenSize(fh);
        if (buff) {
            prog = (NuTexAnimProg *)(((int)*buff + 3) & 0xFFFFFFFC);
            *buff = (char *)prog + size;
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


inline void NuTexAnimEnvDestroy(NuTexAnimEnv *env)
{
    if (env->dynalloc) {
        NuMemFreeFn(env, nutexanm_srcfile, 0x161);
    }
}


inline void NuTexAnimEnvReset(NuTexAnimEnv *env)
{
    if (env) {
        env->pc = 0;
        env->repeat_depth = 0;
        env->call_depth = 0;
        env->wait_base = 0;
        env->wait_random = 0;
        env->wait = 0;
        env->tex = 0;
    }
}


inline NuTexAnimEnv *NuTexAnimEnvCreate(char **buff, struct numtl_s *mtl, short *tids, NuTexAnimProg *prog)
{
    NuTexAnimEnv *env;

    if (buff) {
        env = (NuTexAnimEnv *)(((int)*buff + 3) & 0xFFFFFFFC);
        *buff = (char *)env + sizeof(NuTexAnimEnv);
    } else {
        env = NuMemAllocFn(sizeof(NuTexAnimEnv), nutexanm_srcfile, 0x17D);
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
    NuTexAnim *anim;

    for (list = ntal_first; list; list = list->next) {
        for (anim = list->anims; anim; anim = anim->next) {
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


void NuTexAnimProcessList(NuTexAnim *anim)
{
    for (; anim; anim = anim->next) {
        if (anim->env) {
            NuTexAnimEnvProc(anim->env);
        }
    }
}


NuTexAnim *NuTexAnimFind(NuTexAnim *anim, char *name)
{
    for (; anim; anim = anim->next) {
        if (!strcasecmp(anim->name, name)) {
            return anim;
        }
    }
    return 0;
}


void NuTexAnimDestroy(NuTexAnim *anim)
{
    if (anim->env) {
        NuTexAnimEnvDestroy(anim->env);
    }
    if (anim->dynalloc) {
        NuMemFreeFn(anim, nutexanm_srcfile, 0x4AE);
    }
}


NuTexAnim *NuTexAnimCreate(char **buff, NuTexAnimProg *prog, struct numtl_s *mtl, short *tids, int num_tids)
{
    NuTexAnim *anim;

    if (buff) {
        anim = (NuTexAnim *)(((int)*buff + 3) & 0xFFFFFFFC);
        *buff = (char *)anim + sizeof(NuTexAnim);
        *buff += num_tids * sizeof(short);
    } else {
        anim = NuMemAllocFn(sizeof(NuTexAnim) + num_tids * sizeof(short), nutexanm_srcfile, 0x4CC);
    }
    anim->prev = 0;
    anim->next = 0;
    anim->num_tids = num_tids;
    anim->mtl = mtl;
    anim->name = 0;
    anim->tids = (short *)(anim + 1);
    memcpy(anim->tids, tids, num_tids * sizeof(short));
    anim->env = NuTexAnimEnvCreate(buff, mtl, anim->tids, prog);
    return anim;
}


void NuTexAnimAddList(NuTexAnim *anim)
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


void NuTexAnimRemoveList(NuTexAnim *anim)
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
    NuTexAnim *anim;

    for (list = ntal_first; list; list = list->next) {
        for (anim = list->anims; anim; anim = anim->next) {
            if (anim->env) {
                NuTexAnimEnvProc(anim->env);
            }
        }
    }
}


void pftaTex(NuFPar *parser)
{
    int tex;

    tex = NuFParGetInt(parser);
    nta_prog->code[nta_prog->num_codes++] = NTA_TEX;
    nta_prog->code[nta_prog->num_codes++] = tex;
}


void pftaTexAdj(NuFPar *parser)
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


void pftaWait(NuFPar *parser)
{
    int base;
    int random;

    base = NuFParGetInt(parser);
    random = NuFParGetInt(parser);
    nta_prog->code[nta_prog->num_codes++] = NTA_WAIT;
    nta_prog->code[nta_prog->num_codes++] = base;
    nta_prog->code[nta_prog->num_codes++] = random;
}


void pftaRate(NuFPar *parser)
{
    int base;
    int random;

    base = NuFParGetInt(parser);
    random = NuFParGetInt(parser);
    nta_prog->code[nta_prog->num_codes++] = NTA_RATE;
    nta_prog->code[nta_prog->num_codes++] = base;
    nta_prog->code[nta_prog->num_codes++] = random;
}


void pftaOn(NuFPar *parser)
{
    int signal;

    signal = NuFParGetInt(parser);
    nta_prog->on_pc[signal] = nta_prog->num_codes;
    nta_prog->on_mask |= 1 << signal;
}


void pftaOff(NuFPar *parser)
{
    int signal;

    signal = NuFParGetInt(parser);
    nta_prog->off_pc[signal] = nta_prog->num_codes;
    nta_prog->off_mask |= 1 << signal;
}


void pftaLabel(NuFPar *parser)
{
    int label;

    NuFParGetWord(parser);
    label = NuTexAnimLabelIndex(parser->word);
    nta_labels[label] = nta_prog->num_codes;
}


void pftaRet(NuFPar *parser)
{
    nta_prog->code[nta_prog->num_codes++] = NTA_RET;
}


void pftaRepeat(NuFPar *parser)
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


void pftaRepend(NuFPar *parser)
{
    nta_prog->code[nta_prog->num_codes++] = NTA_REPEND;
}


void pftaEnd(NuFPar *parser)
{
    nta_prog->code[nta_prog->num_codes++] = NTA_END;
}


void pftaScriptname(NuFPar *parser)
{
    NuFParGetWord(parser);
    parser->word[32] = 0;
    strcpy(nta_prog->name, parser->word);
}


void NuTexAnimResetList(NuTexAnim *anim)
{
    for (; anim; anim = anim->next) {
        NuTexAnimEnvReset(anim->env);
    }
}
