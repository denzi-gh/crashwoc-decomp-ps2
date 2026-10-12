/*
 * Unit: sdk/newlib/libc/stdlib/atof
 *
 * Functions:
 *   0x00270388 atof
 */

extern double strtod(const char *s, char **endptr);

double atof(const char *s)
{
    return strtod(s, 0);
}
