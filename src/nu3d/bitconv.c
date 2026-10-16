/*
 * Unit: nu3d/bitconv
 *
 * Functions:
 *   0x001650a8 BlockConv4to32
 *   0x001651b8 PageConv4to32
 *   0x00165460 PageConv8to32
 *   0x00165768 Conv4to32
 *   0x00165a28 Conv8to32
 *   0x00165ce0 NuTexSwizzelBits
 *   0x00165f58 BlockConv8to32
 */

#include "nu_libc.h"
#include "nu_types.h"

#include "nu3d/nutex.h"

extern void *NuMemAllocFn(int size, char *file, int line);
extern void NuMemFreeFn(void *ptr, char *file, int line);

extern char D_00616130[];
#define bitconv_srcfile D_00616130

int BlockConv4to32(u8 *in, u8 *out)
{
    static u32 lut[256] = {
        0, 68, 8, 76, 16, 84, 24, 92, 1, 69, 9, 77, 17, 85, 25, 93, 2, 70, 10, 78,
        18, 86, 26, 94, 3, 71, 11, 79, 19, 87, 27, 95, 4, 64, 12, 72, 20, 80, 28,
        88, 5, 65, 13, 73, 21, 81, 29, 89, 6, 66, 14, 74, 22, 82, 30, 90, 7, 67,
        15, 75, 23, 83, 31, 91, 32, 100, 40, 108, 48, 116, 56, 124, 33, 101, 41,
        109, 49, 117, 57, 125, 34, 102, 42, 110, 50, 118, 58, 126, 35, 103, 43,
        111, 51, 119, 59, 127, 36, 96, 44, 104, 52, 112, 60, 120, 37, 97, 45, 105,
        53, 113, 61, 121, 38, 98, 46, 106, 54, 114, 62, 122, 39, 99, 47, 107, 55,
        115, 63, 123, 4, 64, 12, 72, 20, 80, 28, 88, 5, 65, 13, 73, 21, 81, 29, 89,
        6, 66, 14, 74, 22, 82, 30, 90, 7, 67, 15, 75, 23, 83, 31, 91, 0, 68, 8, 76,
        16, 84, 24, 92, 1, 69, 9, 77, 17, 85, 25, 93, 2, 70, 10, 78, 18, 86, 26,
        94, 3, 71, 11, 79, 19, 87, 27, 95, 36, 96, 44, 104, 52, 112, 60, 120, 37,
        97, 45, 105, 53, 113, 61, 121, 38, 98, 46, 106, 54, 114, 62, 122, 39, 99,
        47, 107, 55, 115, 63, 123, 32, 100, 40, 108, 48, 116, 56, 124, 33, 101, 41,
        109, 49, 117, 57, 125, 34, 102, 42, 110, 50, 118, 58, 126, 35, 103, 43,
        111, 51, 119, 59, 127
    };
    u32 i, j, k;
    u32 index;
    u32 pos, shift0, shift1, offset;
    u8 lo, hi;
    u8 *src;
    int n;

    n = 0;
    src = in;
    for (i = 0; i < 4; i++) {
        index = (i & 1) * 128;
        for (j = 0; j < 16; j++) {
            for (k = 0; k < 4; k++) {
                pos = lut[index++];
                shift0 = (pos & 1) * 4;
                offset = pos >> 1;
                lo = (src[offset] & (0x0F << shift0)) >> shift0;
                pos = lut[index++];
                shift1 = (pos & 1) * 4;
                offset = pos >> 1;
                hi = (src[offset] & (0x0F << shift1)) >> shift1;
                out[n++] = ((hi << 4) & 0xF0) | lo;
            }
        }
        src += 64;
    }
    return 0;
}

int PageConv4to32(int width, int height, u8 *in, u8 *out)
{
    static int block_table4[32] = {
        0, 2, 8, 10, 1, 3, 9, 11, 4, 6, 12, 14, 5, 7, 13, 15, 16, 18, 24, 26, 17,
        19, 25, 27, 20, 22, 28, 30, 21, 23, 29, 31
    };
    static int block_table32[32] = {
        0, 1, 4, 5, 16, 17, 20, 21, 2, 3, 6, 7, 18, 19, 22, 23, 8, 9, 12, 13, 24,
        25, 28, 29, 10, 11, 14, 15, 26, 27, 30, 31
    };
    int *block_x;
    int *block_y;
    u8 in_block[256];
    u8 out_block[256];
    u8 *src;
    u8 *dst;
    u8 *out_src;
    u8 *out_dst;
    int nblocks_w, nblocks_h;
    int in_line, in_size;
    int out_line, out_size;
    int i, j, k, n;
    int id, block;

    block_x = malloc(32 * sizeof(int));
    block_y = malloc(32 * sizeof(int));
    n = 0;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 8; j++) {
            id = block_table32[n++];
            block_x[id] = j;
            block_y[id] = i;
        }
    }
    nblocks_w = width / 32;
    nblocks_h = height / 16;
    memset(in_block, 0, 256);
    memset(out_block, 0, 256);
    in_line = 64;
    in_size = 1024;
    out_line = 256;
    for (i = 0; i < nblocks_h; i++) {
        for (j = 0; j < nblocks_w; j++) {
            block = block_table4[i * nblocks_w + j];
            src = in + i * in_size + j * 16;
            dst = in_block;
            for (k = 0; k < 16; k++) {
                memcpy(dst, src, 16);
                dst += 16;
                src += in_line;
            }
            BlockConv4to32(in_block, out_block);
            out_size = out_line * 8;
            out_src = out_block;
            out_dst = out + block_y[block] * out_size + block_x[block] * 32;
            for (k = 0; k < 8; k++) {
                memcpy(out_dst, out_src, 32);
                out_src += 32;
                out_dst += out_line;
            }
        }
    }
    free(block_x);
    free(block_y);
    return 0;
}

