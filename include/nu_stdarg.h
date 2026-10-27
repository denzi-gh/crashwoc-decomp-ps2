#ifndef NU_STDARG_H
#define NU_STDARG_H

#ifdef NW_HOST

#include <stdarg.h>

#else

/* gcc 2.95 va-mips.h (EABI): va_start backs up over the unused named-argument
 * slots. Slots are 8 bytes; va_arg handles integer arguments only. */
typedef char *va_list;

#define va_start(ap, last) \
    ((ap) = ((va_list)__builtin_next_arg(last) \
        - (__builtin_args_info(2) < 8 ? (8 - __builtin_args_info(2)) * 8 : 0)))
#define va_arg(ap, type) (*(type *)(((ap) += 8) - 8))
#define va_end(ap) ((ap) = (va_list)0)

#endif

#endif /* NU_STDARG_H */
