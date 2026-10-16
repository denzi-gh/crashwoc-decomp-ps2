/*
 * Unit: nucore/nufile
 *
 * Functions:
 *   0x001000c8 NuFileSifLoadModule
 *   0x001002d8 NuFileOpen
 *   0x001005a8 NuFileSeek
 *   0x00100720 NuFileSize
 *   0x00100938 NuFileLoad
 *   0x00100a88 NuFileLoadBuffer
 *   0x00100d08 NuFileRead
 *   0x00100fc0 NuFileWriteStringV
 *   0x00101038 NuFileBeginBlkRead
 *   0x001012e8 NuFileEndBlkRead
 *   0x00101468 NuFileBeginBlkWrite
 *   0x00101648 NuFileEndBlkWrite
 *   0x00101860 NuFileWriteAddress
 *   0x001019d0 NuFileSetAddress
 *   0x00101b68 NuFileAlign
 *   0x00101cb0 NuDatFileLoadBuffer
 *   0x00102148 NuDatFileOpen
 *   0x00102370 NuDatClose
 *   0x00102478 NuDatOpenEx
 *   0x001028f8 NuFileInitEx
 *   0x001029f0 NuFileInit
 *   0x00102ad8 NuFileGetCurrentPath
 *   0x00102b78 NuFileSetCurrentDirectory
 *   0x00102be0 NuFileOpenSize
 *   0x00102c20 NuFileClose
 *   0x00102ce0 NuFileStatus
 *   0x00102d38 NuFilePos
 *   0x00102e20 NuFileReadFloat
 *   0x00102e48 NuFileReadInt
 *   0x00102e70 NuFileReadUnsignedInt
 *   0x00102e98 NuFileReadShort
 *   0x00102ec0 NuFileReadChar
 *   0x00102ee8 NuFileWrite
 *   0x00102f08 NuFileWriteFloat
 *   0x00102f30 NuFileWriteInt
 *   0x00102f58 NuFileWriteUnsignedInt
 *   0x00102f80 NuFileWriteShort
 *   0x00102fa8 NuFileWriteChar
 *   0x00102fd0 NuFileWriteString
 *   0x00103018 NuFileGetBlkSize
 *   0x00103038 NuFileAppendPath
 *   0x001030a0 NuFileExtractPath
 *   0x00103110 NuFileExtractFilename
 *   0x00103168 NuFileExtractFile
 *   0x001031f8 NuFileExtractExt
 *   0x00103250 NuFileInitAddress
 *   0x001032e8 NuFileTidyAddress
 *   0x00103338 NuFilePatchAddress
 *   0x00103430 NuMemFileOpen
 *   0x00103498 NuMemFileClose
 *   0x001034e0 NuMemFilePos
 *   0x00103530 NuMemFileAddr
 *   0x00103550 NuMemFileRead
 *   0x001035f0 NuMemFileSeek
 *   0x001036d8 NuDatFilePos
 *   0x00103728 NuDatFileRead
 *   0x00103760 NuDatFileSeek
 *   0x00103810 NuDatFileOpenSize
 *   0x00103830 NuDatFileClose
 *   0x00103860 NuDatOpen
 *   0x00103880 NuDatSet
 *   0x00103888 NuDatFileFindTree
 */

#include "nu_libc.h"
#include "nu_stdarg.h"

#include "nucore/nuerror.h"
#include "nucore/nufile.h"

struct numemfile_s {
    char* start;              /* 0x00 */
    char* end;               /* 0x04 */
    char *curr;  	     /* 0x08 */
    enum nufilemode_e mode;  /* 0x0c */
    int used;  	            /* 0x10 */
}; /* 0x14 */

struct filebuff_s;

struct fileinfo_s {
    int handle;                     /* 0x00 */
    int read_pos;                   /* 0x04 */
    int file_pos;	            /* 0x08 */
    int file_length;                /* 0x0c */
    int buff_start;  	            /* 0x10 */
    int buff_length;  	            /* 0x14 */
    int use_buff;  	            /* 0x18 */
    struct filebuff_s *buff;        /* 0x1c */
}; /* 0x20 */

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
    int start_lsn;                /* 0x28 */
}; /* 0x2c */

struct filebuff_s {
    struct fileinfo_s * info;    /* 0x00 */
    int time;                    /* 0x04 */
    char data[0x10000];          /* 0x08 */
}; /* 0x10008 */

struct nudatfile_s {
    struct nudathdr_s* ndh;   /* 0x00 */
    int start;                /* 0x04 */
    int len;	              /* 0x08 */
    int uplen;                /* 0x0c */
    int pos;                  /* 0x10 */
    int used;                 /* 0x14 cleared on close, tested by NuDatFileOpen */
    int ppack : 1;            /* 0x18 */
}; /* 0x1c */

struct nublk_s {
    union {
        int id;
        unsigned char tag[4];
    };                        /* 0x00 */
    int size;                 /* 0x04 */
    int start;                /* 0x08 */
}; /* 0x0c */

