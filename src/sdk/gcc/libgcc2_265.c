/*
 * Unit: sdk/gcc/libgcc2_265
 *
 * Functions:
 *   0x00280f50 __floatdidf
 */


typedef int SItype __attribute__((mode(SI)));
typedef unsigned int USItype __attribute__((mode(SI)));
typedef int DItype __attribute__((mode(DI)));
typedef unsigned int UDItype __attribute__((mode(DI)));
typedef float DFtype __attribute__((mode(DF)));

#define BITS_PER_UNIT       8
#define WORD_SIZE           (sizeof(SItype) * BITS_PER_UNIT)
#define HIGH_HALFWORD_COEFF (((UDItype)1) << (WORD_SIZE / 2))
#define HIGH_WORD_COEFF     (((UDItype)1) << WORD_SIZE)


DFtype __floatdidf(DItype u) {
    DFtype d;

    d = (SItype)(u >> WORD_SIZE);
    d *= HIGH_HALFWORD_COEFF;
    d *= HIGH_HALFWORD_COEFF;
    d += (USItype)(u & (HIGH_WORD_COEFF - 1));
    return d;
}
