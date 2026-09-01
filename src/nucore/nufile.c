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

enum nufilemode_e {
	NUFILE_READ = 0,
	NUFILE_WRITE = 1,
	NUFILE_APPEND = 2,
	NUFILE_READ_NOWAIT = 3,
	NUFILE_MODE_CNT = 4
};

struct numemfile_s {
    char* start;              /* 0x00 */
    char* end;               /* 0x04 */
    char *curr;  	     /* 0x08 */
    enum nufilemode_e mode;  /* 0x0c */
    int used;  	            /* 0x10 */
}; /* 0x14 */

struct fileinfo_s {
    int handle;                     /* 0x00 */
    int read_pos;                   /* 0x04 */
    int file_pos;	            /* 0x08 */
    int file_length;                /* 0x0c */
    int buff_start;  	            /* 0x10 */
    int buff_length;  	            /* 0x14 */
    int use_buff;  	            /* 0x18 */
    int start_lsn;  	            /* 0x1c or struct filebuff_s* buff; */
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
    char data[4096];             /* 0x08 */
}; /* 0x1008 */

struct nudatfile_s {
    struct nudathdr_s* ndh;   /* 0x00 */
    int start;                /* 0x04 */
    int len;	              /* 0x08 */
    int fix;                  /* 0x0c */
    int used;                 /* 0x10 */
    int pos;                  /* 0x14 */
    int ppack;                /* 0x18 */
}; /* 0x1c */

