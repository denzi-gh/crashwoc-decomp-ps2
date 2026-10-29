#ifndef NU_PORT_H
#define NU_PORT_H

/* Store a T at p / read the T at p, then step p past it. The EE spelling is gcc's
 * cast-as-lvalue extension; ISO C has none, so the host steps p separately. */
#ifdef NW_HOST
#define NW_PUT(T, p, v) (*(T *)(p) = (v), (p) = (void *)((T *)(p) + 1))
#define NW_GET(T, p) ((p) = (void *)((T *)(p) + 1), ((T *)(p))[-1])
#else
#define NW_PUT(T, p, v) *((T *)p)++ = v
#define NW_GET(T, p) *((T *)p)++
#endif

/* Ends a retail function that falls off the end of a non-void body: nothing on the
 * EE (callers ignore the value), a defined return value on the host. */
#ifdef NW_HOST
#define NW_HOST_RETURN(v) return v
#else
#define NW_HOST_RETURN(v)
#endif

/* Alignment only the host needs (a header before 16-byte aligned data): the EE layout stays. */
#ifdef NW_HOST
#define NW_HOST_ALIGNED(n) __attribute__((aligned(n)))
#else
#define NW_HOST_ALIGNED(n)
#endif

/* A file-scope static the port's replacement of its accessors must reach: static on the EE, global on
 * the host. */
#ifdef NW_HOST
#define NW_STATIC
#else
#define NW_STATIC static
#endif


#ifdef NW_HOST
void nw_ee_idle(void);
#define NW_HOST_IDLE() nw_ee_idle()
#else
#define NW_HOST_IDLE()
#endif

/* The n records x the code above carved off a game heap, its cursor now past them; ps2size is one record's PS2
 * size, which the EE build checks. The host (NuWorld's src/port/heap.c) puts the cursor back past their PS2 size,
 * so the heap keeps retail's layout, and points x at zeroed host records beside the heap. */
#ifdef NW_HOST
void *nw_heap_twin(void *at, __SIZE_TYPE__ charge, __SIZE_TYPE__ size, __SIZE_TYPE__ align);
#define NW_HEAP_TWIN(x, cursor, n, ps2size) \
    ((cursor) = (void *)((char *)(x) + (__SIZE_TYPE__)(n) * (ps2size)), \
     (x) = nw_heap_twin((x), (__SIZE_TYPE__)(n) * (ps2size), (__SIZE_TYPE__)(n) * sizeof(*(x)), __alignof__(*(x))))
#else
#define NW_HEAP_TWIN(x, cursor, n, ps2size) ((void)sizeof(char[sizeof(*(x)) == (ps2size) ? 1 : -1]))
#endif

/* What a load returns of the record it carved at the heap address at: at on the EE, its twin rec on the host. */
#ifdef NW_HOST
#define NW_HEAP_REC(at, rec) ((__typeof__(at))(rec))
#else
#define NW_HEAP_REC(at, rec) at
#endif

/* A pointer p the caller aimed at the heap address at, where the code above carved a record: on the host it aims at
 * the record's twin rec instead. */
#ifdef NW_HOST
#define NW_HEAP_AIM(p, at, rec) ((p) == (void *)(at) ? (void)((p) = (rec)) : (void)0)
#else
#define NW_HEAP_AIM(p, at, rec)
#endif

/* main.c's superbuffer. The loads inside it align to 0x80, so on the host nw_superbuffer_alloc (NuWorld's
 * src/port/heap.c) starts it at retail's address modulo the alignment, keeping retail's layout. */
#ifdef NW_HOST
void *nw_superbuffer_alloc(int size, char *file, int line);
#define NW_SUPERBUFFER_ALLOC(size, file, line) nw_superbuffer_alloc(size, file, line)
#else
#define NW_SUPERBUFFER_ALLOC(size, file, line) NuMemAllocFn(size, file, line)
#endif

/* A read retail makes outside its object (a faithful bug): what retail's layout puts there, which the host reads
 * from where its own layout keeps the same bytes. */
#ifdef NW_HOST
#define NW_RETAIL(ps2, host) (host)
#else
#define NW_RETAIL(ps2, host) (ps2)
#endif

/* Element i of the array obj->member, which may be past its end: the struct's next members, the same on the host.
 * The host indexes from obj, so the read stays inside the object it is in. */
#ifdef NW_HOST
#define NW_PAST(obj, member, i) \
    (((__typeof__(&(obj)->member[0]))((char *)(obj) + __builtin_offsetof(__typeof__(*(obj)), member)))[i])
#else
#define NW_PAST(obj, member, i) ((obj)->member[i])
#endif

/* a + dt / range * (b - a) in the EE's float: a range of 0 (two keys at one time) gives its largest value where IEEE
 * gives an inf or a NaN (NuWorld's src/hle/eefloat.c). */
#ifdef NW_HOST
float nw_ee_lerp(float a, float dt, float range, float b);
#define NW_EE_LERP(a, dt, range, b) nw_ee_lerp(a, dt, range, b)
#else
#define NW_EE_LERP(a, dt, range, b) ((a) + (dt) / (range) * ((b) - (a)))
#endif

/* (int)x as the EE's cvt.w.s: saturating where C's conversion is undefined. */
#ifdef NW_HOST
int nw_ee_s32(float x);
#define NW_EE_S32(x) nw_ee_s32(x)
#else
#define NW_EE_S32(x) ((int)(x))
#endif

