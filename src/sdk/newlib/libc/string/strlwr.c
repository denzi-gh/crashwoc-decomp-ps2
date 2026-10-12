/*
 * Unit: sdk/newlib/libc/string/strlwr
 *
 * Functions:
 *   0x00271d18 strlwr
 */

extern const char _ctype_[];

#define _U 01
#define isupper(c) ((_ctype_ + 1)[c] & _U)
#define _tolower(c) ((c) - 'A' + 'a')

char *strlwr(char *a)
{
    char *ret = a;

    while (*a != '\0') {
        int c = *a;

        if (isupper(c)) {
            *a = _tolower(c);
        }
        ++a;
    }
    return ret;
}
