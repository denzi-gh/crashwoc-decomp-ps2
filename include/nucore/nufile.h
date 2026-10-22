#ifndef NUCORE_NUFILE_H
#define NUCORE_NUFILE_H

enum nufilemode_e {
    NUFILE_READ = 0,
    NUFILE_WRITE = 1,
    NUFILE_APPEND = 2,
    NUFILE_READ_NOWAIT = 3,
    NUFILE_MODE_CNT = 4
};

struct NUDATFINFO_s {
    int foffset;              /* 0x00 */
    int flen;                 /* 0x04 */
    int uplen;	              /* 0x08 */
    int ppack;                /* 0x0c */
}; /* 0x10 */

struct NUDFNODE_s {
    short childix;              /* 0x00 */
    short sibix;                /* 0x02 */
    char* txt;	                /* 0x04 */
}; /* 0x08 */

struct nudathdr_s {
    int ver;                      /* 0x00 */
    int nfiles;                   /* 0x04 */
    struct NUDATFINFO_s * finfo;  /* 0x08 */
    int treecnt;                  /* 0x0c */
    struct NUDFNODE_s * filetree; /* 0x10 */
    int leafnamesize;             /* 0x14 */
    char * leafnames;             /* 0x18 */
    int dfhandle;                 /* 0x1c */
    int fh;                       /* 0x20 */
    short intalloc;               /* 0x24 */
    short openmode;               /* 0x26 */
    int data[0];                  /* 0x28 */
}; /* 0x28 */

#ifdef NW_HOST
/* Retail calls these without a prototype in scope (game/main.c); only the host declares them. */
void NuFileInitEx(int deviceid, int rebootiop);

/* The port replaces the .DAT layer (NuDat*), so it sees nufile.c's current archive. */
extern struct nudathdr_s *curr_dat;
struct nudathdr_s *NuDatOpenEx(char *fname, char **mem, int *used, short mode);
struct nudathdr_s *NuDatOpen(char *fname, char **mem, int *used);
void NuDatSet(struct nudathdr_s *ndh);
void NuDatClose(struct nudathdr_s *ndh);
int NuDatFileFindTree(struct nudathdr_s *ndh, char *fname);
int NuDatFileOpen(struct nudathdr_s *ndh, char *fname, enum nufilemode_e mode);
int NuDatFileRead(int fh, void *data, int size);
int NuDatFileSeek(int fh, int offset, int origin);
int NuDatFilePos(int fh);
int NuDatFileOpenSize(int fh);
void NuDatFileClose(int fh);
int NuDatFileLoadBuffer(struct nudathdr_s *ndh, char *fname, void *dest, int maxsize);
#endif

#endif