struct nublk_s {
    int id;                   /* 0x00 */
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


int NuFileWrite(int fh, void *data, int size)
{
    return sceWrite(fh - 1, data, size);
}


int NuFileGetBlkSize(void)
{
    return blk_stack[blk_level].size;
}


void* NuMemFileAddr(int fh)
{
    return memfiles[fh - 0x400].curr;
}


int NuDatFileOpenSize(int fh)
{
    int ix = fh - 0x800;
    struct nudatfile_s *df = &datfiles[ix];

    return df->len;
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


extern char RO_62BAB0[];
extern char STR_0062BB90[];
extern char STR_0062BBD0[];
extern char STR_0062BBE8[];
extern char STR_0062BC18[];
extern char D_006134B0[];
#define nufile_file             D_006134B0
extern char STR_006456B0[];
extern char STR_006456C0[];


int NuDatFileOpen(struct nudathdr_s* ndh, char* fname, enum nufilemode_e mode);
int NuFileSeek(int fh, int offset, int origin);
int NuFileStatus(int fh);
//void * memset (void * __s, int __c, unsigned long __n);
int sceOpen(char * name, int flag, ...);
char* strcat(char* __dest, const char* __src);
char* strcpy(char* __dest, const char* __src);
int strlen(const char* __s);

typedef void (*NuErrorFunc)(const char *fmt, ...);

extern NuErrorFunc NuErrorProlog(const char *file, int line);

extern NuErrorFunc NuDebugMsgProlog(const char *file, int line);

extern void NuStrCpy(char * s1, char * s2);
extern void NuStrCat (char * s1, char * s2);
extern int sceSifLoadModule(const char* name, int arg_size, const char* args);
extern char RO_62BAB0[];
extern char STR_0062BAD0[];
extern char STR_0062BAF0[];
extern char STR_0062BB10[];
extern char STR_0062BB38[];
extern char STR_0062BB50[];
extern char STR_0062BB68[];
extern char STR_006456B0[];

int NuFileSifLoadModule(char *file,int args,char *argp) {
  int iVar2;
  int rv;
  int i;
  char *pbVar4;
  char path[256];
  
  NuStrCpy(path,filesys_root[nufile_deviceid]);
  NuStrCat(path,working_dir);
  NuStrCat(path,file);
  if (nufile_deviceid == 1) {
    pbVar4 = &path[strlen(filesys_root[1])];
    for (i = 0; pbVar4[i] != 0; i++) {
        if (pbVar4[i] >= 0x61 && pbVar4[i] < 0x7b) {
            pbVar4[i] -= 0x20;
        }
    }
    strcat(path,STR_006456B0);
  }
  rv = sceSifLoadModule(path,args,argp);
  if (rv < 0) {
      NuDebugMsgProlog(nufile_file, 0x1B8)
          ("Unable to load SIF module \"%s\"", path);
      iVar2 = -rv;
      switch (iVar2) {
      case 0x10000:
          NuDebugMsgProlog(nufile_file, 0x1BC)
              ("Binding the IOP module failed");
          break;
      case 0x10004:
          NuDebugMsgProlog(nufile_file, 0x1C0)
              ("The IOP module version does not match");
          break;
      case 0x10001:
          NuDebugMsgProlog(nufile_file, 0x1C4)
              ("RPC to the IOP mailed");
          break;
      default:
          NuDebugMsgProlog(nufile_file, 0x1C8)
              ("Unknown error code %d", rv);
          break;
      }
  } else {
      NuDebugMsgProlog(nufile_file, 0x1CD)
          ("Successfully loaded IRX Module: %s", path);
  }
  return rv;
}


int NuFileOpen(char* file, enum nufilemode_e mode) {
    int fh;
    int fp;
    int iVar5;
    int sVar6;
    char* pbVar8;
    char lfile[256];

    if (curr_dat != 0) {
        fp = NuDatFileOpen(curr_dat, file, mode);
        if (fp != 0)
            return fp;
    }
    strcpy(lfile, filesys_root[nufile_deviceid]);
    strcat(lfile, working_dir);
    strcat(lfile, file);
    pbVar8 = (char*)(lfile + (int)strlen(filesys_root[nufile_deviceid]));
    for (sVar6 = 0; pbVar8[sVar6] != 0; sVar6++) {
        if (pbVar8[sVar6] >= 0x61 && pbVar8[sVar6] < 0x7b) {
            pbVar8[sVar6] -= 0x20;
        }
    }
    if (nufile_deviceid == 1) {
        strcat(lfile, STR_006456B0);
    }
    if (NUFILE_READ_NOWAIT < mode) {
        NuErrorProlog(nufile_file, 0x214)(STR_006456C0);
    }
    fp = sceOpen(lfile, fmode[mode]);
    if (fp < 0) {
        NuDebugMsgProlog(nufile_file, 0x218)(
            "!!!!!!!!!!!!!!!!!!! opening file %s failed!!!!!!!!!!!!!!!!!!!!1", lfile
        );
    } else {
        NuDebugMsgProlog(nufile_file, 0x21b)("opening file %s ok!(%d)", lfile);
        fh = fp + 1;
        memset(&file_info[fp], 0, 0x20);
        file_info[fp].handle = fh;
        if (mode != NUFILE_WRITE) {
            do {
                iVar5 = NuFileSeek(fh, 0, 2);
                if (iVar5 >= 0)
                    break;
                NuDebugMsgProlog(nufile_file, 0x225)("NuFileOpen - size seek failed - retrying");
            } while (1);
            file_info[fp].file_length = iVar5;
            if (mode == NUFILE_READ_NOWAIT) {
                do {
                } while (NuFileStatus(fh) != 0);
            }
            do {
                if (NuFileSeek(fh, 0, 0) >= 0)
                    break;
                NuDebugMsgProlog(nufile_file, 0x230)(
                    "NuFileOpen - size seek restore failed - retrying"
                );
            } while (1);
            if (mode == NUFILE_READ_NOWAIT) {
                do {
                } while (NuFileStatus(fh) != 0);
            }
        } else {
            file_info[fp].file_length = 0;
        }
        if (mode == NUFILE_READ) {
            file_info[fp].use_buff = nufile_buffering_enabled;
        }
    }
    return fp;
}

extern char RO_62BAB0[];
extern char STR_006456C0[];

int sceLseek(int fd, int offset, int how);
int sceRead(int fd, void * addr, int size);
int NuDatFileRead (int fh, void * data, int size);


int NuFileSeek(int fh, int offset, int origin) {
    if (fh >= 0x400) {
        if (fh >= 0x800) {
            return NuDatFileSeek(fh, offset, origin);
        }
        fh -= 0x400;
        switch (origin) {
            default:
            case 0:
                memfiles[fh].curr = memfiles[fh].start + offset;
                break;
            case 1:
                memfiles[fh].curr += offset;
                break;
            case 2:
                memfiles[fh].curr = memfiles[fh].end - offset;
                break;
        }
        return ((int)memfiles[fh].curr - (int)memfiles[fh].start);
    }
    fh--;
    if (file_info[fh].use_buff != 0) {
        if (origin == 1) {
            file_info[fh].read_pos += offset;
            return file_info[fh].read_pos;
        }
        if (origin < 2) {
            file_info[fh].read_pos = offset;
            return file_info[fh].read_pos;
        }
        if (origin != 2) {
            file_info[fh].read_pos = offset;
            return file_info[fh].read_pos;
        }
        file_info[fh].read_pos = file_info[fh].file_length - offset;
        return file_info[fh].read_pos;
    }
    return sceLseek(fh, offset, forig[origin]);
}

void NuMemFileClose(int fh);
int NuDatFileFindTree(struct nudathdr_s * ndh, char * fname);
int NuDatFilePos(int fh);
int sceClose(int fd);
extern char RO_62BAB0[];
extern char RO_62BC50[];
extern char RO_62BC68[];
extern char RO_62BC90[];

int NuFileSize(char *fname) {
  int iVar1;
  int iVar2;
  int iVar3;
  int fd;
  
  iVar1 = nufile_buffering_enabled;
  nufile_buffering_enabled = 0;
  if ((curr_dat != 0) && (iVar2 = NuDatFileFindTree(curr_dat,fname), iVar2 != 0)) {
    iVar2 = curr_dat->finfo[iVar2].uplen;
    iVar1 = nufile_buffering_enabled;
  }
  else {
    iVar2 = -1;
    if (((fname != 0) && (*fname != 0))) {
        iVar3 = NuFileOpen(fname,NUFILE_READ);
        if (iVar3 != 0) {
      do {
        iVar2 = NuFileSeek(iVar3,0,2);
      } while (iVar2 < 0);
      fd = iVar3 - 1;
      if (iVar3 >= 0x400) {
       if (iVar3 >= 0x800) {
        iVar2 = NuDatFilePos(iVar3);
       } else {
        iVar2 = (int)memfiles[iVar3 - 0x400].curr - (int)memfiles[iVar3 - 0x400].start;
       }
        iVar3--;
      }
      else {
        if (file_info[fd].use_buff != 0) {
          iVar2 = file_info[fd].read_pos;
        }
        else {
          while (iVar2 = sceLseek(fd,0,1), iVar2 < 0) {
            NuDebugMsgProlog(nufile_file,0x306)
            ("NuFilePos - seek failed %d - retrying",iVar2);
          }
        }
      }
      NuDebugMsgProlog(nufile_file,0x247)
      ("closing file (%d)",iVar3);
      if (iVar3 >= 0x400) {
        NuMemFileClose(iVar3);
      }
      else {
        while( 1 ) {
          if (sceClose(iVar3) > -1) break;
          NuDebugMsgProlog(nufile_file,0x24e)
          ("NuFileClose - close failed, retrying");
        }
        if (file_info[iVar3].start_lsn != 0) {
          file_info[iVar3].start_lsn = 0;
        }
        memset(&file_info[iVar3],0,0x20);
      }
    }
    }
  }
  nufile_buffering_enabled = iVar1;
  return iVar2;
}

extern char RO_62BAB0[];
extern char RO_62BC50[];
extern char RO_62BC68[];
void* NuMemAllocFn(int size, char * file, int line);
void NuMemFileClose(int fh);

void* NuFileLoad(char *fname) {
  int iVar1;
  int iVar2;
  int iVar3;
  void *data;
  
  iVar1 = nufile_buffering_enabled;
  data = 0;
  nufile_buffering_enabled = 0;
  iVar2 = NuFileOpen(fname,NUFILE_READ);
  if (iVar2 != 0) {
    if (iVar2 > 0x800) {
      iVar3 = NuDatFileOpenSize(iVar2);
    }
    else {
      iVar3 = file_info[iVar2 - 1].file_length;
    }
    if (iVar3 > 0) {
      data = NuMemAllocFn(iVar3,nufile_file,0x35e);
      if (data != 0) {
        NuFileRead(iVar2,data,iVar3);
      }
    }
    NuDebugMsgProlog(nufile_file,0x247)("closing file (%d)",iVar2);
    if (iVar2 >= 0x400) {
      NuMemFileClose(iVar2);
    }
    else {
      iVar2--;
      while(sceClose(iVar2) < 0) {
        NuDebugMsgProlog(nufile_file,0x24e)
        ("NuFileClose - close failed, retrying");
      }
      if (file_info[iVar2].start_lsn != 0) {
        file_info[iVar2].start_lsn = 0;
      }
      memset(&file_info[iVar2],0,0x20);
    }
  }
  nufile_buffering_enabled = iVar1;
  return data;
}

extern char RO_62BAB0[];
extern char RO_62BC50[];
extern char RO_62BC68[];
extern char STR_0062BCB8[];
extern char STR_0062BCF0[];

extern int NuPPLoadBuffer(int fh, void * mem, int buffsize);
extern int NuDatFileLoadBuffer(struct nudathdr_s* ndh, char * fname, void * mem, int buffsize);

int NuFileLoadBuffer(char *filename,void *mem,int buffsize) {
  int size;
  int fh;
  
  size = 0;
  if (curr_dat != 0) {
    size = NuDatFileLoadBuffer(curr_dat,filename,mem,buffsize);
  }
  if (size != 0) return size;
  fh = NuFileOpen(filename,NUFILE_READ);
  if (fh != 0) {
    if (nufile_try_packed != 0) {
      size = NuPPLoadBuffer(fh,mem,buffsize);
    }
    else {
      if (fh >= 0x800) {
        size = NuDatFileOpenSize(fh);
      }
      else {
        fh--;
        size = file_info[fh].file_length;
      }
      if ((size < buffsize) && (size != 0)) {
        while (NuFileRead(fh,mem,size) < 0) {
          NuDebugMsgProlog(nufile_file,0x391)
          ("NuFileLoadBuffer - recoverable read failure - retrying");
          while (NuFileSeek(fh,0,0) < 0) {
            NuDebugMsgProlog(nufile_file,0x393)
            ("NuFileLoadBuffer - recoverable seek failure - retrying");
          }
        }
        NuDebugMsgProlog(nufile_file,0x247)
        ("closing file (%d)",fh);
        if (fh >= 0x400) {
          NuMemFileClose(fh);
        }
        else {
          fh--;
          while(sceClose(fh) < 0) {
            NuDebugMsgProlog(nufile_file,0x24e)
            ("NuFileClose - close failed, retrying");
          }
          if (file_info[fh].start_lsn != 0) {
            file_info[fh].start_lsn = 0;
          }
          memset(&file_info[fh],0,0x20);
        }
      }
    }
    NuDebugMsgProlog(nufile_file,0x247)
    ("closing file (%d)",fh);
    if (fh >= 0x400) {
      NuMemFileClose(fh);
      return size;
    }
      fh--;
      while(sceClose(fh) < 0) {
        NuDebugMsgProlog(nufile_file,0x24e)
        ("NuFileClose - close failed, retrying");
      }
      if (file_info[fh].start_lsn != 0) {
        file_info[fh].start_lsn = 0;
      }
      memset(&file_info[fh],0,0x20);
  }
  return size;
}

int NuFileRead(int fh, void* data, int size) {
    struct fileinfo_s* info;
    int iVar1;
    int iVar2;
    int iVar3;
    int iVar5;
    int __n;
    int iVar6;
    int iVar8;
    char* datac;
    int sVar11;
    int __n_00;
    struct filebuff_s *buff;

    sVar11 = size;
    if (fh > 0x400) {
        if (fh > 0x7ff) {
            return NuDatFileRead(fh, data, size);
        }
        datac = memfiles[fh - 0x400].curr;
        __n_00 = (int)(memfiles[fh - 0x400].end + (1 - (int)&datac[__n_00]));
        if (__n_00 >= sVar11) {
            __n_00 = sVar11;
        }
        if (__n_00 != 0) {
            memcpy(data, &datac[__n_00], __n_00);
        }
    } else {
        iVar2 = fh - 1;
        info = &file_info[iVar2];
        if (info->use_buff != 0) {
            if (info->start_lsn == 0) {
                // data = &file_buff[0];
                iVar3 = file_time_count;
                iVar5 = 0;
                for (iVar6 = 0; iVar6 < 4; iVar6++) {
                    iVar1 = file_buff[iVar6].time;
                    iVar8 = iVar6;
                    if (iVar3 <= iVar1) {
                        iVar1 = iVar3;
                        iVar8 = iVar5;
                    }
                    iVar3 = iVar1;
                    // data = data + 0x4002;
                    iVar5 = iVar8;
                }
                //iVar8 = iVar8 * 0x10008;
                buff = &file_buff[iVar8];
                if (buff->info->handle != 0) {
                    buff->info->start_lsn = 0;
                }
                iVar3 = info->buff_length;
                buff->time = file_time_count;
                //buff->info->handle = info->handle;
                file_time_count++;
                //info->start_lsn = buff->info->start_lsn;
                if (iVar3 != 0) {
                    sceLseek(info->handle, -iVar3, 1);
                    iVar3 = sceRead(info->handle, buff->data, info->buff_length);
                    if (iVar3 != info->buff_length) {
                        NuErrorProlog(nufile_file, 0x286)(STR_006456C0);
                    }
                }
            }
            __n_00 = 0;
            while ((sVar11 > 0 && (iVar3 = info->read_pos, iVar3 < info->file_length))) {
                if ((iVar3 < info->buff_start) || (info->buff_start + info->buff_length >= iVar3)) {
                    iVar5 = info->file_pos;
// LAB_00100edc:
                    if (iVar3 == iVar5) {
                        iVar3 = info->start_lsn;
                    } else {
                        sceLseek(info->handle, iVar3, 0);
                        info->file_pos = info->read_pos;
                        iVar3 = info->start_lsn;
                    }
                    iVar5 = sceRead(info->handle, (void*)(iVar3 + 8), 0x10000);
                    iVar3 = info->file_pos;
                    info->buff_length = iVar5;
                    info->buff_start = iVar3;
                    info->file_pos = iVar3 + iVar5;
                }
                iVar3 = info->read_pos - info->buff_start;
                __n = (info->buff_length - iVar3);
                if (sVar11 <= __n) {
                    __n = sVar11;
                }
                if (__n != 0) {
                    memcpy(data, (void*)(iVar3 + info->start_lsn), __n);
                }
                iVar3 = (int)__n;
                data = (void*)((int)data + iVar3);
                __n_00 += iVar3;
                sVar11 -= iVar3;
                info->read_pos += iVar3;
            }
        } else {
            __n_00 = sceRead(iVar2, data, size);
        }
    }
    return __n_00;
}

extern int sceCdDiskReady(int mode);
extern int sceCdInit(int mode);
extern int sceCdMmode(int media);
extern int sceFsReset();
extern void sceSifInitRpc();
extern int sceSifRebootIop(const char *img_name);
extern int sceSifSyncIop();

void NuFileInitEx(int deviceid,int rebootiop) {
  nufile_deviceid = deviceid;
  if ((deviceid == 1) || (deviceid == 3)) {
    sceSifInitRpc(0);
    sceCdInit(0);
    if (rebootiop != 0) {
      do {
      } while (sceSifRebootIop(iop_img_name) == 0);
      do {
      } while (sceSifSyncIop() == 0);
      sceSifInitRpc(0);
      sceCdInit(0);
    }
    if (nufile_deviceid == 1) {
      sceCdMmode(1);
    }
    else {
      sceCdMmode(2);
    }
    sceFsReset();
    sceCdDiskReady(0);
  }
  memset(memfiles,0,0x190);
  memset(datfiles,0,0x230);
}
