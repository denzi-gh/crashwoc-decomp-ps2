/* nu2crash.ps2/nucore/nuerror.c -- error / warning / debug message output */

#include "nu_libc.h"
#include "nu_stdarg.h"

#include "nucore/nuerror.h"

extern void *NuFileOpen(char *name, int mode);
extern void NuFileClose(void *handle);
extern int NuFileSeek(void *handle, int offset, int whence);
extern int NuFileWrite(void *handle, void *buf, int len);
extern void NuDisableVBlankE(void);
extern void NuEnableVBlankE(void);

int errmsg_to_file = 0;         /* write debug messages to the log file */
static int allow_printf = 1;    /* echo messages to the console (TTY) */

extern char *D_0063300C;    /* full path of the reporting source file */
extern char *D_00633010;    /* basename of the reporting source file */
extern int D_00633014;      /* line number in the reporting source file */


/* Creates the log file once per run. */
static inline void NuErrorLogInit(void)
{
    static int initialised = 0;

    if (initialised == 0) {
        void *handle;

        initialised = 1;
        handle = NuFileOpen("nu2.err", 1);
        if (handle) {
            NuFileClose(handle);
        }
    }
}


void NuErrorFunction(const char *fmt, ...)
{
    char buf[0x1000];
    char hdr[0x100];
    va_list args;

    NuErrorLogInit();
    sprintf(hdr, "NuError - %s(%d) : ", D_00633010, D_00633014);
    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);
    if (allow_printf) {
        printf(hdr);
        printf(buf);
        printf("\n");
    }
    for (;;) {
    }
}


void NuWarningFunction(const char *fmt, ...)
{
    char buf[0x400];
    char hdr[0x100];
    va_list args;

    NuErrorLogInit();
    sprintf(hdr, "NuWarning - %s(%d) : ", D_00633010, D_00633014);
    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);
    if (allow_printf) {
        printf(hdr);
        printf(buf);
        printf("\n");
    }
}


void NuDebugMsgFunction(const char *fmt, ...)
{
    static int err_msg_quiet = 0;
    static int dbg_msg_cnt = 0;
    char buf[0x400];
    char msg[0x400];
    va_list args;

    if (err_msg_quiet == 0) {
        NuDisableVBlankE();
        NuErrorLogInit();
        sprintf(msg, "%05d NuDebugMsg - %s(%d) : ", ++dbg_msg_cnt, D_00633010,
                D_00633014);
        va_start(args, fmt);
        vsprintf(buf, fmt, args);
        va_end(args);
        strcat(msg, buf);
        strcat(msg, "\r\n");
        if (errmsg_to_file) {
            void *handle;

            err_msg_quiet = 1;
            handle = NuFileOpen("nu2.err", 2);
            if (handle == 0) {
                handle = NuFileOpen("nu2.err", 1);
            } else {
                NuFileSeek(handle, 0, 2);
            }
            if (handle) {
                NuFileWrite(handle, msg, strlen(msg));
                NuFileClose(handle);
            }
            err_msg_quiet = 0;
        } else if (allow_printf) {
            printf(msg);
        }
        NuEnableVBlankE();
    }
}


NuErrorFunc NuErrorProlog(const char *file, int line)
{
    D_00633014 = line;
    D_0063300C = (char *)file;
    D_00633010 = strrchr(file, '\\');
    if (D_00633010 != 0) {
        D_00633010 = D_00633010 + 1;
    } else {
        D_00633010 = (char *)file;
    }
    return NuErrorFunction;
}


NuErrorFunc NuWarningProlog(const char *file, int line)
{
    D_00633014 = line;
    D_0063300C = (char *)file;
    D_00633010 = strrchr(file, '\\');
    if (D_00633010 != 0) {
        D_00633010 = D_00633010 + 1;
    } else {
        D_00633010 = (char *)file;
    }
    return NuWarningFunction;
}


NuErrorFunc NuDebugMsgProlog(const char *file, int line)
{
    D_00633014 = line;
    D_0063300C = (char *)file;
    D_00633010 = strrchr(file, '\\');
    if (D_00633010 != 0) {
        D_00633010 = D_00633010 + 1;
    } else {
        D_00633010 = (char *)file;
    }
    return NuDebugMsgFunction;
}


void NuAssertMsg(const char *msg, const char *file, int line)
{
    printf("NuAssert - %s(%d) : assert(%s) failed!", msg, file, line);
}
