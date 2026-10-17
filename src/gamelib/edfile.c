/*
 * Unit: gamelib/edfile
 *
 * Functions:
 *   0x001a7500 EdFileOpen
 *   0x001a7710 EdFileClose
 *   0x001a7810 EdFileWriteFloat
 *   0x001a7920 EdFileWriteInt
 *   0x001a7a30 EdFileWriteUnsignedInt
 *   0x001a7b40 EdFileWriteShort
 *   0x001a7c50 EdFileWriteChar
 *   0x001a7d60 EdFileReadFloat
 *   0x001a7e70 EdFileReadInt
 *   0x001a7f80 EdFileReadUnsignedInt
 *   0x001a8090 EdFileReadShort
 *   0x001a81a0 EdFileReadChar
 *   0x001a82b0 EdFileSetMedia
 *   0x001a82b8 EdFileWrite
 *   0x001a83c0 EdFileRead
 *   0x001a84c8 EdFileWriteMemCard
 *   0x001a85f8 EdFileReadMemCard
 *   0x001a8728 EdFileResetBuffers
 *   0x001a8760 EdFileFillBuffer
 *   0x001a87d8 EdFileFlushBuffer
 */


#include "nu_libc.h"

#include "nucore/nufile.h"

#define EDFILE_BUFFER_SIZE 0x1000

enum edfile_media_e {
    EDFILE_MEDIA_NONE = 0,
    EDFILE_MEDIA_HOST = 1,
    EDFILE_MEDIA_MEMCARD = 2
};

extern int NuFileOpen(char *file, enum nufilemode_e mode);
extern int NuFileRead(int fh, void *data, int size);
extern int NuFileWrite(int fh, void *data, int size);
extern void NuFileClose(int fh);
extern int NuMcRead(int fd, void *buff, int size, int async);
extern int NuMcWrite(int fd, void *buff, int size, int async);
extern int sceMcOpen(int port, int slot, const char *name, int mode);
extern int sceMcClose(int fd);
extern int sceMcSync(int mode, int *cmd, int *result);

char edfile_buffer[EDFILE_BUFFER_SIZE] = {0};
extern int edfile_handle;
extern int edfile_write_flag;
extern int edfile_mcresult;
extern int edfile_lock[2];
extern int edfile_buffer_pointer;
extern int D_0062F748;
#define edfile_media D_0062F748


static inline void EdFileReadBlock(void) {
    switch (edfile_media) {
    case EDFILE_MEDIA_NONE:
        break;
    case EDFILE_MEDIA_HOST:
        NuFileRead(edfile_handle, edfile_buffer, EDFILE_BUFFER_SIZE);
        edfile_buffer_pointer = 0;
        break;
    case EDFILE_MEDIA_MEMCARD:
        NuMcRead(edfile_handle, edfile_buffer, EDFILE_BUFFER_SIZE, 0);
        edfile_buffer_pointer = 0;
        break;
    }
}


static inline void EdFileWriteBlock(int size) {
    switch (edfile_media) {
    case EDFILE_MEDIA_NONE:
        break;
    case EDFILE_MEDIA_HOST:
        NuFileWrite(edfile_handle, edfile_buffer, size);
        edfile_buffer_pointer = 0;
        break;
    case EDFILE_MEDIA_MEMCARD:
        NuMcWrite(edfile_handle, edfile_buffer, size, 0);
        edfile_buffer_pointer = 0;
        break;
    }
}


static inline void EdFileWriteData(char *data, int size) {
    int count;

    while (size > 0) {
        count = EDFILE_BUFFER_SIZE - edfile_buffer_pointer;
        if (size < EDFILE_BUFFER_SIZE - edfile_buffer_pointer) {
            count = size;
        }
        memcpy(edfile_buffer + edfile_buffer_pointer, data, count);
        size -= count;
        edfile_buffer_pointer += count;
        data += count;
        if (edfile_buffer_pointer == EDFILE_BUFFER_SIZE && edfile_handle != -1) {
            EdFileWriteBlock(EDFILE_BUFFER_SIZE);
        }
    }
}


static inline void EdFileReadData(char *data, int size) {
    int count;

    while (size > 0) {
        count = EDFILE_BUFFER_SIZE - edfile_buffer_pointer;
        if (size < EDFILE_BUFFER_SIZE - edfile_buffer_pointer) {
            count = size;
        }
        memcpy(data, edfile_buffer + edfile_buffer_pointer, count);
        size -= count;
        edfile_buffer_pointer += count;
        data += count;
        if (edfile_buffer_pointer == EDFILE_BUFFER_SIZE && edfile_handle != -1) {
            EdFileReadBlock();
        }
    }
}


