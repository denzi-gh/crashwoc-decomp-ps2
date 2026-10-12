/*
 * Unit: sdk/newlib/libc/string/strncasecmp
 *
 * Functions:
 *   0x00271d70 strncasecmp
 */

typedef unsigned int size_t;

extern const char _ctype_[];

#define _U 01
#define isupper(c) ((_ctype_ + 1)[(int)(c)] & _U)
#define tolower(c) ({ int __x = (c); isupper(__x) ? (__x - 'A' + 'a') : __x; })

int strncasecmp(const char *s1, const char *s2, size_t n)
{
    if (n == 0) {
        return 0;
    }

    while (n-- != 0 && tolower(*s1) == tolower(*s2)) {
        if (n == 0 || *s1 == '\0' || *s2 == '\0') {
            break;
        }
        s1++;
        s2++;
    }

    return tolower(*(unsigned char *)s1) - tolower(*(unsigned char *)s2);
}
