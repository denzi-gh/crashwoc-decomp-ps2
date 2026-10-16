/*
 * Unit: sdk/gcc/libgcc2_185
 *
 * Functions:
 *   0x0026dd38 __floatdisf
 */


typedef int SItype __attribute__((mode(SI)));
typedef unsigned int USItype __attribute__((mode(SI)));
typedef int DItype __attribute__((mode(DI)));
typedef unsigned int UDItype __attribute__((mode(DI)));
typedef float SFtype __attribute__((mode(SF)));
typedef float DFtype __attribute__((mode(DF)));

#define BITS_PER_UNIT       8
#define WORD_SIZE           (sizeof(SItype) * BITS_PER_UNIT)
#define HIGH_HALFWORD_COEFF (((UDItype)1) << (WORD_SIZE / 2))
#define HIGH_WORD_COEFF     (((UDItype)1) << WORD_SIZE)
#define DI_SIZE             (sizeof(DItype) * BITS_PER_UNIT)
#define DF_SIZE             53
#define SF_SIZE             24
#define REP_BIT             ((UDItype)1 << (DI_SIZE - DF_SIZE))


SFtype __floatdisf(DItype u) {
    DFtype f;

    if (DF_SIZE < DI_SIZE && DF_SIZE > (DI_SIZE - DF_SIZE + SF_SIZE)) {
        if (!(-((DItype)1 << DF_SIZE) < u && u < ((DItype)1 << DF_SIZE))) {
            if ((USItype)u & (REP_BIT - 1)) {
                u |= REP_BIT;
            }
        }
    }
    f = (SItype)(u >> WORD_SIZE);
    f *= HIGH_HALFWORD_COEFF;
    f *= HIGH_HALFWORD_COEFF;
    f += (USItype)(u & (HIGH_WORD_COEFF - 1));
    return (SFtype)f;
}
