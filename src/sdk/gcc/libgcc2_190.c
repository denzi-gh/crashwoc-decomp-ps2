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


DItype __muldi3(DItype u, DItype v) {
    DIunion w;
    DIunion uu, vv;

    uu.ll = u, vv.ll = v;

    w.ll = (UDItype)(USItype)uu.s.low * (USItype)vv.s.low;
    w.s.high += ((USItype)uu.s.low * (USItype)vv.s.high
                 + (USItype)uu.s.high * (USItype)vv.s.low);

    return w.ll;
}
