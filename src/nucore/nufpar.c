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

struct nufpcomjmp_s;

// Size: 0x1244
struct nufpar_s
{
    int fh; // Offset: 0x0
    char fbuff[4096]; // Offset: 0x4
    char lbuff[257]; // Offset: 0x1004
    char wbuff[257]; // Offset: 0x1105
    int line_num; // Offset: 0x1208
    int line_pos; // Offset: 0x120C
    int cpos; // Offset: 0x1210
    int buffstart; // Offset: 0x1214
    int buffend; // Offset: 0x1218
    struct nufpcomjmp_s* comstack[8]; // Offset: 0x121C
    int compos; // Offset: 0x123C
    int size; // Offset: 0x1240
};

// Size: 0x8
struct nufpcomjmp_s
{
    char* fname; // Offset: 0x0
    void(*func)(struct nufpar_s*); // Offset: 0x4
};

char NuGetChar(struct nufpar_s* fPar) {
    int bytesread;
    int be;
    char rv;

    be = fPar->buffend;
    if (be < 0) {
        be = 0;
    }
    if (fPar->cpos > fPar->buffend) {
        if (fPar->buffend + 1 < fPar->size) {
            bytesread = NuFileRead(fPar->fh, fPar->fbuff, (fPar->size - be > 0x1000) ? 0x1000 : fPar->size - be);
            fPar->buffstart = fPar->buffend + 1;
            fPar->buffend += bytesread;
            if (bytesread == 0) {
                return 0;
            }
        } else {
           return 0;
        }
    }
    rv = fPar->fbuff[fPar->cpos - fPar->buffstart];
    fPar->cpos++;
    return rv;
}