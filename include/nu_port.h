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

#endif /* NU_PORT_H */
