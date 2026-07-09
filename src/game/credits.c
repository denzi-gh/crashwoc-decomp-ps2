/*
 * Unit: game/credits
 *
 * Functions:
 *   0x00260e70 DrawCredits
 *   0x00260ff8 InitCredits
 */

#include "creature.h"

struct credit_s {
    char *txt;    /* 0x0 */
    short colour; /* 0x4 */
    short size;   /* 0x6 */
}; /* 0x8 */

struct game_s {
    u8 _pad[0xF];
    u8 language; /* 0xF */
};

struct gametimer_s {
    u8 _pad[0xC];
    float ftime; /* 0xC */
    u8 _pad2[8];
}; /* 0x18 */

extern struct credit_s Credit[323];
extern s32 CREDITCOUNT;
extern f32 credit_speed;
extern f32 credit_time;
extern struct game_s Game;
extern struct gametimer_s GameTimer;
extern char tbuf[];

/* .rodata string "SR. MANAGER OF PUBLIC RELATIONS" (retail-owned). */
extern char D_0062A608[];
/* .sdata float constants (retail-owned, addressed $gp-relative). */
extern float D_0062E96C; /* upper size cutoff (1.7f) */
extern float D_0062E970; /* y factor for the SR. MANAGER credit */
extern float D_0062E974; /* per-credit scroll step (0.1f) */

extern char *NuStrCpy(char *dst, char *src);
extern s32 strcmp(const char *a, const char *b);
extern void AddSpacesIntoText(char *text, s32 flag);
extern void Text3D(char *text, s32 a, s32 colour, float x, float y, float z,
                   float sx, float sy, float sz);

void DrawCredits(void) {
    struct credit_s *credit;
    float xscale;
    float size;
    float y;
    s32 i;

    credit = Credit;
    size = GameTimer.ftime * credit_speed + (-1.0f);
    for (i = 0; i < CREDITCOUNT; i++, credit++) {
        xscale = credit->size / 100.0f;
        if ((credit->txt != 0) && (size > -1.5f) && (size < D_0062E96C)) {
            NuStrCpy(tbuf, credit->txt);
            y = 1.0f;
            if (strcmp(tbuf, D_0062A608) == 0) {
                y = D_0062E970;
            }
            if (Game.language == 0x63) {
                AddSpacesIntoText(tbuf, 1);
            }
            Text3D(tbuf, 1, credit->colour, 0.0f, size, 1.0f, xscale * y,
                   xscale, xscale + xscale);
        }
        size = size - ((xscale * D_0062E974) + (xscale * D_0062E974));
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
 