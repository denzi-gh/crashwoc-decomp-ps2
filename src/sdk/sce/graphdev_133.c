/*
 * Unit: sdk/sce/graphdev_133
 *
 * Functions:
 *   0x00261cd8 sceGsSetHalfOffset2
 */

typedef unsigned long u_long;

typedef struct {
    u_long SCAX0 : 11;
    u_long pad11 : 5;
    u_long SCAX1 : 11;
    u_long pad27 : 5;
    u_long SCAY0 : 11;
    u_long pad43 : 5;
    u_long SCAY1 : 11;
    u_long pad59 : 5;
} tGS_SCISSOR_1;

typedef struct {
    u_long frame1;
    u_long frame1addr;
    u_long zbuf1;
    u_long zbuf1addr;
    u_long xyoffset1;
    u_long xyoffset1addr;
    tGS_SCISSOR_1 scissor1;
    u_long scissor1addr;
} sceGsDrawEnv1;


void sceGsSetHalfOffset2(sceGsDrawEnv1 *draw, short centerx, short centery, short halfoff)
{
    u_long ofx, ofy;

    ofy = (centery - (u_long)(draw->scissor1.SCAY1 + 1) / 2) << 4;
    ofx = (centerx - (u_long)(draw->scissor1.SCAX1 + 1) / 2) << 4;
    if (halfoff) {
        draw->xyoffset1 = ofx | ((ofy + 8) << 32);
    } else {
        draw->xyoffset1 = ofx | (ofy << 32);
    }
}
