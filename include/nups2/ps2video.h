#ifndef NUPS2_PS2VIDEO_H
#define NUPS2_PS2VIDEO_H

/* One entry of the vblank interrupt chains (NuIntAddVBlankS/E), kept in
 * priority order. */
struct NuVBlankHandler {
    struct NuVBlankHandler *next; /* 0x00 */
    struct NuVBlankHandler *prev; /* 0x04 */
    void (*fn)(void);             /* 0x08 */
    int priority;                 /* 0x0C */
}; /* 0x10 */

void NuIntAddVBlankS(struct NuVBlankHandler *handler);
void NuIntRemVBlankS(struct NuVBlankHandler *handler);

#endif
