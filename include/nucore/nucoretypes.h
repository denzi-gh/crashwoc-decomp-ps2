#ifndef NUCORE_NUCORETYPES_H
#define NUCORE_NUCORETYPES_H

#include "nu_types.h"

/* Cursor into a load buffer (superbuffer_ptr, a loader's heap): read or
 * advanced as whatever type comes next. */
union variptr_u {
    void *voidptr;
    char *charptr;              /* NuGScnRead takes the cursor as char ** */
    u8 *u8;
    s16 *s16;
    u64 *u64;
    nuintptr intaddr;
};

#endif
