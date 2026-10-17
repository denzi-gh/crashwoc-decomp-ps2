/*
 * Unit: game/credits
 *
 * Functions:
 *   0x00260e70 DrawCredits
 *   0x00260ff8 InitCredits
 */

#include "nu_libc.h"

#include "game/gametypes.h"

struct credit_s {
    char *txt;    /* 0x0 */
    short colour; /* 0x4 */
    short size;   /* 0x6 */
}; /* 0x8 */

extern struct credit_s Credit[323];
extern s32 CREDITCOUNT;
extern f32 credit_speed;
extern f32 credit_time;
extern struct game_s Game;
extern struct gtimer_s GameTimer;
extern char tbuf[];

/* .rodata string "SR. MANAGER OF PUBLIC RELATIONS" (retail-owned). */
extern char D_0062A608[];

extern void NuStrCpy(char *dst, char *src);
extern void AddSpacesIntoText(char *text, s32 flag);
extern void Text3D(char *txt, float x, float y, float z, float scalex,
                   float scaley, float scalez, s32 align, s32 colour);

void DrawCredits(void) {
    struct credit_s *credit;
    float xscale;
    float ypos;
    float widthscale;
    s32 i;

    credit = Credit;
    ypos = GameTimer.ftime * credit_speed + (-1.0f);
    for (i = 0; i < CREDITCOUNT; i++, credit++) {
        xscale = credit->size / 100.0f;
        if ((credit->txt != 0) && (ypos > -1.5f) && (ypos < 1.7f)) {
            NuStrCpy(tbuf, credit->txt);
            widthscale = 1.0f;
            if (strcmp(tbuf, D_0062A608) == 0) {
                widthscale = 0.95f;
            }
            if (Game.language == 0x63) {
                AddSpacesIntoText(tbuf, 1);
            }
            Text3D(tbuf, 0.0f, ypos, 1.0f, xscale * widthscale, xscale,
                   xscale + xscale, 1, credit->colour);
        }
        ypos = ypos - ((xscale * 0.1f) + (xscale * 0.1f));
    }
}

void InitCredits(void) {
    struct credit_s *credit;
    float size;

    credit = Credit;
    size = 0.0f;
    for (CREDITCOUNT = 0; credit->size > 0; CREDITCOUNT++, credit++) {
        size += ((float)credit->size / 1000.0f) +
                ((float)credit->size / 1000.0f);
    }
    size += 2.0f;
    credit_time = size / credit_speed;
}
 