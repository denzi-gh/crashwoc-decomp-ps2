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

struct numemfile_s memfiles[20];

void* NuMemFileAddr(int fh)
{
  return memfiles[fh + -0x400].curr;
}