extern struct nudatfile_s D_006396D8[20];
extern struct filebuff_s D_00639B08[4];
extern struct numemfile_s D_00639548[20];
extern struct fileinfo_s D_00639908[16];
extern struct nublk_s D_00633548[];
extern char* D_00293720[4];
extern struct nudathdr_s* D_0062E998;
extern int D_0062E99C;
extern char D_00293730[256];
extern int D_00293700[4];
extern int D_00293710[4];
extern int D_0062E9A0;
extern int D_0062E980;
extern int nufile_buffering_enabled;
extern int nufile_try_packed;
extern char* iop_img_name;

#define datfiles                D_006396D8
#define file_buff               D_00639B08
#define memfiles                D_00639548
#define file_info               D_00639908
#define filesys_root            D_00293720
#define curr_dat                D_0062E998
#define nufile_deviceid         D_0062E99C
#define working_dir             D_00293730
#define fmode                   D_00293700
#define forig                   D_00293710
#define file_time_count         D_0062E9A0
#define blk_stack               D_00633548
#define blk_level               D_0062E980

int sceWrite(int fd, void *addr, int size);
int sceLseek(int fd, int offset, int how);

int sceIoctl(int fd, int req, void*);
int NuFileRead(int fh, void *data, int size);


float NuFileReadFloat(int fh)
{
    float v;

    NuFileRead(fh, &v, sizeof(v));
    return v;
}


int NuFileReadInt(int fh)
{
    int v;

    NuFileRead(fh, &v, sizeof(v));
    return v;
}


unsigned int NuFileReadUnsignedInt(int fh)
{
    unsigned int v;

    NuFileRead(fh, &v, sizeof(v));
    return v;
}


short NuFileReadShort(int fh)
{
    short v;

    NuFileRead(fh, &v, sizeof(v));
    return v;
}


char NuFileReadChar(int fh)
{
    char v;

    NuFileRead(fh, &v, sizeof(v));
    return v;
}


int NuFileWrite(int fh, void *data, int size)
{
    return sceWrite(fh - 1, data, size);
}


void NuFileWriteFloat(int fh, float v)
{
    sceWrite(fh - 1, &v, sizeof(v));
}


inline void NuFileWriteInt(int fh, int v)
{
    fh--;
    sceWrite(fh, &v, sizeof(v));
}


void NuFileWriteUnsignedInt(int fh, unsigned int v)
{
    sceWrite(fh - 1, &v, sizeof(v));
}


void NuFileWriteShort(int fh, short v)
{
    sceWrite(fh - 1, &v, sizeof(v));
}


void NuFileWriteChar(int fh, char v)
{
    sceWrite(fh - 1, &v, sizeof(v));
}


void NuFileWriteString(int fh, char* s)
{
    int len;

    len = strlen(s);
    sceWrite(fh - 1, s, len);
}


int NuFileGetBlkSize(void)
{
    return blk_stack[blk_level].size;
}


int NuFileAppendPath(char *dest, char *s1, char *s2)
{
    char *p;

    strcpy(dest, s1);
    p = dest + strlen(dest);
    if (*p++ != '\\') {
        *p++ = '\\';
    }
    strcpy(p, s2);
    return strlen(dest);
}


int NuFileExtractPath(char *dest, char *path)
{
    char *p;
    int len;

    p = strrchr(path, '\\');
    if (p) {
        p++;
    } else {
        p = path;
    }
    len = p - path;
    strncpy(dest, path, len);
    dest[len] = 0;
    return len;
}


int NuFileExtractFilename(char *dest, char *path)
{
    char *p;

    p = strrchr(path, '\\');
    if (p) {
        p++;
    } else {
        p = path;
    }
    strcpy(dest, p);
    return strlen(dest);
}


int NuFileExtractFile(char *dest, char *path)
{
    char *name;
    char *ext;
    int len;

    name = strrchr(path, '\\');
    if (name) {
        name++;
    } else {
        name = path;
    }
    ext = strrchr(path, '.');
    if (ext == 0) {
        ext = path + strlen(path);
    }
    len = ext - name;
    strncpy(dest, name, len);
    dest[len] = 0;
    return len;
}


int NuFileExtractExt(char *dest, char *path)
{
    char *p;

    p = strrchr(path, '.');
    if (p) {
        p++;
    } else {
        p = path;
    }
    strcpy(dest, p);
    return strlen(dest);
}


extern char D_006134B0[];
#define nufile_file D_006134B0

struct nufileaddr_s {
    int id;                   /* 0x00 */
    int pos;                  /* 0x04 */
}; /* 0x08 */

extern struct nufileaddr_s *D_0062E9CC;
extern struct nufileaddr_s *D_0062E9D0;
extern int D_0062E9C8;
extern int D_00633000;
extern int D_00633004;
extern void NuMemFreeFn(void *ptr, char *file, int line);
void* NuMemAllocFn(int size, char * file, int line);
int NuFileSeek(int fh, int offset, int origin);

#define addr_refs               D_0062E9CC
#define addr_defs               D_0062E9D0
#define addr_max                D_0062E9C8
#define addr_ref_cnt            D_00633000
#define addr_def_cnt            D_00633004


int NuDatFilePos(int fh);


