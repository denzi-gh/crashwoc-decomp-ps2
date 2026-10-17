/*
 * Unit: nucore/nustring
 *
 * Functions:
 *   0x00105a90 NuStrCat
 *   0x00105ad0 NuStrCpy
 *   0x00105af0 NuStrLen
 *   0x00105b18 NuStrStr
 *   0x00105b98 NuStrIStr
 */

#define UPPERCASE(c) (((c) >= 'a' && (c) <= 'z') ? (c) - 32 : (c))

void NuStrCat(char *destination, char *source)
{
    while (*destination) {
        destination++;
    }
    do {
        *destination++ = *source;
    } while (*source++);
}


void NuStrCpy(char *destination, char *source)
{
    do {
        *destination++ = *source;
    } while (*source++);
}


int NuStrLen(char *text)
{
    int length = 0;

    while (*text) {
        text++;
        length++;
    }
    return length;
}


char *NuStrStr(char *text, char *pattern)
{
    while (*text) {
        char *match = pattern;
        char *cursor = text;

        while (*match && *cursor && *cursor == *match) {
            cursor++;
            match++;
        }
        if (!*match) {
            return text;
        }
        text++;
    }
    return 0;
}


char *NuStrIStr(char *text, char *pattern)
{
    while (*text) {
        char *match = pattern;
        char *cursor = text;

        while (*match && *cursor) {
            if (UPPERCASE(*cursor) != UPPERCASE(*match)) {
                break;
            }
            match++;
            cursor++;
        }
        if (!*match) {
            return text;
        }
        text++;
    }
    return 0;
}
