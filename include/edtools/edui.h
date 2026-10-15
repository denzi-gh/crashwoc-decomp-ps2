#ifndef EDTOOLS_EDUI_H
#define EDTOOLS_EDUI_H

#include "nu_types.h"

struct eduimenu_s;
struct numtl_s;
struct nupad_s;

/* Pen per highlight state and the colours of a menu item. */
struct eduistyle_s {
    s32 pen[2];                         /* 0x00 */
    u32 colour[2];                      /* 0x08 */
}; /* 0x10 */

/* One colour stage of a gradient pick. */
struct eduigradstage_s {
    struct eduigradstage_s *next;       /* 0x00 */
    struct eduigradstage_s *prev;       /* 0x04 */
    f32 pos;                            /* 0x08 */
    u32 colour;                         /* 0x0C */
    f32 h;                              /* 0x10 */
    f32 s;                              /* 0x14 */
    f32 v;                              /* 0x18 */
}; /* 0x1C */

/* A gradient stage as eduiGradPickRead returns it. */
struct eduigradread_s {
    f32 pos;                            /* 0x00 */
    f32 h;                              /* 0x04 */
    f32 s;                              /* 0x08 */
    f32 v;                              /* 0x0C */
    f32 r;                              /* 0x10 */
    f32 g;                              /* 0x14 */
    f32 b;                              /* 0x18 */
    u32 colour;                         /* 0x1C */
}; /* 0x20 */

/* One menu item. The word at 0x0C is the creator's: edui and edmain keep a
 * pointer there, the game editors an index. */
struct eduiitem_s {
    struct eduiitem_s *next;            /* 0x00 */
    struct eduiitem_s *prev;            /* 0x04 */
    u32 type;                           /* 0x08 */
    union {
        void *data;                     /* 0x0C */
        s32 value;                      /* 0x0C */
    };
    u8 on;                              /* 0x10 */
    signed char align;                  /* 0x11 */
    u8 pad12[0x2];                      /* 0x12 */
    s32 group;                          /* 0x14 */
    s32 x;                              /* 0x18 */
    s32 y;                              /* 0x1C */
    char *text;                         /* 0x20 */
    struct eduistyle_s style;           /* 0x24 */
    s32 (*press)(struct eduimenu_s *menu, struct eduiitem_s *item, u32 paddata, u32 buttons); /* 0x34 */
    s32 (*process)(struct eduimenu_s *menu, struct eduiitem_s *item, struct nupad_s *pad, f32 dt); /* 0x38 */
    s32 (*render)(struct eduimenu_s *menu, struct eduiitem_s *item, s32 x, s32 y, s32 w, s32 highlight); /* 0x3C */
    void (*destroy)(struct eduimenu_s *menu, struct eduiitem_s *item); /* 0x40 */
    union {
        struct {
            void (*select)(struct eduimenu_s *menu, struct eduiitem_s *item, u32 buttons);
        } sel;
        struct {
            char text[256];
            s32 col;
            s32 row;
            s32 active;
            void (*enter)(struct eduimenu_s *menu, struct eduiitem_s *item, u32 paddata);
        } textpick;
        struct {
            f32 hue;
            f32 brightness;
            f32 h;
            f32 s;
            f32 v;
            f32 r;
            f32 g;
            f32 b;
            void (*change)(struct eduimenu_s *menu, struct eduiitem_s *item, u32 buttons);
        } colour;
        struct {
            f32 u[2];
            f32 v[2];
            void (*change)(struct eduimenu_s *menu, struct eduiitem_s *item, u32 buttons);
            struct numtl_s *mtl;
            s32 corner;
            f32 zoom;
        } texpick;
        struct {
            struct eduigradstage_s *first;
            struct eduigradstage_s *stage;
            void (*change)(struct eduimenu_s *menu, struct eduiitem_s *item, u32 paddata);
            s32 flash;
        } grad;
        struct {
            void (*change)(struct eduimenu_s *menu, struct eduiitem_s *item, s32 buttons);
            f32 frac;
            f32 val;
            f32 min;
            f32 range;
            s32 flash;
            char *fmt;
            f32 granularity;
            u8 r;
            u8 g;
            u8 b;
        } slider;
    } u;                                /* 0x44 */
}; /* 0x154 */

/* A menu: a list of items, linked to its parent and child menus. */
struct eduimenu_s {
    struct eduiitem_s *head;            /* 0x00 */
    struct eduiitem_s *tail;            /* 0x04 */
    struct eduiitem_s *current;         /* 0x08 */
    struct eduiitem_s *cursor;          /* 0x0C */
    s32 x;                              /* 0x10 */
    s32 y;                              /* 0x14 */
    s32 w;                              /* 0x18 */
    s32 h;                              /* 0x1C */
    void *font;                         /* 0x20 */
    void (*cancel)(struct eduimenu_s *menu, struct eduimenu_s *parent); /* 0x24 */
    char *title;                        /* 0x28 */
    struct eduimenu_s *child;           /* 0x2C */
    struct eduimenu_s *parent;          /* 0x30 */
    char titlebuf[0];                   /* 0x34 */
}; /* 0x34 */

extern struct eduiitem_s *edui_last_item;

void eduiInit(void);
void eduiMenuRender(struct eduimenu_s *menu);
void eduiCreateMessageMenu(struct eduimenu_s *parent, char *text, s32 error);
void eduiItemSliderSetVal(struct eduiitem_s *item, f32 val);
void eduiItemSliderSetGranularity(struct eduiitem_s *item, f32 granularity);
s32 eduiGradPickRead(struct eduiitem_s *pick, struct eduigradread_s *buf, s32 max);
struct eduigradstage_s *eduiGradStageAddRGB(struct eduiitem_s *pick, f32 pos, f32 r, f32 g, f32 b);

#endif /* EDTOOLS_EDUI_H */
