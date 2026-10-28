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

#endif /* NU_PORT_H */
