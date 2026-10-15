/*
 * Unit: nucore/nufpar
 *
 * Functions:
 *   0x00103978 NuFParGetLine
 *   0x00103ca0 NuFParGetInt
 *   0x00103da8 NuFParGetFloat
 *   0x00103ec0 NuFParGetPos
 *   0x00103ef0 NuFParSetPos
 *   0x00103f30 NuFParPushCom
 *   0x00103f60 NuFParPopCom
 *   0x00103f78 NuFParInterpretWord
 *   0x00104048 NuFParUnGetWord
 *   0x00104058 NuFParGetWord
 *   0x00104140 NuFParClose
 *   0x00104168 NuFParOpen
 *   0x001041e0 NuFParDestroy
 *   0x00104220 NuFParCreate
 *   0x001042b8 NuGetChar
 */

#include "nu_libc.h"

#include "nucore/nufpar.h"

/* The target names this word as _fbss + 0x8. */
extern int D_00633008;
extern char D_00613978[];

#define nufpar_word_start D_00633008
#define nufpar_srcfile    D_00613978

extern int NuFileOpen(char *name, int mode);
extern int NuFileRead(int handle, void *data, int size);
extern int NuFileOpenSize(int handle);
extern void NuFileClose(int handle);
extern void *NuMemAllocFn(int size, char *file, int line);
extern void NuMemFreeFn(void *ptr, char *file, int line);


inline char NuGetChar(struct nufpar_s *parser)
{
    int start;
    int count;
    int read;
    char c;

    start = parser->buffend;
    if (start < 0) {
        start = 0;
    }
    if (parser->cpos > parser->buffend) {
        if (parser->buffend + 1 >= parser->size) {
            return 0;
        }
        count = parser->size - start;
        if (count > 0x1000) {
            count = 0x1000;
        }
        read = NuFileRead(parser->fh, parser->fbuff, count);
        parser->buffstart = parser->buffend + 1;
        parser->buffend += read;
        if (read == 0) {
            return 0;
        }
    }
    c = parser->fbuff[parser->cpos - parser->buffstart];
    parser->cpos++;
    return c;
}


int NuFParGetLine(struct nufpar_s *parser)
{
    int length;
    int done;
    char c;

    length = 0;
    parser->line_pos = 0;
    parser->line_num++;
    for (;;) {
        c = NuGetChar(parser);
        switch (c) {
        case '\r':
            NuGetChar(parser);
        case '\n':
            if (length == 0) {
                parser->line_num++;
                break;
            }
        case 0:
            parser->lbuff[length] = 0;
            return length;
        case ';':
            if (length == 0) {
                done = 0;
                do {
                    c = NuGetChar(parser);
                    switch (c) {
                    case '\r':
                        NuGetChar(parser);
                    case '\n':
                    case 0:
                        done = 1;
                        break;
                    }
                } while (!done);
                length = 0;
                parser->line_pos = 0;
                parser->line_num++;
                break;
            }
        default:
            parser->lbuff[length & 0xFF] = c;
            length++;
            break;
        }
    }
}


inline int NuFParGetWord(struct nufpar_s *parser)
{
    int length;
    int quoted;
    char c;

    length = 0;
    quoted = 0;
    nufpar_word_start = parser->line_pos;
    while (parser->lbuff[parser->line_pos & 0xFF] != 0) {
        c = parser->lbuff[parser->line_pos];
        switch (c) {
        case ' ':
        case '\t':
        case ',':
            if (!quoted) {
                if (length != 0) {
                    parser->wbuff[length & 0xFF] = 0;
                    return length;
                }
                break;
            }
        default:
            if (c == '"') {
                quoted = 1 - quoted;
            } else {
                parser->wbuff[length & 0xFF] = c;
                length++;
            }
            break;
        }
        parser->line_pos++;
    }
    parser->wbuff[length & 0xFF] = 0;
    return length;
}


int NuFParGetInt(struct nufpar_s *parser)
{
    NuFParGetWord(parser);
    if (parser->wbuff[0] != 0) {
        return atoi(parser->wbuff);
    }
    return 0;
}


float NuFParGetFloat(struct nufpar_s *parser)
{
    NuFParGetWord(parser);
    if (parser->wbuff[0] != 0) {
        return atof(parser->wbuff);
    }
    return 0.0f;
}


void NuFParGetPos(struct nufpar_s *parser, struct nufparpos_s *pos)
{
    pos->line_num = parser->line_num;
    pos->line_pos = parser->line_pos;
    pos->cpos = parser->cpos;
    pos->buffstart = parser->buffstart;
    pos->buffend = parser->buffend;
}


void NuFParSetPos(struct nufpar_s *parser, struct nufparpos_s *pos)
{
    parser->cpos = pos->cpos;
    parser->line_pos = pos->line_pos;
    if (parser->cpos < parser->buffstart ||
        parser->cpos > parser->buffend) {
        parser->buffend = parser->cpos - 1;
    }
}


int NuFParPushCom(struct nufpar_s *parser, struct nufpcomjmp_s *command)
{
    int result;

    if (parser->compos < 7) {
        parser->compos++;
        parser->comstack[parser->compos] = command;
        result = parser->compos;
    } else {
        result = -1;
    }
    return result;
}


void NuFParPopCom(struct nufpar_s *parser)
{
    if (parser->compos >= 0) {
        parser->compos--;
    }
}


int NuFParInterpretWord(struct nufpar_s *parser)
{
    int i;

    if (parser->compos >= 0) {
        for (i = 0; parser->comstack[parser->compos][i].fname != 0; i++) {
            if (strcasecmp(parser->comstack[parser->compos][i].fname,
                           parser->wbuff) == 0) {
                parser->comstack[parser->compos][i].func(parser);
                return 1;
            }
        }
    }
    return 0;
}


void NuFParUnGetWord(struct nufpar_s *parser)
{
    parser->line_pos = nufpar_word_start;
}


inline void NuFParClose(struct nufpar_s *parser)
{
    NuMemFreeFn(parser, nufpar_srcfile, 0x130);
}


inline struct nufpar_s *NuFParOpen(int handle)
{
    struct nufpar_s *parser;

    parser = NuMemAllocFn(sizeof(struct nufpar_s), nufpar_srcfile, 0x13E);
    if (parser) {
        memset(parser, 0, sizeof(struct nufpar_s));
        parser->fh = handle;
        parser->buffend = -1;
        parser->line_num = -1;
        parser->compos = -1;
        parser->size = NuFileOpenSize(handle);
    }
    return parser;
}


void NuFParDestroy(struct nufpar_s *parser)
{
    int handle;

    handle = parser->fh;
    NuFParClose(parser);
    NuFileClose(handle);
}


struct nufpar_s *NuFParCreate(char *name)
{
    int handle;
    struct nufpar_s *parser;

    handle = NuFileOpen(name, 0);
    if (handle) {
        parser = NuFParOpen(handle);
        if (parser) {
            return parser;
        }
        NuFileClose(handle);
    }
    return 0;
}
