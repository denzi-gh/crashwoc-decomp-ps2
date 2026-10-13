/*
 * Unit: nu3d/nutexcmp
 *
 * Functions:
 *   0x00162b88 Blend16Pixels
 *   0x00162e40 NuTexCreateLowResColourMap
 *   0x00163070 BilinearExpand
 *   0x00163230 NuTexCreateLuminanceMultiplyMap
 */


typedef unsigned char u_char;
typedef unsigned int u_int;

typedef struct NuTex {
    int type;
    int width;
    int height;
    int mmcnt;
    u_char *bits;
    u_int *pal;
} NuTex;

extern void *NuMemAllocFn(int size, char *file, int line);
extern int printf(const char *format, ...);
extern int abs(int value);

extern char D_00616000[];
#define nutexcmp_srcfile D_00616000
extern char D_006160E0[];
#define underflow_msg D_006160E0


int Blend16Pixels(u_int *pixels, int *lo, int *hi)
{
    float weights[16] = {
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f
    };
    float red, green, blue;
    float pr, pg, pb, pa;
    float weight;
    float fr, fg, fb;
    int brightest;
    int y, x;
    int r, g, b;

    red = 0.0f;
    green = red;
    blue = red;
    brightest = pixels[0] & 0xFF;
    for (y = 0; y < 4; y++) {
        for (x = 0; x < 4; x++) {
            pr = (float)(pixels[y * 4 + x] & 0xFF);
            pg = (float)((pixels[y * 4 + x] >> 8) & 0xFF);
            pb = (float)((pixels[y * 4 + x] >> 16) & 0xFF);
            pa = (float)(pixels[y * 4 + x] >> 24);
            if (brightest < pr) {
                brightest = pr;
            }
            if (brightest < pg) {
                brightest = pg;
            }
            if (brightest < pb) {
                brightest = pb;
            }
            if (brightest < pa) {
            }
            weight = weights[y * 4 + x];
            red += pr * weight;
            green += pg * weight;
            blue += pb * weight;
        }
    }
    fr = (int)red;
    fg = (int)green;
    fb = (int)blue;
    r = (int)fr >> 4;
    g = (int)fg >> 4;
    b = (int)fb >> 4;
    if (r > 255) {
        r = 255;
    }
    if (g > 255) {
        g = 255;
    }
    if (b > 255) {
        b = 255;
    }
    return r + (g << 8) + (b << 16);
}



void NuTexCreateLowResColourMap(NuTex *src, NuTex *dst)
{
    u_int block[16];
    int lo, hi;
    u_char *srcbits;
    u_char *dstbits;
    u_char *texel;
    int width, height;
    int lowwidth, lowheight;
    int x, y, i, j;
    int sx, sy;
    int col, row;
    int index;
    u_int colour;

    width = src->width;
    height = src->height;
    *dst = *src;
    lowwidth = width >> 2;
    lowheight = height >> 2;
    dst->width = lowwidth;
    dst->height = lowheight;
    lo = 0;
    hi = 255;
    dstbits = NuMemAllocFn(lowwidth * 3 * lowheight, nutexcmp_srcfile, 0x86);
    dst->bits = dstbits;
    srcbits = src->bits;
    for (x = 0; x < lowwidth; x++) {
        for (y = 0; y < lowheight; y++) {
            for (j = 0; j < 4; j++) {
                for (i = 0; i < 4; i++) {
                    sx = x * 4 + i;
                    sy = y * 4 + j;
                    col = (sx < 0) ? 0 : sx;
                    if (col >= width) {
                        col = width - 1;
                    }
                    row = (sy < 0) ? 0 : sy;
                    if (row >= height) {
                        row = height - 1;
                    }
                    texel = &srcbits[(col + row * width) * 3];
                    block[j * 4 + i] = texel[0] + (texel[1] << 8) + (texel[2] << 16);
                }
            }
            colour = Blend16Pixels(block, &lo, &hi);
            index = y * lowwidth + x;
            dstbits[index * 3] = colour;
            dstbits[index * 3 + 1] = colour >> 8;
            dstbits[index * 3 + 2] = colour >> 16;
        }
    }
}


int BilinearExpand(u_int *pixels, float u, float v)
{
    int channels[4][4];
    float result[4];
    u_char *bytes;
    float fu, fv;
    int i, c;

    u += 2.5f;
    v += 2.5f;
    if (u >= 4.0f) {
        u -= 4.0f;
    }
    if (v >= 4.0f) {
        v -= 4.0f;
    }
    if (u < 0.0f) {
        u += 4.0f;
    }
    if (v < 0.0f) {
        v += 4.0f;
    }
    for (i = 0; i < 4; i++) {
        channels[0][i] = pixels[i] & 0xFF;
        channels[1][i] = (pixels[i] >> 8) & 0xFF;
        channels[2][i] = (pixels[i] >> 16) & 0xFF;
        channels[3][i] = pixels[i] >> 24;
    }
    fu = u * 0.25f;
    fv = v * 0.25f;
    bytes = (u_char *)pixels;
    for (c = 0; c < 3; c++) {
        result[c] = (1.0f - fu) * (1.0f - fv) * bytes[c] + fu * (1.0f - fv) * bytes[c + 4]
                  + (1.0f - fu) * fv * bytes[c + 8] + fu * fv * bytes[c + 12];
    }
    return (int)result[0] + ((int)result[1] << 8) + ((int)result[2] << 16);
}


