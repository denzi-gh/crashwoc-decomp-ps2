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

#ifdef NW_HOST
/* Retail calls these without a prototype in scope (game/main.c); only the host declares them. */
struct TbSet;
void NuPs2Init(void);
void NuPs2InitIOP(void);
struct TbSet *tbsetCreate(int *colours);
void tbslotBeginExFn(struct TbSet *set, int slot, char *name);
void tbslotBeginFn(struct TbSet *set, int slot);
int tbslotEndFn(void *tbset, int slot);
int tbslotLastValue(struct TbSet *set, int slot);
void tbslotReset(struct TbSet *set, int slot);
#endif

#endif
