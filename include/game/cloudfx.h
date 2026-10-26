#ifndef GAME_CLOUDFX_H
#define GAME_CLOUDFX_H

#include "nu_types.h"

/* A cloud particle (clouds[4096], ttptls[4096]). */
struct cloud_s {
    s16 x;                              /* 0x00 */
    s16 y;                              /* 0x02 */
    s16 z;                              /* 0x04 */
}; /* 0x06 */

void CloudFxInit(void);
void DoClouds(s32 paused);

#endif /* GAME_CLOUDFX_H */
