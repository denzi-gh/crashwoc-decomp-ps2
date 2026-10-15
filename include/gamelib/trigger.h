#ifndef GAMELIB_TRIGGER_H
#define GAMELIB_TRIGGER_H

#include "nu_types.h"
#include "numath/numathtypes.h"

struct NuSceneInstance;
union variptr_u;

enum NUTRIGGERPRIMTYPES_e {
    NUTRIGGERPRIMTYPE_NONE = 0,
    NUTRIGGERPRIMTYPE_SPHERE = 1,
    NUTRIGGERPRIMTYPE_CUBE = 2,
    NUTRIGGERPRIMTYPE_CYLINDER = 3,
};

struct NUTRIGGERPRIM_s {
    enum NUTRIGGERPRIMTYPES_e type;     /* 0x00 */
    void *data;                         /* 0x04 */
}; /* 0x08 */

struct NUTRIGGERPRIM_SPHERE_s {
    struct nuvec_s centre;              /* 0x00 */
    float radius;                       /* 0x0C */
}; /* 0x10 */

struct NUTRIGGERPRIM_CUBE_s {
    struct numtx_s invmtx;              /* 0x00 */
    struct nuvec_s min;                 /* 0x40 */
    struct nuvec_s max;                 /* 0x4C */
}; /* 0x58 */

struct NUTRIGGERPRIM_CYLINDER_s {
    struct nuvec_s bottom;              /* 0x00 */
    float height;                       /* 0x0C */
    float radius;                       /* 0x10 */
}; /* 0x14 */

enum NUTRIGGERTYPE_s {
    NUTRIGGER_AUTO = 0,
    NUTRIGGER_PLAYER_CONTACT = 1,
    NUTRIGGER_PLAYER_WEAPON_CONTACT = 2,
};

struct NUTRIGGER_s {
    char *triggername;                  /* 0x00 */
    enum NUTRIGGERTYPE_s trigger_type;  /* 0x04 */
    short hitpoints;                    /* 0x08 */
    unsigned char enableflags;          /* 0x0A */
    char pad;                           /* 0x0B */
    int scale_transform : 1;            /* 0x0C */
    int display_box : 1;
    int persistant : 1;
    float radius;                       /* 0x10 */
    struct nuvec_s min;                 /* 0x14 */
    struct nuvec_s max;                 /* 0x20 */
    short numprims;                     /* 0x2C */
    short instance_ix;                  /* 0x2E */
    struct NUTRIGGERPRIM_s *prims;      /* 0x30 */
}; /* 0x34 */

struct NUTRIGGERSYS_s {
    int version;                        /* 0x00 */
    int address_offset;                 /* 0x04 */
    int ntriggers;                      /* 0x08 */
    struct NUTRIGGER_s *triggers;       /* 0x0C */
}; /* 0x10 */

struct instNUTRIGGER_s {
    short hitpoints;                    /* 0x00 */
    unsigned char enableflags;          /* 0x02 */
    unsigned char flags;                /* 0x03 */
}; /* 0x04 */

struct instNUTRIGGERSYS_s {
    struct instNUTRIGGERSYS_s *next;    /* 0x00 */
    struct instNUTRIGGERSYS_s *prev;    /* 0x04 */
    struct NUTRIGGERSYS_s *triggersys;  /* 0x08 */
    struct instNUTRIGGER_s *itriggers;  /* 0x0C */
    struct NuSceneInstance *scenemanager; /* 0x10 */
    int is_disabled : 1;                /* 0x14 */
}; /* 0x18 */

struct NUTRIGGERSYS_s *NuTriggerSysLoad(char *name, char **heap, char **heapend);
int CheckParentedTriggerWithPos(struct NUTRIGGER_s *trigger, struct numtx_s *mtx, struct nuvec_s *pos, float r);
int CheckUnparentedTriggerWithPos(struct NUTRIGGER_s *trigger, struct nuvec_s *pos, float r);
void NuTriggerSysUpdate(struct nuvec_s *pos, f32 radius);
int WeaponCheckTriggers(struct nuvec_s *pos, float r, int hitpoints);
void DebugRenderTriggers(void);
void NuTriggerSysInit(void);
void NuTriggerSysRender(void);
struct instNUTRIGGERSYS_s *instNuTriggerSysCreate(struct NUTRIGGERSYS_s *triggersys, struct NuSceneInstance *scenemanager, union variptr_u *buff);
void instNuTriggerSysDestroy(struct instNUTRIGGERSYS_s *itriggersys);
void instNuTriggerSetFlag(struct instNUTRIGGERSYS_s *itriggersys, char *name, int index, int set);
void instNuTriggerReset(struct instNUTRIGGERSYS_s *itriggersys, char *name);
void instNuTriggerEnable(struct instNUTRIGGERSYS_s *itriggersys);
void instNuTriggerDisable(struct instNUTRIGGERSYS_s *itriggersys);
void NuTriggerPull(struct instNUTRIGGERSYS_s *itriggersys, struct NUTRIGGER_s *trigger, struct instNUTRIGGER_s *itrigger, int pull);

#endif
