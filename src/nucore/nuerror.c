/* nu2crash.ps2/nucore/nuerror.c -- error / warning / debug message output */

/* gcc 2.95 va-mips.h (EABI): the register save area sits below the caller's
   overflow slots, so va_start backs up over the unused named-arg slots. */
typedef char *va_list;
#define va_start(ap, last) \
    ((ap) = ((va_list)__builtin_next_arg(last) \
        - (__builtin_args_info(2) < 8 ? (8 - __builtin_args_info(2)) * 8 : 0)))
#define va_end(ap)

typedef void (*NuErrorFunc)(const char *fmt, ...);

extern int printf(const char *fmt, ...);
extern int sprintf(char *buf, const char *fmt, ...);
extern int vsprintf(char *buf, const char *fmt, va_list args);
extern char *strcat(char *dst, const char *src);
extern int strlen(const char *s);
extern char *strrchr(const char *s, int c);

extern void *NuFileOpen(char *name, int mode);
extern void NuFileClose(void *handle);
extern int NuFileSeek(void *handle, int offset, int whence);
extern int NuFileWrite(void *handle, void *buf, int len);
extern void NuDisableVBlankE(void);
extern void NuEnableVBlankE(void);

extern int D_0062E9DC;      /* echo messages to the console (TTY) */
extern int D_0062E9E0;      /* log file has been created this run */
extern char D_0062E9E8[];   /* log file name */
extern char D_0062E9F0[];   /* trailer printed after an error */
extern int D_0062E9F4;      /* re-entrancy guard for NuDebugMsgFunction */
extern int D_0062E9F8;      /* debug message counter */
extern char D_0062EA00[];   /* debug message line terminator */
extern int errmsg_to_file;  /* write debug messages to the log file */

extern char D_00613998[];   /* error header format */
extern char D_006139B0[];   /* warning header format */
extern char D_006139C8[];   /* debug message header format */
extern char D_006139E8[];   /* assert message format */

extern char *D_0063300C;    /* full path of the reporting source file */
extern char *D_00633010;    /* basename of the reporting source file */
extern int D_00633014;      /* line number in the reporting source file */


void NuErrorFunction(const char *fmt, ...)
{
    char buf[0x1000];
    char hdr[0x100];
    va_list args;

    if (D_0062E9E0 == 0) {
        void *handle;

        D_0062E9E0 = 1;
        handle = NuFileOpen(D_0062E9E8, 1);
        if (handle) {
            NuFileClose(handle);
        }
    }
    sprintf(hdr, D_00613998, D_00633010, D_00633014);
    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);
    if (D_0062E9DC) {
        printf(hdr);
        printf(buf);
        printf(D_0062E9F0);
    }
    for (;;) {
    }
}


void NuWarningFunction(const char *fmt, ...)
{
    char buf[0x400];
    char hdr[0x100];
    va_list args;

    if (D_0062E9E0 == 0) {
        void *handle;

        D_0062E9E0 = 1;
        handle = NuFileOpen(D_0062E9E8, 1);
        if (handle) {
            NuFileClose(handle);
        }
    }
    sprintf(hdr, D_006139B0, D_00633010, D_00633014);
    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);
    if (D_0062E9DC) {
        printf(hdr);
        printf(buf);
        printf(D_0062E9F0);
    }
}


void NuDebugMsgFunction(const char *fmt, ...)
{
    char buf[0x400];
    char msg[0x400];
    va_list args;

    if (D_0062E9F4 == 0) {
        NuDisableVBlankE();
        if (D_0062E9E0 == 0) {
            void *handle;

            D_0062E9E0 = 1;
            handle = NuFileOpen(D_0062E9E8, 1);
            if (handle) {
                NuFileClose(handle);
            }
        }
        sprintf(msg, D_006139C8, ++D_0062E9F8, D_00633010, D_00633014);
        va_start(args, fmt);
        vsprintf(buf, fmt, args);
        va_end(args);
        strcat(msg, buf);
        strcat(msg, D_0062EA00);
        if (errmsg_to_file) {
            void *handle;

            D_0062E9F4 = 1;
            handle = NuFileOpen(D_0062E9E8, 2);
            if (handle == 0) {
                handle = NuFileOpen(D_0062E9E8, 1);
            } else {
                NuFileSeek(handle, 0, 2);
            }
            if (handle) {
                NuFileWrite(handle, msg, strlen(msg));
                NuFileClose(handle);
            }
            D_0062E9F4 = 0;
        } else if (D_0062E9DC) {
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
    printf(D_006139E8, msg, file, line);
}
