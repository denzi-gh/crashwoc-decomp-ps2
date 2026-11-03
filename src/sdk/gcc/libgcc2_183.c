/*
 * Unit: sdk/gcc/libgcc2_183
 *
 * Data only (no functions): libgcc2's L_ctors member, the default empty
 * constructor/destructor lists (uninitialised, so the linker's set
 * extension can supply them).
 */

typedef void (*func_ptr)(void);

func_ptr __CTOR_LIST__[2];
func_ptr __DTOR_LIST__[2];
