/*
 * Unit: sdk/gcc/libgcc2_264
 *
 * Functions:
 *   0x00280e60 __fixunsdfdi
 */


typedef int SItype __attribute__((mode(SI)));
typedef unsigned int USItype __attribute__((mode(SI)));
typedef int DItype __attribute__((mode(DI)));
typedef unsigned int UDItype __attribute__((mode(DI)));
typedef float DFtype __attribute__((mode(DF)));

#define BITS_PER_UNIT   8
#define WORD_SIZE       (sizeof(SItype) * BITS_PER_UNIT)
#define HIGH_WORD_COEFF (((UDItype)1) << WORD_SIZE)

DFtype __floatdidf(DItype u);


DItype __fixunsdfdi(DFtype a) {
    DFtype b;
    UDItype v;

    if (a < 0) {
        return 0;
    }
    b = (a / HIGH_WORD_COEFF);
    v = (USItype)b;
    v <<= WORD_SIZE;
    a -= (DFtype)v;
    if (a < 0) {
        v -= (USItype)(-a);
    } else {
        v += (USItype)a;
    }
    return v;
}
