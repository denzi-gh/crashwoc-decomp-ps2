#ifndef NUCORE_NUFILE_H
#define NUCORE_NUFILE_H

enum nufilemode_e {
    NUFILE_READ = 0,
    NUFILE_WRITE = 1,
    NUFILE_APPEND = 2,
    NUFILE_READ_NOWAIT = 3,
    NUFILE_MODE_CNT = 4
};

#ifdef NW_HOST
/* Retail calls these without a prototype in scope (game/main.c); only the host declares them. */
void NuFileInitEx(int deviceid, int rebootiop);
#endif

#endif
