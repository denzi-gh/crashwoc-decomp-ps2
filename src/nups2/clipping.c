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

extern int clipflags;


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