inline int NuMemFilePos(int fh)
{
    if (fh >= 0x800) {
        return NuDatFilePos(fh);
    }
    fh -= 0x400;
    return (int)memfiles[fh].curr - (int)memfiles[fh].start;
}

extern char D_00613690[];
#define msg_pos_seek_failed     D_00613690


inline int NuFilePos(int fh)
{
    struct fileinfo_s *info;
    int pos;

    if (fh >= 0x400) {
        return NuMemFilePos(fh);
    }
    fh--;
    info = &file_info[fh];
    if (info->use_buff != 0) {
        return info->read_pos;
    }
    while ((pos = sceLseek(fh, 0, 1)) < 0) {
        NuDebugMsgProlog(nufile_file, 0x30C)(msg_pos_seek_failed, pos);
    }
    return pos;
}


int NuFileAlign(int fh, int mask)
{
    unsigned char pad;
    int pos;
    int written;
    int fd;

    pad = 0xFE;
    written = 0;
    pos = NuFilePos(fh);
    if (pos & mask) {
        fd = fh - 1;
        do {
            sceWrite(fd, &pad, 1);
            pos++;
            written++;
        } while (pos & mask);
    }
    return written;
}


extern char D_00613728[];
#define msg_blk_mismatch        D_00613728
extern char D_00613758[];
#define msg_blk_overrun         D_00613758
extern struct nublk_s D_00636548[];
extern int D_0062E984;
extern char D_00613788[];
#define wblk_stack              D_00636548
#define wblk_level              D_0062E984
#define msg_blk_write_begin     D_00613788


int NuFileBeginBlkRead(int fh, int id)
{
    int level;
    int pos;

    level = blk_level++;
    pos = NuFilePos(fh);
    if (level >= 2) {
        if (pos >= blk_stack[level - 1].start + blk_stack[level - 1].size) {
            blk_level--;
            return 0;
        }
    }
    blk_stack[level].start = NuFilePos(fh);
    NuFileRead(fh, &blk_stack[level], 8);
    if (blk_stack[level].size < 0) {
        blk_stack[level].size = -blk_stack[level].size;
    }
    if (id != 0 && id != blk_stack[level].id) {
        NuErrorProlog(nufile_file, 0x483)(msg_blk_mismatch);
    }
    return blk_stack[level].id;
}


/* Leaves the current block, seeking to its end if the reader stopped short. */
void NuFileEndBlkRead(int fh)
{
    int level;
    int pos;

    level = --blk_level;
    pos = NuFilePos(fh);
    if (pos != blk_stack[level].start + blk_stack[level].size) {
        if (blk_stack[level].start + blk_stack[level].size < pos) {
            NuDebugMsgProlog(nufile_file, 0x4A0)(msg_blk_overrun);
        }
        NuFileSeek(fh, blk_stack[level].start + blk_stack[level].size, 0);
    }
}


void NuFileBeginBlkWrite(int fh, int id, int flags)
{
    int level;

    level = wblk_level++;
    wblk_stack[level].start = NuFilePos(fh);
    wblk_stack[level].id = id;
    if (flags == 1) {
        wblk_stack[level].size = -1;
    } else {
        wblk_stack[level].size = 1;
    }
    NuFileWriteInt(fh, id);
    NuFileWriteInt(fh, id);
    NuDebugMsgProlog(nufile_file, 0x4BB)(msg_blk_write_begin,
        wblk_stack[level].tag[0], wblk_stack[level].tag[1],
        wblk_stack[level].tag[2], wblk_stack[level].tag[3]);
}


extern int D_0062E988;
extern char D_006137C0[];
#define wblk_align              D_0062E988
#define msg_blk_write_end       D_006137C0


void NuFileEndBlkWrite(int fh)
{
    int pad;
    int size;
    int level;
    int pos;
    int fill;

    pad = 0;
    level = --wblk_level;
    pos = NuFilePos(fh);
    size = pos - wblk_stack[level].start;
    fill = wblk_align - size % wblk_align;
    if (fill != wblk_align) {
        size += fill;
        pos += fill;
        sceWrite(fh - 1, &pad, fill);
    }
    if (wblk_stack[level].size < 0) {
        size = -size;
    }
    NuFileSeek(fh, wblk_stack[level].start + 4, 0);
    sceWrite(fh - 1, &size, 4);
    NuFileSeek(fh, pos, 0);
    NuDebugMsgProlog(nufile_file, 0x4DB)(msg_blk_write_end,
        wblk_stack[level].tag[0], wblk_stack[level].tag[1],
        wblk_stack[level].tag[2], wblk_stack[level].tag[3]);
}


extern char D_00613800[];
#define msg_too_many_addr_refs  D_00613800
extern int D_0062E9D4;
#define addr_placeholder        D_0062E9D4

/* Writes a placeholder dword for address `id`, remembering where it went so
   it can be patched once the address is known. */
void NuFileWriteAddress(int fh, int id)
{
    if (id) {
        if (addr_ref_cnt >= addr_max) {
            NuErrorProlog(nufile_file, 0x585)(msg_too_many_addr_refs);
        }
        addr_refs[addr_ref_cnt].id = id;
        addr_refs[addr_ref_cnt].pos = NuFilePos(fh);
        addr_ref_cnt++;
    }
    sceWrite(fh - 1, &addr_placeholder, 4);
}