/* a / b as the EE's div.s: by zero its largest value with the operands' signs combined, where IEEE gives an inf or
 * a NaN (NuWorld's src/hle/eefloat.c). */
#ifdef NW_HOST
float nw_ee_fdiv(float a, float b);
#define NW_EE_DIV(a, b) nw_ee_fdiv(a, b)
#else
#define NW_EE_DIV(a, b) ((a) / (b))
#endif

/* A function that calls through pointers to functions retail defined with other prototypes (fewer parameters,
 * another return type), which both ABIs take: the host's -fsanitize=function leaves its calls alone. */
#ifdef NW_HOST
#define NW_HOST_LOOSE_CALLS __attribute__((no_sanitize("function")))
#else
#define NW_HOST_LOOSE_CALLS
#endif

/* A local whose address retail hands out past the function's return (a looping sound's position): the EE thread's
 * stack keeps its last values there, the host keeps the local in static storage. */
#ifdef NW_HOST
#define NW_HOST_KEPT static
#else
#define NW_HOST_KEPT
#endif

/* An initializer only the host has, for a variable retail reads before it is set: what retail finds there. */
#ifdef NW_HOST
#define NW_HOST_INIT(v) = (v)
#else
#define NW_HOST_INIT(v)
#endif

/* Hor+ widescreen (NuWorld's src/port/wide.c): the host's view is nw_wide.k times retail's 4:3 width. Drawing widens
 * with it (frame, camera, HUD, draw radii, PVS); logic keeps retail's 4:3 tests. k = 1 is retail.
 * NW_WIDE: the host's value instead of retail's. NW_WIDE_SCALE: x times the host's factor f.
 * NW_WIDE_ONLY: a statement only the host runs (no top-level commas).
 * NW_WIDE_PVS: an instance the PVS hid, drawn on the host when the wider view needs it.
 * NW_WIDE_ANIM: the same for its animation, which the host keeps on retail's test where it carries collision.
 * NW_WIDE_SEEN: what a draw returned, narrowed to retail's 4:3 frustum where logic reads it.
 * nw_wide_vp: the game viewport's origin, which the pause screen's shrink (paused 0-25) takes to retail's 4:3 area;
 * nw_wide_vp_x: its offset from the frame's left edge, in 1/16 pixels. */
#ifdef NW_HOST
struct nuinstance_s;
struct nw_wide_s {
    float k;       /* the view's width over retail's 4:3 */
    float aspect;  /* the camera's aspect, h/w */
    int width;     /* the GS frame's width, 640 at 4:3 */
    float hud;     /* the HUD's x reach over retail's */
    float draw;    /* draw radii over retail's */
    float draw2;   /* its square, for squared radii */
    int pvs_all;   /* every instance the PVS hid is drawn */
};
extern struct nw_wide_s nw_wide;
int nw_wide_anim(struct nuinstance_s *inst, int test);
void nw_wide_mark(void);
int nw_wide_seen(int drawn);
void nw_wide_vp(int paused);
int nw_wide_vp_x(void);
#define NW_WIDE(ps2, host) (host)
#define NW_WIDE_SCALE(x, f) ((x) * (f))
#define NW_WIDE_ONLY(stmt) stmt
#define NW_WIDE_PVS(test) ((test) || nw_wide.pvs_all)
#define NW_WIDE_ANIM(inst, test) nw_wide_anim(inst, test)
#define NW_WIDE_SEEN(drawn) (nw_wide_mark(), nw_wide_seen(drawn))
#else
#define NW_WIDE(ps2, host) (ps2)
#define NW_WIDE_SCALE(x, f) (x)
#define NW_WIDE_ONLY(stmt)
#define NW_WIDE_PVS(test) (test)
#define NW_WIDE_ANIM(inst, test) (test)
#define NW_WIDE_SEEN(drawn) (drawn)
#endif

/* Display-rate interpolation (NuWorld's src/gfx/interp.c), which blends each draw from the frame before by a key.
 * NW_INTERP_KEY: the object the draws after it belong to (0: none), where a draw that skips an object would shift
 * the keys of the ones after it. NW_INTERP_SNAP: the frame shows a cut, drawn without blending. */
#ifdef NW_HOST
void nw_interp_key(const void *key);
void nw_interp_snap(void);
#define NW_INTERP_KEY(p) nw_interp_key(p)
#define NW_INTERP_SNAP() nw_interp_snap()
#else
#define NW_INTERP_KEY(p)
#define NW_INTERP_SNAP()
#endif

/* 50/60 Hz (NuWorld's src/port/hz.c): game logic on PAL's 50 Hz tick or NTSC's 60 Hz one. NW_HZ: a value tied to
 * the tick, PAL's on the PS2, the one of nw_hz's rate on the host. NW_HZ_NTSC builds the PS2 code with the NTSC
 * values, which NuWorld's tools/ntsc_diff.py compares with the US release. */
#ifdef NW_HOST
extern int nw_hz;
#define NW_HZ(pal, ntsc) (nw_hz == 60 ? (ntsc) : (pal))
#elif defined(NW_HZ_NTSC)
#define NW_HZ(pal, ntsc) (ntsc)
#else
#define NW_HZ(pal, ntsc) (pal)
#endif

/* The title's attract demos replay .ppd recordings, one pad state per 50 Hz tick. NW_DEMOS: whether the title may
 * start one, always on the PS2. */
#ifdef NW_HOST
int nw_demos(void);
#define NW_DEMOS() nw_demos()
#else
#define NW_DEMOS() 1
#endif

#endif /* NU_PORT_H */
