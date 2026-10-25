/*
 * Unit: nucore/rnc
 *
 * Functions:
 *   0x00105db0 NuPPLoadBuffer
 *   0x001060f0 NuPPUnpack
 *   0x001065f8 NuPPGetSize
 */

#include "nucore/nuerror.h"

#define RNC_ID 0x02434E52

#define SWAP32(x) (((x) >> 24) | (((x) & 0xFF0000) >> 8) | (((x) & 0xFF00) << 8) | ((x) << 24))

#define PP_NEXTBIT() bits <<= 1; carry = bits >> 8; carry &= 1
#define PP_REFILL() bits = *in++; bits <<= 1; bits += carry; carry = bits >> 8; carry &= 1
#define PP_GETBIT(refill) PP_NEXTBIT(); bits &= 0xFF; if (bits == 0) goto refill

typedef struct RNCHeader {
    unsigned int id;
    unsigned int unpacked;
    unsigned int packed;
} RNCHeader;

extern int NuFileSeek(int fh, int offset, int origin);
extern int NuFileRead(int fh, void *data, int size);
extern int NuFileOpenSize(int fh);

extern void timeUserReset(void);
extern unsigned int timeUserRead(void);
void NuPPUnpack(unsigned char *src, unsigned char *out);


#define rnc_file "..\\nu2crash.ps2\\nucore\\rnc.c"


int NuPPLoadBuffer(int fh, unsigned char *dest, int maxsize) {
    RNCHeader hdr;
    unsigned char *packed;
    unsigned char *start;
    int size;
    int result;
    unsigned int length;

    do {
        while (NuFileSeek(fh, 0, 0) < 0) {
            NuDebugMsgProlog(rnc_file, 0x89)("NuPPLoadBuffer1 - recoverable seek failure, retrying");
        }
        size = NuFileRead(fh, &hdr, sizeof(hdr));
        if (size < 0) {
            NuDebugMsgProlog(rnc_file, 0x8D)("NuPPLoadBuffer2 - recoverable read failure, retrying");
        }
    } while (size < 0);

    if (hdr.id == RNC_ID) {
        hdr.unpacked = SWAP32(hdr.unpacked);
        length = SWAP32(hdr.packed) + 18;
        packed = &dest[hdr.unpacked + 0x100];
        packed -= length;
        hdr.packed = length;
        timeUserReset();
        start = packed - 4;
        NuDebugMsgProlog(rnc_file, 0x9B)("Packed read commenced");
        do {
            NuDebugMsgProlog(rnc_file, 0x9D)("Seeking");
            while (NuFileSeek(fh, 4, 0) < 0) {
                NuDebugMsgProlog(rnc_file, 0x9F)("NuPPLoadBuffer3 - recoverable seek failure, retrying");
            }
            NuDebugMsgProlog(rnc_file, 0xA2)("Reading");
            size = NuFileRead(fh, packed, hdr.packed - 4);
            if (size < 0) {
                NuDebugMsgProlog(rnc_file, 0xA5)("NuPPLoadBuffer4 - recoverable read failure, retrying (err%d)", size);
            }
        } while (size < 0);
        NuDebugMsgProlog(rnc_file, 0xA8)("Packed read finished");
        NuPPUnpack(start, dest);
        size = timeUserRead();
        NuDebugMsgProlog(rnc_file, 0xAD)("Loaded %d bytes and unpacked to %d bytes in %d ticks", hdr.packed, hdr.unpacked, size);
        size = hdr.unpacked;
    } else {
        size = NuFileOpenSize(fh);
        if (maxsize < size) {
            NuErrorProlog(rnc_file, 0xB3)("given buffer (%d) too small for (%d)", maxsize, size);
        }
        NuDebugMsgProlog(rnc_file, 0xB5)("Unpacked read commenced");
        do {
            while (NuFileSeek(fh, 0, 0) < 0) {
                NuDebugMsgProlog(rnc_file, 0xB9)("NuPPLoadBuffer5 - recoverable seek failure, retrying");
            }
            result = NuFileRead(fh, dest, size);
            if (result < 0) {
                NuDebugMsgProlog(rnc_file, 0xBD)("NuPPLoadBuffer6 - recoverable read failure, retrying");
            }
        } while (result < 0);
        NuDebugMsgProlog(rnc_file, 0xC0)("Unpacked read finished");
    }
    return size;
}


