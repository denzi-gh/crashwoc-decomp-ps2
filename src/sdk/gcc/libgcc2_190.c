/*
 * Unit: sdk/gcc/libgcc2_190
 *
 * Functions:
 *   0x00270328 __muldi3
 */


typedef int SItype __attribute__((mode(SI)));
typedef unsigned int USItype __attribute__((mode(SI)));
typedef int DItype __attribute__((mode(DI)));
typedef unsigned int UDItype __attribute__((mode(DI)));

struct DIstruct {
    SItype low, high;
};

typedef union {
    struct DIstruct s;
    DItype ll;
} DIunion;

#define umul_ppmm(w1, w0, u, v) \
    __asm__ ("multu %2,%3" \
             : "=l" ((USItype) (w0)), \
               "=h" ((USItype) (w1)) \
             : "d" ((USItype) (u)), \
               "d" ((USItype) (v)))

#define __umulsidi3(u, v) \
    ({ \
        DIunion __w; \
        umul_ppmm(__w.s.high, __w.s.low, u, v); \
        __w.ll; \
    })


DItype __muldi3(DItype u, DItype v) {
    DIunion w;
    DIunion uu, vv;

    uu.ll = u, vv.ll = v;

    w.ll = __umulsidi3(uu.s.low, vv.s.low);
    w.s.high += ((USItype)uu.s.low * (USItype)vv.s.high
                 + (USItype)uu.s.high * (USItype)vv.s.low);

    return w.ll;
}
