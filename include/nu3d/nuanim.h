#ifndef NU3D_NUANIM_H
#define NU3D_NUANIM_H

#include "numath/numathtypes.h"

struct nuanimkey_s {
    float time;                         /* 0x00 */
    float dtime;                        /* 0x04 */
    float c;                            /* 0x08 */
    float d;                            /* 0x0C */
}; /* 0x10 */

union nuanimmask_u {
    unsigned char bytes[4];
    unsigned int bits;
}; /* 0x4 */

struct nuanimcurve_s {
    union nuanimmask_u mask;            /* 0x00 */
    struct nuanimkey_s *animkeys;       /* 0x04 */
    unsigned int numkeys;               /* 0x08 */
    unsigned int flags;                 /* 0x0C */
}; /* 0x10 */

struct nuanimcurveset_s {
    int flags;                          /* 0x00 */
    float *constants;                   /* 0x04 */
    struct nuanimcurve_s **set;         /* 0x08 */
    signed char ncurves;                /* 0x0C */
    signed char pad[3];                 /* 0x0D */
}; /* 0x10 */

struct nuanimdatachunk_s {
    int numnodes;                       /* 0x00 */
    int num_valid_animcurvesets;        /* 0x04 */
    struct nuanimcurveset_s **animcurvesets; /* 0x08 */
    struct nuanimkey_s *keys;           /* 0x0C */
    struct nuanimcurve_s *curves;       /* 0x10 */
}; /* 0x14 */

struct nuanimdata_s {
    float time;                         /* 0x00 */
    char *node_names;                   /* 0x04 */
    int nchunks;                        /* 0x08 */
    struct nuanimdatachunk_s **chunks;  /* 0x0C */
}; /* 0x10 */

struct nuanimdatahdr_s {
    int version;                        /* 0x00 */
    int address_offset;                 /* 0x04 */
    struct nuanimdata_s *data;          /* 0x08 */
}; /* 0x0C */

struct nuanimkey2_s {
    float time;                         /* 0x00 */
    float dtime;                        /* 0x04 */
    float value;                        /* 0x08 */
    float slope;                        /* 0x0C */
}; /* 0x10 */

struct nuanimstepkey_s {
    float value;                        /* 0x00 */
    int reserved;                       /* 0x04 */
}; /* 0x08 */

struct nuanimcurvedata2_s {
    union nuanimmask_u *mask;           /* 0x00 */
    unsigned short *key_ixs;            /* 0x04 */
    struct nuanimkey2_s *keys;          /* 0x08 */
}; /* 0x0C */

union nuanimcurve2_u {
    float constant;
    struct nuanimcurvedata2_s *curvedata;
}; /* 0x4 */

struct nuanimdata2_s {
    float time;                         /* 0x00 */
    short nnodes;                       /* 0x04 */
    short ncurves;                      /* 0x06 */
    short nchunks;                      /* 0x08 */
    union nuanimcurve2_u *curves;       /* 0x0C */
    signed char *curveflags;            /* 0x10 */
    char *curvesetflags;                /* 0x14 */
}; /* 0x18 */

struct nuanimdata2hdr_s {
    int version;                        /* 0x00 */
    int address_offset;                 /* 0x04 */
    struct nuanimdata2_s *data;         /* 0x08 */
}; /* 0x0C */

struct nuanimtime_s {
    float time;                         /* 0x00 */
    float time_offset;                  /* 0x04 */
    int chunk;                          /* 0x08 */
    unsigned int time_mask;             /* 0x0C */
    unsigned int time_byte;             /* 0x10 */
}; /* 0x14 */

struct NUJOINTANIM_s {
    float rx;                           /* 0x00 */
    float ry;                           /* 0x04 */
    float rz;                           /* 0x08 */
    float tx;                           /* 0x0C */
    float ty;                           /* 0x10 */
    float tz;                           /* 0x14 */
    float sx;                           /* 0x18 */
    float sy;                           /* 0x1C */
    float sz;                           /* 0x20 */
    short max_rx;                       /* 0x24 */
    short max_ry;                       /* 0x26 */
    short max_rz;                       /* 0x28 */
    short min_rx;                       /* 0x2A */
    short min_ry;                       /* 0x2C */
    short min_rz;                       /* 0x2E */
    unsigned char joint_id;             /* 0x30 */
    signed char flags;                  /* 0x31 */
    char pad[2];                        /* 0x32 */
}; /* 0x34 */

struct NUJOINTDATA_s;
struct nujointnode_s;
union variptr_u;

