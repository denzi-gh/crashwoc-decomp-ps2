/*
 * Unit: sdk/gcc/libgcc2_263
 *
 * Functions:
 *   0x00280e00 __fixdfdi
 */


typedef int DItype __attribute__((mode(DI)));
typedef float DFtype __attribute__((mode(DF)));

DItype __fixunsdfdi(DFtype a);


DItype __fixdfdi(DFtype a) {
    if (a < 0) {
        return -__fixunsdfdi(-a);
    }
    return __fixunsdfdi(a);
}