void NuPPUnpack(unsigned char *src, unsigned char *out) {
    unsigned short stack[66];
    unsigned short *sp;
    unsigned char *in;
    unsigned int bits;
    unsigned int carry;
    unsigned int len;
    unsigned int count;
    unsigned int high;
    unsigned int even;
    unsigned int odd;
    unsigned int excess;

    src += 4;
    sp = &stack[64];
    /* the header bytes go through the working variables, which keeps the
     * temps off $a0/$a1 so src and out stay in their argument registers */
    bits = src[0];
    even = src[1];
    odd = src[2];
    carry = src[3];
    bits <<= 24;
    even <<= 16;
    odd <<= 8;
    bits |= even;
    bits |= odd;
    bits |= carry;
    *(unsigned int *)sp = bits;
    *--sp = 0;
    in = src;
    in += 14;
    carry = 1;
    PP_REFILL();
    bits <<= 1;
    goto next;

refill1:
    PP_REFILL();
    goto back1;
refill2:
    PP_REFILL();
    goto back2;
refill3:
    PP_REFILL();
    goto back3;
refill4:
    PP_REFILL();
    goto back4;
refill5:
    PP_REFILL();
    goto back5;
refill6:
    PP_REFILL();
    goto back6;
refill7:
    PP_REFILL();
    goto back7;
refill8:
    PP_REFILL();
    goto back8;

raw3:
    count = 3;
raw:
    PP_GETBIT(refill1);
back1:
    len <<= 1;
    len += carry;
    if ((int)--count >= 0) goto raw;
    len += 2;
    do {
        even = in[0];
        odd = in[1];
        out[0] = even;
        out[1] = odd;
        even = in[2];
        odd = in[3];
        out[2] = even;
        out[3] = odd;
        in += 4;
        out += 4;
    } while ((int)--len >= 0);
    goto next;

getlen:
    PP_GETBIT(refill2);
back2:
    count <<= 1;
    count += carry;
    PP_GETBIT(refill3);
back3:
    if (!carry) goto getoffs_hi;
    count--;
    PP_GETBIT(refill4);
back4:
    count <<= 1;
    count += carry;
    excess = count - 9;
    if (excess == 0) goto raw3;
getoffs_hi:
    PP_GETBIT(refill5);
back5:
    if (!carry) goto getoffs;
    PP_GETBIT(refill6);
back6:
    len <<= 1;
    len += carry;
    PP_GETBIT(refill7);
back7:
    if (carry) goto bigoffs;
    if (len != 0) goto swap;
    len = 1;
getoffs_lo:
    PP_GETBIT(refill8);
back8:
    len <<= 1;
    len += carry;
swap:
    high = (len << 8) & 0xFF00;
    len >>= 8;
    len |= high;
getoffs:
    high = len & 0xFF00;
    carry = count & 1;
    len = *in++;
    len |= high;
    count >>= 1;
    src = out - len;
    src--;
    if (carry) {
        *out++ = *src++;
    }
    count--;
    if (len == 0) {
        len = *src;
        do {
            out[0] = len;
            out[1] = len;
            out += 2;
        } while ((int)--count >= 0);
        goto next;
    }
pairs:
    odd = src[1];
    even = src[0];
    out[1] = odd;
    out[0] = even;
    src += 2;
    out += 2;
    if ((int)--count >= 0) goto pairs;
    goto next;

refill0:
    PP_REFILL();
    if (carry) goto copy;
literal:
    *out++ = *in++;
next:
    PP_NEXTBIT();
    if (!carry) {
        *out++ = *in++;
        PP_NEXTBIT();
        if (!carry) goto literal;
    }
    bits &= 0xFF;
    if (bits == 0) goto refill0;
copy:
    count = 2;
    len = 0;
    PP_GETBIT(refill9);
back9:
    if (!carry) goto getlen;
    PP_GETBIT(refill10);
back10:
    if (!carry) goto getoffs;
    count = 3;
    PP_GETBIT(refill11);
back11:
    if (!carry) goto getoffs_hi;
    count = *in++;
    if (count == 0) goto chunkend;
    count += 8;
    goto getoffs_hi;
bigoffs:
    PP_GETBIT(refill12);
back12:
    len <<= 1;
    len += carry;
    len |= 4;
    PP_GETBIT(refill13);
back13:
    if (carry) goto swap;
    goto getoffs_lo;

refill9:
    PP_REFILL();
    goto back9;
refill10:
    PP_REFILL();
    goto back10;
refill11:
    PP_REFILL();
    goto back11;
refill12:
    PP_REFILL();
    goto back12;
refill13:
    PP_REFILL();
    goto back13;

chunkend:
    PP_NEXTBIT();
    bits &= 0xFF;
    if (bits == 0) {
        PP_REFILL();
    }
    if (carry) goto next;
    for (bits = *sp++; bits != 0; bits--) {
        even = *sp++;
        *out++ = even;
    }
}


int NuPPGetSize(int fh) {
    RNCHeader hdr;
    int pos;
    int size;

    pos = NuFileSeek(fh, 0, 1);
    NuFileSeek(fh, 0, 0);
    NuFileRead(fh, &hdr, sizeof(hdr));
    if (hdr.id == RNC_ID) {
        hdr.unpacked = SWAP32(hdr.unpacked);
        hdr.packed = SWAP32(hdr.packed) + 6;
        size = hdr.unpacked;
    } else {
        size = NuFileOpenSize(fh);
    }
    NuFileSeek(fh, pos, 0);
    return size;
}
