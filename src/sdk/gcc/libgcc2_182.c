/*
 * Unit: sdk/gcc/libgcc2_182
 *
 * Functions:
 *   0x0026d520 __do_global_dtors
 *   0x0026d578 __do_global_ctors
 *   0x0026d628 __main
 */


typedef void (*func_ptr)(void);

extern func_ptr __CTOR_LIST__[];
extern func_ptr __DTOR_LIST__[];
extern int D_00702A40;

#define initialized D_00702A40


void __do_global_dtors(void) {
    static func_ptr *p = __DTOR_LIST__ + 1;

    while (*p) {
        p++;
        (*(p - 1))();
    }
}


void __do_global_ctors(void) {
    do {
        unsigned long nptrs = (unsigned long)__CTOR_LIST__[0];
        unsigned int i;

        if (nptrs == (unsigned long)-1) {
            for (nptrs = 0; __CTOR_LIST__[nptrs + 1] != 0; nptrs++) {
            }
        }
        for (i = nptrs; i >= 1; i--) {
            __CTOR_LIST__[i]();
        }
    } while (0);
}


void __main(void) {
    if (!initialized) {
        initialized = 1;
        __do_global_ctors();
    }
}