inline int BlockConv8to32(u8 *in, u8 *out)
{
    static u32 lut[128] = {
        0, 36, 8, 44, 1, 37, 9, 45, 2, 38, 10, 46, 3, 39, 11, 47, 4, 32, 12, 40, 5,
        33, 13, 41, 6, 34, 14, 42, 7, 35, 15, 43, 16, 52, 24, 60, 17, 53, 25, 61,
        18, 54, 26, 62, 19, 55, 27, 63, 20, 48, 28, 56, 21, 49, 29, 57, 22, 50, 30,
        58, 23, 51, 31, 59, 4, 32, 12, 40, 5, 33, 13, 41, 6, 34, 14, 42, 7, 35, 15,
        43, 0, 36, 8, 44, 1, 37, 9, 45, 2, 38, 10, 46, 3, 39, 11, 47, 20, 48, 28,
        56, 21, 49, 29, 57, 22, 50, 30, 58, 23, 51, 31, 59, 16, 52, 24, 60, 17, 53,
        25, 61, 18, 54, 26, 62, 19, 55, 27, 63
    };
    u32 i, j, k;
    u32 index;
    int n;

    n = 0;
    for (i = 0; i < 4; i++) {
        index = (i & 1) * 64;
        for (j = 0; j < 16; j++) {
            for (k = 0; k < 4; k++) {
                out[n++] = in[lut[index++]];
            }
        }
        in += 64;
    }
    return 0;
}

int PageConv8to32(int width, int height, u8 *in, u8 *out)
{
    static int block_table8[32] = {
        0, 1, 4, 5, 16, 17, 20, 21, 2, 3, 6, 7, 18, 19, 22, 23, 8, 9, 12, 13, 24,
        25, 28, 29, 10, 11, 14, 15, 26, 27, 30, 31
    };
    static int block_table32[32] = {
        0, 1, 4, 5, 16, 17, 20, 21, 2, 3, 6, 7, 18, 19, 22, 23, 8, 9, 12, 13, 24,
        25, 28, 29, 10, 11, 14, 15, 26, 27, 30, 31
    };
    int *block_x;
    int *block_y;
    u8 in_block[256];
    u8 out_block[256];
    u8 *src;
    u8 *dst;
    u8 *out_src;
    u8 *out_dst;
    int nblocks_w, nblocks_h;
    int in_line, in_size;
    int out_line, out_size;
    int i, j, k, n;
    int id, block;

    block_x = malloc(32 * sizeof(int));
    block_y = malloc(32 * sizeof(int));
    n = 0;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 8; j++) {
            id = block_table32[n++];
            block_x[id] = j;
            block_y[id] = i;
        }
    }
    nblocks_w = width / 16;
    nblocks_h = height / 16;
    memset(in_block, 0, 256);
    memset(out_block, 0, 256);
    in_line = 128;
    in_size = 2048;
    out_line = 256;
    for (i = 0; i < nblocks_h; i++) {
        for (j = 0; j < nblocks_w; j++) {
            block = block_table8[i * nblocks_w + j];
            src = in + i * in_size + j * 16;
            dst = in_block;
            for (k = 0; k < 16; k++) {
                memcpy(dst, src, 16);
                dst += 16;
                src += in_line;
            }
            BlockConv8to32(in_block, out_block);
            out_size = out_line * 8;
            out_src = out_block;
            out_dst = out + block_y[block] * out_size + block_x[block] * 32;
            for (k = 0; k < 8; k++) {
                memcpy(out_dst, out_src, 32);
                out_src += 32;
                out_dst += out_line;
            }
        }
    }
    free(block_x);
    free(block_y);
    return 0;
}

