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

typedef unsigned char u_char;
typedef unsigned int u_int;

typedef struct NuTex {
    int type;
    int width;
    int height;
    int mmcnt;
    u_char *bits;
} NuTex;

extern void *malloc(u_int size);
extern void free(void *ptr);
extern void *memset(void *dst, int value, u_int size);
extern void *memcpy(void *dst, const void *src, u_int size);
extern void *NuMemAllocFn(int size, char *file, int line);
extern void NuMemFreeFn(void *ptr, char *file, int line);

extern u_int D_002E98B0[256];
#define block4_lut D_002E98B0
extern u_int D_002E9CB0[128];
#define block8_lut D_002E9CB0
extern int D_002E9EB0[32];
#define page4_block_table D_002E9EB0
extern int D_002E9F30[32];
#define page4_block32_table D_002E9F30
extern int D_002E9FB0[32];
#define page8_block_table D_002E9FB0
extern int D_002EA030[32];
#define page8_block32_table D_002EA030
extern char D_00616130[];
#define bitconv_srcfile D_00616130

int BlockConv4to32(u_char *in, u_char *out)
{
    u_int i, j, k;
    u_int index;
    u_int pos, shift0, shift1, offset;
    u_char lo, hi;
    u_char *src;
    int n;

    n = 0;
    src = in;
    for (i = 0; i < 4; i++) {
        index = (i & 1) * 128;
        for (j = 0; j < 16; j++) {
            for (k = 0; k < 4; k++) {
                pos = block4_lut[index++];
                shift0 = (pos & 1) * 4;
                offset = pos >> 1;
                lo = (src[offset] & (0x0F << shift0)) >> shift0;
                pos = block4_lut[index++];
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

int PageConv4to32(int width, int height, u_char *in, u_char *out)
{
    int *block_x;
    int *block_y;
    u_char in_block[256];
    u_char out_block[256];
    u_char *src;
    u_char *dst;
    u_char *out_src;
    u_char *out_dst;
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
            id = page4_block32_table[n++];
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
            block = page4_block_table[i * nblocks_w + j];
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

inline int BlockConv8to32(u_char *in, u_char *out)
{
    u_int i, j, k;
    u_int index;
    int n;

    n = 0;
    for (i = 0; i < 4; i++) {
        index = (i & 1) * 64;
        for (j = 0; j < 16; j++) {
            for (k = 0; k < 4; k++) {
                out[n++] = in[block8_lut[index++]];
            }
        }
        in += 64;
    }
    return 0;
}

int PageConv8to32(int width, int height, u_char *in, u_char *out)
{
    int *block_x;
    int *block_y;
    u_char in_block[256];
    u_char out_block[256];
    u_char *src;
    u_char *dst;
    u_char *out_src;
    u_char *out_dst;
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
            id = page8_block32_table[n++];
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
            block = page8_block_table[i * nblocks_w + j];
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

int Conv4to32(int width, int height, u_char *in, u_char *out)
{
    u_char in_page[0x2000];
    u_char out_page[0x2000];
    u_char *page;
    u_char *image;
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

int Conv8to32(int width, int height, u_char *in, u_char *out)
{
    u_char in_page[0x2000];
    u_char out_page[0x2000];
    u_char *page;
    u_char *image;
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

void NuTexSwizzelBits(NuTex *tex)
{
    int maxlevel;
    int level;
    int width, height;
    int size;
    int i;
    u_char *bits;
    u_char *swizzled;

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