extern char D_00613838[];
#define msg_too_many_addr_defs  D_00613838

/* Records the current file position as the definition of address `id`. */
void NuFileSetAddress(int fh, int id)
{
    int i;

    if (id) {
        for (i = 0; i < addr_def_cnt; i++) {
            if (addr_defs[i].id == id) {
                break;
            }
        }
        if (i >= addr_max) {
            NuErrorProlog(nufile_file, 0x59A)(msg_too_many_addr_defs);
        }
        if (i == addr_def_cnt) {
            addr_def_cnt = i + 1;
        }
        addr_defs[i].id = id;
        addr_defs[i].pos = NuFilePos(fh);
    }
}


void NuFileInitAddress(int max)
{
    if (max != addr_max) {
        if (addr_refs != 0) {
            NuMemFreeFn(addr_refs, nufile_file, 0x564);
        }
        if (addr_defs != 0) {
            NuMemFreeFn(addr_defs, nufile_file, 0x565);
        }
        addr_max = max;
        addr_refs = NuMemAllocFn(max * sizeof(struct nufileaddr_s), nufile_file, 0x569);
        addr_defs = NuMemAllocFn(addr_max * sizeof(struct nufileaddr_s), nufile_file, 0x56A);
    }
    addr_ref_cnt = 0;
    addr_def_cnt = 0;
}


void NuFileTidyAddress(void)
{
    if (addr_refs != 0) {
        NuMemFreeFn(addr_refs, nufile_file, 0x574);
        addr_refs = 0;
    }
    if (addr_defs != 0) {
        NuMemFreeFn(addr_defs, nufile_file, 0x579);
        addr_defs = 0;
    }
    addr_max = 0;
}


void NuFilePatchAddress(int fh)
{
    int i;
    int j;
    int fd;
    int offset;

    fd = fh - 1;
    for (i = 0; i < addr_ref_cnt; i++) {
        for (j = 0; j < addr_def_cnt; j++) {
            if (addr_refs[i].id == addr_defs[j].id) {
                offset = addr_defs[j].pos - addr_refs[i].pos;
                NuFileSeek(fh, addr_refs[i].pos, 0);
                sceWrite(fd, &offset, sizeof(offset));
            }
        }
    }
    NuFileSeek(fh, 0, 2);
}


int NuMemFileOpen(char *start, int size, int mode)
{
    int i;
    struct numemfile_s *mf;
    char *end;

    if (size > 0) {
        if (mode == 0) {
            end = start + size - 1;
            mf = memfiles;
            for (i = 0; i < 20; i++) {
                if (mf->used == 0) {
                    mf->end = end;
                    mf->start = start;
                    mf->curr = start;
                    mf->mode = mode;
                    mf->used = 1;
                    return i + 0x400;
                }
                mf++;
            }
        }
    }
    return 0;
}


void NuDatFileClose(int fh);


void NuMemFileClose(int fh)
{
    if (fh >= 0x800) {
        NuDatFileClose(fh);
        return;
    }
    fh -= 0x400;
    memfiles[fh].used = 0;
}


void* NuMemFileAddr(int fh)
{
    return memfiles[fh - 0x400].curr;
}


int NuDatFileRead(int fh, void *data, int size);
int NuDatFileSeek(int fh, int offset, int origin);


inline int NuMemFileRead(int fh, void *data, int size)
{
    int avail;

    if (fh >= 0x800) {
        return NuDatFileRead(fh, data, size);
    }
    fh -= 0x400;
    avail = memfiles[fh].end - memfiles[fh].curr + 1;
    if (avail < size) {
        size = avail;
    }
    if (size != 0) {
        memcpy(data, memfiles[fh].curr, size);
        memfiles[fh].curr += size;
    }
    return size;
}


int NuMemFileSeek(int fh, int offset, int origin)
{
    if (fh >= 0x800) {
        return NuDatFileSeek(fh, offset, origin);
    }
    fh -= 0x400;
    switch (origin) {
    case 0:
    default:
        memfiles[fh].curr = memfiles[fh].start + offset;
        break;
    case 1:
        memfiles[fh].curr += offset;
        break;
    case 2:
        memfiles[fh].curr = memfiles[fh].end - offset;
        break;
    }
    return memfiles[fh].curr - memfiles[fh].start;
}


int NuFileSeek(int fh, int offset, int origin);


int NuDatFilePos(int fh)
{
    int ix;
    struct nudatfile_s *df;

    ix = fh - 0x800;
    df = &datfiles[ix];
    return NuFileSeek(df->ndh->fh, 0, 1) - df->start;
}


int NuDatFileRead(int fh, void *data, int size)
{
    return NuFileRead(datfiles[fh - 0x800].ndh->fh, data, size);
}


