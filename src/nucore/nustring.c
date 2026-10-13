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
            int cursor_character = *cursor;
            int pattern_character = *match;

            if ((unsigned char)(cursor_character - 'a') <= 25) {
                cursor_character -= 32;
            }
            if ((unsigned int)((unsigned char)pattern_character - 'a') <= 25) {
                pattern_character -= 32;
            }
            if (cursor_character != pattern_character) {
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
