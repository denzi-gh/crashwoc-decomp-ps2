/*
 * Unit: nups2/clipping
 *
 * Functions:
 *   0x0016efb0 AddClipPoint
 *   0x0016f1e0 ClipPolygon
 *   0x0016f728 ClipDrawTriangle
 *   0x0016fc50 TestCurrZN
 *   0x0016fc60 TestCurrLeft
 *   0x0016fc70 TestCurrRight
 *   0x0016fc80 TestCurrTop
 *   0x0016fc90 TestCurrBottom
 *   0x0016fca0 TestPrevZN
 *   0x0016fcb0 TestPrevLeft
 *   0x0016fcc0 TestPrevRight
 *   0x0016fcd0 TestPrevTop
 *   0x0016fce0 TestPrevBottom
 *   0x0016fcf0 IntersectZN
 *   0x0016fd28 IntersectLeft
 *   0x0016fd58 IntersectRight
 *   0x0016fd80 IntersectTop
 *   0x0016fdb0 IntersectBottom
 */

/*
 * Outcodes of the clip-space vertex currently being fed to the clipper.
 * ClipPolygon shifts the word left by CLIP_PREV_SHIFT before testing the
 * next vertex, so the previous vertex's outcodes live in the high half.
 */
#define CLIP_RIGHT      0x01
#define CLIP_LEFT       0x02
#define CLIP_TOP        0x04
#define CLIP_BOTTOM     0x08
#define CLIP_ZFAR       0x10
#define CLIP_ZNEAR      0x20
#define CLIP_PREV_SHIFT 6
#define CLIP_PREV(bit)  ((bit) << CLIP_PREV_SHIFT)
#define CLIP_ALL3(bit)  ((bit) | CLIP_PREV(bit) | CLIP_PREV(CLIP_PREV(bit)))

typedef struct ClipVec4 {
    float x;
    float y;
    float z;
    float w;
} ClipVec4;

typedef struct ClipPoint {
    ClipVec4 pos;
    ClipVec4 colour;
    ClipVec4 tex;
    ClipVec4 clip;
} ClipPoint;

typedef int (*ClipTestFn)(void);
typedef float (*ClipIntersectFn)(ClipPoint *curr, ClipPoint *prev);

ClipPoint pnts[2][8] = {0};
extern int npnts[2];
extern int inbuffer;
extern int clipflags;

int TestCurrZN(void);
int TestCurrLeft(void);
int TestCurrRight(void);
int TestCurrTop(void);
int TestCurrBottom(void);
int TestPrevZN(void);
int TestPrevLeft(void);
int TestPrevRight(void);
int TestPrevTop(void);
int TestPrevBottom(void);
float IntersectZN(ClipPoint *curr, ClipPoint *prev);
float IntersectLeft(ClipPoint *curr, ClipPoint *prev);
float IntersectRight(ClipPoint *curr, ClipPoint *prev);
float IntersectTop(ClipPoint *curr, ClipPoint *prev);
float IntersectBottom(ClipPoint *curr, ClipPoint *prev);


static inline void LerpClipVec4(ClipVec4 *dst, ClipVec4 *curr, ClipVec4 *prev, float t)
{
    dst->x = prev->x + (curr->x - prev->x) * t;
    dst->y = prev->y + (curr->y - prev->y) * t;
    dst->z = prev->z + (curr->z - prev->z) * t;
    dst->w = prev->w + (curr->w - prev->w) * t;
}


void AddClipPoint(int curr, int prev, ClipIntersectFn intersect)
{
    ClipPoint *a = &pnts[inbuffer][curr];
    ClipPoint *b = &pnts[inbuffer][prev];
    int out = 1 - inbuffer;
    ClipPoint *dst = &pnts[out][npnts[out]];
    float t = intersect(a, b);

    LerpClipVec4(&dst->pos, &a->pos, &b->pos, t);
    LerpClipVec4(&dst->tex, &a->tex, &b->tex, t);
    LerpClipVec4(&dst->colour, &a->colour, &b->colour, t);
    LerpClipVec4(&dst->clip, &a->clip, &b->clip, t);
}


static inline void SetClipFlags(int i)
{
    ClipVec4 v = pnts[inbuffer][i].clip;

    clipflags <<= CLIP_PREV_SHIFT;
    if (v.x > v.w) {
        clipflags |= CLIP_RIGHT;
    }
    if (v.x < -v.w) {
        clipflags |= CLIP_LEFT;
    }
    if (v.y > v.w) {
        clipflags |= CLIP_TOP;
    }
    if (v.y < -v.w) {
        clipflags |= CLIP_BOTTOM;
    }
    if (v.z > v.w) {
        clipflags |= CLIP_ZFAR;
    }
    if (v.z < -v.w) {
        clipflags |= CLIP_ZNEAR;
    }
}


