/*
 * Unit: sdk/newlib/libc/stdlib/atoi
 *
 * Functions:
 *   0x002703a8 atoi
 */

long strtol(const char *s, char **ptr, int base);

int atoi(const char *s)
{
    return (int)strtol(s, 0, 10);
}
