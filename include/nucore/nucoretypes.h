#ifndef NUCORE_NUCORETYPES_H
#define NUCORE_NUCORETYPES_H

#include "nu_types.h"

struct nuvec_s;
struct nuvec4_s;
struct nuivec_s;
struct nuivec4_s;
struct numtx_s;
struct numtl_s;
struct nuvutri_s;
struct nutristream_s;
struct _sceDmaTag;

/* Cursor into a buffer (superbuffer_ptr, a loader's heap, a VU packet):
 * read or advanced as whatever type comes next. Members follow the DWARF
 * of another game built on this engine. */
union variptr_u {
    void *voidptr;
    char *charptr;              /* NuGScnRead takes the cursor as char ** */
    s8 *s8;
    u8 *u8;
    u16 *u16;
    s16 *s16;
    u32 *u32;
    s32 *s32;                   /* unsigned int * in that DWARF; retail converts
                                   floats stored through it as signed */
    u64 *u64;
    f32 *f32;
    struct nuvec_s *vec3;
    struct nuvec4_s *vec4;
    struct nuivec_s *ivec3;
    struct nuivec4_s *ivec4;
    struct numtx_s *mtx44;
    struct numtl_s *mtl;
    struct nuvutri_s *vutri;
    struct nutristream_s *tristream;
    u32 *viftag;
    nuintptr intaddr;
    struct _sceDmaTag *dmatag;
    void *giftag;
    void *zbuf;
    void *test;
    void *prmode;
    void *frame;
};

#endif