int NuDatFileSeek(int fh, int offset, int origin)
{
    int ix;
    struct nudatfile_s *df;

    ix = fh - 0x800;
    df = &datfiles[ix];
    switch (origin) {
    case 0:
    default:
        return NuFileSeek(df->ndh->fh, offset + df->start, 0) - df->start;
    case 1:
        return NuFileSeek(df->ndh->fh, offset, 0) - df->start;
    case 2:
        return NuFileSeek(df->ndh->fh, df->start + df->len - offset, 0) - df->start;
    }
}


int NuDatFileOpenSize(int fh)
{
    int ix = fh - 0x800;
    struct nudatfile_s *df = &datfiles[ix];

    return df->len;
}


void NuDatFileClose(int fh)
{
    int ix = fh - 0x800;

    datfiles[ix].ndh->dfhandle = 0;
    datfiles[ix].used = 0;
}


struct nudathdr_s* NuDatOpenEx(char* fname, char** mem, int* used, short mode);


struct nudathdr_s* NuDatOpen(char* fname, char** mem, int* used)
{
    return NuDatOpenEx(fname, mem, used, 0);
}


void NuDatSet(struct nudathdr_s* ndh)
{
    curr_dat = ndh;
}


extern char D_0062E9B8[];
#define path_sep                D_0062E9B8
extern void NuStrCpy(char * s1, char * s2);
extern char *NuStrStr(char *text, char *pattern);
extern int NuStrLen(char *text);


int NuDatFileFindTree(struct nudathdr_s *ndh, char *fname)
{
    char path[256];
    char *name;
    char *sep;
    int node;

    NuStrCpy(path, fname);
    name = path;
    node = 1;
    sep = NuStrStr(path, path_sep);
    if (sep != 0) {
        *sep = 0;
    }
    do {
        if (strcasecmp(ndh->filetree[node].txt, name) == 0) {
            if (sep == 0) {
                if (ndh->filetree[node].childix > 0) {
                    return -1;
                }
                return -ndh->filetree[node].childix;
            }
            name = sep + 1;
            node = ndh->filetree[node].childix;
            sep = NuStrStr(name, path_sep);
            if (sep != 0) {
                *sep = 0;
            }
        } else {
            node = ndh->filetree[node].sibix;
        }
    } while (node != 0);
    return -1;
}


extern char D_006134B0[];
#define nufile_file             D_006134B0


int NuDatFileOpen(struct nudathdr_s* ndh, char* fname, enum nufilemode_e mode);
int NuFileSeek(int fh, int offset, int origin);
int NuFileStatus(int fh);
int sceOpen(char * name, int flag, ...);


extern void NuStrCpy(char * s1, char * s2);
extern void NuStrCat (char * s1, char * s2);
extern int sceSifLoadModule(const char* name, int arg_size, const char* args);

extern char D_0062E9B0[];
extern char D_006134D0[];
extern char D_006134F0[];
extern char D_00613510[];
extern char D_00613538[];
extern char D_00613550[];
extern char D_00613568[];
#define cd_version_suffix       D_0062E9B0
#define msg_sif_load_failed     D_006134D0
#define msg_sif_bind_failed     D_006134F0
#define msg_sif_version         D_00613510
#define msg_sif_rpc_failed      D_00613538
#define msg_sif_unknown_error   D_00613550
#define msg_sif_loaded          D_00613568


int NuFileSifLoadModule(char *file, int args, char *argp)
{
    char path[256];
    unsigned char *p;
    int i;
    int rv;

    NuStrCpy(path, filesys_root[nufile_deviceid]);
    NuStrCat(path, working_dir);
    NuStrCat(path, file);
    if (nufile_deviceid == 1 || nufile_deviceid == 3) {
        p = (unsigned char *)path;
        for (i = strlen(filesys_root[nufile_deviceid]); p[i] != 0; i++) {
            if (p[i] >= 'a' && p[i] <= 'z') {
                p[i] = p[i] - 'a' + 'A';
            }
        }
        strcat(path, cd_version_suffix);
    }
    rv = sceSifLoadModule(path, args, argp);
    if (rv < 0) {
        NuDebugMsgProlog(nufile_file, 0x1BE)(msg_sif_load_failed, path);
        switch (-rv) {
        case 0x10000:
            NuDebugMsgProlog(nufile_file, 0x1C2)(msg_sif_bind_failed);
            break;
        case 0x10004:
            NuDebugMsgProlog(nufile_file, 0x1C6)(msg_sif_version);
            break;
        case 0x10001:
            NuDebugMsgProlog(nufile_file, 0x1CA)(msg_sif_rpc_failed);
            break;
        default:
            NuDebugMsgProlog(nufile_file, 0x1CE)(msg_sif_unknown_error, rv);
            break;
        }
    } else {
        NuDebugMsgProlog(nufile_file, 0x1D3)(msg_sif_loaded, path);
    }
    return rv;
}


extern char D_00613590[];
extern char D_006135D0[];
extern char D_006135E8[];
extern char D_00613618[];
extern char D_0062E9C0[];
#define msg_open_failed         D_00613590
#define msg_open_ok             D_006135D0
#define msg_size_seek_failed    D_006135E8
#define msg_size_seek_restore   D_00613618
#define msg_assert              D_0062E9C0


