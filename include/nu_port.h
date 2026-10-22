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

#endif /* NU_PORT_H */
