#ifndef NU3D_NUCVTGEO_H
#define NU3D_NUCVTGEO_H

struct nucolourref_s {
    union {
        unsigned int *addr;
        unsigned int addr_offset;
    };                                  /* 0x00 */
    int count;                          /* 0x04 */
    /* The PS2 colour stream interleaves an unlit/lit pair per entry. */
    union {
        unsigned int cols[2][1];
        struct {
            unsigned int colour;
            unsigned int lit;
        } pair[0];
    };                                  /* 0x08 */
}; /* 0x10 */
typedef struct nucolourref_s NUCOLOURREF;

#endif /* NU3D_NUCVTGEO_H */