int NuFileOpen(char *file, enum nufilemode_e mode)
{
    char path[256];
    unsigned char *p;
    int i;
    int dh;
    int fd;
    int len;

    if (curr_dat != 0) {
        dh = NuDatFileOpen(curr_dat, file, mode);
        if (dh != 0) {
            return dh;
        }
    }
    strcpy(path, filesys_root[nufile_deviceid]);
    strcat(path, working_dir);
    strcat(path, file);
    p = (unsigned char *)path;
    for (i = strlen(filesys_root[nufile_deviceid]); p[i] != 0; i++) {
        if (p[i] >= 'a' && p[i] <= 'z') {
            p[i] = p[i] - 'a' + 'A';
        }
    }
    if (nufile_deviceid == 1 || nufile_deviceid == 3) {
        strcat(path, cd_version_suffix);
    }
    if (mode >= NUFILE_MODE_CNT) {
        NuErrorProlog(nufile_file, 0x21A)(msg_assert);
    }
    fd = sceOpen(path, fmode[mode]);
    if (fd < 0) {
        NuDebugMsgProlog(nufile_file, 0x21E)(msg_open_failed, path);
        return 0;
    }
    NuDebugMsgProlog(nufile_file, 0x221)(msg_open_ok, path, fd + 1);
    memset(&file_info[fd], 0, sizeof(struct fileinfo_s));
    file_info[fd].handle = fd;
    if (mode != NUFILE_WRITE) {
        do {
            len = NuFileSeek(fd + 1, 0, 2);
            if (len < 0) {
                NuDebugMsgProlog(nufile_file, 0x22B)(msg_size_seek_failed);
            }
        } while (len < 0);
        file_info[fd].file_length = len;
        if (mode == NUFILE_READ_NOWAIT) {
            while (NuFileStatus(fd + 1) != 0) {
            }
        }
        do {
            len = NuFileSeek(fd + 1, 0, 0);
            if (len < 0) {
                NuDebugMsgProlog(nufile_file, 0x236)(msg_size_seek_restore);
            }
        } while (len < 0);
        if (mode == NUFILE_READ_NOWAIT) {
            while (NuFileStatus(fd + 1) != 0) {
            }
        }
    } else {
        file_info[fd].file_length = 0;
    }
    if (mode == NUFILE_READ) {
        file_info[fd].use_buff = nufile_buffering_enabled;
    }
    return fd + 1;
}


int sceLseek(int fd, int offset, int how);
int sceRead(int fd, void * addr, int size);
int NuDatFileRead (int fh, void * data, int size);


int NuFileSeek(int fh, int offset, int origin)
{
    struct fileinfo_s *info;

    if (fh >= 0x400) {
        if (fh >= 0x800) {
            return NuDatFileSeek(fh, offset, origin);
        }
        fh -= 0x400;
        switch (origin) {
        case 0:
        default:
            memfiles[fh].curr = memfiles[fh].start + offset;
            break;
        case 1:
            memfiles[fh].curr += offset;
            break;
        case 2:
            memfiles[fh].curr = memfiles[fh].end - offset;
            break;
        }
        return memfiles[fh].curr - memfiles[fh].start;
    }
    fh--;
    info = &file_info[fh];
    if (info->use_buff != 0) {
        switch (origin) {
        case 0:
        default:
            info->read_pos = offset;
            break;
        case 1:
            info->read_pos += offset;
            break;
        case 2:
            info->read_pos = info->file_length - offset;
            break;
        }
        return info->read_pos;
    }
    return sceLseek(fh, offset, forig[origin]);
}


void NuMemFileClose(int fh);
int NuDatFileFindTree(struct nudathdr_s * ndh, char * fname);
int NuDatFilePos(int fh);
int sceClose(int fd);

extern char D_00613650[];
extern char D_00613668[];
#define msg_closing_file        D_00613650
#define msg_close_failed        D_00613668


/* Inline: retail carries a copy of it in every function that closes a file. */
inline void NuFileClose(int fh)
{
    struct fileinfo_s *info;
    struct filebuff_s *buff;

    NuDebugMsgProlog(nufile_file, 0x24D)(msg_closing_file, fh);
    if (fh >= 0x400) {
        NuMemFileClose(fh);
        return;
    }
    fh--;
    while (sceClose(fh) < 0) {
        NuDebugMsgProlog(nufile_file, 0x254)(msg_close_failed);
    }
    info = &file_info[fh];
    buff = info->buff;
    if (buff != 0) {
        buff->info = 0;
    }
    memset(info, 0, sizeof(*info));
}


void NuDatClose(struct nudathdr_s *ndh)
{
    NuFileClose(ndh->fh);
    if (ndh->dfhandle != 0) {
        datfiles[ndh->dfhandle - 0x800].used = 0;
    }
    if (ndh->intalloc) {
        NuMemFreeFn(ndh, nufile_file, 0x6AD);
    }
}

