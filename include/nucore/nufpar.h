#ifndef NUCORE_NUFPAR_H
#define NUCORE_NUFPAR_H

struct nufpar_s;

/* One entry of a parser command table: a word and its handler. */
struct nufpcomjmp_s {
    char *fname;                          /* 0x00 */
    void (*func)(struct nufpar_s *parser); /* 0x04 */
}; /* 0x08 */

/* Line/word parser over a text file. */
struct nufpar_s {
    int fh;                               /* 0x0000 */
    char fbuff[0x1000];                   /* 0x0004 */
    char lbuff[0x101];                    /* 0x1004 */
    char wbuff[0x101];                    /* 0x1105 */
    int line_num;                         /* 0x1208 */
    int line_pos;                         /* 0x120C */
    int cpos;                             /* 0x1210 */
    int buffstart;                        /* 0x1214 */
    int buffend;                          /* 0x1218 */
    struct nufpcomjmp_s *comstack[8];     /* 0x121C */
    int compos;                           /* 0x123C */
    int size;                             /* 0x1240 */
}; /* 0x1244 */

/* A saved parser position. */
struct nufparpos_s {
    int line_num;                         /* 0x00 */
    int line_pos;                         /* 0x04 */
    int cpos;                             /* 0x08 */
    int buffstart;                        /* 0x0C */
    int buffend;                          /* 0x10 */
}; /* 0x14 */

char NuGetChar(struct nufpar_s *parser);
int NuFParGetLine(struct nufpar_s *parser);
int NuFParGetWord(struct nufpar_s *parser);
int NuFParGetInt(struct nufpar_s *parser);
float NuFParGetFloat(struct nufpar_s *parser);
void NuFParGetPos(struct nufpar_s *parser, struct nufparpos_s *pos);
void NuFParSetPos(struct nufpar_s *parser, struct nufparpos_s *pos);
int NuFParPushCom(struct nufpar_s *parser, struct nufpcomjmp_s *command);
void NuFParPopCom(struct nufpar_s *parser);
int NuFParInterpretWord(struct nufpar_s *parser);
void NuFParUnGetWord(struct nufpar_s *parser);
void NuFParClose(struct nufpar_s *parser);
struct nufpar_s *NuFParOpen(int handle);
void NuFParDestroy(struct nufpar_s *parser);
struct nufpar_s *NuFParCreate(char *name);

#endif /* NUCORE_NUFPAR_H */
