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


enum nufilemode_e {
	NUFILE_READ = 0,
	NUFILE_WRITE = 1,
	NUFILE_APPEND = 2,
	NUFILE_READ_NOWAIT = 3,
	NUFILE_MODE_CNT = 4
};

int NuFileOpen(char * file, enum nufilemode_e mode);
int NuFileRead(int fh, void * data, int size);
int NuMcRead(int fd, void * buff, int size, int async);
int NuFileWrite (int fh, void * data, int size);
int NuMcWrite (int fd, void * buff, int size, int async);
void NuFileClose(int fh);
int sceMcClose(int fd);
int sceMcOpen(int port, int slot, const char* name, int mode);
int sceMcSync(int mode, int* cmd, int* result);
int sceMcRead(int fd, void* buff, int size);
char edfile_buffer[4096];
int edfile_buffer_pointer;
int edfile_handle;
int edfile_lock[2];
int edfile_mcresult;
int edfile_media;
int edfile_write_flag;


int EdFileOpen(char *name,enum nufilemode_e mode) {
  int fh;
  
  fh = edfile_handle;
  if (edfile_handle == -1) {
    memset(edfile_buffer,0,0x1000);
    edfile_buffer_pointer = 0;
    edfile_lock[1] = 0;
    edfile_lock[0] = 0;
    edfile_handle = fh;
    edfile_write_flag = 0;
    switch (edfile_media) {
        case 0:
        default:
            return 0;
        break;
        case 1:
          if (mode != NUFILE_READ) {
            if (mode != NUFILE_WRITE) {
                return 0;
            }
            edfile_write_flag = edfile_media;
            edfile_handle = NuFileOpen(name,NUFILE_WRITE);
            if (edfile_handle != -1) return 1;
            return 0;
          }
          edfile_handle = NuFileOpen(name,NUFILE_READ);
          if (edfile_handle == -1) {
              return 0;
          }
            switch (edfile_media) {
                case 0:
                break;
                case 1:
                    NuFileRead(edfile_handle,edfile_buffer,0x1000);
                    edfile_buffer_pointer = 0;
                break;
                case 2:
                    NuMcRead(edfile_handle,edfile_buffer,0x1000,0);
                    edfile_buffer_pointer = 0;
                break;
            }
        break;
        case 2:
          if (mode != NUFILE_READ) {
            if (mode != NUFILE_WRITE) {
                return 0;
            }
                sceMcOpen(0,0,name,0x200);
                sceMcSync(0,0,&edfile_mcresult);
                if (edfile_mcresult < 0) {
                  return 0;
                }
                edfile_handle = edfile_mcresult;
                edfile_write_flag = 1;
                return 1;
          }
          sceMcOpen(0,0,name,1);
          sceMcSync(0,0,&edfile_mcresult);
          if (edfile_mcresult < 0) {
            return 0;
          }
          edfile_handle = edfile_mcresult;
          if (edfile_mcresult != -1) {
            switch (edfile_media) {
                case 0:
                break;
                case 1:
                    NuFileRead(edfile_handle,edfile_buffer,0x1000);
                    edfile_buffer_pointer = 0;
                break;
                case 2:
                    NuMcRead(edfile_handle,edfile_buffer,0x1000,0);
                    edfile_buffer_pointer = 0;
                break;
            }
          }
        break;
    }
    return 1;
  }
  return 0;
}

int EdFileClose(void) {
    if (edfile_handle != -1) {
        if ((edfile_write_flag != 0) && (edfile_buffer_pointer != 0)) {
            switch (edfile_media) {
            case 0:
            break;
            case 1:
                NuFileWrite(edfile_handle, &edfile_buffer, edfile_buffer_pointer);
                edfile_buffer_pointer = 0;
                break;
            case 2:
                NuMcWrite(edfile_handle, &edfile_buffer, edfile_buffer_pointer, 0);
                edfile_buffer_pointer = 0;
                break;
            }
        }
        switch (edfile_media) {
            case 0:
            break;
            case 1:
                NuFileClose(edfile_handle);
                edfile_media = 0;
                edfile_handle = -1;
                return 1;
            break;
            case 2:
                sceMcClose(edfile_handle);
                sceMcSync(0, 0, 0);
                edfile_media = 0;
                edfile_handle = -1;
                return 1;
            break;
        }

    }
    return 0;
}