int NuFileSize(char *fname)
{
    int saved_buffering;
    int ix;
    int fh;
    int size;

    saved_buffering = nufile_buffering_enabled;
    nufile_buffering_enabled = 0;
    if (curr_dat != 0) {
        ix = NuDatFileFindTree(curr_dat, fname);
        if (ix != 0) {
            return curr_dat->finfo[ix].uplen;
        }
    }
    size = -1;
    if (fname != 0 && *fname != 0) {
        fh = NuFileOpen(fname, NUFILE_READ);
        if (fh != 0) {
            while (NuFileSeek(fh, 0, 2) < 0) {
            }
            size = NuFilePos(fh);
            NuFileClose(fh);
        }
    }
    nufile_buffering_enabled = saved_buffering;
    return size;
}


void* NuMemAllocFn(int size, char * file, int line);
void NuMemFileClose(int fh);

inline int NuFileOpenSize(int fh)
{
    if (fh >= 0x800) {
        return NuDatFileOpenSize(fh);
    }
    fh--;
    return file_info[fh].file_length;
}


void *NuFileLoad(char *fname)
{
    int saved_buffering;
    int fh;
    int size;
    void *data;

    saved_buffering = nufile_buffering_enabled;
    data = 0;
    nufile_buffering_enabled = 0;
    fh = NuFileOpen(fname, NUFILE_READ);
    if (fh != 0) {
        size = NuFileOpenSize(fh);
        if (size > 0) {
            data = NuMemAllocFn(size, nufile_file, 0x364);
            if (data != 0) {
                NuFileRead(fh, data, size);
            }
        }
        NuFileClose(fh);
    }
    nufile_buffering_enabled = saved_buffering;
    return data;
}


extern int NuPPLoadBuffer(int fh, void * mem, int buffsize);
extern int NuDatFileLoadBuffer(struct nudathdr_s* ndh, char * fname, void * mem, int buffsize);

extern char D_006136B8[];
extern char D_006136F0[];
#define msg_load_read_failed    D_006136B8
#define msg_load_seek_failed    D_006136F0


int NuFileLoadBuffer(char *filename, void *mem, int buffsize)
{
    int size;
    int fh;

    size = 0;
    if (curr_dat != 0) {
        size = NuDatFileLoadBuffer(curr_dat, filename, mem, buffsize);
    }
    if (size != 0) {
        return size;
    }
    fh = NuFileOpen(filename, NUFILE_READ);
    if (fh != 0) {
        if (nufile_try_packed != 0) {
            size = NuPPLoadBuffer(fh, mem, buffsize);
        } else {
            size = NuFileOpenSize(fh);
            if (size <= buffsize && size != 0) {
                while (NuFileRead(fh, mem, size) < 0) {
                    NuDebugMsgProlog(nufile_file, 0x397)(msg_load_read_failed);
                    while (NuFileSeek(fh, 0, 0) < 0) {
                        NuDebugMsgProlog(nufile_file, 0x399)(msg_load_seek_failed);
                    }
                }
                NuFileClose(fh);
            }
        }
        NuFileClose(fh);
    }
    return size;
}


int NuFileRead(int fh, void *data, int size)
{
    struct fileinfo_s *info;
    struct filebuff_s *buff;
    int oldest;
    int oldest_time;
    int i;
    int len;
    int offset;
    int avail;
    int n;
    int total;

    if (fh >= 0x400) {
        return NuMemFileRead(fh, data, size);
    }
    fh--;
    info = &file_info[fh];
    if (info->use_buff != 0) {
        if (info->buff == 0) {
            oldest_time = file_time_count;
            oldest = 0;
            for (i = 0; i < 4; i++) {
                if (file_buff[i].time < oldest_time) {
                    oldest_time = file_buff[i].time;
                    oldest = i;
                }
            }
            buff = &file_buff[oldest];
            if (buff->info != 0) {
                buff->info->buff = 0;
            }
            buff->time = file_time_count++;
            buff->info = info;
            info->buff = buff;
            if (info->buff_length != 0) {
                sceLseek(info->handle, -info->buff_length, 1);
                if (sceRead(info->handle, buff->data, info->buff_length) != info->buff_length) {
                    NuErrorProlog(nufile_file, 0x28C)(msg_assert);
                }
            }
        }
        total = 0;
        while (size > 0) {
            if (info->read_pos >= info->file_length) {
                return total;
            }
            if (info->read_pos < info->buff_start
                || info->read_pos >= info->buff_start + info->buff_length) {
                if (info->read_pos != info->file_pos) {
                    sceLseek(info->handle, info->read_pos, 0);
                    info->file_pos = info->read_pos;
                }
                len = sceRead(info->handle, info->buff->data, 0x10000);
                info->buff_length = len;
                info->buff_start = info->file_pos;
                info->file_pos += len;
            }
            offset = info->read_pos - info->buff_start;
            avail = info->buff_length - offset;
            n = size;
            if (avail < size) {
                n = avail;
            }
            if (n != 0) {
                memcpy(data, info->buff->data + offset, n);
            }
            data = (char *)data + n;
            total += n;
            size -= n;
            info->read_pos += n;
        }
        return total;
    }
    return sceRead(fh, data, size);
}


inline int NuFileStatus(int fh)
{
    int status;

    if (fh < 0x400) {
        if (sceIoctl(fh - 1, 1, &status) != 0) {
            NuErrorProlog(nufile_file, 0x269)(msg_assert);
        }
        return status;
    }
    return 0;
}


