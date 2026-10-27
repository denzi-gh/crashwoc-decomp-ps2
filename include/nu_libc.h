#ifndef NU_LIBC_H
#define NU_LIBC_H

#include "nu_stdarg.h"

/* The subset of the SDK's newlib that the game calls, with newlib's own
 * signatures. */
typedef unsigned int size_t;

/* A unit whose retail code saw memset as a plain function (its own prototype
 * with a signed size turned off gcc's builtin) defines
 * NU_LIBC_MEMSET_NOT_BUILTIN before including this header. */
#ifdef NU_LIBC_MEMSET_NOT_BUILTIN
void *memset(void *s, int c, int n);
#else
void *memset(void *s, int c, size_t n);
#endif
void *memcpy(void *dst, const void *src, size_t n);
int memcmp(const void *a, const void *b, size_t n);

size_t strlen(const char *s);
char *strcpy(char *dst, const char *src);
char *strncpy(char *dst, const char *src, size_t n);
char *strcat(char *dst, const char *src);
int strcmp(const char *a, const char *b);
int strncmp(const char *a, const char *b, size_t n);
int strcasecmp(const char *a, const char *b);
int strncasecmp(const char *a, const char *b, size_t n);
char *strstr(const char *s, const char *find);
char *strrchr(const char *s, int c);

int printf(const char *fmt, ...);
int sprintf(char *buf, const char *fmt, ...);
int vsprintf(char *buf, const char *fmt, va_list args);

int abs(int x);
int rand(void);
void srand(unsigned int seed);
int atoi(const char *s);
double atof(const char *s);
void *malloc(size_t n);
void *realloc(void *p, size_t n);
void free(void *p);

double fabs(double x);
double sqrt(double x);
double floor(double x);
double fmod(double x, double y);
double pow(double x, double y);
double sin(double x);
double cos(double x);
double tan(double x);
double atan2(double y, double x);

#endif /* NU_LIBC_H */