static inline void EdFileWrite(void *data,int size) {
    int cnt;

    while (size > 0) {
        cnt = (0x1000 - edfile_buffer_pointer);
        if (size < (0x1000 - edfile_buffer_pointer)) {
          cnt = size;
        }
        memcpy(edfile_buffer + edfile_buffer_pointer,data,cnt);
        size -= cnt;
        edfile_buffer_pointer = edfile_buffer_pointer + cnt;
        data = (void *)(data + cnt);
        if ((edfile_buffer_pointer == 0x1000) && (edfile_handle != -1)) {
          switch (edfile_media) {
          case 0:
          break;
          case 1:
              NuFileWrite(edfile_handle, &edfile_buffer, 0x1000);
              edfile_buffer_pointer = 0;
              break;
          case 2:
              NuMcWrite(edfile_handle, &edfile_buffer, 0x1000, 0);
              edfile_buffer_pointer = 0;
              break;
          }
        }
      }
}

static inline void EdFileRead(void *data,int size) {
  int cnt;

  while (size > 0) {
    cnt = (0x1000 - edfile_buffer_pointer);
    if (size < (0x1000 - edfile_buffer_pointer)) {
      cnt = size;
    }
    memcpy(data,&edfile_buffer[edfile_buffer_pointer],cnt);
    size -= cnt;
    edfile_buffer_pointer = edfile_buffer_pointer + cnt;
    data = (void *)(data + cnt);
    if ((edfile_buffer_pointer == 0x1000) && (edfile_handle != -1)) {
      switch (edfile_media) {
            case 0:
            break;
            case 1:
                NuFileRead(edfile_handle, &edfile_buffer, 0x1000);
                edfile_buffer_pointer = 0;
            break;
            case 2:
                NuMcRead(edfile_handle, &edfile_buffer, 0x1000, 0);
                edfile_buffer_pointer = 0;
            break;
        }
    }
  }
}

void EdFileWriteFloat(float data) {
  EdFileWrite(&data,4);
}

void EdFileWriteInt(int data) {
  EdFileWrite(&data,4);
}

void EdFileWriteUnsignedInt(int data) {
  EdFileWrite(&data,4);
}

void EdFileWriteShort(short data) {
  EdFileWrite(&data,2);
}

void EdFileWriteChar(char data) {
  EdFileWrite(&data,1);
}

float EdFileReadFloat(void) {
  float data;

  EdFileRead(&data,4);
  return data;
}

int EdFileReadInt(void) {
  int data;

  EdFileRead(&data,4);
  return data;
}

unsigned int EdFileReadUnsignedInt(void) {
  unsigned int data;

  EdFileRead(&data,4);
  return data;
}

short EdFileReadShort(void) {
  short data;

  EdFileRead(&data,2);
  return data;
}

char EdFileReadChar(void) {
  char data;

  EdFileRead(&data,1);
  return data;
}

void EdFileSetMedia(int media) {
  edfile_media = media;
}

int EdFileWriteMemCard(char *fname,int size,void *data) {
  int cnt;
  
  edfile_media = 2;
  if (EdFileOpen(fname,NUFILE_WRITE) == 0) {
    return 0;
  }
  EdFileWrite(data,size);
  return EdFileClose();
}

int EdFileReadMemCard(char *fname,int size,void *data) {
  edfile_media = 2;
  if (EdFileOpen(fname,NUFILE_READ) == 0) {
    return 0;
  }
  EdFileRead(data,size);
  return EdFileClose();
}

void EdFileResetBuffers(void) {
  memset(edfile_buffer,0,0x1000);
  edfile_buffer_pointer = 0;
  edfile_lock[0] = edfile_lock[1] = 0;
}

void EdFileFillBuffer(void) {
    if (edfile_handle != -1) {
        switch (edfile_media) {
        case 0:
            break;
        case 1:
            NuFileRead(edfile_handle, &edfile_buffer, 0x1000);
            edfile_buffer_pointer = 0;
            break;
        case 2:
            NuMcRead(edfile_handle, &edfile_buffer, 0x1000, 0);
            edfile_buffer_pointer = 0;
            break;
        }
    }
}

void EdFileFlushBuffer(void) {
    if ((edfile_handle != -1) && (edfile_buffer_pointer != 0)) {
        switch (edfile_media) {
        case 0:
            break;
        case 1:
            NuFileWrite(edfile_handle, &edfile_buffer, edfile_buffer_pointer);
            edfile_buffer_pointer = 0;
            break;
        case 2:
            NuMcWrite(edfile_handle, &edfile_buffer, edfile_buffer_pointer, 0);
            edfile_buffer_pointer = 0;
            break;
        }
    }
}