extern char D_00613870[];
extern char D_006138A8[];
#define msg_dat_read_failed     D_00613870
#define msg_dat_seek_failed     D_006138A8

void NuPPUnpack(unsigned char *src, unsigned char *out);


int NuDatFileLoadBuffer(struct nudathdr_s *ndh, char *fname, void *dest, int maxsize)
{
    unsigned char *packed;
    int fh;
    int size;
    int packed_len;

    fh = NuDatFileOpen(ndh, fname, NUFILE_READ);
    if (fh == 0) {
        return 0;
    }
    if (ndh->openmode == NUFILE_READ_NOWAIT) {
        while (NuFileStatus(ndh->fh) != 0) {
        }
    }
    if (datfiles[fh - 0x800].ppack) {
        size = datfiles[fh - 0x800].uplen;
        packed_len = datfiles[fh - 0x800].len;
        packed = (unsigned char *)dest + (size + 0x100);
        packed -= packed_len;
        if (maxsize >= size + 0x100 && size != 0) {
            while (NuDatFileRead(fh, packed, packed_len) < 0) {
                NuDebugMsgProlog(nufile_file, 0x5E1)(msg_dat_read_failed);
                while (NuDatFileSeek(fh, 0, 0) < 0) {
                    NuDebugMsgProlog(nufile_file, 0x5E4)(msg_dat_seek_failed);
                }
            }
            if (ndh->openmode == NUFILE_READ_NOWAIT) {
                while (NuFileStatus(ndh->fh) != 0) {
                }
            }
            NuFileClose(fh);
            NuPPUnpack(packed, dest);
            return size;
        }
    } else {
        size = datfiles[fh - 0x800].len;
        if (maxsize >= size && size != 0) {
            while (NuDatFileRead(fh, dest, size) < 0) {
                NuDebugMsgProlog(nufile_file, 0x5F2)(msg_dat_read_failed);
                while (NuDatFileSeek(fh, 0, 0) < 0) {
                    NuDebugMsgProlog(nufile_file, 0x5F5)(msg_dat_seek_failed);
                }
            }
            if (ndh->openmode == NUFILE_READ_NOWAIT) {
                while (NuFileStatus(ndh->fh) != 0) {
                }
            }
            NuFileClose(fh);
            return size;
        }
    }
    NuFileClose(fh);
    return 0;
}


void NuFileWriteStringV(int fh, char *fmt, ...)
{
    char buf[0x400];
    va_list args;
    int len;

    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);
    len = strlen(fmt);
    sceWrite(fh - 1, buf, len);
}


extern int sceCdDiskReady(int mode);
extern int sceCdInit(int mode);
extern int sceCdMmode(int media);
extern int sceFsReset();
extern void sceSifInitRpc();
extern int sceSifRebootIop(const char *img_name);
extern int sceSifSyncIop();

void NuFileInitEx(int deviceid, int rebootiop)
{
    nufile_deviceid = deviceid;
    if (deviceid == 1 || deviceid == 3) {
        sceSifInitRpc(0);
        sceCdInit(0);
        if (rebootiop != 0) {
            while (sceSifRebootIop(iop_img_name) == 0) {
            }
            while (sceSifSyncIop() == 0) {
            }
            sceSifInitRpc(0);
            sceCdInit(0);
        }
        if (nufile_deviceid == 1) {
            sceCdMmode(1);
        } else {
            sceCdMmode(2);
        }
        sceFsReset();
        sceCdDiskReady(0);
    }
    memset(memfiles, 0, sizeof(memfiles));
    memset(datfiles, 0, sizeof(datfiles));
}


void NuFileInit(int deviceid)
{
    nufile_deviceid = deviceid;
    if (deviceid == 1 || deviceid == 3) {
        sceSifInitRpc(0);
        sceCdInit(0);
        while (sceSifRebootIop(iop_img_name) == 0) {
        }
        while (sceSifSyncIop() == 0) {
        }
        sceSifInitRpc(0);
        sceCdInit(0);
        if (nufile_deviceid == 1) {
            sceCdMmode(1);
        } else {
            sceCdMmode(2);
        }
        sceFsReset();
        sceCdDiskReady(0);
    }
    memset(memfiles, 0, sizeof(memfiles));
    memset(datfiles, 0, sizeof(datfiles));
}


void NuFileGetCurrentPath(char *path)
{
    unsigned char *p;
    int i;

    strcpy(path, filesys_root[nufile_deviceid]);
    strcat(path, working_dir);
    p = (unsigned char *)path;
    for (i = NuStrLen(filesys_root[nufile_deviceid]); p[i] != 0; i++) {
        if (p[i] >= 'a' && p[i] <= 'z') {
            p[i] = p[i] - 'a' + 'A';
        }
    }
}


extern char D_0062E9B8[];


void NuFileSetCurrentDirectory(char *dir)
{
    char *suffix;

    suffix = D_0062E9B8;
    if (dir[0] != 0) {
        strcpy(working_dir, dir);
        NuStrCat(working_dir, suffix);
    } else {
        working_dir[0] = 0;
    }
}