static inline void KeepClipPoint(int i)
{
    int out = 1 - inbuffer;

    pnts[out][npnts[out]] = pnts[inbuffer][i];
    npnts[out]++;
}


void ClipPolygon(ClipTestFn currtest, ClipTestFn prevtest, ClipIntersectFn intersect)
{
    int i;

    npnts[1 - inbuffer] = 0;
    SetClipFlags(0);
    if (!currtest()) {
        KeepClipPoint(0);
    }

    for (i = 1; i < npnts[inbuffer];) {
        SetClipFlags(i);
        if (currtest() != prevtest()) {
            AddClipPoint(i, i - 1, intersect);
        }
        if (!currtest()) {
            KeepClipPoint(i);
        }
    }

    SetClipFlags(0);
    if (currtest() != prevtest()) {
        AddClipPoint(0, i - 1, intersect);
    }
    inbuffer = 1 - inbuffer;
}


void ClipDrawTriangle(ClipPoint *p0, ClipPoint *p1, ClipPoint *p2)
{
    pnts[inbuffer][0] = *p0;
    pnts[inbuffer][1] = *p1;
    pnts[inbuffer][2] = *p2;
    npnts[inbuffer] = 3;

    SetClipFlags(0);
    SetClipFlags(1);
    SetClipFlags(2);

    if ((clipflags & CLIP_ALL3(CLIP_ZNEAR)) != CLIP_ALL3(CLIP_ZNEAR) &&
        (clipflags & CLIP_ALL3(CLIP_BOTTOM)) != CLIP_ALL3(CLIP_BOTTOM) &&
        (clipflags & CLIP_ALL3(CLIP_TOP)) != CLIP_ALL3(CLIP_TOP) &&
        (clipflags & CLIP_ALL3(CLIP_LEFT)) != CLIP_ALL3(CLIP_LEFT) &&
        (clipflags & CLIP_ALL3(CLIP_RIGHT)) != CLIP_ALL3(CLIP_RIGHT)) {
        ClipPolygon(TestCurrZN, TestPrevZN, IntersectZN);
        ClipPolygon(TestCurrLeft, TestPrevLeft, IntersectLeft);
        ClipPolygon(TestCurrRight, TestPrevRight, IntersectRight);
        ClipPolygon(TestCurrTop, TestPrevTop, IntersectTop);
        ClipPolygon(TestCurrBottom, TestPrevBottom, IntersectBottom);
    }
    npnts[inbuffer] = 0;
}


int TestCurrZN(void)
{
    return clipflags & CLIP_ZNEAR;
}


int TestCurrLeft(void)
{
    return clipflags & CLIP_LEFT;
}


int TestCurrRight(void)
{
    return clipflags & CLIP_RIGHT;
}


int TestCurrTop(void)
{
    return clipflags & CLIP_TOP;
}


int TestCurrBottom(void)
{
    return clipflags & CLIP_BOTTOM;
}


int TestPrevZN(void)
{
    return clipflags & CLIP_PREV(CLIP_ZNEAR);
}


int TestPrevLeft(void)
{
    return clipflags & CLIP_PREV(CLIP_LEFT);
}


int TestPrevRight(void)
{
    return clipflags & CLIP_PREV(CLIP_RIGHT);
}


int TestPrevTop(void)
{
    return clipflags & CLIP_PREV(CLIP_TOP);
}


int TestPrevBottom(void)
{
    return clipflags & CLIP_PREV(CLIP_BOTTOM);
}


float IntersectZN(ClipPoint *curr, ClipPoint *prev)
{
    ClipVec4 *a = &curr->clip;
    ClipVec4 *b = &prev->clip;

    return (-b->z - b->w) / (((a->z - b->z) + a->w) - b->w);
}


float IntersectLeft(ClipPoint *curr, ClipPoint *prev)
{
    return (-prev->clip.x - prev->clip.w) /
           (((curr->clip.x - prev->clip.x) + curr->clip.w) - prev->clip.w);
}


float IntersectRight(ClipPoint *curr, ClipPoint *prev)
{
    return (prev->clip.w - prev->clip.x) /
           ((curr->clip.x - prev->clip.x) - (curr->clip.w - prev->clip.w));
}


float IntersectTop(ClipPoint *curr, ClipPoint *prev)
{
    ClipVec4 *a = &curr->clip;
    ClipVec4 *b = &prev->clip;

    return (b->w - b->y) / ((a->y - b->y) - (a->w - b->w));
}


float IntersectBottom(ClipPoint *curr, ClipPoint *prev)
{
    ClipVec4 *a = &curr->clip;
    ClipVec4 *b = &prev->clip;

    return (-b->y - b->w) / (((a->y - b->y) + a->w) - b->w);
}
