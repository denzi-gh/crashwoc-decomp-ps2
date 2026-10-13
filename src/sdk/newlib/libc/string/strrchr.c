/*
 * Unit: sdk/newlib/libc/string/strrchr
 *
 * Functions:
 *   0x002721c8 strrchr
 */

char *strrchr(const char *s, int c)
{
    char *last = 0;
    char ch = c;

    while (*s) {
        if (*s == ch) {
            last = (char *)s;
        }
        s++;
    }
    if (*s == ch) {
        return (char *)s;
    }
    return last;
}
