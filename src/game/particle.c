/*
 * Unit: game/particle
 *
 * Functions:
 *   0x001cb678 DrawParticles
 *   0x001cb798 InitParticles
 *   0x001cb7b8 CloseParticles
 *   0x001cb7e0 AddParticle
 *   0x001cb8c8 DestroyParticlesByHandle
 *   0x001cb958 ProcessParticles
 */

#include "creature.h"

struct nulsthdr_s;
struct nulnkhdr_s;
struct numtl_s;

struct particletype_s {
    s32 life;
    f32 width;
    f32 height;
    f32 tc[4];
    s32 pad1c;
    struct numtl_s *mtl;
};

struct particle_s {
    s32 pad00[2];
    struct nuvec_s pos;
    struct nuvec_s vel;
    struct nuvec_s accel;
    s16 angle;
    s16 angvel;
    s32 life;
    s16 handle;
    s16 pad36;
    struct particletype_s *type;
};

struct faceon_s {
    struct nuvec_s pos;
    f32 w;
    f32 tc[4];
    f32 width;
    f32 height;
    f32 sinang;
    f32 cosang;
    s32 colour[4];
};

extern struct nulnkhdr_s *NuLstGetNext(struct nulsthdr_s *hdr, struct nulnkhdr_s *lnk);
extern struct nulnkhdr_s *NuLstAlloc(struct nulsthdr_s *hdr);
extern void NuLstFree(struct nulnkhdr_s *lnk);
extern struct nulsthdr_s *NuLstCreate(int num, int size);
extern void NuLstDestroy(struct nulsthdr_s *hdr);
extern void *NuScratchAlloc32(u32 size);
extern void NuScratchRelease(void);
extern void NuRndrFaceOn(struct faceon_s *faceon, struct numtl_s *mtl, s32 flags);
extern void NuVecAdd(struct nuvec_s *d, struct nuvec_s *a, struct nuvec_s *b);
extern f32 NuTrigTable[];

extern struct nulsthdr_s *D_0062FCF0;
#define particles D_0062FCF0


void DrawParticles(void)
{
    struct faceon_s *faceon;
    struct particle_s *p;
    struct particle_s *next;
    struct particletype_s *type;

    faceon = NuScratchAlloc32(sizeof(struct faceon_s));
    p = (struct particle_s *)NuLstGetNext(particles, 0);
    while (p) {
        next = (struct particle_s *)NuLstGetNext(particles, (struct nulnkhdr_s *)p);
        type = p->type;
        faceon->pos = p->pos;
        faceon->w = 0.0f;
        faceon->tc[0] = type->tc[0];
        faceon->tc[1] = type->tc[1];
        faceon->tc[2] = type->tc[2];
        faceon->tc[3] = type->tc[3];
        faceon->width = type->width;
        faceon->height = type->height;
        faceon->sinang = NuTrigTable[(u16)p->angle];
        faceon->cosang = NuTrigTable[(p->angle + 0x4000) & 0xFFFF];
        faceon->colour[3] = 0xFF;
        faceon->colour[2] = 0xFF;
        faceon->colour[1] = 0xFF;
        faceon->colour[0] = 0xFF;
        NuRndrFaceOn(faceon, type->mtl, 0);
        p = next;
    }
    NuScratchRelease();
}


void InitParticles(int num)
{
    particles = NuLstCreate(num, sizeof(struct particle_s));
}


void CloseParticles(void)
{
    if (particles) {
        NuLstDestroy(particles);
    }
    particles = 0;
}


struct particle_s *AddParticle(struct nuvec_s *pos, struct nuvec_s *vel, struct nuvec_s *accel,
                               s16 angle, s16 angvel, s16 handle, struct particletype_s *type)
{
    struct particle_s *p;

    p = (struct particle_s *)NuLstAlloc(particles);
    if (p) {
        p->pos = *pos;
        p->vel = *vel;
        p->accel = *accel;
        p->angle = angle;
        p->angvel = angvel;
        p->handle = handle;
        p->type = type;
        p->life = type->life;
    }
    return p;
}


s32 DestroyParticlesByHandle(s16 handle)
{
    struct particle_s *p;
    struct particle_s *next;
    s32 count;

    count = 0;
    p = (struct particle_s *)NuLstGetNext(particles, 0);
    while (p) {
        next = (struct particle_s *)NuLstGetNext(particles, (struct nulnkhdr_s *)p);
        if (p->handle == handle) {
            NuLstFree((struct nulnkhdr_s *)p);
            count++;
        }
        p = next;
    }
    return count;
}


void ProcessParticles(void)
{
    struct particle_s *p;
    struct particle_s *next;

    p = (struct particle_s *)NuLstGetNext(particles, 0);
    while (p) {
        next = (struct particle_s *)NuLstGetNext(particles, (struct nulnkhdr_s *)p);
        NuVecAdd(&p->vel, &p->accel, &p->vel);
        NuVecAdd(&p->pos, &p->vel, &p->pos);
        p->angle += p->angvel;
        p->life--;
        if (p->life == 0) {
            NuLstFree((struct nulnkhdr_s *)p);
        }
        p = next;
    }
}