void NuTexCreateLuminanceMultiplyMap(NuTex *colourmap, NuTex *tex, NuTex *dst, int twobit)
{
    u_int block[16];
    int ncolours;
    int mapheight;
    u_char *texbits;
    u_char *mapbits;
    u_char *dstbits;
    u_char *texel;
    u_int *pal;
    int width, height;
    int mapwidth;
    int nentries;
    int x, y, i, j, k;
    int cx, cy, mx, my;
    u_int colour;
    u_int r, g, b;
    int lum, alpha, level, index;
    int diff, nextdiff;
    int byte;
    u_char nibble;

    ncolours = 16;
    texbits = tex->bits;
    mapbits = colourmap->bits;
    width = tex->width;
    height = tex->height;
    dst->width = width;
    dst->height = height;
    dst->mmcnt = 0;
    dst->bits = NuMemAllocFn(width * height / 2, nutexcmp_srcfile, 0x15B);
    pal = NuMemAllocFn(0x40, nutexcmp_srcfile, 0x15C);
    dst->pal = pal;
    dstbits = dst->bits;
    dst->type = 4;
    mapheight = colourmap->height;
    mapwidth = colourmap->width;
    if (twobit) {
        ncolours = 4;
    }
    {
        int levels16[16] = {
            -128, -84, -52, -32, -20, -12, -8, -4,
            0, 5, 11, 17, 29, 47, 76, 123
        };
        int levels32[32] = {
            -42, -8, 8, 42, -42, -8, 8, 42,
            -42, -8, 8, 42, -42, -8, 8, 42,
            -42, -42, -42, -42, -8, -8, -8, -8,
            8, 8, 8, 8, 42, 42, 42, 42
        };

        nentries = twobit ? 32 : 16;
        for (k = 0; k < nentries; k++) {
            if (nentries == 16) {
                level = levels16[k];
            } else {
                level = levels32[k];
            }
            level = (level + 128) & 0xFF;
            pal[k] = level + (level << 8) + (level << 16) + (level << 24);
        }
    }
    for (x = 0; x < width; x++) {
        for (y = 0; y < height; y++) {
            cx = (x < 0) ? 0 : x;
            if (cx >= width) {
                cx = width - 1;
            }
            cy = (y < 0) ? 0 : y;
            if (cy >= height) {
                cy = height - 1;
            }
            texel = &texbits[(cx + cy * width) * 3];
            colour = texel[0] + (texel[1] << 8) + (texel[2] << 16);
            r = colour & 0xFF;
            g = (colour >> 8) & 0xFF;
            b = (colour >> 16) & 0xFF;
            lum = r;
            if (lum < g) {
                lum = g;
            }
            if (lum < b) {
                lum = b;
            }
            alpha = lum << 7;
            for (j = 0; j < 2; j++) {
                for (i = 0; i < 2; i++) {
                    mx = ((x + 2) >> 2) - 1 + i;
                    my = ((y + 2) >> 2) - 1 + j;
                    mx = (mx < 0) ? 0 : mx;
                    if (mx >= mapwidth) {
                        mx = mapwidth - 1;
                    }
                    my = (my < 0) ? 0 : my;
                    if (my >= mapheight) {
                        my = mapheight - 1;
                    }
                    texel = &mapbits[(mx + my * mapwidth) * 3];
                    block[j * 2 + i] = texel[0] + (texel[1] << 8) + (texel[2] << 16);
                }
            }
            colour = BilinearExpand(block, x % 4, y % 4);
            r = colour & 0xFF;
            g = (colour >> 8) & 0xFF;
            b = (colour >> 16) & 0xFF;
            lum = r;
            if (lum < g) {
                lum = g;
            }
            if (lum < b) {
                lum = b;
            }
            if (lum == 0) {
                lum = 1;
            }
            alpha = alpha / lum;
            if (alpha < 0) {
                printf(underflow_msg);
            }
            if (alpha > 255) {
                alpha = 255;
            }
            pal = dst->pal;
            for (k = 0; k < ncolours - 1; k++) {
                if ((pal[k + 1] >> 24) >= alpha) {
                    nextdiff = abs((pal[k + 1] >> 24) - alpha);
                    diff = abs((pal[k] >> 24) - alpha);
                    index = k + 1;
                    if (diff < nextdiff) {
                        index = k;
                    }
                    goto found;
                }
            }
            index = ncolours - 1;
        found:
            if (twobit) {
                byte = dstbits[((x % (width >> 1)) >> 1) + y * (width >> 2)];
                index &= 3;
                if (x % 2 == 1) {
                    if (x * 2 >= width) {
                        byte = (byte & 0x3F) + (index << 6);
                    } else {
                        byte = (byte & 0xCF) + (index << 4);
                    }
                } else {
                    if (x * 2 >= width) {
                        byte = (byte & 0xF3) + (index << 2);
                    } else {
                        byte = (byte & 0xFC) + index;
                    }
                }
                dstbits[((x % (width >> 1)) >> 1) + y * (width >> 2)] = byte;
            } else {
                nibble = index;
                if ((x & 1) == 0) {
                    dstbits[(x >> 1) + y * (width >> 1)] = (dstbits[(x >> 1) + y * (width >> 1)] & 0x0F) + ((nibble & 0xF) << 4);
                } else {
                    dstbits[(x >> 1) + y * (width >> 1)] = (dstbits[(x >> 1) + y * (width >> 1)] & 0xF0) + (nibble & 0xF);
                }
            }
        }
    }
}
