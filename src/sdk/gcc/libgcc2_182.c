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
extern func_ptr *D_00612440;
extern int D_00702A40;

#define dtor_cursor D_00612440
#define initialized D_00702A40


void __do_global_dtors(void) {
    while (*dtor_cursor) {
        dtor_cursor++;
        (*(dtor_cursor - 1))();
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