void NuAnimDataWrite(int fh, struct nuanimdata_s *data);
struct nuanimdata_s *NuAnimDataRead(int fh);
struct nuanimdata_s *NuAnimDataFixPtrs(struct nuanimdata_s *data, int base);
struct nuanimdata2_s *NuAnimData2FixPtrs(struct nuanimdata2_s *data, int base);
float NuAnimCurveCalcVal2(struct nuanimcurve_s *curve, struct nuanimtime_s *atime);
float NuAnimCurve2CalcVal(union nuanimcurve2_u *curve, struct nuanimtime_s *atime, int keytype);
void NuAnimCurveSetApplyToJoint(struct nuanimdatachunk_s *chunk, struct nuanimtime_s *atime, int node, struct nujointnode_s *joint);
void NuAnimCurveSetApplyBlendToJoint(struct nuanimdatachunk_s *chunk1, struct nuanimtime_s *atime1, struct nuanimdatachunk_s *chunk2, struct nuanimtime_s *atime2, float blend, int node, struct nujointnode_s *joint);
void NuAnimCurveSetApplyBlendToJoint2(struct nuanimcurveset_s *curveset1, struct nuanimtime_s *atime1, struct nuanimcurveset_s *curveset2, struct nuanimtime_s *atime2, float blend, struct NUJOINTDATA_s *joint, struct nuvec_s *scale, struct nuvec_s *parentscale, struct numtx_s *m, struct NUJOINTANIM_s *offset);
void NuAnimCurve2SetApplyBlendToJoint2(union nuanimcurve2_u *curves1, signed char *curveflags1, signed char curvesetflags1, struct nuanimtime_s *atime1, union nuanimcurve2_u *curves2, signed char *curveflags2, signed char curvesetflags2, struct nuanimtime_s *atime2, float blend, struct NUJOINTDATA_s *joint, struct nuvec_s *scale, struct nuvec_s *parentscale, struct numtx_s *m, struct NUJOINTANIM_s *offset);
void NuAnimCurveSetApplyToJoint2(struct nuanimcurveset_s *curveset, struct nuanimtime_s *atime, struct NUJOINTDATA_s *joint, struct nuvec_s *scale, struct nuvec_s *parentscale, struct numtx_s *m, struct NUJOINTANIM_s *offset);
void NuAnimCurve2SetApplyToJoint(union nuanimcurve2_u *curves, signed char *curveflags, signed char curvesetflags, struct nuanimtime_s *atime, struct NUJOINTDATA_s *joint, struct nuvec_s *scale, struct nuvec_s *parentscale, struct numtx_s *m, struct NUJOINTANIM_s *offset);
void NuAnimCurveSetApplyToJointBasic(struct nuanimcurveset_s *curveset, struct nuanimtime_s *atime, struct NUJOINTDATA_s *joint, struct nuvec_s *scale, struct nuvec_s *parentscale, struct numtx_s *m, struct NUJOINTANIM_s *offset);
void NuAnimCurve2SetApplyToJointBasic(union nuanimcurve2_u *curves, signed char *curveflags, signed char curvesetflags, struct nuanimtime_s *atime, struct NUJOINTDATA_s *joint, struct nuvec_s *scale, struct nuvec_s *parentscale, struct numtx_s *m, struct NUJOINTANIM_s *offset);
void NuAnimCurveSetApplyToMatrix(struct nuanimcurveset_s *curveset, struct nuanimtime_s *atime, struct numtx_s *m);
void NuAnimCurve2SetApplyToMatrix(union nuanimcurve2_u *curves, signed char *curveflags, signed char curvesetflags, struct nuanimtime_s *atime, struct numtx_s *m);
struct nuanimdata_s *NuAnimDataLoad(char *file);
struct nuanimdata_s *NuAnimDataLoadBuff(char *file, union variptr_u *buffer, union variptr_u *buffend);
void NuAnimDataDestroy(struct nuanimdata_s *data);
void NuAnimDataCalcTime(struct nuanimdata_s *data, float time, struct nuanimtime_s *atime);
struct nuanimdata_s *NuAnimDataCreate(int nchunks);
struct nuanimcurveset_s *NuAnimCurveSetCreate(int ncurves);
void NuAnimCurveSetDestroy(struct nuanimcurveset_s *curveset, int destroy_curves);
struct nuanimcurve_s *NuAnimCurveCreate(int numkeys);
void NuAnimCurveSetWrite(int fh, struct nuanimcurveset_s *curveset);
struct nuanimcurve_s *NuAnimCurveRead(int fh);
int NuAnimDataFindVersion(char *file);
struct nuanimdata2_s *NuAnimData2LoadBuff(char *file, union variptr_u *buffer, union variptr_u *buffend);
void NuAnimData2CalcTime(struct nuanimdata2_s *data, float time, struct nuanimtime_s *atime);
struct nuanimdatachunk_s *NuAnimDataChunkCreate(int numnodes);
void NuAnimDataChunkDestroy(struct nuanimdatachunk_s *chunk);

#endif /* NU3D_NUANIM_H */
