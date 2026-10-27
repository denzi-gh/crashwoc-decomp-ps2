#ifndef NUSOUND_NUSOUND_H
#define NUSOUND_NUSOUND_H

/* One sound sample: its file name and playback setup. */
struct nusound_filename_info_s {
    char Filename[32];              /* 0x00 */
    int Pitch;                      /* 0x20 */
    int LoopInfo;                   /* 0x24 */
    int ID;                         /* 0x28 */
}; /* 0x2C */

typedef struct nusound_filename_info_s NUSOUND_FILENAME_INFO;

#endif /* NUSOUND_NUSOUND_H */