int EdFileOpen(char *name, enum nufilemode_e mode) {
    if (edfile_handle == -1) {
        memset(edfile_buffer, 0, EDFILE_BUFFER_SIZE);
        edfile_buffer_pointer = 0;
        edfile_lock[1] = 0;
        edfile_lock[0] = 0;
        edfile_handle = -1;
        edfile_write_flag = 0;
        switch (edfile_media) {
        case EDFILE_MEDIA_NONE:
        default:
            return 0;
        case EDFILE_MEDIA_HOST:
            if (mode != NUFILE_READ) {
                if (mode != NUFILE_WRITE) {
                    return 0;
                }
                edfile_write_flag = 1;
                edfile_handle = NuFileOpen(name, NUFILE_WRITE);
                if (edfile_handle != -1) {
                    return 1;
                }
                return 0;
            }
            edfile_handle = NuFileOpen(name, NUFILE_READ);
            if (edfile_handle == -1) {
                return 0;
            }
            EdFileReadBlock();
            break;
        case EDFILE_MEDIA_MEMCARD:
            if (mode != NUFILE_READ) {
                if (mode != NUFILE_WRITE) {
                    return 0;
                }
                sceMcOpen(0, 0, name, 0x200);
                sceMcSync(0, 0, &edfile_mcresult);
                if (edfile_mcresult < 0) {
                    return 0;
                }
                edfile_handle = edfile_mcresult;
                edfile_write_flag = 1;
                return 1;
            }
            sceMcOpen(0, 0, name, 1);
            sceMcSync(0, 0, &edfile_mcresult);
            if (edfile_mcresult < 0) {
                return 0;
            }
            edfile_handle = edfile_mcresult;
            if (edfile_mcresult != -1) {
                EdFileReadBlock();
            }
            break;
        }
        return 1;
    }
    return 0;
}


int EdFileClose(void) {
    if (edfile_handle != -1) {
        if (edfile_write_flag != 0 && edfile_buffer_pointer != 0) {
            EdFileWriteBlock(edfile_buffer_pointer);
        }
        switch (edfile_media) {
        case EDFILE_MEDIA_NONE:
            break;
        case EDFILE_MEDIA_HOST:
            NuFileClose(edfile_handle);
            edfile_media = EDFILE_MEDIA_NONE;
            edfile_handle = -1;
            return 1;
        case EDFILE_MEDIA_MEMCARD:
            sceMcClose(edfile_handle);
            sceMcSync(0, 0, 0);
            edfile_media = EDFILE_MEDIA_NONE;
            edfile_handle = -1;
            return 1;
        }
    }
    return 0;
}


void EdFileWriteFloat(float value) {
    EdFileWriteData((char *)&value, sizeof(value));
}


void EdFileWriteInt(int value) {
    EdFileWriteData((char *)&value, sizeof(value));
}


void EdFileWriteUnsignedInt(unsigned int value) {
    EdFileWriteData((char *)&value, sizeof(value));
}


void EdFileWriteShort(short value) {
    EdFileWriteData((char *)&value, sizeof(value));
}


void EdFileWriteChar(char value) {
    EdFileWriteData(&value, sizeof(value));
}


float EdFileReadFloat(void) {
    float value;

    EdFileReadData((char *)&value, sizeof(value));
    return value;
}


int EdFileReadInt(void) {
    int value;

    EdFileReadData((char *)&value, sizeof(value));
    return value;
}


unsigned int EdFileReadUnsignedInt(void) {
    unsigned int value;

    EdFileReadData((char *)&value, sizeof(value));
    return value;
}


short EdFileReadShort(void) {
    short value;

    EdFileReadData((char *)&value, sizeof(value));
    return value;
}


char EdFileReadChar(void) {
    char value;

    EdFileReadData(&value, sizeof(value));
    return value;
}


void EdFileSetMedia(int media) {
    edfile_media = media;
}


void EdFileWrite(void *data, int size) {
    EdFileWriteData(data, size);
}


void EdFileRead(void *data, int size) {
    EdFileReadData(data, size);
}


int EdFileWriteMemCard(char *name, int size, void *data) {
    edfile_media = EDFILE_MEDIA_MEMCARD;
    if (EdFileOpen(name, NUFILE_WRITE) == 0) {
        return 0;
    }
    EdFileWriteData(data, size);
    return EdFileClose();
}


int EdFileReadMemCard(char *name, int size, void *data) {
    edfile_media = EDFILE_MEDIA_MEMCARD;
    if (EdFileOpen(name, NUFILE_READ) == 0) {
        return 0;
    }
    EdFileReadData(data, size);
    return EdFileClose();
}


void EdFileResetBuffers(void) {
    memset(edfile_buffer, 0, EDFILE_BUFFER_SIZE);
    edfile_buffer_pointer = 0;
    edfile_lock[1] = 0;
    edfile_lock[0] = 0;
}


void EdFileFillBuffer(void) {
    if (edfile_handle != -1) {
        EdFileReadBlock();
    }
}


void EdFileFlushBuffer(void) {
    if (edfile_handle != -1 && edfile_buffer_pointer != 0) {
        EdFileWriteBlock(edfile_buffer_pointer);
    }
}
