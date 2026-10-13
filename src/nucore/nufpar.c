/*
 * Unit: nucore/nufpar
 *
 * Functions:
 *   0x00103978 NuFParGetLine
 *   0x00103ca0 NuFParGetInt
 *   0x00103da8 NuFParGetFloat
 *   0x00103ec0 NuFParGetPos
 *   0x00103ef0 NuFParSetPos
 *   0x00103f30 NuFParPushCom
 *   0x00103f60 NuFParPopCom
 *   0x00103f78 NuFParInterpretWord
 *   0x00104048 NuFParUnGetWord
 *   0x00104058 NuFParGetWord
 *   0x00104140 NuFParClose
 *   0x00104168 NuFParOpen
 *   0x001041e0 NuFParDestroy
 *   0x00104220 NuFParCreate
 *   0x001042b8 NuGetChar
 */

typedef struct NuFPar NuFPar;

typedef struct NuFParCommand {
    char *name;
    void (*handler)(NuFPar *parser);
} NuFParCommand;

struct NuFPar {
    int handle;
    char buffer[0x1000];
    char line[0x101];
    char word[0x103];
    int line_number;
    int word_offset;
    int position;
    int buffer_start;
    int buffer_end;
    NuFParCommand *commands[8];
    int command_index;
    int size;
};

typedef struct NuFParPos {
    int line_number;
    int word_offset;
    int position;
    int buffer_start;
    int buffer_end;
} NuFParPos;

/* The target names this word as _fbss + 0x8. */
extern int D_00633008;
extern char D_00613978[];

#define nufpar_word_start D_00633008
#define nufpar_srcfile    D_00613978

extern int NuFileOpen(char *name, int mode);
extern int NuFileRead(int handle, void *data, int size);
extern int NuFileOpenSize(int handle);
extern void NuFileClose(int handle);
extern void *NuMemAllocFn(int size, char *file, int line);
extern void NuMemFreeFn(void *ptr, char *file, int line);
extern void *memset(void *dst, int c, unsigned int n);
extern int strcasecmp(const char *a, const char *b);
extern int atoi(const char *s);
extern double atof(const char *s);


inline char NuGetChar(NuFPar *parser)
{
    int start;
    int count;
    int read;
    char c;

    start = parser->buffer_end;
    if (start < 0) {
        start = 0;
    }
    if (parser->position > parser->buffer_end) {
        if (parser->buffer_end + 1 >= parser->size) {
            return 0;
        }
        count = parser->size - start;
        if (count > 0x1000) {
            count = 0x1000;
        }
        read = NuFileRead(parser->handle, parser->buffer, count);
        parser->buffer_start = parser->buffer_end + 1;
        parser->buffer_end += read;
        if (read == 0) {
            return 0;
        }
    }
    c = parser->buffer[parser->position - parser->buffer_start];
    parser->position++;
    return c;
}


int NuFParGetLine(NuFPar *parser)
{
    int length;
    int done;
    char c;

    length = 0;
    parser->word_offset = 0;
    parser->line_number++;
    for (;;) {
        c = NuGetChar(parser);
        switch (c) {
        case '\r':
            NuGetChar(parser);
        case '\n':
            if (length == 0) {
                parser->line_number++;
                break;
            }
        case 0:
            parser->line[length] = 0;
            return length;
        case ';':
            if (length == 0) {
                done = 0;
                do {
                    c = NuGetChar(parser);
                    switch (c) {
                    case '\r':
                        NuGetChar(parser);
                    case '\n':
                    case 0:
                        done = 1;
                        break;
                    }
                } while (!done);
                length = 0;
                parser->word_offset = 0;
                parser->line_number++;
                break;
            }
        default:
            parser->line[length & 0xFF] = c;
            length++;
            break;
        }
    }
}


inline int NuFParGetWord(NuFPar *parser)
{
    int length;
    int quoted;
    char c;

    length = 0;
    quoted = 0;
    nufpar_word_start = parser->word_offset;
    while (parser->line[parser->word_offset & 0xFF] != 0) {
        c = parser->line[parser->word_offset];
        switch (c) {
        case ' ':
        case '\t':
        case ',':
            if (!quoted) {
                if (length != 0) {
                    parser->word[length & 0xFF] = 0;
                    return length;
                }
                break;
            }
        default:
            if (c == '"') {
                quoted = 1 - quoted;
            } else {
                parser->word[length & 0xFF] = c;
                length++;
            }
            break;
        }
        parser->word_offset++;
    }
    parser->word[length & 0xFF] = 0;
    return length;
}


int NuFParGetInt(NuFPar *parser)
{
    NuFParGetWord(parser);
    if (parser->word[0] != 0) {
        return atoi(parser->word);
    }
    return 0;
}


float NuFParGetFloat(NuFPar *parser)
{
    NuFParGetWord(parser);
    if (parser->word[0] != 0) {
        return atof(parser->word);
    }
    return 0.0f;
}


void NuFParGetPos(NuFPar *parser, NuFParPos *pos)
{
    pos->line_number = parser->line_number;
    pos->word_offset = parser->word_offset;
    pos->position = parser->position;
    pos->buffer_start = parser->buffer_start;
    pos->buffer_end = parser->buffer_end;
}


void NuFParSetPos(NuFPar *parser, NuFParPos *pos)
{
    parser->position = pos->position;
    parser->word_offset = pos->word_offset;
    if (parser->position < parser->buffer_start ||
        parser->position > parser->buffer_end) {
        parser->buffer_end = parser->position - 1;
    }
}


int NuFParPushCom(NuFPar *parser, NuFParCommand *command)
{
    int result;

    if (parser->command_index < 7) {
        parser->command_index++;
        parser->commands[parser->command_index] = command;
        result = parser->command_index;
    } else {
        result = -1;
    }
    return result;
}


void NuFParPopCom(NuFPar *parser)
{
    if (parser->command_index >= 0) {
        parser->command_index--;
    }
}


int NuFParInterpretWord(NuFPar *parser)
{
    int i;

    if (parser->command_index >= 0) {
        for (i = 0; parser->commands[parser->command_index][i].name != 0; i++) {
            if (strcasecmp(parser->commands[parser->command_index][i].name,
                           parser->word) == 0) {
                parser->commands[parser->command_index][i].handler(parser);
                return 1;
            }
        }
    }
    return 0;
}


void NuFParUnGetWord(NuFPar *parser)
{
    parser->word_offset = nufpar_word_start;
}


inline void NuFParClose(NuFPar *parser)
{
    NuMemFreeFn(parser, nufpar_srcfile, 0x130);
}


inline NuFPar *NuFParOpen(int handle)
{
    NuFPar *parser;

    parser = NuMemAllocFn(sizeof(NuFPar), nufpar_srcfile, 0x13E);
    if (parser) {
        memset(parser, 0, sizeof(NuFPar));
        parser->handle = handle;
        parser->buffer_end = -1;
        parser->line_number = -1;
        parser->command_index = -1;
        parser->size = NuFileOpenSize(handle);
    }
    return parser;
}


void NuFParDestroy(NuFPar *parser)
{
    int handle;

    handle = parser->handle;
    NuFParClose(parser);
    NuFileClose(handle);
}


NuFPar *NuFParCreate(char *name)
{
    int handle;
    NuFPar *parser;

    handle = NuFileOpen(name, 0);
    if (handle) {
        parser = NuFParOpen(handle);
        if (parser) {
            return parser;
        }
        NuFileClose(handle);
    }
    return 0;
}
