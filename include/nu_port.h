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

#endif /* NU_PORT_H */
