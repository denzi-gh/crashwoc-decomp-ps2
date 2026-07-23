/*
 * Unit: game/chase
 *
 * Functions:
 *   0x002586f8 InitChases
 *   0x00258db0 UpdateCrateBallsOfFireDoors
 *   0x00259160 ResetChases
 *   0x002592f8 InitChase
 *   0x00259798 UpdateChase
 *   0x0025a588 DrawChases
 *   0x0025aab8 NuSplineFindPartial
 *   0x0025ab58 ChaseActive
 *   0x0025ab98 UpdateChases
 *   0x0025ac98 NearestChaserDistance
 */

typedef signed char s8;
typedef int s32;

/* Only the fields ChaseActive needs are anchored; sizeof(chase_s)=0x7548 and
 * status@0x7540 are verified from the ChaseActive loop stride/offset. */
struct chase_s {
    char pad0[0x7540];
    s8 status;        /* +0x7540 verified in ChaseActive */
    char pad1[0x7];   /* pad to stride 0x7548 */
};

extern struct chase_s Chase[];


s32 ChaseActive(void) {
    s32 i;

    for (i = 0; i < 3; i++) {
        if (Chase[i].status == 2) {
            return i;
        }
    }
    return -1;
}
