/*
 * Unit: sdk/newlib/libc/string/strstr
 *
 * Functions:
 *   0x00272218 strstr
 */

char *strstr(const char *searchee, const char *lookfor) {
    if (*searchee == 0) {
        if (*lookfor) {
            return 0;
        }
        return (char *)searchee;
    }

    while (*searchee) {
        unsigned int i = 0;

        while (1) {
            if (lookfor[i] == 0) {
                return (char *)searchee;
            }
            if (lookfor[i] != searchee[i]) {
                break;
            }
            i++;
        }
        searchee++;
    }

    return 0;
}