int Conv4to32(int width, int height, u8 *in, u8 *out)
{
    u8 in_page[0x2000];
    u8 out_page[0x2000];
    u8 *page;
    u8 *image;
    int npages_w, npages_h;
    int in_line, out_line;
    int in_rows, out_rows;
    int i, j, k;

    for (i = 0; i < 11; i++) {
        if (width == (1024 >> i)) {
            break;
        }
    }
    if (i == 11) {
        return -1;
    }
    memset(in_page, 0, 0x2000);
    memset(out_page, 0, 0x2000);
    for (i = 0; i < 11; i++) {
        if (height == (1024 >> i)) {
            break;
        }
    }
    if (i == 11) {
        return -1;
    }
    npages_w = (width - 1) / 128 + 1;
    npages_h = (height - 1) / 128 + 1;
    if (npages_w == 1) {
        in_line = width / 2;
        out_rows = width / 4;
    } else {
        out_rows = 32;
        in_line = 64;
    }
    if (npages_h == 1) {
        in_rows = height;
        out_line = height * 2;
    } else {
        out_line = 256;
        in_rows = 128;
    }
    for (i = 0; i < npages_h; i++) {
        for (j = 0; j < npages_w; j++) {
            image = in + in_line * 128 * npages_w * i + in_line * j;
            page = in_page;
            for (k = 0; k < in_rows; k++) {
                memcpy(page, image, in_line);
                image += in_line * npages_w;
                page += 64;
            }
            PageConv4to32(128, 128, in_page, out_page);
            page = out_page;
            image = out + out_line * 32 * npages_w * i + out_line * j;
            for (k = 0; k < out_rows; k++) {
                memcpy(image, page, out_line);
                image += out_line * npages_w;
                page += 256;
            }
        }
    }
    return 0;
}

int Conv8to32(int width, int height, u8 *in, u8 *out)
{
    u8 in_page[0x2000];
    u8 out_page[0x2000];
    u8 *page;
    u8 *image;
    int npages_w, npages_h;
    int in_line, out_line;
    int in_rows, out_rows;
    int i, j, k;

    for (i = 0; i < 11; i++) {
        if (width == (1024 >> i)) {
            break;
        }
    }
    if (i == 11) {
        return -1;
    }
    for (i = 0; i < 11; i++) {
        if (height == (1024 >> i)) {
            break;
        }
    }
    if (i == 11) {
        return -1;
    }
    memset(in_page, 0, 0x2000);
    memset(out_page, 0, 0x2000);
    npages_w = (width - 1) / 128 + 1;
    npages_h = (height - 1) / 64 + 1;
    if (npages_w == 1) {
        in_line = width;
        out_line = width * 2;
    } else {
        in_line = 128;
        out_line = 256;
    }
    if (npages_h == 1) {
        in_rows = height;
        out_rows = height / 2;
    } else {
        in_rows = 64;
        out_rows = 32;
    }
    for (i = 0; i < npages_h; i++) {
        for (j = 0; j < npages_w; j++) {
            image = in + in_line * 64 * npages_w * i + in_line * j;
            page = in_page;
            for (k = 0; k < in_rows; k++) {
                memcpy(page, image, in_line);
                image += in_line * npages_w;
                page += 128;
            }
            PageConv8to32(128, 64, in_page, out_page);
            page = out_page;
            image = out + out_line * out_rows * npages_w * i + out_line * j;
            for (k = 0; k < out_rows; k++) {
                memcpy(image, page, out_line);
                image += out_line * npages_w;
                page += 256;
            }
        }
    }
    return 0;
}

void NuTexSwizzelBits(struct nutex_s *tex)
{
    int maxlevel;
    int level;
    int width, height;
    int size;
    int i;
    u8 *bits;
    u8 *swizzled;

    if (tex->mmcnt < 2) {
        maxlevel = tex->mmcnt ? tex->mmcnt - 1 : 0;
        switch (tex->type) {
        case 4:
            if ((tex->width >> maxlevel) < 128) {
                return;
            }
            bits = tex->bits;
            for (level = 0; level <= maxlevel; level++) {
                width = tex->width >> level;
                height = tex->height >> level;
                size = (width * height) >> 1;
                for (i = 0; i < size; i++) {
                    bits[i] = (bits[i] << 4) | (bits[i] >> 4);
                }
                swizzled = NuMemAllocFn(size, bitconv_srcfile, 0x2DD);
                Conv4to32(width, height, bits, swizzled);
                memcpy(bits, swizzled, size);
                bits += size;
                NuMemFreeFn(swizzled, bitconv_srcfile, 0x2E1);
            }
            tex->type = 6;
            tex->width = tex->width / 2;
            tex->height = tex->height / 4;
            break;
        case 5:
            if ((tex->width >> maxlevel) < 128) {
                return;
            }
            bits = tex->bits;
            for (level = 0; level <= maxlevel; level++) {
                width = tex->width >> level;
                height = tex->height >> level;
                size = width * height;
                swizzled = NuMemAllocFn(size, bitconv_srcfile, 0x2F0);
                Conv8to32(width, height, bits, swizzled);
                memcpy(bits, swizzled, size);
                bits += size;
                NuMemFreeFn(swizzled, bitconv_srcfile, 0x2F4);
            }
            tex->type = 7;
            tex->width = tex->width / 2;
            tex->height = tex->height / 2;
            break;
        }
    }
}
