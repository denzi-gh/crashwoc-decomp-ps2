/*
 * Unit: sdk/newlib/libc/string/strcasecmp
 *
 * Functions:
 *   0x002717a8 strcasecmp
 */

extern const char _ctype_[];

#define _U 01
#define isupper(c) ((_ctype_ + 1)[(int)(c)] & _U)
#define tolower(c) ({ int __x = (c); isupper(__x) ? __x - 'A' + 'a' : __x; })

int strcasecmp(const char *s1, const char *s2)
{
    while (*s1 != '\0' && tolower(*s1) == tolower(*s2)) {
        s1++;
        s2++;
    }
    return tolower(*(unsigned char *)s1) - tolower(*(unsigned char *)s2);
}
