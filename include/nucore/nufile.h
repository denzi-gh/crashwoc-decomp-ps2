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

#endif
