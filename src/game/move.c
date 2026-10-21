/*
 * Unit: game/move
 *
 * Functions:
 *   0x00250588 InitPlayerEvents
 *   0x002507d0 CheckPlayerEvents
 *   0x00250908 UpdateChaseRunAnim
 *   0x00250a98 MoveCRASH
 *   0x00251c98 AnimateCRASH
 *   0x00252248 MoveSWIMMING
 *   0x00252478 MoveCOCO
 *   0x00252d18 AnimateCOCO
 *   0x00252fb0 MoveSCOOTER
 *   0x00253278 MoveSNOWBOARD
 *   0x00253590 AnimateSNOWBOARD
 *   0x00253720 MoveOFFROADER
 *   0x00253cd8 MoveMECH
 *   0x00254338 AnimateMECH
 *   0x002544d0 MoveFIREENGINE
 *   0x002547c0 AnimateFIREENGINE
 *   0x002548a8 MoveGYRO
 *   0x002552f8 AnimateGYRO
 *   0x002553f0 DrawCarpet
 *   0x002554e0 MoveSUBMARINE
 *   0x002556c8 MoveMINECART
 *   0x00255ca8 ResetTubs
 *   0x00255ec8 MoveTub
 *   0x00256a18 MoveMINETUB
 *   0x00257270 AnimateMINETUB
 *   0x00257570 CheckGates
 *   0x00257700 ResetRings
 *   0x00257880 CheckRings
 *   0x00257d70 DrawRings
 *   0x00257e80 AnimateSWIMMING
 *   0x00257f88 AnimateSCOOTER
 *   0x00258020 AnimateOFFROADER
 *   0x002580f8 AnimateSUBMARINE
 *   0x00258158 AnimateMINECART
 *   0x002581b0 AnimateGLIDER
 *   0x00258270 AnimateDROPSHIP
 *   0x002582c0 AnimateATLASPHERE
 *   0x00258348 AnimateJEEP
 *   0x00258380 AnimateMOSQUITO
 *   0x002583d0 ResetGates
 *   0x002584b8 ResetPlayerEvents
 *   0x00258568 UpdateArrow
 *   0x00258618 DrawPanel3DArrow
 *   0x002586a8 AnimateDIVE
 *   0x002586d0 ResetTub
 */

#include "nu_libc.h"

#include "numath/numtx.h"
#include "numath/nutrig.h"
#include "numath/nuvec.h"
#include "numath/nufloat.h"
#include "nu3d/nugscn.h"
#include "game/gametypes.h"
#include "game/creature.h"
#include "nups2/ps2ctl.h"

#ifdef NW_HOST
#include "game/crate.h"
#include "game/game_obj.h"
/* gnu89 inline globals stay out of module headers. */
struct obj_s;
s32 HitItems(struct obj_s *obj);
s32 KillPlayer(struct obj_s *obj, s32 anim);
#endif

extern s32 FlyingLevelVictoryDance;
extern s32 Level;
extern s32 SmokeyCountDownValue;
extern struct RPos_s *best_cRPos;
extern s32 FireBossHoldPlayer;

extern struct nupad_s *Pad[];

struct plrevent_s {
    struct nugspline_s *spl;        /* 0x00 */
    struct nuhspecial_s obj[16];    /* 0x04 */
    struct nuvec_s pos;             /* 0x84 */
    s8 triggered;                   /* 0x90 */
    s8 iRAIL;                       /* 0x91 */
    s16 iALONG;                     /* 0x92 */
    f32 fALONG;                     /* 0x94 */
};

struct tracklink_s {
    s8 i_left;     /* 0x0 */
    s8 i_mid;      /* 0x1 */
    s8 i_right;    /* 0x2 */
    u8 start;      /* 0x3 */
    u8 end;        /* 0x4 */
    u8 flags;      /* 0x5 */
}; /* 0x6 */

struct MoveInfo CrashMoveInfo = {
    0.0075f /* IDLESPEED */,
    0.015f /* TIPTOESPEED */,
    0.03f /* WALKSPEED */,
    0.072f /* RUNSPEED */,
    0.096f /* SPRINTSPEED */,
    0.096f /* SLIDESPEED */,
    0.012f /* CRAWLSPEED */,
    0.015599999f /* DANGLESPEED */,
    0.0139999995f /* WADESPEED */,
    1.1f /* JUMPHEIGHT */,
    0.22f /* DANGLEGAP */,
    5 /* JUMPFRAMES0 */,
    10 /* JUMPFRAMES1 */,
    15 /* JUMPFRAMES2 */,
    20 /* STARJUMPFRAMES */,
    50 /* SOMERSAULTFRAMES */,
    16 /* SPINFRAMES */,
    12 /* SPINRESETFRAMES */,
    33 /* SUPERSPINFRAMES */,
    16 /* SUPERSPINWAITFRAMES */,
    50 /* SLAMWAITFRAMES */,
    25 /* SLIDEFRAMES */,
    25 /* CROUCHINGFRAMES */,
    50 /* JUMPLANDFRAMES */,
    -1 /* spad */
};

struct MoveInfo SwimmingMoveInfo = {
    0.0075f /* IDLESPEED */,
    0.015f /* TIPTOESPEED */,
    0.024999999f /* WALKSPEED */,
    0.049999997f /* RUNSPEED */,
    0.099999994f /* SPRINTSPEED */,
    0.096f /* SLIDESPEED */,
    0.012f /* CRAWLSPEED */,
    0.03f /* DANGLESPEED */,
    0.0139999995f /* WADESPEED */,
    1.1f /* JUMPHEIGHT */,
    0.2f /* DANGLEGAP */,
    5 /* JUMPFRAMES0 */,
    10 /* JUMPFRAMES1 */,
    15 /* JUMPFRAMES2 */,
    20 /* STARJUMPFRAMES */,
    50 /* SOMERSAULTFRAMES */,
    16 /* SPINFRAMES */,
    12 /* SPINRESETFRAMES */,
    33 /* SUPERSPINFRAMES */,
    16 /* SUPERSPINWAITFRAMES */,
    50 /* SLAMWAITFRAMES */,
    25 /* SLIDEFRAMES */,
    25 /* CROUCHINGFRAMES */,
    50 /* JUMPLANDFRAMES */,
    -1 /* spad */
};

struct MoveInfo CocoMoveInfo = {
    0.0075f /* IDLESPEED */,
    0.012f /* TIPTOESPEED */,
    0.024f /* WALKSPEED */,
    0.06f /* RUNSPEED */,
    0.089999996f /* SPRINTSPEED */,
    0.096f /* SLIDESPEED */,
    0.012f /* CRAWLSPEED */,
    0.015599999f /* DANGLESPEED */,
    0.0139999995f /* WADESPEED */,
    1.1f /* JUMPHEIGHT */,
    0.2f /* DANGLEGAP */,
    5 /* JUMPFRAMES0 */,
    10 /* JUMPFRAMES1 */,
    15 /* JUMPFRAMES2 */,
    20 /* STARJUMPFRAMES */,
    16 /* SOMERSAULTFRAMES */,
    16 /* SPINFRAMES */,
    10 /* SPINRESETFRAMES */,
    33 /* SUPERSPINFRAMES */,
    16 /* SUPERSPINWAITFRAMES */,
    50 /* SLAMWAITFRAMES */,
    25 /* SLIDEFRAMES */,
    25 /* CROUCHINGFRAMES */,
    50 /* JUMPLANDFRAMES */,
    -1 /* spad */
};

struct MoveInfo ScooterMoveInfo = {
    0.004f /* IDLESPEED */,
    0.01f /* TIPTOESPEED */,
    0.03f /* WALKSPEED */,
    0.07f /* RUNSPEED */,
    0.099999994f /* SPRINTSPEED */,
    0.08f /* SLIDESPEED */,
    0.012f /* CRAWLSPEED */,
    0.03f /* DANGLESPEED */,
    0.0139999995f /* WADESPEED */,
    1.1f /* JUMPHEIGHT */,
    0.2f /* DANGLEGAP */,
    5 /* JUMPFRAMES0 */,
    10 /* JUMPFRAMES1 */,
    15 /* JUMPFRAMES2 */,
    20 /* STARJUMPFRAMES */,
    16 /* SOMERSAULTFRAMES */,
    16 /* SPINFRAMES */,
    10 /* SPINRESETFRAMES */,
    33 /* SUPERSPINFRAMES */,
    16 /* SUPERSPINWAITFRAMES */,
    50 /* SLAMWAITFRAMES */,
    25 /* SLIDEFRAMES */,
    25 /* CROUCHINGFRAMES */,
    50 /* JUMPLANDFRAMES */,
    -1 /* spad */
};

struct MoveInfo SnowBoardMoveInfo = {
    0.004f /* IDLESPEED */,
    0.01f /* TIPTOESPEED */,
    0.03f /* WALKSPEED */,
    0.08f /* RUNSPEED */,
    0.12f /* SPRINTSPEED */,
    0.08f /* SLIDESPEED */,
    0.012f /* CRAWLSPEED */,
    0.03f /* DANGLESPEED */,
    0.0139999995f /* WADESPEED */,
    1.1f /* JUMPHEIGHT */,
    0.2f /* DANGLEGAP */,
    5 /* JUMPFRAMES0 */,
    10 /* JUMPFRAMES1 */,
    15 /* JUMPFRAMES2 */,
    20 /* STARJUMPFRAMES */,
    16 /* SOMERSAULTFRAMES */,
    16 /* SPINFRAMES */,
    10 /* SPINRESETFRAMES */,
    33 /* SUPERSPINFRAMES */,
    16 /* SUPERSPINWAITFRAMES */,
    50 /* SLAMWAITFRAMES */,
    25 /* SLIDEFRAMES */,
    25 /* CROUCHINGFRAMES */,
    50 /* JUMPLANDFRAMES */,
    -1 /* spad */
};

struct MoveInfo OffRoaderMoveInfo = {
    0.004f /* IDLESPEED */,
    0.01f /* TIPTOESPEED */,
    0.03f /* WALKSPEED */,
    0.12f /* RUNSPEED */,
    0.17999999f /* SPRINTSPEED */,
    0.08f /* SLIDESPEED */,
    0.012f /* CRAWLSPEED */,
    0.03f /* DANGLESPEED */,
    0.0139999995f /* WADESPEED */,
    1.1f /* JUMPHEIGHT */,
    0.2f /* DANGLEGAP */,
    5 /* JUMPFRAMES0 */,
    10 /* JUMPFRAMES1 */,
    15 /* JUMPFRAMES2 */,
    20 /* STARJUMPFRAMES */,
    16 /* SOMERSAULTFRAMES */,
    16 /* SPINFRAMES */,
    10 /* SPINRESETFRAMES */,
    33 /* SUPERSPINFRAMES */,
    16 /* SUPERSPINWAITFRAMES */,
    50 /* SLAMWAITFRAMES */,
    25 /* SLIDEFRAMES */,
    25 /* CROUCHINGFRAMES */,
    50 /* JUMPLANDFRAMES */,
    -1 /* spad */
};

struct MoveInfo MechMoveInfo = {
    0.004f /* IDLESPEED */,
    0.01f /* TIPTOESPEED */,
    0.018f /* WALKSPEED */,
    0.08f /* RUNSPEED */,
    0.12f /* SPRINTSPEED */,
    0.08f /* SLIDESPEED */,
    0.012f /* CRAWLSPEED */,
    0.03f /* DANGLESPEED */,
    0.0139999995f /* WADESPEED */,
    1.3f /* JUMPHEIGHT */,
    0.2f /* DANGLEGAP */,
    5 /* JUMPFRAMES0 */,
    10 /* JUMPFRAMES1 */,
    20 /* JUMPFRAMES2 */,
    20 /* STARJUMPFRAMES */,
    16 /* SOMERSAULTFRAMES */,
    16 /* SPINFRAMES */,
    10 /* SPINRESETFRAMES */,
    33 /* SUPERSPINFRAMES */,
    16 /* SUPERSPINWAITFRAMES */,
    50 /* SLAMWAITFRAMES */,
    25 /* SLIDEFRAMES */,
    25 /* CROUCHINGFRAMES */,
    50 /* JUMPLANDFRAMES */,
    -1 /* spad */
};

struct MoveInfo FireEngineMoveInfo = {
    0.004f /* IDLESPEED */,
    0.01f /* TIPTOESPEED */,
    0.018f /* WALKSPEED */,
    0.099999994f /* RUNSPEED */,
    0.08f /* SPRINTSPEED */,
    0.08f /* SLIDESPEED */,
    0.012f /* CRAWLSPEED */,
    0.03f /* DANGLESPEED */,
    0.0139999995f /* WADESPEED */,
    1.3f /* JUMPHEIGHT */,
    0.2f /* DANGLEGAP */,
    5 /* JUMPFRAMES0 */,
    10 /* JUMPFRAMES1 */,
    20 /* JUMPFRAMES2 */,
    20 /* STARJUMPFRAMES */,
    16 /* SOMERSAULTFRAMES */,
    16 /* SPINFRAMES */,
    10 /* SPINRESETFRAMES */,
    33 /* SUPERSPINFRAMES */,
    16 /* SUPERSPINWAITFRAMES */,
    50 /* SLAMWAITFRAMES */,
    25 /* SLIDEFRAMES */,
    25 /* CROUCHINGFRAMES */,
    50 /* JUMPLANDFRAMES */,
    -1 /* spad */
};

struct MoveInfo GyroMoveInfo = {
    0.0075f /* IDLESPEED */,
    0.015f /* TIPTOESPEED */,
    0.06f /* WALKSPEED */,
    0.089999996f /* RUNSPEED */,
    0.12f /* SPRINTSPEED */,
    0.096f /* SLIDESPEED */,
    0.012f /* CRAWLSPEED */,
    0.03f /* DANGLESPEED */,
    0.0139999995f /* WADESPEED */,
    1.1f /* JUMPHEIGHT */,
    0.2f /* DANGLEGAP */,
    5 /* JUMPFRAMES0 */,
    10 /* JUMPFRAMES1 */,
    15 /* JUMPFRAMES2 */,
    20 /* STARJUMPFRAMES */,
    50 /* SOMERSAULTFRAMES */,
    16 /* SPINFRAMES */,
    12 /* SPINRESETFRAMES */,
    33 /* SUPERSPINFRAMES */,
    16 /* SUPERSPINWAITFRAMES */,
    50 /* SLAMWAITFRAMES */,
    25 /* SLIDEFRAMES */,
    25 /* CROUCHINGFRAMES */,
    50 /* JUMPLANDFRAMES */,
    -1 /* spad */
};

struct MoveInfo SubmarineMoveInfo = {
    0.0075f /* IDLESPEED */,
    0.015f /* TIPTOESPEED */,
    0.024999999f /* WALKSPEED */,
    0.06f /* RUNSPEED */,
    0.08f /* SPRINTSPEED */,
    0.096f /* SLIDESPEED */,
    0.012f /* CRAWLSPEED */,
    0.03f /* DANGLESPEED */,
    0.0139999995f /* WADESPEED */,
    1.1f /* JUMPHEIGHT */,
    0.2f /* DANGLEGAP */,
    5 /* JUMPFRAMES0 */,
    10 /* JUMPFRAMES1 */,
    15 /* JUMPFRAMES2 */,
    20 /* STARJUMPFRAMES */,
    50 /* SOMERSAULTFRAMES */,
    16 /* SPINFRAMES */,
    12 /* SPINRESETFRAMES */,
    33 /* SUPERSPINFRAMES */,
    16 /* SUPERSPINWAITFRAMES */,
    50 /* SLAMWAITFRAMES */,
    25 /* SLIDEFRAMES */,
    25 /* CROUCHINGFRAMES */,
    50 /* JUMPLANDFRAMES */,
    -1 /* spad */
};

struct MoveInfo MineCartMoveInfo = {
    0.004f /* IDLESPEED */,
    0.01f /* TIPTOESPEED */,
    0.03f /* WALKSPEED */,
    0.08f /* RUNSPEED */,
    0.12f /* SPRINTSPEED */,
    0.08f /* SLIDESPEED */,
    0.012f /* CRAWLSPEED */,
    0.03f /* DANGLESPEED */,
    0.0139999995f /* WADESPEED */,
    1.1f /* JUMPHEIGHT */,
    0.2f /* DANGLEGAP */,
    5 /* JUMPFRAMES0 */,
    10 /* JUMPFRAMES1 */,
    15 /* JUMPFRAMES2 */,
    20 /* STARJUMPFRAMES */,
    16 /* SOMERSAULTFRAMES */,
    16 /* SPINFRAMES */,
    10 /* SPINRESETFRAMES */,
    33 /* SUPERSPINFRAMES */,
    16 /* SUPERSPINWAITFRAMES */,
    50 /* SLAMWAITFRAMES */,
    25 /* SLIDEFRAMES */,
    25 /* CROUCHINGFRAMES */,
    50 /* JUMPLANDFRAMES */,
    -1 /* spad */
};

struct MoveInfo MineTubMoveInfo = {
    0.004f /* IDLESPEED */,
    0.01f /* TIPTOESPEED */,
    0.03f /* WALKSPEED */,
    0.08f /* RUNSPEED */,
    0.12f /* SPRINTSPEED */,
    0.08f /* SLIDESPEED */,
    0.012f /* CRAWLSPEED */,
    0.03f /* DANGLESPEED */,
    0.0139999995f /* WADESPEED */,
    1.1f /* JUMPHEIGHT */,
    0.2f /* DANGLEGAP */,
    5 /* JUMPFRAMES0 */,
    10 /* JUMPFRAMES1 */,
    15 /* JUMPFRAMES2 */,
    20 /* STARJUMPFRAMES */,
    16 /* SOMERSAULTFRAMES */,
    16 /* SPINFRAMES */,
    10 /* SPINRESETFRAMES */,
    33 /* SUPERSPINFRAMES */,
    16 /* SUPERSPINWAITFRAMES */,
    50 /* SLAMWAITFRAMES */,
    25 /* SLIDEFRAMES */,
    25 /* CROUCHINGFRAMES */,
    50 /* JUMPLANDFRAMES */,
    -1 /* spad */
};

struct tracklink_s TubLinkCR[9] = {
    /* i_left, i_mid, i_right, start, end, flags */
    { 1, 2, -1, 0, 0, 0 },
    { -1, 3, -1, 0, 0, 0 },
    { -1, 3, -1, 0, 0, 0 },
    { -1, 5, 4, 0, 0, 0 },
    { -1, -1, -1, 0, 0, 0 },
    { -1, 6, 7, 0, 0, 0 },
    { -1, 8, -1, 0, 0, 0 },
    { -1, 8, -1, 0, 0, 0 },
    { -1, -1, -1, 0, 0, 0 },
};

struct tracklink_s TubLinkGT[16] = {
    /* i_left, i_mid, i_right, start, end, flags */
    { -1, 2, -1, 18, 19, 0 },
    { -1, 2, -1, 18, 19, 0 },
    { 3, 4, -1, 19, 1, 0 },
    { -1, 7, -1, 1, 6, 0 },
    { -1, 6, 5, 1, 4, 0 },
    { -1, 8, -1, 4, 13, 0 },
    { -1, 7, -1, 4, 6, 0 },
    { -1, 8, -1, 6, 13, 0 },
    { 9, 10, -1, 13, 14, 0 },
    { -1, 11, -1, 14, 15, 0 },
    { -1, 11, -1, 14, 15, 0 },
    { -1, 13, 12, 15, 16, 0 },
    { 14, 15, -1, 16, 17, 0 },
    { 14, 15, -1, 16, 17, 0 },
    { -1, 0, -1, 17, 18, 0 },
    { -1, 0, -1, 17, 18, 0 },
};

struct nuvec_s CircuitOriginGT = { -15.0f, 0.0f, -22.0f };

struct MoveInfo DefaultMoveInfo = {
    0.008f /* IDLESPEED */,
    0.0139999995f /* TIPTOESPEED */,
    0.03f /* WALKSPEED */,
    0.07f /* RUNSPEED */,
    0.089999996f /* SPRINTSPEED */,
    0.089999996f /* SLIDESPEED */,
    0.01f /* CRAWLSPEED */,
    0.03f /* DANGLESPEED */,
    0.0139999995f /* WADESPEED */,
    1.1f /* JUMPHEIGHT */,
    0.2f /* DANGLEGAP */,
    5 /* JUMPFRAMES0 */,
    10 /* JUMPFRAMES1 */,
    15 /* JUMPFRAMES2 */,
    20 /* STARJUMPFRAMES */,
    50 /* SOMERSAULTFRAMES */,
    16 /* SPINFRAMES */,
    12 /* SPINRESETFRAMES */,
    33 /* SUPERSPINFRAMES */,
    16 /* SUPERSPINWAITFRAMES */,
    50 /* SLAMWAITFRAMES */,
    25 /* SLIDEFRAMES */,
    25 /* CROUCHINGFRAMES */,
    25 /* JUMPLANDFRAMES */,
    -1 /* spad */
};

struct tubinfo_s PlrTub = { 0 };
struct tubinfo_s OppTub = { 0 };
struct creature_s OppTubCreature = { 0 };
struct plrevent_s PlrEvent[8] = { 0 };
struct nuvec_s ring_pos = { 0.0f, 0.0f, 0.0f };
struct nuvec_s jeeptyrepos = { 0.0f, 0.0f, 0.0f };
struct nuvec_s carpet_splpos = { 0.0f, 0.0f, 0.0f };
struct nuvec_s carpet_pos = { 0.0f, 0.0f, 0.0f };
struct nuvec_s carpet_oldpos = { 0.0f, 0.0f, 0.0f };
struct nuvec_s carpet_mom = { 0.0f, 0.0f, 0.0f };
struct anim_s carpet_anim = { 0 };
struct RPos_s carpet_RPos = { 0 };
struct Nearest_Light_s carpet_lights = { 0 };
struct nuvec_s gyropos = { 0.0f, 0.0f, 0.0f };
struct nuvec_s TempTubPos = { 0.0f, 0.0f, 0.0f };
u16 ring_xrot[48] = { 0 };
u16 ring_yrot[48] = { 0 };
f32 GRAVITY = -0.0072000003f;
f32 TERMINALVELOCITY = 0.24f;
s32 MAXLAPS = 3;
f32 SWEEPATTACKSTARTTIME = 15.0f;
f32 SWEEPATTACKENDTIME = 25.0f;
f32 SCOOTERJUMPPAUSEFRAME = 12.0f;
f32 SNOWBOARDJUMPPAUSEFRAME = 22.0f;
f32 OFFROADERSPEEDTIME = 2.0f;
s16 jeepsfx[4] = { 87, 102, 108, 107 };
s32 i_jkeepsfx = -1;
f32 MECHRUNNINGJUMPPAUSEFRAME = 31.0f;
f32 MECHSTANDINGJUMPPAUSEFRAME = 43.0f;
f32 FIREENGINERUNNINGJUMPPAUSEFRAME = 31.0f;
f32 FIREENGINESTANDINGJUMPPAUSEFRAME = 43.0f;
f32 CARPETTIME = 90.0f;
s32 carpet_character[2] = { 14, -1 };
s32 carpet_place = 1;
struct CharacterModel *carpet_model[2] = { 0 };
s32 carpet_action = 29;
s32 carpet_overlap = 0;
f32 FASTMINECARTMUL = 20.0f;
f32 SLOWMINECARTMUL = 1.0f;
s32 cart_sfx = -1;
f32 TUBWAIT = 12.0f;
f32 TUBBOOSTFRAMES = 50.0f;
s32 plr_tub_tilt = 0;
s32 plr_tub_tilttarget = 0;
s32 plr_tub_tiltlock = 10;
s32 tub_overlap = 0;
s32 plr_gates = 0;
s32 GATECOUNT = 0;
u64 gate_bits = 0;
s32 i_ring = 0;
s32 RINGCOUNT = 0;
f32 RINGPULL = 0.001f;
f32 VEHICLETIME = 0.0f;
f32 plr_vehicle_time = 0.0f;
f32 plr_vehicle_speedmul = 0.0f;
s32 plr_tocam_wait = 0;
s32 plr_tocam_turn = 0;
s32 plr_tumblehack = 0;
f32 offroader_speedtime = 0.0f;
f32 CARPETLOOPTIME = 0.0f;
u16 carpet_yrot = 0;
u16 carpet_xrot = 0;
f32 carpet_time = 0.0f;
s32 carpet_panelpos = 0;
struct tracklink_s *TubLink = 0;
s32 race_finished = 0;
s32 opptub_action = 0;
s32 opptub_wait = 0;
u16 arrow_xrot = 0;
u16 arrow_yrot = 0;

extern struct spltab_s SplTab[];
extern struct tersurface_s TerSurface[];
extern s32 GameMode;
extern s32 last_hub;
extern s32 tumble_action;
extern f32 tumble_time;
extern f32 tumble_duration;
extern s32 plr_rebound;
extern f32 FALLONCRATEBREAKSPEED;
extern struct chase_s Chase[3];
extern struct nugscn_s *world_scene[];
extern struct objtab_s ObjTab[];
extern struct gtimer_s GameTimer;
extern struct cammtx_s GameCam[];
extern s32 bonusgem_ok;
extern s32 LineCrossed(f32 xold, f32 zold, f32 xnew, f32 znew,
                       f32 x0, f32 z0, f32 x1, f32 z1);
extern s32 StartHGobjAnim(struct nuhspecial_s *obj);
extern void JudderGameCamera(struct cammtx_s *cam, f32 time, struct nuvec_s *pos);
extern void GameSfx(s32 sfx, struct nuvec_s *pos);
extern s32 temp_iRAIL;
extern s32 temp_iALONG;
extern f32 temp_fALONG;

s32 RotDiff(u16 a, u16 b);
void AddGameDebrisRot(s32 i, struct nuvec_s *pos, s32 n, u16 xrot, u16 yrot);
void GetALONG(struct nuvec_s *pos, struct RPos_s *rpos, s32 iRAIL, s32 iALONG, s32 dir);
s32 AheadOfCheckpoint(s32 iRAIL, s32 iALONG, f32 fALONG);
f32 NuFsqrt(f32 x);
u16 SeekRot(u16 a, u16 target, s32 rate);
s32 qrand(void);
s32 ResetHGobjAnim(struct nuhspecial_s *obj);
void SetNearestLights(struct Nearest_Light_s *l);
void SetLevelLights(void);
s32 NuHGobjRndrMtxDwa(struct NUHGOBJ_s *hobj, struct numtx_s *m, s32 nlayers,
                      s16 *layers, struct numtx_s *mtx_array, f32 **dwa);
s32 DrawPanel3DObject(s32 object, f32 x, f32 y, f32 z, f32 scalex, f32 scaley,
                      f32 scalez, u16 xrot, u16 yrot, u16 zrot,
                      struct nugscn_s *scn, struct nuspecial_s *obj, s32 rot);


extern s32 gamesfx_pitch;
void GameSfxLoop(s32 sfx, struct nuvec_s *pos);
void AddProjectile(struct nuvec_s *pos, struct nuvec_s *target, struct nuvec_s *mom, s32 type,
                   u16 hdg, struct obj_s *owner);
extern struct game_s Game;
extern u8 D_00591FDE[];
#define menu_mode ((s8)D_00591FDE[0])
extern s32 fadeval;
extern s32 plr_allow_jump;
void AddGameDebris(s32 type, struct nuvec_s *pos);
extern s32 gamesfx_effect_volume;
extern void ResetLights(struct Nearest_Light_s *lights);
extern f32 NuTrigTable[];
extern s32 ExtraMoves;
extern struct nuvec_s plr_target_pos[2];
extern struct nuvec_s plr_target_dir;
extern struct nuvec_s plr_target_firepos;
extern struct numtx_s plr_target_mtx;
void AddFlyingWumpa(struct nuvec_s *src, struct nuvec_s *dir, struct nuvec_s *dst, s32 destroy);
void AddGameDebrisMtx(s32 i, struct nuvec_s *pos, s32 n, struct numtx_s *m);
void AddKaboom(s32 type, struct nuvec_s *pos, f32 radius);
extern char tbuf[];
struct nugspline_s *NuSplineFindPartial(struct nugscn_s *scene, char *name, char *txt);

void InitPlayerEvents(void) {
    struct plrevent_s *ev;
    struct nugspline_s *spl;
    struct nuvec_s *p0;
    struct nuvec_s *p1;
    char name[256];
    char obj_name[32];
    s32 i;
    s32 j;
    char c;
    s32 len;
    s32 n;
    s32 found;
    s32 ok;

    if (world_scene[0] != 0) {
        ev = PlrEvent;
        for (i = 0; i < 8; i++, ev++) {
            ev->spl = 0;
            ev->triggered = 0;
            sprintf(tbuf, "player_trigger_%.2i_", i);
            len = strlen(tbuf);
            ev->spl = NuSplineFindPartial(world_scene[0], tbuf, name);
            if (ev->spl != 0) {
                if (ev->spl->len != 2) {
                    ev->spl = 0;
                }
                spl = ev->spl;
                if (spl != 0) {
                    p0 = (struct nuvec_s *)spl->pts;
                    p1 = (struct nuvec_s *)(spl->pts + spl->ptsize);
                    ev->pos.x = (p0->x + p1->x) * 0.5f;
                    ev->pos.y = (p0->y + p1->y) * 0.5f;
                    ev->pos.z = (p0->z + p1->z) * 0.5f;
                    GetALONG(&ev->pos, 0, -1, -1, 1);
                    ev->iRAIL = temp_iRAIL;
                    ev->iALONG = temp_iALONG;
                    ev->fALONG = temp_fALONG;
                    for (j = 15; j >= 0; j--) {
                        ev->obj[j].special = 0;
                    }
                    found = 0;
                    n = strlen(name);
                    j = 0;
                    for (; len < n; len++) {
                        c = name[len];
                        if (c == 'x' || len == n - 1) {
                            if (len == n - 1) {
                                obj_name[j++] = c;
                            }
                            obj_name[j] = 0;
                            ok = NuSpecialFind(world_scene[0], &ev->obj[found], obj_name);
                            j = 0;
                            if (ok != 0) {
                                found++;
                                if (found == 16) {
                                    break;
                                }
                            }
                        } else {
                            obj_name[j++] = c;
                        }
                    }
                }
            }
        }
    }
}


void UpdateChaseRunAnim(struct creature_s *plr) {
    if ((plr->obj.anim.newaction == 0x3A) &&
        ((Chase[0].status == 2) || (Chase[1].status == 2) ||
         (Chase[2].status == 2)) &&
        (best_cRPos != 0) &&
        (abs(RotDiff(plr->obj.hdg, best_cRPos->angle)) < 0x2000)) {
        if (plr_tocam_wait != 0) {
            plr_tocam_wait--;
            if (plr_tocam_wait == 0) {
                plr_tocam_wait = ((qrand() * 50) >> 16) + 50;
                if (plr_tocam_turn == -1) {
                    plr_tocam_turn = qrand() / 0x8000;
                } else if (plr_tocam_turn == 1) {
                    plr_tocam_turn = -qrand() / 0x8000;
                } else {
                    plr_tocam_turn = (qrand() < 0x8000) ? -1 : 1;
                }
            }
        }
    } else {
        plr_tocam_turn = 0;
        plr_tocam_wait = ((qrand() * 50) >> 16) + 50;
    }
    if (plr->obj.anim.newaction == 0x3A) {
        if (plr_tocam_turn < 0) {
            plr->obj.anim.newaction = 0x32;
        } else if (plr_tocam_turn > 0) {
            plr->obj.anim.newaction = 0x39;
        } else {
            plr->obj.anim.newaction = 0x3A;
        }
    }
}

extern u64 LBIT;
extern struct nuvec_s loadsavepos;

void MoveCRASH(struct creature_s *plr, struct nupad_s *pad) {
    struct MoveInfo *minfo = &CrashMoveInfo;
    s32 jump_pressed;
    s32 spin_pressed;
    s32 crouch_pressed;
    s32 duck_pressed;
    s32 sprint_pressed;
    s32 tiptoe_pressed;
    s32 target_pressed;
    s32 can_jump;
    s32 frames;
    s32 old_target;

    jump_pressed = pad->paddata & 0x40;
    spin_pressed = pad->paddata & 0x80;
    sprint_pressed = pad->paddata & 0x2;
    crouch_pressed = pad->paddata & 0x20;
    duck_pressed = pad->paddata & 0x8;
    tiptoe_pressed = pad->paddata & 0x1;
    target_pressed = (ExtraMoves != 0 || (Game.powerbits & 8) != 0) && Level != 0x25 &&
                     (LBIT & 0x1E00000) == 0 && (pad->paddata & 0x4) != 0;
    can_jump = (plr->obj.max.y - plr->obj.min.y) * plr->obj.SCALE < plr->obj.clearance;
    if (menu_mode == 0x1F || menu_mode == 0x21 || GameTimer.frame < 0x32 || fadeval > 0) {
        target_pressed = 0;
        duck_pressed = 0;
        tiptoe_pressed = 0;
        crouch_pressed = 0;
        sprint_pressed = 0;
        spin_pressed = 0;
        jump_pressed = 0;
    }
    if (plr_tumblehack != 0) {
        plr_tumblehack--;
    }
    if (plr->target != 0 && plr->fire == 0 && plr->fire_lock == 0 && plr->tap == 0 &&
        (pad->paddata_db & 0x28) != 0) {
        AddFlyingWumpa(&plr_target_pos[0], &plr_target_dir, &plr_target_pos[1], 1);
        frames = (s32)(ModelAnimDuration(0, 0x4F, 0.0f, 0.0f) * 50.0f);
        plr->obj.anim.anim_time = 1.0f;
        plr->fire = frames;
        plr->tap = frames + 10;
        AddGameDebrisMtx(0x51, &plr_target_firepos, 0x20, &plr_target_mtx);
        AddGameDebrisMtx(0x52, &plr_target_firepos, 0x10, &plr_target_mtx);
        GameSfx(6, &plr->obj.pos);
    }
    if (plr->obj.transporting == 0) {
        if (plr->obj.dangle != 0) {
            plr->somersault = 0;
            plr->ok_slam = 0;
            if (plr->jump != 0) {
                plr->jump = 6;
                plr->jump_frame = plr->jump_frames;
                plr->jump_type = 4;
            }
            if (pad->paddata_db & 0x40) {
                plr->obj.dangle = 0;
                plr->obj.mom.y = 0.0f;
                GameSfx(0x1C, &plr->obj.pos);
            } else {
                if (crouch_pressed != 0 || duck_pressed != 0) {
                    plr->obj.dangle = 2;
                }
                plr->obj.pos.y = plr->obj.roof_y - minfo->DANGLEGAP - plr->obj.max.y * plr->obj.SCALE;
                goto landing;
            }
        }
        if (plr->jump != 0 && plr->jump_frame < plr->jump_frames &&
            !(plr->slam == 1 || plr->slam == 2)) {
            f32 jump_frames = minfo->JUMPFRAMES2;
            f32 height = minfo->JUMPHEIGHT;

            if (plr->jump_type == 3) {
                plr->obj.mom.y = (height * 1.5f) / jump_frames;
            } else {
                plr->obj.mom.y = height / jump_frames;
            }
        }
        if (plr->slam == 2) {
            plr->obj.mom.y += GRAVITY * 3.0f;
        } else if (plr->slam == 1 && plr->obj.mom.y > 0.0f) {
            plr->obj.mom.y += GRAVITY * 2.0f;
        } else if (plr->obj.mom.y < 0.0f && (ExtraMoves != 0 || (Game.powerbits & 4) != 0) &&
                   plr->spin != 0 &&
                   plr->spin_frame < plr->spin_frames - plr->minfo->SPINRESETFRAMES &&
                   plr->spin >= 3) {
            if (plr->obj.mom.y > -0.02f) {
                plr->obj.mom.y = -0.02f;
            } else if (plr->spin < 8 && (plr->spin & 1) == 0) {
                plr->obj.mom.y += GRAVITY * 0.2f;
            }
        } else if ((plr->obj.ground & 1) == 0) {
            plr->obj.mom.y += GRAVITY;
        }
        if (plr->jump != 0) {
            if (plr->somersault > 1) {
                plr->somersault--;
            }
            if (plr->jump_frame < plr->jump_frames) {
                plr->jump_frame++;
                if (plr->ok_slam != 0 && plr->jump_type != 0 && plr->jump_frame == plr->jump_frames &&
                    (plr->jump >= 4 || minfo->JUMPFRAMES1 < plr->jump_frame) &&
                    plr->obj.pad_speed > 0.0f) {
                    plr->somersault = minfo->SOMERSAULTFRAMES;
                    gamesfx_effect_volume = 0x2FFF;
                    GameSfx(0x2F, &plr->obj.pos);
                }
            }
        } else {
            plr->somersault = 0;
        }
        if (jump_pressed != 0 || plr->jump_hack != 0 || plr->obj.boing != 0) {
            if (plr->jump_hack != 0) {
                plr->jump_hack = 0;
            }
            switch (plr->jump) {
            case 0:
                if (plr->obj.ground != 0 && plr->obj.idle_gametime > 0.0f &&
                    NuVecDistSqr(&plr->obj.pos, &loadsavepos, 0) < 1.0f) {
                    break;
                }
                if ((plr->obj.ground != 0 || plr->allow_jump != 0) && plr->slam_wait == 0 &&
                    can_jump != 0 && plr->obj.transporting == 0 && plr->target == 0 &&
                    plr->fire == 0 && plr_rebound == 0 && plr->freeze == 0 &&
                    (Level != 0x16 || FireBossHoldPlayer == 0)) {
                    if (plr->crawl != 0 || plr->slide != 0) {
                        plr->jump_type = 0;
                        plr->jump_subtype = qrand() / 0x8000;
                    } else {
                        plr->jump_type = 1;
                    }
                    plr->jump_frames = (plr->jump_type == 0) ? minfo->STARJUMPFRAMES : minfo->JUMPFRAMES0;
                    plr->jump_frame = 0;
                    plr->jump = 1;
                    plr->somersault = 0;
                    plr->land = 0;
                    if (plr->jump_type == 0) {
                        plr->crawl = 0;
                        plr->slide = 0;
                        plr->ok_slam = 0;
                    } else {
                        plr->ok_slam = 1;
                    }
                    plr->obj.ground = 0;
                    plr->obj.anim.anim_time = 1.0f;
                    plr->jump_hack = 1;
                    AddGameDebris(0x10, &plr->obj.pos);
                    AddGameDebris(0x11, &plr->obj.pos);
                    GameSfx(0x2F, &plr->obj.pos);
                }
                break;
            case 1:
                if (plr->jump_type != 0 && plr->jump_frames < minfo->JUMPFRAMES2) {
                    plr->jump_frames++;
                } else {
                    plr->jump = 2;
                }
                break;
            case 2:
                break;
            case 3:
                if ((ExtraMoves != 0 || (Game.powerbits & 2) != 0) && plr->obj.mom.y > -0.099999994f &&
                    (plr->spin == 0 ||
                     plr->spin_frame >= plr->spin_frames - plr->minfo->SPINRESETFRAMES)) {
                    if (plr->obj.pad_speed == 0.0f) {
                        plr->jump_type = 0;
                        plr->jump_subtype = qrand() / 0x8000;
                    } else {
                        plr->jump_type = 1;
                    }
                    plr->jump_frames = minfo->JUMPFRAMES0;
                    plr->jump_frame = 0;
                    plr->somersault = 0;
                    plr->jump = 4;
                    plr->obj.anim.anim_time = 1.0f;
                    plr->obj.mom.x = plr->obj.mom.z = 0.0f;
                    AddGameDebris(0x10, &plr->obj.pos);
                    AddGameDebris(0x11, &plr->obj.pos);
                    GameSfx(0x2F, &plr->obj.pos);
                }
                break;
            case 4:
                if (plr->jump_frames < minfo->JUMPFRAMES2) {
                    plr->jump_frames++;
                } else {
                    plr->jump = 5;
                }
                break;
            }
        } else if (plr->obj.ground != 0) {
            plr->jump = 0;
        } else if (plr->jump == 1 || plr->jump == 2) {
            plr->jump = 3;
        } else if (plr->jump == 4) {
            plr->jump = 5;
        }
    }
landing:
    if (plr->land != 0) {
        plr->land--;
    } else if (plr->obj.ground != 0 && plr->obj.old_ground == 0 && plr->obj.pad_speed == 0.0f &&
               plr->jump != 1 && plr_allow_jump == 0) {
        plr->land = minfo->JUMPLANDFRAMES;
        AddGameDebris(0x10, &plr->obj.pos);
    }
    if (plr->spin_wait != 0) {
        plr->spin_wait--;
    }
    if (plr->spin != 0 && plr->spin_frame < plr->spin_frames) {
        plr->spin_frame++;
    }
    if (spin_pressed != 0) {
        if (plr->spin == 0 && plr->obj.wade == 0 && plr->target == 0 && plr_rebound == 0 &&
            (Level != 0x16 || FireBossHoldPlayer == 0)) {
            if ((plr->slam == 0 || plr->slam >= 3) && plr->crawl == 0 && plr->slide == 0 &&
                plr->freeze == 0) {
                plr->spin_frame = 0;
                plr->spin = 1;
                plr->spin_frames = minfo->SPINFRAMES + minfo->SPINRESETFRAMES;
                plr->spin_wait = minfo->SUPERSPINWAITFRAMES;
                GameSfx(0x13, &plr->obj.pos);
                NewRumble(&plr->rumble, 0x7F);
            }
        } else if ((ExtraMoves != 0 || (Game.powerbits & 4) != 0) && plr->spin < 7) {
            if (plr->spin_wait != 0 && (plr->spin & 1) == 0) {
                plr->spin++;
                plr->spin_frames += minfo->SUPERSPINFRAMES;
                plr->spin_wait = minfo->SUPERSPINWAITFRAMES;
                GameSfx(0x13, &plr->obj.pos);
                NewRumble(&plr->rumble, 0x7F);
            }
        }
    } else if (plr->spin_frame == plr->spin_frames) {
        plr->spin = 0;
    } else if ((ExtraMoves != 0 || (Game.powerbits & 4) != 0) && (plr->spin & 1) != 0) {
        plr->spin++;
    }
    if (plr->slam_wait != 0) {
        plr->slam_wait--;
        if (plr->slam_wait == 0) {
            plr->slam = 3;
        }
    } else if (plr->slam != 0 && plr->slam < 3 && plr->obj.ground != 0 && plr->obj.old_ground == 0) {
        if (plr->obj.boing != 0) {
            plr->slam_wait = 0;
            plr->slam = 3;
        } else {
            plr->slam_wait = minfo->SLAMWAITFRAMES;
            GameSfx(0x11, &plr->obj.pos);
            if (plr->slam == 2) {
                AddKaboom(8, &plr->obj.pos, 0.0f);
            } else {
                JudderGameCamera(GameCam, 0.2f, 0);
            }
            AddGameDebris(0xD, &plr->obj.pos);
            AddGameDebris(0xE, &plr->obj.pos);
        }
    }
    if (crouch_pressed != 0 || duck_pressed != 0) {
        if (plr->slam == 0 && plr->ok_slam != 0 && plr->jump > 0 && plr->jump < 4 && plr->spin == 0 &&
            plr->target == 0 && plr_rebound == 0 && plr->freeze == 0) {
            if (plr->obj.mom.y > 0.0f) {
                if (ExtraMoves != 0 || (Game.powerbits & 0x20) != 0) {
                    f32 height;
                    f32 jump_frames;

                    plr->slam = 2;
                    height = minfo->JUMPHEIGHT;
                    jump_frames = minfo->JUMPFRAMES2;
                    plr->obj.mom.y = ((plr->jump_type == 3) ? (height * 1.5f) / jump_frames
                                                            : height / jump_frames) * 4.0f;
                } else {
                    f32 height;
                    f32 jump_frames;

                    plr->slam = 1;
                    height = minfo->JUMPHEIGHT;
                    jump_frames = minfo->JUMPFRAMES2;
                    plr->obj.mom.y = ((plr->jump_type == 3) ? (height * 1.5f) / jump_frames
                                                            : height / jump_frames) * 2.0f;
                }
                plr->slam_frame = 0;
            }
        }
    } else if (plr->obj.ground != 0 && plr->slam_wait == 0) {
        plr->slam = 0;
    }
    if (plr->slam != 0 && plr->slam < 3) {
        plr->slam_frame++;
    }
    plr->tiptoe = (ExtraMoves != 0 || (Game.powerbits & 1) != 0) && tiptoe_pressed != 0 &&
                  plr->obj.ground != 0 && plr->target == 0 && plr_rebound == 0 &&
                  (Level != 0x16 || FireBossHoldPlayer == 0);
    plr->sprint = (ExtraMoves != 0 || (Game.powerbits & 0x10) != 0) && sprint_pressed != 0 &&
                  (plr->obj.ground != 0 || plr_allow_jump != 0) && plr->target == 0 &&
                  plr_rebound == 0 && (Level != 0x16 || FireBossHoldPlayer == 0);
    if ((duck_pressed != 0 || crouch_pressed != 0 || (plr->obj.ground != 0 && can_jump == 0)) &&
        (Level != 0x16 || FireBossHoldPlayer == 0)) {
        if (plr->target == 0) {
            if (plr->crouch_pos < minfo->CROUCHINGFRAMES &&
                (plr->crouch_pos != 0 || plr->crawl_lock == 0)) {
                plr->crouch_pos++;
            }
            if (plr->slide != 0) {
                plr->slide--;
                if (plr->slide == 0) {
                    plr->crawl = 1;
                    plr->obj.mom.x = plr->obj.mom.z = 0.0f;
                }
            } else if (plr->obj.ground != 0 && plr->crawl_lock == 0) {
                if (plr->spin == 0 ||
                    plr->spin_frame >= plr->spin_frames - plr->minfo->SPINRESETFRAMES) {
                    if (plr->obj.pad_speed > 0.0f && plr->crawl == 0 && plr_rebound == 0 &&
                        plr->freeze == 0 && (Level != 0x16 || FireBossHoldPlayer == 0)) {
                        plr->slide = minfo->SLIDEFRAMES;
                        GameSfx(0x12, &plr->obj.pos);
                    } else {
                        plr->crawl = 1;
                    }
                }
            }
        }
        plr->crawl_lock = 1;
    } else {
        if (plr->crouch_pos > 0) {
            plr->crouch_pos--;
        }
        plr->crawl = 0;
        if (plr->slide != 0) {
            plr->slide--;
            if (plr->slide == 0 &&
                (plr->obj.ground == 0 || (TerSurface[plr->obj.surface_type].flags & 0x20) == 0)) {
                plr->obj.mom.x = plr->obj.mom.z = 0.0f;
            }
        }
        if (plr->slide == 0 && plr->crawl == 0 && plr->obj.ground != 0) {
            plr->crawl_lock = 0;
        }
    }
    old_target = plr->target;
    if (target_pressed != 0 && plr->obj.ground != 0 && plr->jump == 0 && plr->crouch_pos == 0 &&
        plr->slam_wait == 0 && plr->crawl == 0 && plr->slide == 0 &&
        !(plr->spin != 0 && plr->spin_frame < plr->spin_frames - plr->minfo->SPINRESETFRAMES) &&
        plr->obj.dangle == 0 && plr_rebound == 0 && plr->freeze == 0 &&
        (Level != 0x16 || FireBossHoldPlayer == 0)) {
        plr->target = 1;
    } else {
        plr->target = 0;
    }
    if (plr->target != 0 && old_target == 0) {
        plr->obj.target_yrot = 0;
        plr->obj.target_xrot = 0;
    }
    plr->obj.attack = 0;
    if (plr->spin != 0 && plr->spin_frame < plr->spin_frames - plr->minfo->SPINRESETFRAMES) {
        plr->obj.attack = 2;
    } else if (plr->slam != 0 && plr->slam < 3 && plr->obj.ground == 0 && plr->slam_wait == 0) {
        plr->obj.attack |= (ExtraMoves != 0 || (Game.powerbits & 0x20) != 0) ? 8 : 4;
    } else if (plr->slide != 0) {
        plr->obj.attack |= 0x10;
    } else if (plr->obj.ground == 0 && plr->obj.mom.y <= 0.0f) {
        plr->obj.attack |= 0x20;
    }
    if (plr->obj.mask != 0 && plr->obj.mask->active >= 3) {
        plr->obj.attack |= 0x80;
    }
}


void AnimateCRASH(struct creature_s *plr) {
    struct MoveInfo *info = &CrashMoveInfo;
    struct nuvec_s pos;
    s16 action;

    if ((u32)plr->obj.dead >= 2) {
        plr->obj.anim.newaction = plr->obj.die_action;
    } else if (GameMode == 1) {
        plr->obj.anim.newaction = 0x22;
    } else if ((Level == 0x25) && (tumble_time < tumble_duration) &&
               (last_hub != -1)) {
        plr->obj.anim.newaction = tumble_action;
    } else if (plr_tumblehack != 0) {
        plr->obj.anim.newaction = 0x22;
    } else if (plr_rebound != 0) {
        plr->obj.anim.newaction = 0x19;
    } else if (plr->freeze != 0) {
        plr->obj.anim.newaction = 0x1E;
    } else if (plr->target != 0) {
        if (plr->fire != 0) {
            plr->obj.anim.newaction = 0x4F;
        } else if (plr->obj.dyrot != 0) {
            plr->obj.anim.newaction = 0x50;
        } else {
            plr->obj.anim.newaction = 0x4E;
        }
    } else if (plr->obj.dangle != 0) {
        if (plr->obj.dangle == 2) {
            plr->obj.anim.newaction = 0x36;
        } else if (plr->obj.pad_speed != 0.0f) {
            plr->obj.anim.newaction = 0x4D;
        } else {
            plr->obj.anim.newaction = 0x20;
        }
    } else if (plr->slam_wait != 0) {
        plr->obj.anim.newaction = 0x1F;
    } else if (plr->slam == 2) {
        plr->obj.anim.newaction = 0x4B;
    } else if (plr->slam == 1) {
        plr->obj.anim.newaction = 0x41;
    } else if (plr->slide != 0) {
        plr->obj.anim.newaction = 0x43;
    } else if ((plr->jump != 0) &&
               ((plr->obj.ground == 0) || (plr->jump_frame < plr->jump_frames))) {
        if (plr->somersault != 0) {
            plr->obj.anim.newaction = 0x44;
        } else if (plr->jump_type == 4) {
            plr->obj.anim.newaction = 0x19;
        } else if (plr->jump_type == 0) {
            plr->obj.anim.newaction = (plr->jump_subtype == 1) ? 0x2D : 0x2E;
        } else if (plr->jump == 5) {
            plr->obj.anim.newaction = 0x44;
        } else {
            plr->obj.anim.newaction = 0x2C;
        }
    } else if ((plr->land != 0) && (plr->obj.pad_speed == 0.0f)) {
        plr->obj.anim.newaction = 0x30;
    } else if ((plr->crouch_pos > 0) &&
               (plr->crouch_pos < plr->minfo->CROUCHINGFRAMES) &&
               (plr->obj.pad_speed == 0.0f)) {
        plr->obj.anim.newaction = (plr->crawl != 0) ? 3 : 5;
    } else if ((plr->crawl != 0) && ((plr->jump == 0) || (plr->obj.ground != 0))) {
        if (plr->obj.pad_speed == 0.0f) {
            plr->obj.anim.newaction = 4;
        } else {
            plr->obj.anim.newaction = 2;
        }
    } else if ((plr->obj.wade != 0) && (plr->obj.ground != 0) &&
               (plr->obj.pad_speed != 0.0f)) {
        plr->obj.anim.newaction = 0x72;
    } else if (((plr->obj.ground != 0) || (plr->allow_jump != 0)) &&
               (TerSurface[plr->obj.surface_type].flags & 0x20) &&
               ((plr->obj.pad_speed != 0.0f) ||
                (plr->obj.xz_distance >= info->IDLESPEED))) {
        if (plr->obj.pad_speed != 0.0f) {
            plr->obj.anim.newaction = 0x3F;
        } else {
            plr->obj.anim.newaction = 0x40;
        }
    } else if ((plr->obj.ground == 0) && (plr->allow_jump == 0) &&
               (plr->obj.mom.y < -FALLONCRATEBREAKSPEED)) {
        plr->obj.anim.newaction = 0x19;
    } else if ((plr->obj.pad_speed == 0.0f) &&
               !((plr->pad_type == 1) &&
                 (plr->obj.mom.x * plr->obj.mom.x + plr->obj.mom.z * plr->obj.mom.z >=
                  plr->minfo->IDLESPEED * plr->minfo->IDLESPEED))) {
        if (((plr->obj.ground == 1) ||
             ((plr->obj.last_ground == 1) && (plr->allow_jump != 0))) &&
            ((plr->obj.pos.y + plr->obj.bot * plr->obj.SCALE) - plr->obj.shadow > 0.25f)) {
            plr->obj.anim.newaction = 0x51;
        } else {
            plr->obj.anim.newaction = 0x22;
        }
    } else if (plr->tiptoe != 0) {
        plr->obj.anim.newaction = 0x53;
    } else if (plr->sprint != 0) {
        plr->obj.anim.newaction = 0x48;
    } else if (plr->obj.pad_speed == info->WALKSPEED) {
        plr->obj.anim.newaction = 0x74;
    } else {
        plr->obj.anim.newaction = 0x3A;
    }

    UpdateChaseRunAnim(plr);

    action = plr->obj.anim.newaction;
    if ((action == 2) || (action == 0x43) || (action == 4)) {
        plr->crouch_pos = info->CROUCHINGFRAMES;
    } else if ((plr->obj.pad_speed != 0.0f) || ((action != 3) && (action != 5))) {
        plr->crouch_pos = 0;
    }
    UpdateCharacterIdle(plr, 0);

    if ((plr->obj.dead == 0) && (plr->spin != 0) &&
        (plr->spin_frame < plr->spin_frames - plr->minfo->SPINRESETFRAMES)) {
        pos.x = plr->obj.pos.x;
        pos.y = plr->obj.pos.y + (plr->obj.bot + plr->obj.top) * plr->obj.SCALE * 0.5f +
                (plr->obj.top - plr->obj.bot) * plr->obj.SCALE *
                    (qrand() * (1.0f / 65535.0f) - 0.5f);
        pos.z = plr->obj.pos.z;
        AddGameDebrisRot(0xB, &pos, 1, 0x4000, qrand());
        AddGameDebrisRot(0xC, &plr->obj.pos, 1, 0x2000, qrand());
    }
}


void MoveSWIMMING(struct creature_s *plr, struct nupad_s *pad) {
    struct MoveInfo *minfo = &SwimmingMoveInfo;
    s32 spin_pressed;

    spin_pressed = pad->paddata & 0xA8;
    GameSfxLoop(0x27, &plr->obj.pos);
    if (plr->tap == 0 && (pad->paddata_db & 0x40) != 0) {
        plr->tap = 10;
        gamesfx_pitch = 0x583;
        GameSfx(0x28, &plr->obj.pos);
    }
    if (plr->spin_wait != 0) {
        plr->spin_wait--;
    }
    if (plr->spin != 0 && plr->spin_frame < plr->spin_frames) {
        plr->spin_frame++;
    }
    if (spin_pressed != 0) {
        if (plr->spin == 0) {
            plr->spin_frame = 0;
            plr->spin = 1;
            plr->spin_frames = minfo->SPINFRAMES + minfo->SPINRESETFRAMES;
            plr->spin_wait = minfo->SUPERSPINWAITFRAMES;
            GameSfx(0x28, &plr->obj.pos);
        } else if ((ExtraMoves != 0 || (Game.powerbits & 4) != 0) && plr->spin < 7) {
            if (plr->spin_wait != 0 && (plr->spin & 1) == 0) {
                plr->spin++;
                plr->spin_frames += minfo->SUPERSPINFRAMES;
                plr->spin_wait = minfo->SUPERSPINWAITFRAMES;
                GameSfx(0x28, &plr->obj.pos);
            }
        }
    } else if (plr->spin_frame == plr->spin_frames) {
        plr->spin = 0;
    } else if ((ExtraMoves != 0 || (Game.powerbits & 4) != 0) && (plr->spin & 1) != 0) {
        plr->spin++;
    }
    plr->obj.attack = 0;
    if (plr->spin != 0 &&
        plr->spin_frame < plr->spin_frames - plr->minfo->SPINRESETFRAMES) {
        plr->obj.attack = 2;
    }
    if (plr->obj.mask != 0 && plr->obj.mask->active >= 3) {
        plr->obj.attack |= 0x80;
    }
}


void MoveCOCO(struct creature_s *plr, struct nupad_s *pad) {
    struct MoveInfo *minfo = &CocoMoveInfo;
    s32 can_jump;
    s32 sprint_pressed;
    s32 jump_pressed;
    s32 spin_pressed;
    s32 duck_pressed;
    s32 crouch_pressed;

    jump_pressed = pad->paddata & 0x40;
    spin_pressed = pad->paddata & 0x80;
    crouch_pressed = pad->paddata & 0x20;
    duck_pressed = pad->paddata & 0x8;
    sprint_pressed = pad->paddata & 0x2;
    can_jump = (plr->obj.max.y - plr->obj.min.y) * plr->obj.SCALE < plr->obj.clearance;
    if (menu_mode == 0x1F || menu_mode == 0x21 || GameTimer.frame < 0x32 || fadeval > 0) {
        sprint_pressed = 0;
        duck_pressed = 0;
        crouch_pressed = 0;
        spin_pressed = 0;
        jump_pressed = 0;
    }
    if (plr->obj.transporting == 0 && plr->jump != 0 && plr->jump_frame < plr->jump_frames &&
        !(plr->slam == 1 || plr->slam == 2)) {
        f32 frames = minfo->JUMPFRAMES2;
        f32 height = minfo->JUMPHEIGHT;

        if (plr->jump_type == 3) {
            plr->obj.mom.y = (height * 1.5f) / frames;
        } else {
            plr->obj.mom.y = height / frames;
        }
    }
    if (plr->slam == 1) {
        plr->obj.mom.y += GRAVITY * 2.0f;
    } else if ((plr->obj.ground & 1) == 0) {
        plr->obj.mom.y += GRAVITY;
    }
    if (plr->jump != 0) {
        if (plr->somersault > 1) {
            plr->somersault--;
        }
        if (plr->jump_frame < plr->jump_frames) {
            plr->jump_frame++;
            if (plr->ok_slam != 0 && plr->jump_type != 0 && plr->jump_frame == plr->jump_frames &&
                (plr->jump >= 4 || minfo->JUMPFRAMES1 < plr->jump_frame) &&
                plr->obj.pad_speed > 0.0f) {
                plr->somersault = minfo->SOMERSAULTFRAMES;
                gamesfx_effect_volume = 0x2FFF;
                GameSfx(0x2F, &plr->obj.pos);
            }
        }
    } else {
        plr->somersault = 0;
    }
    if (jump_pressed != 0 || plr->jump_hack != 0 || plr->obj.boing != 0) {
        if (plr->jump_hack != 0) {
            plr->jump_hack = 0;
        }
        switch (plr->jump) {
        case 0:
            if ((plr->obj.ground != 0 || plr->allow_jump != 0) && plr->slam_wait == 0 &&
                plr->slide == 0 && can_jump != 0 && plr->obj.transporting == 0) {
                plr->jump_type = (plr->obj.pad_speed == 0.0f) ? 2 : 1;
                plr->jump_frames = minfo->JUMPFRAMES0;
                plr->jump_frame = 0;
                plr->jump = 1;
                plr->somersault = 0;
                plr->land = 0;
                plr->ok_slam = 1;
                AddGameDebris(0x10, &plr->obj.pos);
                AddGameDebris(0x11, &plr->obj.pos);
                plr->jump_hack = 1;
                plr->obj.ground = 0;
                plr->obj.anim.anim_time = 1.0f;
                GameSfx(0x2F, &plr->obj.pos);
            }
            break;
        case 1:
            if (plr->jump_frames < minfo->JUMPFRAMES2) {
                plr->jump_frames++;
            } else {
                plr->jump = 2;
            }
            break;
        }
    } else if (plr->obj.ground != 0) {
        plr->jump = 0;
    } else if (plr->jump == 1 || plr->jump == 2) {
        plr->jump = 3;
    }
    if (plr->land != 0) {
        plr->land--;
    } else if (plr->obj.ground != 0 && plr->obj.old_ground == 0 && plr->obj.pad_speed == 0.0f &&
               plr->jump != 1 && plr->allow_jump == 0) {
        plr->land = minfo->JUMPLANDFRAMES;
    }
    if (plr->spin_wait != 0) {
        plr->spin_wait--;
    }
    if (plr->spin != 0 && plr->spin_frame < plr->spin_frames) {
        plr->spin_frame++;
    }
    if (spin_pressed != 0) {
        if (plr->spin == 0 && plr->obj.wade == 0 && (plr->slam == 0 || plr->slam >= 3) &&
            plr->crawl == 0 && plr->slide == 0) {
            plr->spin_frame = 0;
            plr->spin = 1;
            plr->spin_frames = minfo->SPINFRAMES + minfo->SPINRESETFRAMES;
            GameSfx(0x1A, &plr->obj.pos);
            plr->somersault = 0;
            plr->ok_slam = 0;
        }
    } else if (plr->spin_frame == plr->spin_frames) {
        plr->spin = 0;
    }
    if (plr->slam_wait != 0) {
        plr->slam_wait--;
        if (plr->slam_wait == 0) {
            plr->slam = 3;
        }
    } else if (plr->slam != 0 && plr->slam < 3 && plr->obj.ground != 0 && plr->obj.old_ground == 0) {
        if (plr->obj.boing != 0) {
            plr->slam_wait = 0;
            plr->slam = 3;
        } else {
            plr->slam_wait = minfo->SLAMWAITFRAMES;
            GameSfx(0x11, &plr->obj.pos);
            AddKaboom((plr->slam == 2) ? 8 : 4, &plr->obj.pos, 0.0f);
        }
    }
    if (crouch_pressed != 0 || duck_pressed != 0) {
        if (plr->slam == 0 && plr->ok_slam != 0 && plr->jump > 0 && plr->jump < 4 && plr->spin == 0) {
            if (plr->obj.mom.y > 0.0f) {
                f32 height;
                f32 frames;

                plr->slam = 1;
                height = minfo->JUMPHEIGHT;
                frames = minfo->JUMPFRAMES2;
                plr->obj.mom.y = ((plr->jump_type == 3) ? (height * 1.5f) / frames : height / frames) * 2.0f;
            }
            plr->slam_frame = 0;
        }
    } else if (plr->obj.ground != 0 && plr->slam_wait == 0) {
        plr->slam = 0;
    }
    if (plr->slam != 0 && plr->slam < 3) {
        plr->slam_frame++;
    }
    plr->sprint = (ExtraMoves != 0 || (Game.powerbits & 0x10) != 0) && sprint_pressed != 0 &&
                  (plr->obj.ground != 0 || plr_allow_jump != 0) && plr->target == 0;
    if (duck_pressed != 0 || crouch_pressed != 0) {
        if (plr->slide != 0) {
            plr->slide--;
        } else if (plr->obj.ground != 0 && plr->crawl_lock == 0 &&
                   (plr->spin == 0 ||
                    plr->spin_frame >= plr->spin_frames - plr->minfo->SPINRESETFRAMES)) {
            plr->slide = minfo->SLIDEFRAMES;
        }
        plr->crawl_lock = 1;
    } else {
        if (plr->slide != 0) {
            plr->slide--;
        }
        if (plr->slide == 0 && plr->obj.ground != 0) {
            plr->crawl_lock = 0;
        }
    }
    plr->obj.attack = 0;
    if (plr->spin != 0 && plr->spin_frame < plr->spin_frames - plr->minfo->SPINRESETFRAMES) {
        plr->obj.attack = 2;
    } else if (plr->obj.anim.action == 0x43 && plr->obj.anim.anim_time >= SWEEPATTACKSTARTTIME &&
               plr->obj.anim.anim_time < SWEEPATTACKENDTIME) {
        if (plr->obj.anim.anim_time - 0.59999996f < SWEEPATTACKSTARTTIME) {
            GameSfx(0x46, &plr->obj.pos);
        }
        plr->obj.attack |= 0x12;
    } else if (plr->slam != 0 && plr->slam < 3 && plr->obj.ground == 0 && plr->slam_wait == 0) {
        plr->obj.attack |= 4;
    } else if (plr->obj.ground == 0 && plr->obj.mom.y <= 0.0f) {
        plr->obj.attack |= 0x20;
    }
    if (plr->obj.mask != 0 && plr->obj.mask->active >= 3) {
        plr->obj.attack |= 0x80;
    }
}


void AnimateCOCO(struct creature_s *plr) {
    struct MoveInfo *info = &CocoMoveInfo;

    if ((u32)plr->obj.dead >= 2) {
        plr->obj.anim.newaction = plr->obj.die_action;
    } else if ((Level == 0x25) && (tumble_time < tumble_duration) &&
               (last_hub != -1)) {
        plr->obj.anim.newaction = tumble_action;
    } else if ((plr->spin != 0) &&
               (plr->spin_frame <
                plr->spin_frames - plr->minfo->SPINRESETFRAMES)) {
        plr->obj.anim.newaction = 0x46;
    } else if (plr->slam_wait != 0) {
        plr->obj.anim.newaction = 0x1F;
    } else if (plr->slam == 1) {
        plr->obj.anim.newaction = 0x41;
    } else if (plr->slide != 0) {
        plr->obj.anim.newaction = 0x43;
    } else if ((plr->jump != 0) &&
               ((plr->obj.ground == 0) || (plr->jump_frame < plr->jump_frames))) {
        if (plr->somersault != 0) {
            plr->obj.anim.newaction = 0x44;
        } else if (plr->jump_type == 4) {
            plr->obj.anim.newaction = 0x19;
        } else if (plr->jump_type == 2) {
            plr->obj.anim.newaction = 0x2D;
        } else {
            plr->obj.anim.newaction = 0x2C;
        }
    } else if ((plr->land != 0) && (plr->obj.pad_speed == 0.0f)) {
        if (plr->jump_type == 2) {
            plr->obj.anim.newaction = 0x31;
        } else {
            plr->obj.anim.newaction = 0x30;
        }
    } else if (((plr->obj.ground != 0) || (plr->allow_jump != 0)) &&
               (TerSurface[plr->obj.surface_type].flags & 0x20) &&
               ((plr->obj.pad_speed != 0.0f) ||
                (plr->obj.xz_distance >= info->IDLESPEED))) {
        plr->obj.anim.newaction = 0x40;
    } else if ((plr->obj.ground == 0) && (plr->allow_jump == 0) &&
               (plr->obj.mom.y < -FALLONCRATEBREAKSPEED)) {
        plr->obj.anim.newaction = 0x19;
    } else if ((plr->obj.pad_speed == 0.0f) &&
               !((plr->pad_type == 1) &&
                 (plr->obj.mom.x * plr->obj.mom.x + plr->obj.mom.z * plr->obj.mom.z >=
                  plr->minfo->IDLESPEED * plr->minfo->IDLESPEED))) {
        plr->obj.anim.newaction = 0x22;
    } else if (plr->sprint != 0) {
        plr->obj.anim.newaction = 0x48;
    } else if (plr->obj.pad_speed == info->WALKSPEED) {
        plr->obj.anim.newaction = 0x74;
    } else {
        plr->obj.anim.newaction = 0x3A;
    }
    UpdateCharacterIdle(plr, 1);
}


void MoveSCOOTER(struct creature_s *plr, struct nupad_s *pad) {
    struct MoveInfo *minfo = &ScooterMoveInfo;
    s32 jump_pressed;
    f32 height;
    f32 frames;

    jump_pressed = pad->paddata & 0x40;
    if (plr->obj.ground != 0) {
        GameSfxLoop(0xD0, &plr->obj.pos);
    }
    if (plr->jump != 0 && plr->jump_frame < plr->jump_frames) {
        frames = minfo->JUMPFRAMES2;
        height = minfo->JUMPHEIGHT;
        if (plr->jump_type == 3) {
            plr->obj.mom.y = (height * 1.5f) / frames;
        } else {
            plr->obj.mom.y = height / frames;
        }
    }
    if ((plr->obj.ground & 1) == 0) {
        plr->obj.mom.y += GRAVITY;
    }
    if (plr->jump != 0 && plr->jump_frame < plr->jump_frames) {
        plr->jump_frame++;
    }
    if (jump_pressed != 0 || plr->jump_hack != 0 || plr->obj.boing != 0) {
        if (plr->jump_hack != 0) {
            plr->jump_hack = 0;
        }
        switch (plr->jump) {
        case 0:
            if ((plr->obj.ground != 0 || plr->allow_jump != 0) && plr->land == 0) {
                plr->jump_type = 1;
                plr->jump_frames = minfo->JUMPFRAMES0;
                plr->jump_frame = 0;
                plr->jump = 1;
                plr->land = 0;
                AddGameDebris(0x10, &plr->obj.pos);
                AddGameDebris(0x11, &plr->obj.pos);
                plr->jump_hack = 1;
                plr->obj.ground = 0;
                plr->obj.anim.anim_time = 1.0f;
                GameSfx(0xCC, &plr->obj.pos);
                GameSfx(0x2F, &plr->obj.pos);
            }
            break;
        case 1:
            if (plr->jump_frames < minfo->JUMPFRAMES2) {
                plr->jump_frames++;
            } else {
                plr->jump = 2;
            }
            break;
        }
    } else if (plr->obj.ground != 0) {
        plr->jump = 0;
    } else if (plr->jump == 1 || plr->jump == 2) {
        plr->jump = 3;
    }
    if (plr->land != 0) {
        if (plr->obj.anim.flags & 1) {
            plr->land = 0;
        }
    } else if (plr->obj.ground != 0 && plr->obj.old_ground == 0 && plr->jump != 1 &&
               plr_allow_jump == 0 && plr->obj.anim.action == 0x63) {
        plr->land = 1;
        AddGameDebris(0x10, &plr->obj.pos);
        GameSfx(0xCD, &plr->obj.pos);
    }
    plr->obj.attack = 0;
    if (plr->obj.mask != 0 && plr->obj.mask->active >= 3) {
        plr->obj.attack = 0x80;
    }
    plr->obj.attack |= 0x400;
}



void MoveFIREENGINE(struct creature_s *plr, struct nupad_s *pad) {
    struct MoveInfo *minfo = &FireEngineMoveInfo;
    s32 jump_pressed;
    f32 height;
    f32 frames;

    jump_pressed = pad->paddata & 0x40;
    if (plr->obj.ground != 0 && plr->jump != 0 && plr->jump_frame == plr->jump_frames &&
        plr->jump_hold == 0) {
        GameSfx(0x48, &plr->obj.pos);
    }
    if (plr->jump != 0 && plr->jump_frame < plr->jump_frames && plr->jump_hold == 0) {
        frames = minfo->JUMPFRAMES2;
        height = minfo->JUMPHEIGHT;
        if (plr->jump_type == 3) {
            plr->obj.mom.y = (height * 1.5f) / frames;
        } else {
            plr->obj.mom.y = height / frames;
        }
    }
    if ((plr->obj.ground & 1) == 0) {
        plr->obj.mom.y += GRAVITY;
    }
    if (plr->jump != 0 && plr->jump_hold == 0 && plr->jump_frame < plr->jump_frames) {
        plr->jump_frame++;
    }
    if (jump_pressed != 0 || plr->jump_hack != 0 || plr->obj.boing != 0 || plr->jump_hold != 0) {
        if (plr->jump_hack != 0) {
            plr->jump_hack = 0;
        }
        switch (plr->jump) {
        case 0:
            if ((plr->obj.ground != 0 || plr->allow_jump != 0) && plr->target == 0 &&
                (*(u32 *)&plr->fire_action & 0xFFFF0000) == 0 && plr->obj.transporting == 0) {
                plr->jump_type = 1;
                plr->jump_frames = minfo->JUMPFRAMES0;
                plr->jump_hold = 0;
                GameSfx(0x2F, &plr->obj.pos);
                AddGameDebris(0x10, &plr->obj.pos);
                AddGameDebris(0x11, &plr->obj.pos);
                plr->jump_frame = 0;
                plr->jump = 1;
                plr->jump_hack = 1;
                plr->land = 0;
                plr->obj.ground = 0;
                plr->obj.anim.anim_time = 1.0f;
                GameSfx(0x32, &plr->obj.pos);
            }
            break;
        case 1:
            if (plr->jump_frames < minfo->JUMPFRAMES2) {
                plr->jump_frames++;
            } else {
                plr->jump = 2;
            }
            break;
        }
    } else if (plr->obj.ground != 0) {
        plr->jump = 0;
    } else if (plr->jump == 1 || plr->jump == 2) {
        plr->jump = 3;
    }
    plr->obj.attack = 0x400;
    if (plr->obj.ground == 0 && plr->obj.mom.y <= 0.0f) {
        plr->obj.attack = 0x420;
    }
    if (plr->obj.mask != 0 && plr->obj.mask->active >= 3) {
        plr->obj.attack |= 0x80;
    }
}


void AnimateFIREENGINE(struct creature_s *plr) {
    if ((u32)plr->obj.dead >= 2) {
        plr->obj.anim.newaction = plr->obj.die_action;
    } else if ((Level == 0x16) && (FireBossHoldPlayer != 0)) {
        plr->obj.anim.newaction = 0x62;
    } else if ((plr->jump != 0) &&
               ((plr->obj.ground == 0) || (plr->jump_frame < plr->jump_frames))) {
        if (plr->jump_type == 2) {
            if (plr->obj.anim.anim_time >= FIREENGINESTANDINGJUMPPAUSEFRAME) {
                plr->obj.anim.anim_time = FIREENGINESTANDINGJUMPPAUSEFRAME;
            }
            plr->obj.anim.newaction = 0x64;
        } else {
            if (plr->obj.anim.anim_time >= FIREENGINERUNNINGJUMPPAUSEFRAME) {
                plr->obj.anim.anim_time = FIREENGINERUNNINGJUMPPAUSEFRAME;
            }
            plr->obj.anim.newaction = 0x63;
        }
    } else if (plr->land != 0) {
        if (plr->jump_type == 2) {
            plr->obj.anim.newaction = 0x64;
        } else {
            plr->obj.anim.newaction = 0x63;
        }
    } else {
        plr->obj.anim.newaction = 0x68;
    }
    UpdateCharacterIdle(plr, 0);
}

extern u16 D_005E7812[];
extern u16 D_005E7A82[];
#define gyro_sfx_volume D_005E7812[0]
#define gyro_loop_sfx_volume D_005E7A82[0]
extern f32 CRATEBALLOONOFFSET;
extern f32 CRATEBALLOONRADIUS;
extern struct cratecube_s *temp_pCrate;
struct creature_s *FindClock(void);
void StartTimeTrial(struct nuvec_s *pos, s32 clock);
void PointAlongSpline(struct nugspline_s *spl, f32 ratio, struct nuvec_s *out, u16 *yrot, u16 *xrot);
void GetLights(struct nuvec_s *pos, struct Nearest_Light_s *lights, s32 glbflag);
s32 FurtherALONG(s32 iRAIL, s32 iALONG, f32 fALONG, s32 iRAIL2, s32 iALONG2, f32 fALONG2);
struct cratecube_s *HitCrateBalloons(struct nuvec_s *pos, f32 radius);
void AddScreenWumpa(f32 x, f32 y, f32 z, s32 n);

void MoveGYRO(struct creature_s *plr, struct nupad_s *pad) {
    struct MoveInfo *minfo = &GyroMoveInfo;
    struct nuvec_s v;
    u16 yrot;
    u16 xrot;
    struct creature_s *clock;
    s32 spin_pressed;
    s32 pitch;
    s32 sfx;
    f32 radius;
    f32 carpet_bot;
    f32 carpet_top;
    f32 carpet_mid;

    spin_pressed = pad->paddata & 0x80;
    gyropos = plr->obj.pos;
    pitch = (s32)(plr->obj.xyz_distance * 5120.0005f);
    if (pitch > 0x200) {
        pitch = 0x400;
    }
    gamesfx_pitch = pitch + 0x200;
    if (Level == 0xF) {
        gamesfx_effect_volume = (gyro_sfx_volume * 2) / 3;
    }
    GameSfxLoop(0x90, &plr->obj.pos);
    gamesfx_pitch = pitch * 4 + 0x800;
    if (Level == 0xF) {
        gamesfx_effect_volume = (gyro_loop_sfx_volume * 2) / 3;
    }
    GameSfxLoop(0x9D, &gyropos);
    if (Level == 0x1D) {
        if (GameTimer.frame == 0 || GameTimer.frame == 50 || GameTimer.frame == 100) {
            sfx = 0x4F;
        } else {
            sfx = (GameTimer.frame == 150) ? 0x4C : -1;
        }
        if (sfx != -1) {
            GameSfx(sfx, 0);
        }
    }
    if (Level != 0x1D || GameTimer.frame >= 150) {
        if (Level == 0x1D) {
            clock = FindClock();
            if (clock != 0) {
                v.x = clock->obj.pos.x;
                v.y = clock->obj.pos.y + CRATEBALLOONOFFSET;
                v.z = clock->obj.pos.z;
                radius = CRATEBALLOONRADIUS + plr->obj.RADIUS * 5.0f;
                if (NuVecDistSqr(&plr->obj.pos, &v, 0) < radius * radius) {
                    NewBuzz(&player->rumble, 5);
                    StartTimeTrial(&clock->obj.pos, 1);
                }
            }
        }
        if (plr->spin_wait != 0) {
            plr->spin_wait--;
        }
        if (plr->spin != 0 && plr->spin_frame < plr->spin_frames) {
            plr->spin_frame++;
        }
        if (spin_pressed != 0) {
            if (plr->spin == 0) {
                if ((plr->slam == 0 || plr->slam >= 3) && plr->crawl == 0 && plr->slide == 0) {
                    plr->spin_frame = 0;
                    plr->spin = 1;
                    plr->spin_frames = minfo->SPINFRAMES + minfo->SPINRESETFRAMES;
                    plr->spin_wait = minfo->SUPERSPINWAITFRAMES;
                    GameSfx(0x13, &plr->obj.pos);
                    NewRumble(&plr->rumble, 0x7F);
                }
            } else if ((ExtraMoves != 0 || (Game.powerbits & 4) != 0) && plr->spin < 7) {
                if (plr->spin_wait != 0 && (plr->spin & 1) == 0) {
                    plr->spin++;
                    plr->spin_frames += minfo->SUPERSPINFRAMES;
                    plr->spin_wait = minfo->SUPERSPINWAITFRAMES;
                    GameSfx(0x13, &plr->obj.pos);
                    NewRumble(&plr->rumble, 0x7F);
                }
            }
        } else if (plr->spin_frame == plr->spin_frames) {
            plr->spin = 0;
        } else if ((ExtraMoves != 0 || (Game.powerbits & 4) != 0) && (plr->spin & 1) != 0) {
            plr->spin++;
        }
        plr->obj.attack = 0x40;
        if (plr->spin != 0 &&
            plr->spin_frame < plr->spin_frames - plr->minfo->SPINRESETFRAMES) {
            plr->obj.attack = 0x42;
        }
        if (plr->obj.mask != 0 && plr->obj.mask->active >= 3) {
            plr->obj.attack |= 0x80;
        }
    }
    carpet_model[0] = 0;
    carpet_model[1] = 0;
    if (Level == 0x1D && SplTab[71].spl != 0 && CRemap[carpet_character[0]] != -1) {
        carpet_model[0] = &CModel[CRemap[carpet_character[0]]];
        if (GameTimer.frame == 0) {
            PointAlongSpline(SplTab[71].spl, 0.0f, &carpet_pos, &carpet_yrot, &carpet_xrot);
            carpet_oldpos = carpet_pos;
            ResetAnimPacket(&carpet_anim, carpet_action);
            carpet_RPos.iRAIL = -1;
            carpet_RPos.iALONG = -1;
            ResetLights(&carpet_lights);
            carpet_time = 0.0f;
            carpet_place = 1;
            carpet_panelpos = 0;
            carpet_overlap = 0;
        }
        CARPETLOOPTIME = CARPETTIME * 0.94418603f;
        if (GameTimer.frame >= 150) {
            carpet_time += 0.02f;
            if (carpet_time > CARPETTIME) {
                carpet_time -= CARPETTIME - CARPETLOOPTIME;
            }
        }
        PointAlongSpline(SplTab[71].spl, carpet_time / CARPETTIME, &carpet_splpos, &yrot, &xrot);
        if (GameTimer.ftime - 3.0f >= CARPETLOOPTIME) {
            carpet_xrot = SeekRot(carpet_xrot, xrot, 4);
            carpet_yrot = SeekRot(carpet_yrot, yrot, 4);
        } else {
            carpet_xrot = xrot;
            carpet_yrot = yrot;
        }
        carpet_oldpos = carpet_pos;
        carpet_pos.x += (carpet_splpos.x - carpet_pos.x) * 0.099999994f;
        carpet_pos.y += (carpet_splpos.y - carpet_pos.y) * 0.099999994f;
        carpet_pos.z += (carpet_splpos.z - carpet_pos.z) * 0.099999994f;
        NuVecSub(&carpet_mom, &carpet_pos, &carpet_oldpos);
        GetALONG(&carpet_pos, &carpet_RPos, carpet_RPos.iRAIL, carpet_RPos.iALONG, 1);
        carpet_anim.oldaction = carpet_anim.action;
        carpet_anim.newaction = carpet_action;
        UpdateAnimPacket(carpet_model[0], &carpet_anim, 0.59999996f, 0.0f);
        GetLights(&carpet_pos, &carpet_lights, 1);
        if (GameTimer.frame >= 150 && i_ring < RINGCOUNT &&
            GameTimer.ftime - 3.0f < CARPETLOOPTIME) {
            carpet_place = FurtherALONG(carpet_RPos.iRAIL, carpet_RPos.iALONG, carpet_RPos.fALONG,
                                        player->obj.RPos.iRAIL, player->obj.RPos.iALONG,
                                        player->obj.RPos.fALONG) ? 1 : 2;
        }
        if (i_ring > 0 && carpet_panelpos < 0x19) {
            carpet_panelpos++;
        }
        carpet_bot = carpet_pos.y;
        carpet_top = carpet_bot + 0.5f;
        if (NuVecXZDistSqr(&carpet_pos, &plr->obj.pos, 0) < 0.25f && !(plr->obj.pos.y + plr->obj.top * plr->obj.SCALE < carpet_bot) &&
            !(carpet_top < plr->obj.pos.y + plr->obj.bot * plr->obj.SCALE)) {
            carpet_mid = (carpet_bot + carpet_top) * 0.5f;
            yrot = NuAtan2D(plr->obj.pos.x - carpet_pos.x, plr->obj.pos.z - carpet_pos.z);
            plr->obj.mom.x = carpet_mom.x + NuTrigTable[yrot] * 0.04f;
            plr->obj.mom.z = carpet_mom.z + NuTrigTable[(yrot + 0x4000) & 0xFFFF] * 0.04f;
            if (carpet_overlap == 0) {
                v.x = (plr->obj.pos.x + carpet_pos.x) * 0.5f;
                v.y = (plr->obj.pos.y + (plr->obj.bot + plr->obj.top) * plr->obj.SCALE * 0.5f +
                       carpet_mid) * 0.5f;
                v.z = (plr->obj.pos.z + carpet_pos.z) * 0.5f;
                AddGameDebris(0x82, &v);
                NewRumble(&player->rumble, 0xBF);
                NewBuzz(&player->rumble, 0x10);
                GameSfx(0x5A, &v);
            }
            carpet_overlap = 1;
        } else {
            carpet_overlap = 0;
        }
        if (HitCrateBalloons(&plr->obj.pos, plr->obj.RADIUS * 5.0f) != 0) {
            NewBuzz(&player->rumble, 5);
            AddScreenWumpa(temp_pCrate->pos.x, temp_pCrate->pos.y + CRATEBALLOONOFFSET,
                           temp_pCrate->pos.z, 1);
            temp_pCrate->flags = (temp_pCrate->flags | 0x400) ^ 0x400;
        }
        v = (struct nuvec_s){-0.5f, 0.0f, -0.75f};
        NuVecRotateX(&v, &v, -carpet_xrot);
        NuVecRotateY(&v, &v, carpet_yrot);
        NuVecAdd(&v, &v, &carpet_pos);
        if (qrand() < 0x8000) {
            AddGameDebris(0x9C, &v);
        }
        v = (struct nuvec_s){0.5f, 0.0f, -0.75f};
        NuVecRotateX(&v, &v, -carpet_xrot);
        NuVecRotateY(&v, &v, carpet_yrot);
        NuVecAdd(&v, &v, &carpet_pos);
        if (qrand() < 0x8000) {
            AddGameDebris(0x9C, &v);
        }
    }
}


void AnimateGYRO(struct creature_s *plr, struct nupad_s *pad) {
    s32 held = pad->paddata & 0x60;

    if ((plr->spin != 0) &&
        (plr->spin_frame <
         plr->spin_frames - plr->minfo->SPINRESETFRAMES)) {
        plr->obj.anim.newaction = 0x69;
    } else if (plr->tap != 0) {
        plr->obj.anim.newaction = 0x67;
    } else if ((held == 0x40) || (held == 0x20)) {
        plr->obj.anim.newaction = 0x70;
    } else if (plr->obj.pad_speed > 0.0f) {
        if (0xC000u < (u16)(plr->obj.pad_angle - 0x2000)) {
            plr->obj.anim.newaction = 0x5A;
        } else if ((u16)(plr->obj.pad_angle - 0x6001) < 0x3FFFu) {
            plr->obj.anim.newaction = 0x70;
        } else if ((s16)plr->obj.pad_angle >= 0) {
            plr->obj.anim.newaction = (plr->obj.direction == 0) ? 0x67 : 0x65;
        } else {
            plr->obj.anim.newaction = (plr->obj.direction == 0) ? 0x65 : 0x67;
        }
    } else {
        plr->obj.anim.newaction = 0x62;
    }
    UpdateCharacterIdle(plr, 0);
}

void DrawCarpet(void) {
    struct numtx_s m;
    struct numtx_s mtx[256];
    struct nuvec_s scale;
    f32 **dwa;

    if (carpet_model[0] != 0) {
        SetNearestLights(&carpet_lights);
        scale.x = scale.y = scale.z = CData[carpet_character[0]].scale * 0.5f;
        NuMtxSetScale(&m, &scale);
        NuMtxRotateX(&m, carpet_xrot);
        NuMtxRotateY(&m, carpet_yrot + 0x8000);
        NuMtxTranslate(&m, &carpet_pos);
        EvalModelAnim(carpet_model[0], &carpet_anim, &m, mtx, &dwa, 0);
        NuHGobjRndrMtxDwa(carpet_model[0]->obj, &m, 1, 0, mtx, dwa);
        SetLevelLights();
    }
}

void MoveSUBMARINE(struct creature_s *plr, struct nupad_s *pad) {
    struct nuvec_s mom;
    struct nuvec_s pos;
    s32 tap_button;
    s32 fire_button;
    s32 pitch;

    pitch = (s32)(plr->obj.xyz_distance * 5120.0005f);
    fire_button = pad->paddata_db & 0x80;
    tap_button = pad->paddata_db & 0x40;
    if (pitch > 0x200) {
        pitch = 0x200;
    }
    gamesfx_pitch = pitch + 0x200;
    GameSfxLoop(0x3A, &plr->obj.pos);
    if (plr->fire == 0 && fire_button != 0 &&
        (plr->obj.hdg < 0x1555 || plr->obj.hdg > 0x6AAB)) {
        mom.x = 0.0f;
        mom.y = 0.0f;
        mom.z = 0.16f;
        if (plr->obj.hdg > 0x4000) {
            mom.z = -mom.z;
        }
        pos.x = plr->obj.pos.x;
        pos.y = plr->obj.pos.y + (plr->obj.bot + plr->obj.top) * plr->obj.SCALE * 0.5f;
        pos.z = plr->obj.pos.z;
        AddProjectile(&pos, 0, &mom, 4, (mom.z > 0.0f) ? 0 : 0x8000, &plr->obj);
        plr->fire = 0x19;
    } else if (plr->tap == 0 && tap_button != 0) {
        mom.x = 0.0f;
        mom.y = plr->obj.mom.y - 0.02f;
        mom.z = 0.0f;
        pos.x = plr->obj.pos.x;
        pos.y = plr->obj.pos.y + plr->obj.bot * plr->obj.SCALE + 0.1f;
        pos.z = plr->obj.pos.z;
        AddProjectile(&pos, 0, &mom, 10, qrand(), &plr->obj);
        plr->tap = 0x32;
    }
    plr->obj.attack = 0;
    if (plr->obj.mask != 0 && plr->obj.mask->active >= 3) {
        plr->obj.attack = 0x80;
    }
    plr->obj.attack |= 0x400;
}


extern s32 VEHICLECONTROL;
extern struct nuvec_s v000;
extern s32 temp_crate_type;

void MoveMINECART(struct creature_s *plr, struct nupad_s *pad) {
    struct obj_s obj;
    u16 yrot;
    u16 xrot;
    struct MoveInfo *minfo = &MineCartMoveInfo;
    s32 boost_held;
    s32 jump_held;
    s32 sfx;
    f32 t;
    double bounce;

    boost_held = pad->paddata_db & 0x40;
    jump_held = pad->paddata & 0x80;
    GameSfxLoop(0xCE, &plr->obj.pos);
    if (boost_held != 0 && plr->jump == 0) {
        plr_vehicle_speedmul += FASTMINECARTMUL * 0.02f;
        if (plr_vehicle_speedmul > 4.0f) {
            plr_vehicle_speedmul = 4.0f;
        }
    } else {
        plr_vehicle_speedmul -= SLOWMINECARTMUL * 0.02f;
        if (plr_vehicle_speedmul < 1.0f) {
            plr_vehicle_speedmul = 1.0f;
        }
    }
    plr_vehicle_time += plr_vehicle_speedmul * 0.02f;
    if (plr_vehicle_time >= VEHICLETIME) {
        VEHICLECONTROL = 0;
        plr_vehicle_time = VEHICLETIME;
    }
    PointAlongSpline(SplTab[24].spl, plr_vehicle_time / VEHICLETIME, &plr->obj.pos, &yrot, &xrot);
    plr->obj.hdg = yrot;
    plr->obj.xrot = xrot;
    plr->obj.zrot = 0;
    plr->obj.mom = v000;
    if (plr->jump != 0 && plr->jump_frame < plr->jump_frames) {
        plr->jump_frame++;
    }
    if (jump_held != 0) {
        if (plr->jump == 0) {
            plr->jump_frame = 0;
            plr->jump_frames = minfo->JUMPFRAMES0;
            plr->land = 0;
            plr->jump = 1;
            GameSfx(0x2F, &plr->obj.pos);
        }
    } else if (plr->jump_frame == plr->jump_frames) {
        plr->jump = 0;
    }
    if (plr->obj.draw_frame != 0) {
        obj = plr->obj;
        obj.pos.x = plr->obj.pos.x + NuTrigTable[plr->obj.hdg] * 0.25f;
        obj.pos.z = plr->obj.pos.z + NuTrigTable[(plr->obj.hdg + 0x4000) & 0xFFFF] * 0.25f;
        obj.bot = 0.0f;
        obj.top = 0.75f;
        HitItems(&obj);
        if (HitCrates(&obj, 1) != 0 && (temp_crate_type == 0x10 || temp_crate_type == 9)) {
            KillPlayer(&plr->obj, 0xB);
        }
        WumpaCollisions(&obj);
        obj.top = 1.25f;
        if (plr->jump != 0 && plr->jump_frame < plr->jump_frames) {
            t = (f32)plr->jump_frame / (f32)plr->jump_frames;
            bounce = 0.5f - t;
            if (bounce < 0.0) {
                bounce = 0.0 - bounce;
            }
            obj.top += (f32)((0.5 - bounce) * 2.0) * 0.75f;
        }
        obj.pos.x = plr->obj.pos.x - NuTrigTable[plr->obj.hdg] * 0.25f;
        obj.pos.z = plr->obj.pos.z - NuTrigTable[(plr->obj.hdg + 0x4000) & 0xFFFF] * 0.25f;
        HitItems(&obj);
        if (HitCrates(&obj, 1) != 0 && (temp_crate_type == 0x10 || temp_crate_type == 9)) {
            KillPlayer(&plr->obj, 0xB);
        }
        WumpaCollisions(&obj);
    }
    plr->obj.attack = 0;
    if (plr->obj.mask != 0 && plr->obj.mask->active >= 3) {
        plr->obj.attack = 0x80;
    }
    plr->obj.attack |= 0x400;
    gamesfx_pitch = (s32)(plr->obj.xyz_distance * 235.0f) / 0.099999994f + 470.0f;
    GameSfxLoop(0xCE, &plr->obj.pos);
    if (plr->obj.anim.action == 0x68 && plr->obj.model != 0 && plr->obj.model->anmdata[0x68] != 0) {
        t = plr->obj.model->animlist[0x68]->speed * 0.59999996f * plr->obj.xz_distance * 10.0f;
        if ((plr->obj.anim.anim_time >= 14.0f && plr->obj.anim.anim_time - t < 14.0f) ||
            (plr->obj.anim.anim_time >= 2.0f && plr->obj.anim.anim_time - t < 2.0f)) {
            do {
                sfx = qrand() / 0x4000;
            } while (sfx == cart_sfx);
            cart_sfx = sfx;
            GameSfx(sfx + 0xCA, &plr->obj.pos);
        }
    }
}


extern s32 Draw3DObject(s32 object, struct nuvec_s *pos, u16 xrot, u16 yrot, u16 zrot,
                        f32 scalex, f32 scaley, f32 scalez,
                        struct nugscn_s *scn, struct nuspecial_s *obj, s32 rot);

extern s32 TimeTrial;
extern void PickupBonusGem(s32 mask);

/* state=asm, but byte-exact (compile-diff 400/400) since game_s uses the full
 * game.c layout: retail hoists &Game.level[5] (Game+0x8C) and reads flags at
 * +0x28. Not promoted yet. */
void CheckGates(struct obj_s *obj) {
    struct nugspline_s *spl;
    struct nuvec_s *p0;
    struct nuvec_s *p1;
    s32 i;
    s32 cross;
    s32 sfx;

    if (TimeTrial == 0 && SplTab[28].spl != 0) {
        spl = SplTab[28].spl;
        for (i = 0; i < GATECOUNT; i++) {
            if ((gate_bits & (1 << i)) == 0) {
                p1 = (struct nuvec_s *)(spl->pts + i * 2 * spl->ptsize);
                p0 = (struct nuvec_s *)(spl->pts + (i * 2 + 1) * spl->ptsize);
                cross = LineCrossed(obj->oldpos.x, obj->oldpos.z, obj->pos.x, obj->pos.z,
                                    p1->x, p1->z, p0->x, p0->z);
                if (cross != 0) {
                    NewBuzz(&player->rumble, 5);
                }
                if (cross == 2) {
                    sfx = 0x36;
                    gate_bits |= 1 << i;
                    plr_gates++;
                    if (plr_gates == GATECOUNT) {
                        if (Level == 5 && (Game.level[Level].flags & 0x20) == 0) {
                            PickupBonusGem(4);
                        }
                    }
                } else if (cross == 1) {
                    sfx = 0x3C;
                } else {
                    sfx = -1;
                }
                if (sfx != -1) {
                    GameSfx(sfx, 0);
                }
            }
        }
    }
}


void ResetRings(void) {
    struct nuvec_s *pos;
    struct nuvec_s v;
    s32 i;

    RINGCOUNT = 0;
    if (SplTab[64].spl != 0 && ObjTab[0x67].obj.special != 0 && ObjTab[0x68].obj.special != 0) {
        RINGCOUNT = SplTab[64].spl->len / 2;
        if (RINGCOUNT > 0x30) {
            RINGCOUNT = 0x30;
        }
        pos = (struct nuvec_s *)SplTab[64].spl->pts;
        GetALONG(pos, 0, -1, -1, 1);
        if (AheadOfCheckpoint(temp_iRAIL, temp_iALONG, temp_fALONG) != 0) {
            i_ring = 0;
        }
        for (i = 0; i < RINGCOUNT; i++, pos += 2) {
            NuVecSub(&v, pos + 1, pos);
            ring_yrot[i] = NuAtan2D(v.x, v.z);
            ring_xrot[i] = -NuAtan2D(v.y, NuFsqrt(v.x * v.x + v.z * v.z));
            if (i == i_ring) {
                ring_pos = *pos;
            }
        }
    }
}

extern struct rail_s Rail[];

inline void UpdateArrow(struct nuvec_s *target, struct nuvec_s *origin) {
    struct nuvec_s d;
    u16 xrot;
    u16 yrot;

    NuVecSub(&d, target, origin);
    xrot = -NuAtan2D(d.y, NuFsqrt(d.x * d.x + d.z * d.z));
    yrot = NuAtan2D(d.x, d.z);
    if (GameTimer.frame != 0) {
        arrow_xrot = SeekRot(arrow_xrot, xrot, 2);
        arrow_yrot = SeekRot(arrow_yrot, yrot, 2);
    } else {
        arrow_xrot = xrot;
        arrow_yrot = yrot;
    }
}

void CheckRings(struct obj_s *obj, s32 *ring) {
    struct nuvec_s old;
    struct nuvec_s cur;
    struct nuvec_s to_old;
    struct nuvec_s to_cur;
    struct nuvec_s axis;
    struct nuvec_s left;
    struct nuvec_s right;
    struct nuvec_s mid;
    struct nuvec_s *p;
    struct nuvec_s *p0;
    struct nuvec_s *p1;
    struct RPos_s *rpos;
    s32 is_player;
    s32 i;
    f32 len;

    if (RINGCOUNT == 0 || *ring == RINGCOUNT) {
        return;
    }
    is_player = (obj == &player->obj);
    old.x = obj->oldpos.x;
    old.y = ((obj->oldpos.y + obj->bot * obj->old_SCALE) +
             (obj->oldpos.y + obj->top * obj->old_SCALE)) * 0.5f;
    old.z = obj->oldpos.z;
    cur.x = obj->pos.x;
    cur.y = obj->pos.y + (obj->bot + obj->top) * obj->SCALE * 0.5f;
    cur.z = obj->pos.z;
    p = (struct nuvec_s *)SplTab[64].spl->pts;
    for (i = 0; i < RINGCOUNT; i++, p += 2) {
        len = NuVecDist(p + 1, p, 0);
        if (NuVecDist(&old, p, 0) <= len && NuVecDist(&cur, p, 0) <= len) {
            NuVecSub(&axis, p + 1, p);
            NuVecNorm(&axis, &axis);
            NuVecSub(&to_old, &old, p);
            NuVecNorm(&to_old, &to_old);
            NuVecSub(&to_cur, &cur, p);
            NuVecNorm(&to_cur, &to_cur);
            if ((NuVecDot(&axis, &to_old) < 0.0f && NuVecDot(&axis, &to_cur) >= 0.0f) ||
                (NuVecDot(&axis, &to_old) >= 0.0f && NuVecDot(&axis, &to_cur) < 0.0f)) {
                NewBuzz(&player->rumble, 5);
                if (i == *ring) {
                    GameSfx(0x1E, &obj->pos);
                    (*ring)++;
                    if (is_player && *ring < RINGCOUNT) {
                        ring_pos = p[3];
                    }
                } else {
                    GameSfx(0x4F, &obj->pos);
                }
                break;
            }
        }
    }
    if (is_player && best_cRPos != 0) {
        rpos = best_cRPos;
        p0 = (struct nuvec_s *)(Rail[rpos->iRAIL].pLEFT->pts +
                                rpos->iALONG * Rail[rpos->iRAIL].pLEFT->ptsize);
        p1 = (struct nuvec_s *)(Rail[rpos->iRAIL].pLEFT->pts +
                                rpos->i1 * Rail[rpos->iRAIL].pLEFT->ptsize);
        left.x = (p0->x + p1->x) * 0.5f;
        left.y = (p0->y + p1->y) * 0.5f;
        left.z = (p0->z + p1->z) * 0.5f;
        p0 = (struct nuvec_s *)(Rail[rpos->iRAIL].pRIGHT->pts +
                                rpos->iALONG * Rail[rpos->iRAIL].pRIGHT->ptsize);
        p1 = (struct nuvec_s *)(Rail[rpos->iRAIL].pRIGHT->pts +
                                rpos->i1 * Rail[rpos->iRAIL].pRIGHT->ptsize);
        right.x = (p0->x + p1->x) * 0.5f;
        right.y = (p0->y + p1->y) * 0.5f;
        right.z = (p0->z + p1->z) * 0.5f;
        mid.x = (left.x + right.x) * 0.5f;
        mid.y = (left.y + right.y) * 0.5f;
        mid.z = (left.z + right.z) * 0.5f;
        if (NuVecDist(&cur, &mid, 0) > 2.5f) {
            obj->mom.x += (mid.x - cur.x) * RINGPULL;
            obj->mom.y += (mid.y - cur.y) * RINGPULL;
            obj->mom.z += (mid.z - cur.z) * RINGPULL;
        }
    }
    if (is_player) {
        UpdateArrow(&cur, &ring_pos);
    }
}


void DrawRings(void) {
    struct nugspline_s *spl;
    struct nuvec_s *pos;
    s32 obj;
    s32 i;

    if (RINGCOUNT != 0) {
        spl = SplTab[64].spl;
        for (i = 0; i < RINGCOUNT; i++) {
            pos = (struct nuvec_s *)(spl->pts + i * 2 * spl->ptsize);
            if (i == RINGCOUNT - 1 && i_ring == RINGCOUNT) {
                obj = 0x69;
            } else if (i == i_ring) {
                if (i == 0 || i == RINGCOUNT - 1) {
                    obj = 0x69;
                } else {
                    obj = 0x68;
                }
            } else {
                obj = 0x67;
            }
            /* (ObjTab + obj), not ObjTab[obj]: the index form changes codegen. */
            Draw3DObject(obj, pos, ring_xrot[i], ring_yrot[i], 0, 1.0f, 1.0f, 1.0f,
                         (ObjTab + obj)->obj.scene, (ObjTab + obj)->obj.special, 0);
        }
    }
}


void MoveMECH(struct creature_s *plr, struct nupad_s *pad) {
    struct MoveInfo *minfo = &MechMoveInfo;
    s32 jump_pressed;
    s32 fire_pressed;
    s32 old;
    s32 action;
    s32 frames;
    f32 height;
    f32 jump_frames;

    jump_pressed = pad->paddata & 0x40;
    fire_pressed = pad->paddata & 0x80;
    if (menu_mode == 0x1F || menu_mode == 0x21 || GameTimer.frame < 0x32 || fadeval > 0) {
        fire_pressed = 0;
        jump_pressed = 0;
    }
    if (plr->obj.ground != 0 && plr->jump != 0 && plr->jump_frame == plr->jump_frames &&
        plr->jump_hold == 0) {
        GameSfx(0x48, &plr->obj.pos);
    }
    if (plr->target == 2 && plr->fire == 0 && plr->fire_lock == 0 && plr->tap == 0) {
        plr->target = 1;
        AddFlyingWumpa(&plr_target_pos[0], &plr_target_dir, &plr_target_pos[1], 3);
        frames = (s32)(ModelAnimDuration(0, plr->fire_action, 0.0f, 0.0f) * 50.0f);
        plr->obj.anim.anim_time = 1.0f;
        plr->fire = frames;
        plr->tap = frames + 5;
        AddGameDebrisMtx(0x51, &plr_target_firepos, 0x20, &plr_target_mtx);
        AddGameDebrisMtx(0x52, &plr_target_firepos, 0x10, &plr_target_mtx);
        GameSfx(6, &plr->obj.pos);
    }
    if (plr->jump != 0 && plr->jump_frame < plr->jump_frames && plr->jump_hold == 0) {
        jump_frames = minfo->JUMPFRAMES2;
        height = minfo->JUMPHEIGHT;
        if (plr->jump_type == 3) {
            plr->obj.mom.y = (height * 1.5f) / jump_frames;
        } else {
            plr->obj.mom.y = height / jump_frames;
        }
    }
    if ((plr->obj.ground & 1) == 0) {
        plr->obj.mom.y += GRAVITY;
    }
    if (plr->jump != 0 && plr->jump_hold == 0 && plr->jump_frame < plr->jump_frames) {
        plr->jump_frame++;
    }
    if (jump_pressed != 0 || plr->jump_hack != 0 || plr->obj.boing != 0 || plr->jump_hold != 0) {
        if (plr->jump_hack != 0) {
            plr->jump_hack = 0;
        }
        switch (plr->jump) {
        case 0:
            if ((plr->obj.ground != 0 || plr->allow_jump != 0) && plr->target == 0 &&
                (*(u32 *)&plr->fire_action & 0xFFFF0000) == 0 && plr->obj.transporting == 0) {
                if (plr->obj.pad_speed == 0.0f) {
                    plr->jump_type = 2;
                    plr->jump_frames = minfo->JUMPFRAMES2;
                    plr->jump_hold = 1;
                } else {
                    plr->jump_type = 1;
                    plr->jump_frames = minfo->JUMPFRAMES0;
                    plr->jump_hold = 0;
                    AddGameDebris(0x10, &plr->obj.pos);
                    AddGameDebris(0x11, &plr->obj.pos);
                }
                GameSfx(0x32, &plr->obj.pos);
                plr->obj.anim.anim_time = 1.0f;
                plr->jump_frame = 0;
                plr->jump = 1;
                plr->jump_hack = 1;
                plr->land = 0;
                plr->obj.ground = 0;
            }
            break;
        case 1:
            if (plr->jump_hold != 0) {
                if (plr->obj.anim.anim_time >= 15.0f) {
                    plr->jump_hold = 0;
                    AddGameDebris(0x10, &plr->obj.pos);
                    AddGameDebris(0x11, &plr->obj.pos);
                    GameSfx(0x32, &plr->obj.pos);
                    plr->jump_hack = 1;
                }
            } else if (plr->jump_frames < minfo->JUMPFRAMES2) {
                plr->jump_frames++;
            } else {
                plr->jump = 2;
            }
            break;
        }
    } else if (plr->obj.ground != 0) {
        plr->jump = 0;
    } else if (plr->jump == 1 || plr->jump == 2) {
        plr->jump = 3;
    }
    if (plr->land != 0) {
        if ((plr->obj.anim.flags & 1) || plr->obj.pad_speed != 0.0f) {
            plr->land = 0;
        }
    } else if (plr->obj.ground != 0 && plr->obj.old_ground == 0 && plr->obj.pad_speed == 0.0f &&
               plr->jump != 1 && plr_allow_jump == 0) {
        if ((plr->obj.anim.action == 0x63 &&
             plr->obj.anim.anim_time >= MECHRUNNINGJUMPPAUSEFRAME) ||
            (plr->obj.anim.action == 0x64 &&
             plr->obj.anim.anim_time >= MECHSTANDINGJUMPPAUSEFRAME)) {
            plr->land = 1;
            AddGameDebris(0x10, &plr->obj.pos);
        }
    }
    old = plr->target;
    if (fire_pressed != 0 && plr->obj.ground != 0 && plr->jump == 0) {
        plr->target = 1;
        if (old == 0) {
            plr->obj.target_yrot = 0;
            plr->obj.target_xrot = 0;
            plr->fire_action = 0x5C;
            plr->tap = 10;
            GameSfx(0x30, &plr->obj.pos);
        } else if ((*(u32 *)&plr->fire_action & 0xFFFF0000) == 0) {
            old = plr->fire_action;
            if (plr->obj.target_yrot >= 0xE3A) {
                action = 0x60;
            } else if (plr->obj.target_yrot < -0xE39) {
                action = 0x5F;
            } else {
                action = (plr->obj.target_xrot > 0x71C) ? 0x5D : 0x5C;
            }
            if (action != plr->fire_action) {
                if (plr->fire_action != 0x5C && action != 0x5C) {
                    action = 0x5C;
                }
                plr->fire_action = action;
            }
            if (plr->fire_action != old) {
                plr->tap = 10;
            }
        }
    } else {
        plr->target = 0;
        if (old == 1) {
            plr->target = 2;
        }
    }
    plr->obj.attack = 0x400;
    if (plr->obj.ground == 0 && plr->obj.mom.y <= 0.0f) {
        plr->obj.attack = 0x420;
    }
    if (plr->obj.mask != 0 && plr->obj.mask->active >= 3) {
        plr->obj.attack |= 0x80;
    }
}


void AnimateMECH(struct creature_s *plr) {
    struct MoveInfo *minfo;

    minfo = &MechMoveInfo;
    if ((u32)plr->obj.dead > 1) {
        plr->obj.anim.newaction = plr->obj.die_action;
    } else if (plr->fire != 0) {
        plr->obj.anim.newaction = plr->fire_action;
    } else if (plr->target != 0) {
        if (plr->fire_action == 0x5D) {
            plr->obj.anim.newaction = 0x6C;
        } else if (plr->fire_action == 0x5F) {
            plr->obj.anim.newaction = 0x6E;
        } else if (plr->fire_action == 0x60) {
            plr->obj.anim.newaction = 0x6F;
        } else {
            plr->obj.anim.newaction = 0x6B;
        }
    } else if (plr->jump != 0 && (plr->obj.ground == 0 || plr->jump_frame < plr->jump_frames)) {
        if (plr->jump_type == 2) {
            if (plr->obj.anim.anim_time >= MECHSTANDINGJUMPPAUSEFRAME) {
                plr->obj.anim.anim_time = MECHSTANDINGJUMPPAUSEFRAME;
            }
            plr->obj.anim.newaction = 100;
        } else {
            if (plr->obj.anim.anim_time >= MECHRUNNINGJUMPPAUSEFRAME) {
                plr->obj.anim.anim_time = MECHRUNNINGJUMPPAUSEFRAME;
            }
            plr->obj.anim.newaction = 99;
        }
    } else if (plr->land != 0) {
        if (plr->jump_type == 2) {
            plr->obj.anim.newaction = 100;
        } else {
            plr->obj.anim.newaction = 99;
        }
    } else if (plr->obj.pad_speed == 0.0f
               && (plr->pad_type != 1
                   || !(plr->obj.mom.x * plr->obj.mom.x + plr->obj.mom.z * plr->obj.mom.z
                        >= plr->minfo->IDLESPEED * plr->minfo->IDLESPEED))) {
        plr->obj.anim.newaction = 0x62;
    } else if (plr->obj.pad_speed == minfo->WALKSPEED) {
        plr->obj.anim.newaction = 0x71;
    } else {
        plr->obj.anim.newaction = 0x68;
    }
    UpdateCharacterIdle(plr, 0);
}

void ProcessGenericTrail(s32 id, struct nuvec_s *pos, f32 radius, f32 width);

void MoveSNOWBOARD(struct creature_s *plr, struct nupad_s *pad) {
    struct MoveInfo *minfo = &SnowBoardMoveInfo;
    s32 jump_pressed;
    f32 height;
    f32 frames;

    jump_pressed = pad->paddata & 0x40;
    if (plr->obj.ground != 0) {
        gamesfx_pitch = 0x75A;
        gamesfx_effect_volume = 0x1FFF;
        GameSfxLoop(0x67, &plr->obj.pos);
    }
    if (plr->jump != 0 && plr->jump_frame < plr->jump_frames) {
        frames = minfo->JUMPFRAMES2;
        height = minfo->JUMPHEIGHT;
        if (plr->jump_type == 3) {
            plr->obj.mom.y = (height * 1.5f) / frames;
        } else {
            plr->obj.mom.y = height / frames;
        }
    }
    if ((plr->obj.ground & 1) == 0) {
        plr->obj.mom.y += GRAVITY;
    }
    if (plr->jump != 0 && plr->jump_frame < plr->jump_frames) {
        plr->jump_frame++;
    }
    if (jump_pressed != 0 || plr->jump_hack != 0 || plr->obj.boing != 0) {
        if (plr->jump_hack != 0) {
            plr->jump_hack = 0;
        }
        switch (plr->jump) {
        case 0:
            if (plr->obj.ground != 0 || plr->allow_jump != 0) {
                plr->jump_type = 1;
                plr->jump_frames = minfo->JUMPFRAMES0;
                plr->jump_frame = 0;
                plr->jump = 1;
                plr->land = 0;
                AddGameDebris(0x10, &plr->obj.pos);
                AddGameDebris(0x11, &plr->obj.pos);
                plr->jump_hack = 1;
                plr->obj.ground = 0;
                plr->obj.anim.anim_time = 1.0f;
                GameSfx(0x2F, &plr->obj.pos);
            }
            break;
        case 1:
            if (plr->jump_frames < minfo->JUMPFRAMES2) {
                plr->jump_frames++;
            } else {
                plr->jump = 2;
            }
            break;
        }
    } else if (plr->obj.ground != 0) {
        plr->jump = 0;
    } else if (plr->jump == 1 || plr->jump == 2) {
        plr->jump = 3;
    }
    if (plr->land != 0) {
        if (plr->obj.anim.flags & 1) {
            plr->land = 0;
        }
    } else if (plr->obj.ground != 0 && plr->obj.old_ground == 0 && plr->jump != 1 &&
               plr_allow_jump == 0 && plr->obj.anim.action == 0x63) {
        plr->land = 1;
        AddGameDebris(0x10, &plr->obj.pos);
        GameSfx(0x54, &plr->obj.pos);
    }
    plr->obj.attack = 0;
    if (plr->obj.mask != 0 && plr->obj.mask->active >= 3) {
        plr->obj.attack = 0x80;
    }
    plr->obj.attack |= 0x400;
    plr->obj.pos.y -= 0.05f;
    ProcessGenericTrail(0, &plr->obj.pos, 0.4f, 0.1f);
    plr->obj.pos.y += 0.05f;
}


void AnimateSNOWBOARD(struct creature_s *plr) {
    s32 d;

    if (best_cRPos != 0) {
        d = RotDiff(best_cRPos->angle, plr->obj.hdg);
    } else {
        d = 0;
    }
    if ((u32)plr->obj.dead > 1) {
        plr->obj.anim.newaction = plr->obj.die_action;
    } else if (plr->jump != 0 && (plr->obj.ground == 0 || plr->jump_frame < plr->jump_frames)) {
        if (plr->obj.anim.anim_time >= SNOWBOARDJUMPPAUSEFRAME) {
            plr->obj.anim.anim_time = SNOWBOARDJUMPPAUSEFRAME;
        }
        plr->obj.anim.newaction = 0x63;
    } else if (plr->land != 0) {
        plr->obj.anim.newaction = 0x63;
    } else if (plr->sprint != 0) {
        plr->obj.anim.newaction = 0x6A;
    } else if (d > 0x1000) {
        plr->obj.anim.newaction = 0x65;
    } else if (d < -0x1000) {
        plr->obj.anim.newaction = 0x67;
    } else {
        plr->obj.anim.newaction = 0x68;
    }
    UpdateCharacterIdle(plr, 1);
    /* Snow spray from the board's locators. */
    if (plr->obj.anim.newaction == 0x68 || plr->obj.anim.newaction == 0x6A || plr->land != 0) {
        AddGameDebrisRot(0x53, (struct nuvec_s *)&plr->mtxLOCATOR[1][0]._30, 1, 0, 0);
        AddGameDebrisRot(0x53, (struct nuvec_s *)&plr->mtxLOCATOR[1][1]._30, 1, 0, 0);
        AddGameDebrisRot(0x54, (struct nuvec_s *)&plr->mtxLOCATOR[1][2]._30, 2, 0, 0);
    } else if (plr->obj.anim.newaction == 0x65) {
        AddGameDebrisRot(0x55, (struct nuvec_s *)&plr->mtxLOCATOR[1][4]._30, 3, 0, 0);
    } else if (plr->obj.anim.newaction == 0x67) {
        AddGameDebrisRot(0x55, (struct nuvec_s *)&plr->mtxLOCATOR[1][3]._30, 3, 0, 0);
    }
}

extern s32 jonframe1;
extern s32 SmokeyFinished;
extern struct gdeb_s GDeb[];
void PlayerHitBuggies(struct creature_s *c);
s32 ChaseActive(void);
void AddVariableShotDebrisEffect(s32 type, struct nuvec_s *pos, s32 count, s32 a, s32 b);

void MoveOFFROADER(struct creature_s *plr, struct nupad_s *pad) {
    struct nuvec_s v;
    s32 accel;
    s32 n;
    s32 k;
    s32 i;

    accel = pad->paddata & 0x60;
    jeeptyrepos = plr->obj.pos;
    n = (s32)(plr->obj.xyz_distance * 2560.0002f);
    if (n > 0x200) {
        n = 0x200;
    }
    gamesfx_pitch = n + 0x200;
    GameSfxLoop(0x6A, &plr->obj.pos);
    if (pad->paddata_db & 0x60) {
        GameSfx(0x6D, &plr->obj.pos);
    }
    if (pad->paddata_db & 0x60) {
        GameSfx(0x57, &plr->obj.pos);
    }
    if ((jonframe1 & 7) == 0 && plr->obj.ground != 0) {
        v.x = player->obj.mom.x;
        v.z = player->obj.mom.z;
        NuVecRotateY(&v, &v, -player->obj.hdg);
        if (NuFabs(v.x) * 2.0f > NuFabs(v.z) && plr->obj.xz_distance > 0.02f) {
            GameSfx(0x12, &plr->obj.pos);
        }
    }
    if (plr->obj.ground != 0 && plr->obj.old_ground == 0 &&
        (qrand() < 0x4000 || plr->obj.mom.y < -0.02f)) {
        do {
            n = qrand() / 0x4000;
        } while (n == i_jkeepsfx);
        i_jkeepsfx = n;
        GameSfx(jeepsfx[n], &plr->obj.pos);
    }
    if (plr->obj.ground != 0) {
        gamesfx_effect_volume = (s32)(plr->obj.xyz_distance * 81915.008f);
        if (gamesfx_effect_volume > 0x3FFF) {
            gamesfx_effect_volume = 0x3FFF;
        }
        GameSfxLoop(0x67, &jeeptyrepos);
    }
    if (Level == 3) {
        PlayerHitBuggies(plr);
    }
    if ((Level != 8 || ChaseActive() == -1) && plr->obj.ground != 0 && qrand() < 0x800) {
        n = (s32)(plr->obj.xz_distance / 0.099999994f * 127.0f);
        NewRumble(&plr->rumble, (n > 0x7F) ? 0x7F : n);
    }
    if (plr->obj.ground != 0 && plr->obj.surface_type == 0xB) {
        if (plr->fire == 0) {
            GameSfx(0xC8, &plr->obj.pos);
            JudderGameCamera(GameCam, 0.3f, 0);
            gamesfx_effect_volume = 0x5FFE;
            GameSfx(0x6D, &plr->obj.pos);
        }
        plr->fire = 0x32;
    }
    if (plr->fire != 0) {
        NewBuzz(&plr->rumble, 2);
    }
    if (GameTimer.frame == 0 || SmokeyFinished != 0 || SmokeyCountDownValue > 0) {
        offroader_speedtime = 0.0f;
    } else if (plr->fire != 0) {
        offroader_speedtime = OFFROADERSPEEDTIME;
    } else if (accel != 0) {
        offroader_speedtime += 0.02f;
        if (offroader_speedtime > OFFROADERSPEEDTIME) {
            offroader_speedtime = OFFROADERSPEEDTIME;
        }
    } else {
        offroader_speedtime -= 0.04f;
        if (offroader_speedtime < 0.0f) {
            offroader_speedtime = 0.0f;
        }
    }
    if ((plr->obj.ground & 1) == 0) {
        plr->obj.mom.y += GRAVITY;
    }
    plr->obj.attack = 0;
    if (plr->obj.mask != 0 && plr->obj.mask->active >= 3) {
        plr->obj.attack = 0x80;
    }
    plr->obj.attack |= 0x400;
    if ((plr->obj.ground != 0 || plr->fire != 0) &&
        (plr->obj.xz_distance > 0.02f || abs(plr->obj.dyrot) > 0x60) &&
        (plr->sprint != 0 || (jonframe1 & 3) == 0 || accel != 0 || plr->fire != 0 ||
         abs(plr->obj.dyrot) > 0x60)) {
        for (i = 0; i < 4; i++) {
            v = plr->obj.pos;
            switch (i) {
            case 0:
            default:
                k = 0x1B58;
                break;
            case 1:
                k = -0x1B58;
                break;
            case 2:
                k = 0x6590;
                break;
            case 3:
                k = 0x9A70;
                break;
            }
            v.x += NuTrigTable[(k + plr->obj.hdg) & 0xFFFF] * 0.7f;
            v.y += 0.1f;
            v.z += NuTrigTable[(k + plr->obj.hdg + 0x4000) & 0xFFFF] * 0.7f;
            k = (plr->fire != 0) ? 0x9E : 0x9D;
            AddVariableShotDebrisEffect(GDeb[k].i, &v, 1, 0, 0);
        }
    }
}


void AnimateSWIMMING(struct creature_s *plr) {
    struct MoveInfo *info = &SwimmingMoveInfo;

    if ((u32)plr->obj.dead >= 2) {
        plr->obj.anim.newaction = plr->obj.die_action;
    } else if ((plr->spin != 0) &&
               (plr->spin_frame <
                plr->spin_frames - plr->minfo->SPINRESETFRAMES)) {
        plr->obj.anim.newaction = 0x46;
    } else if ((plr->obj.pad_speed > 0.0f) || (plr->tap != 0) ||
               (fabs(plr->obj.mom.z) > info->WALKSPEED)) {
        plr->obj.anim.newaction = 0x4C;
    } else {
        plr->obj.anim.newaction = 0x22;
    }
    UpdateCharacterIdle(plr, 0x73);
}


void AnimateSCOOTER(struct creature_s *plr) {
    if ((u32)plr->obj.dead >= 2) {
        plr->obj.anim.newaction = plr->obj.die_action;
    } else if ((plr->jump != 0) &&
               ((plr->obj.ground == 0) || (plr->jump_frame < plr->jump_frames))) {
        if (plr->obj.anim.anim_time >= SCOOTERJUMPPAUSEFRAME) {
            plr->obj.anim.anim_time = SCOOTERJUMPPAUSEFRAME;
        }
        plr->obj.anim.newaction = 0x63;
    } else if (plr->land != 0) {
        plr->obj.anim.newaction = 0x63;
    } else if (plr->sprint != 0) {
        plr->obj.anim.newaction = 0x6A;
    } else {
        plr->obj.anim.newaction = 0x68;
    }
    UpdateCharacterIdle(plr, 1);
}

void AnimateOFFROADER(struct creature_s *plr) {
    s32 d;

    if (best_cRPos != 0) {
        d = RotDiff(best_cRPos->angle, plr->obj.hdg);
    } else {
        d = 0;
    }
    if ((u32)plr->obj.dead >= 2) {
        plr->obj.anim.newaction = plr->obj.die_action;
    } else {
        if ((Level == 3) &&
            ((Pad[0] == 0) || ((Pad[0]->paddata & 0x60) == 0) ||
             (SmokeyCountDownValue > 0)) &&
            (plr->obj.xz_distance < 0.02f)) {
            plr->obj.anim.newaction = 0x62;
        } else if (d >= 0x801) {
            plr->obj.anim.newaction = 0x67;
        } else if (d < -0x800) {
            plr->obj.anim.newaction = 0x65;
        } else {
            plr->obj.anim.newaction = 0x68;
        }
    }
    UpdateCharacterIdle(plr, 0);
}

void AnimateSUBMARINE(struct creature_s *plr) {
    plr->obj.anim.newaction = 0x62;
    UpdateCharacterIdle(plr, 0);
    AddGameDebrisRot(0x56, (struct nuvec_s *)&plr->mtxLOCATOR[2][0]._30, 1, 0, 0);
    AddGameDebrisRot(0x56, (struct nuvec_s *)&plr->mtxLOCATOR[3][0]._30, 1, 0, 0);
}

void AnimateMINECART(struct creature_s *plr) {
    if ((u32)plr->obj.dead > 1) {
        plr->obj.anim.newaction = plr->obj.die_action;
    } else {
        if (plr->jump != 0) {
            if (plr->jump_frame >= plr->jump_frames) {
                plr->obj.anim.newaction = 0x68;
            } else {
                plr->obj.anim.newaction = 99;
            }
        } else {
            plr->obj.anim.newaction = 0x68;
        }
    }
    UpdateCharacterIdle(plr, 0);
}

void AnimateGLIDER(struct creature_s *plr) {
    if (FlyingLevelVictoryDance != 0) {
        if (plr->obj.anim.newaction != 0x75) {
            plr->obj.anim.oldaction = plr->obj.anim.action;
        }
        plr->obj.anim.newaction = 0x75;
    } else {
        if ((plr->obj.pad_speed > 0.0f) && (plr->obj.pad_angle > 0x9555) &&
            (plr->obj.pad_angle < 0xeaab)) {
            plr->obj.anim.newaction = 0x65;
        } else {
            if ((plr->obj.pad_speed > 0.0f) && (plr->obj.pad_angle > 0x1555)) {
                if (plr->obj.pad_angle > 0x6aaa) {
                    plr->obj.anim.newaction = 0x62;
                } else {
                    plr->obj.anim.newaction = 0x67;
                }
            } else {
                plr->obj.anim.newaction = 0x62;
            }
        }
    }
    UpdateCharacterIdle(plr, 0);
}

void AnimateDROPSHIP(struct creature_s *plr) {
    if (FlyingLevelVictoryDance != 0) {
        if (plr->obj.anim.newaction != 0x75) {
            plr->obj.anim.oldaction = plr->obj.anim.action;
        }
        plr->obj.anim.newaction = 0x75;
    } else {
        plr->obj.anim.newaction = 0x62;
    }
    UpdateCharacterIdle(plr, 1);
}

void AnimateATLASPHERE(struct creature_s *plr) {
    if ((u32)plr->obj.dead > 1) {
        plr->obj.anim.newaction = plr->obj.die_action;
    } else {
        if ((plr->obj.pad_speed == 0.0f) &&
            (plr->obj.xz_distance < 0.005f)) {
            plr->obj.anim.newaction = 0x22;
        } else {
            if (plr->obj.xz_distance < CrashMoveInfo.WALKSPEED) {
                plr->obj.anim.newaction = 0x71;
            } else {
                plr->obj.anim.newaction = 0x68;
            }
        }
    }
    UpdateCharacterIdle(plr, 0);
}

void AnimateJEEP(struct creature_s *plr) {
    if ((u32)plr->obj.dead < 2) {
        plr->obj.anim.newaction = 0x68;
    } else {
        plr->obj.anim.newaction = plr->obj.die_action;
    }
    UpdateCharacterIdle(plr, 0);
}

void AnimateMOSQUITO(struct creature_s *plr) {
    if (FlyingLevelVictoryDance != 0) {
        if (plr->obj.anim.newaction != 0x75) {
            plr->obj.anim.oldaction = plr->obj.anim.action;
        }
        plr->obj.anim.newaction = 0x75;
    } else {
        plr->obj.anim.newaction = 0x62;
    }
    UpdateCharacterIdle(plr, 0);
}

void ResetGates(void) {
    struct nugspline_s *spl = SplTab[28].spl;
    struct nuvec_s *first;
    struct nuvec_s *second;
    struct nuvec_s pos;

    GATECOUNT = 0;
    if (spl != 0) {
        GATECOUNT = spl->len / 2;
        if (GATECOUNT > 0x40) {
            GATECOUNT = 0x40;
        }
        first = (struct nuvec_s *)spl->pts;
        second = (struct nuvec_s *)(spl->pts + spl->ptsize);
        pos.x = (first->x + second->x) * 0.5f;
        pos.y = (first->y + second->y) * 0.5f;
        pos.z = (first->z + second->z) * 0.5f;
        GetALONG(&pos, 0, -1, -1, 1);
        if (AheadOfCheckpoint(temp_iRAIL, temp_iALONG, temp_fALONG) != 0) {
            plr_gates = 0;
            gate_bits = 0;
            if (Level == 5) {
                bonusgem_ok = 0;
            }
        }
    }
}

void CheckPlayerEvents(struct obj_s *obj) {
    struct plrevent_s *ev;
    struct nuvec_s *p0;
    struct nuvec_s *p1;
    s32 i;
    s32 j;
    s32 started;

    if (world_scene[0] != 0) {
        ev = PlrEvent;
        for (i = 0; i < 8; i++, ev++) {
            if (ev->spl != 0) {
                p0 = (struct nuvec_s *)ev->spl->pts;
                p1 = (struct nuvec_s *)(ev->spl->pts + ev->spl->ptsize);
                if (ev->triggered == 0 &&
                    LineCrossed(obj->oldpos.x, obj->oldpos.z, obj->pos.x, obj->pos.z,
                                p0->x, p0->z, p1->x, p1->z) == 2) {
                    ev->triggered = 1;
                    started = 0;
                    for (j = 0; j < 16; j++) {
                        if (StartHGobjAnim(&ev->obj[j]) != 0) {
                            started++;
                        }
                    }
                    if (started != 0) {
                        JudderGameCamera(&GameCam[0], 0.5f, 0);
                        GameSfx(0x3B, 0);
                    }
                }
            }
        }
    }
}

void ResetPlayerEvents(void) {
    struct plrevent_s *ev;
    s32 i;
    s32 j;

    if (world_scene[0] != 0) {
        ev = PlrEvent;
        for (i = 0; i < 8; i++, ev++) {
            if ((ev->spl != 0) &&
                (AheadOfCheckpoint(ev->iRAIL, ev->iALONG, ev->fALONG) != 0)) {
                for (j = 0; j < 16; j++) {
                    ResetHGobjAnim(&ev->obj[j]);
                    ev->triggered = 0;
                }
            }
        }
    }
}

void DrawPanel3DArrow(void) {
    u16 xrot;
    u16 yrot;

    if (ObjTab[0x6C].obj.special != 0) {
        xrot = arrow_xrot;
        yrot = RotDiff(GameCam[0].yrot, arrow_yrot);
        DrawPanel3DObject(0x6C, 0.0f, 0.5f, 1.0f, 0.125f, 0.125f, 0.125f, xrot,
                          yrot, 0, ObjTab[0x6C].obj.scene,
                          ObjTab[0x6C].obj.special, 0);
    }
}

void AnimateDIVE(struct creature_s *plr, f32 ratio) {
    if (ratio < 0.333f) {
        plr->obj.anim.newaction = 0x2c;
        return;
    }
    plr->obj.anim.newaction = 0x44;
}

inline void ResetTub(struct tubinfo_s *tub, struct creature_s *c) {
    tub->c = c;
    tub->track = -1;
    tub->count = -1;
    tub->wait = 0;
    tub->laps = -1;
    tub->finished = 0;
    tub->stall = 0;
    tub->boost = 0;
}

/* Puts the player and the rival tub at the start, the rival halfway
   between the two start splines. */
void ResetTubs(void) {
    struct CharacterData *cdata;
    struct nuvec_s *p0;
    struct nuvec_s *p1;
    struct creature_s *c;
    s32 i;

    ResetTub(&PlrTub, player);
    c = &OppTubCreature;
    ResetTub(&OppTub, c);
    memset(c, 0, sizeof(struct creature_s));
    ResetAnimPacket(&c->obj.anim, 0x62);
    c->obj.character = 0x7F;
    c->obj.vehicle = (CRemap[0xA1] != -1) ? 0xA1 : -1;
    c->obj.reflect_y = 2000000.0f;
    c->obj.model = (CRemap[0x7F] != -1) ? &CModel[CRemap[0x7F]] : 0;
    cdata = &CData[c->obj.character];
    c->obj.RADIUS = cdata->radius;
    c->obj.radius = cdata->radius;
    c->obj.min = cdata->min;
    c->obj.max = cdata->max;
    c->obj.SCALE = 0.77f;
    c->obj.scale = 0.77f;
    c->obj.bot = c->obj.min.y;
    c->obj.top = c->obj.max.y;
    c->used = 1;
    c->on = 1;
    ResetLights(&c->lights);
    OppTub.c = c;
    if (SplTab[30].spl != 0 && SplTab[46].spl != 0) {
        p0 = (struct nuvec_s *)SplTab[30].spl->pts;
        p1 = (struct nuvec_s *)SplTab[46].spl->pts;
        c->obj.pos.x = (p0->x + p1->x) * 0.5f;
        c->obj.pos.y = (p0->y + p1->y) * 0.5f;
        c->obj.pos.z = (p0->z + p1->z) * 0.5f;
    }
    opptub_action = 0x62;
    i = qrand() * 50;
    opptub_wait = i / 0x10000 + 50;
}



extern s32 gempath_open;
extern s32 GemPath;
f32 SplineDistance(struct nugspline_s *spl);

void MoveTub(struct tubinfo_s *tub, s32 LEFT, s32 RIGHT) {
    s32 dirs[3];
    struct nuvec_s p0;
    struct nuvec_s p1;
    struct nuvec_s mid;
    u16 yrot0;
    u16 xrot0;
    u16 yrot1;
    u16 xrot1;
    struct nugspline_s *spl;
    struct nugspline_s *spl2;
    struct nugspline_s *left;
    struct nuvec_s *a;
    struct nuvec_s *b;
    struct creature_s *plr = player;
    s32 is_player;
    s32 control;
    s32 next;
    s32 n;
    s32 dir;
    s32 start;
    s32 end;
    u16 yrot;
    u16 xrot;
    u16 ang;
    f32 speed;
    f32 ratio;

    is_player = (tub == &PlrTub);
    control = is_player ? VEHICLECONTROL : 1;
    if (Level != 0x1C || (GameTimer.frame >= 150 && tub->stall == 0 &&
                          (tub->finished == 0 || tub->finished < tub->finishframes))) {
        speed = 0.02f;
        if (Level == 0x1C && tub->finished != 0) {
            speed = NuTrigTable[((tub->finished << 14) / tub->finishframes + 0x4000) & 0xFFFF] * speed;
        } else if (Level == 0x1C && GameTimer.frame < 200) {
            speed = (1.0f - NuTrigTable[(((GameTimer.frame - 150) << 14) / 50 + 0x4000) & 0xFFFF]) *
                    speed;
        } else if (is_player) {
            tub->spark = 0;
            if (tub->boost != 0) {
                speed = (NuTrigTable[(s32)((tub->boost << 14) / TUBBOOSTFRAMES) & 0xFFFF] + 1.0f) * speed;
                tub->spark = 1;
            } else if (plr->obj.pad_speed > 0.0f) {
                if ((u16)(plr->obj.pad_angle - 0x2AAB) > 0xAAAA) {
                    if (qrand() < 0x3000 && (Level != 0x1C || race_finished == 0)) {
                        NewRumble(&player->rumble, qrand() / 0x156);
                    }
                    speed *= 1.2f;
                } else if ((u16)(plr->obj.pad_angle - 0x5556) < 0x5555) {
                    if (qrand() < 0x3000 && (Level != 0x1C || race_finished == 0)) {
                        NewBuzz(&player->rumble, 2);
                    }
                    tub->spark = 1;
                    speed *= 0.8f;
                }
            } else if (qrand() < 0x4000 && (Level != 0x1C || race_finished == 0)) {
                NewRumble(&player->rumble, qrand() / 0x200);
            }
        }
        tub->time += speed;
    }
    if (tub->stall != 0) {
        tub->stall--;
    }
    next = -1;
    if (tub->track != -1) {
        if (tub->time >= tub->duration) {
            tub->time = tub->duration;
            if (tub->laps >= MAXLAPS) {
                RIGHT = 0;
                LEFT = 0;
            } else if (!is_player) {
                n = 0;
                if (TubLink[tub->track].i_left != -1) {
                    dirs[n++] = 3;
                }
                if (TubLink[tub->track].i_mid != -1) {
                    dirs[n++] = 0;
                }
                if (TubLink[tub->track].i_right != -1) {
                    dirs[n++] = 4;
                }
                RIGHT = 0;
                LEFT = 0;
                if (n != 0) {
                    dir = dirs[qrand() / (0xFFFF / n + 1)];
                    if (dir == 3) {
                        LEFT = 1;
                    } else {
                        RIGHT = (dir == 4);
                    }
                }
            }
            if (is_player && Level == 4 && TubLink[tub->track].i_right == 4 &&
                (gempath_open == 0 || TimeTrial != 0)) {
                if (TubLink[tub->track].i_mid != -1) {
                    next = TubLink[tub->track].i_mid;
                } else {
                    control = 0;
                }
            } else if (LEFT != 0 && TubLink[tub->track].i_left != -1) {
                next = TubLink[tub->track].i_left;
            } else if (RIGHT != 0 && TubLink[tub->track].i_right != -1) {
                next = TubLink[tub->track].i_right;
            } else if (TubLink[tub->track].i_mid != -1) {
                next = TubLink[tub->track].i_mid;
            } else {
                control = 0;
            }
        }
    } else {
        next = is_player ^ 1;
    }
    if (next != -1) {
        tub->track = next;
        tub->wait = (s32)TUBWAIT;
        if (tub->count <= 0) {
            tub->count++;
        }
        left = SplTab[tub->track + 0x1D].spl;
        spl2 = SplTab[tub->track + 0x2D].spl;
        if (left != 0 && spl2 != 0) {
            speed = SplineDistance(left);
            speed += SplineDistance(spl2);
            tub->time = 0.0f;
            speed *= 0.5f;
            speed *= 0.125f;
            tub->duration = speed;
            if (is_player && Level == 4 && tub->track == 4) {
                GemPath = 2;
            }
        } else {
            control = 0;
        }
    } else if (tub->wait != 0) {
        tub->wait--;
    }
    if (control == 1) {
        ratio = tub->time / tub->duration;
        spl = SplTab[tub->track + 0x1D].spl;
        spl2 = SplTab[tub->track + 0x2D].spl;
        PointAlongSpline(spl, ratio, &p0, &yrot0, &xrot0);
        PointAlongSpline(spl2, ratio, &p1, &yrot1, &xrot1);
        mid.x = (p0.x + p1.x) * 0.5f;
        mid.y = (p0.y + p1.y) * 0.5f;
        mid.z = (p0.z + p1.z) * 0.5f;
        if (player->obj.vehicle_frame >= 2) {
            tub->c->obj.pos.x += (mid.x - tub->c->obj.pos.x) * 0.2f;
            tub->c->obj.pos.y += (mid.y - tub->c->obj.pos.y) * 0.2f;
            tub->c->obj.pos.z += (mid.z - tub->c->obj.pos.z) * 0.2f;
        } else {
            tub->c->obj.pos = mid;
        }
        yrot = yrot0 + RotDiff(yrot0, yrot1) / 2;
        xrot = xrot0 + RotDiff(xrot0, xrot1) / 2;
        NuVecSub(&p0, &p0, &tub->c->obj.pos);
        NuVecSub(&p1, &p1, &tub->c->obj.pos);
        ang = NuAtan2D(p1.x - p0.x, p1.z - p0.z);
        NuVecRotateY(&p0, &p0, -ang);
        NuVecRotateY(&p1, &p1, -ang);
        ang = -NuAtan2D(p1.y - p0.y, p1.z - p0.z);
        if (tub->count != 0 && tub->wait != 0) {
            tub->c->obj.xrot = tub->old_xrot + RotDiff(tub->old_xrot, xrot) * (TUBWAIT - tub->wait) / TUBWAIT;
            tub->c->obj.hdg = tub->old_yrot + RotDiff(tub->old_yrot, yrot) * (TUBWAIT - tub->wait) / TUBWAIT;
            tub->c->obj.zrot = tub->old_zrot + RotDiff(tub->old_zrot, ang) * (TUBWAIT - tub->wait) / TUBWAIT;
        } else {
            tub->c->obj.hdg = yrot;
            tub->c->obj.xrot = xrot;
            tub->c->obj.zrot = ang;
            tub->old_xrot = tub->c->obj.xrot;
            tub->old_yrot = tub->c->obj.hdg;
            tub->old_zrot = tub->c->obj.zrot;
        }
        spl = SplTab[63].spl;
        if (spl != 0) {
            a = (struct nuvec_s *)spl->pts;
            b = (struct nuvec_s *)(spl->pts + spl->ptsize);
            if (Level == 0x1C && race_finished == 0) {
                start = TubLink[tub->track].start;
                end = TubLink[tub->track].end;
                if (end < start) {
                    if ((tub->c->obj.pos.x - a->x) * (b->z - a->z) +
                        (tub->c->obj.pos.z - a->z) * (a->x - b->x) >= 0.0f) {
                        end = start + 1;
                    } else {
                        start = 0;
                    }
                }
                tub->lap_position = start + (end - start) * ratio;
            }
            if (LineCrossed(tub->c->obj.oldpos.x, tub->c->obj.oldpos.z, tub->c->obj.pos.x,
                            tub->c->obj.pos.z, a->x, a->z, b->x, b->z) != 0) {
                tub->laps++;
                if (tub->laps == MAXLAPS) {
                    race_finished = 1;
                    tub->finishframes = (tub->place == 1) ? 100 : 50;
                }
            }
            if (tub->laps >= MAXLAPS && tub->finished < tub->finishframes) {
                tub->finished++;
            }
        }
    }
    tub->c->obj.mom = v000;
    if (is_player) {
        VEHICLECONTROL = control;
    }
}

void MoveMINETUB(struct creature_s *plr, struct nupad_s *pad) {
    struct nuvec_s d;
    struct nuvec_s pos;
    struct tubinfo_s *tub[3];
    struct obj_s obj;
    struct nugspline_s *spl;
    struct nuvec_s *p0;
    struct nuvec_s *p1;
    s32 LEFT;
    s32 RIGHT;
    s32 tilt;
    s32 sfx;
    f32 ratio;

    TubLink = (Level == 0x1C) ? TubLinkGT : TubLinkCR;
    OppTub.c->obj.oldpos = OppTub.c->obj.pos;
    if (Level == 0x1C && GameTimer.frame < 150) {
        race_finished = 0;
    }
    if (PlrTub.boost != 0) {
        PlrTub.boost--;
    }
    if (Level == 0x1C) {
        if (GameTimer.frame == 0 || GameTimer.frame == 50 || GameTimer.frame == 100) {
            sfx = 0x4F;
        } else {
            sfx = (GameTimer.frame == 150) ? 0x4C : -1;
        }
        if (sfx != -1) {
            GameSfx(sfx, 0);
        }
    }
    RIGHT = 0;
    LEFT = 0;
    if (plr->obj.pad_speed > 0.0f &&
        (Level != 0x1C || (PlrTub.laps < MAXLAPS && GameTimer.frame >= 150))) {
        if ((u16)(plr->obj.pad_angle + 0x6AAA) < 0x5555) {
            LEFT = 1;
        } else {
            RIGHT = ((u16)(plr->obj.pad_angle - 0x1556) < 0x5555);
        }
    }
    tilt = plr_tub_tilt;
    if (plr_tub_tilt < plr_tub_tilttarget) {
        plr_tub_tilt++;
    } else if (plr_tub_tilt > plr_tub_tilttarget) {
        plr_tub_tilt--;
    }
    if (plr_tub_tilt == plr_tub_tilttarget) {
        if (plr_tub_tilt > 0) {
            if (RIGHT == 0) {
                plr_tub_tilttarget = 0;
            }
        } else if (plr_tub_tilt < 0) {
            if (LEFT == 0) {
                plr_tub_tilttarget = 0;
            }
        } else {
            if (plr_tub_tilt != tilt) {
                NewBuzz(&plr->rumble, 5);
            }
            if (LEFT != 0) {
                plr_tub_tilttarget = -plr_tub_tiltlock;
            } else if (RIGHT != 0) {
                plr_tub_tilttarget = plr_tub_tiltlock;
            }
        }
    }
    RIGHT = 0;
    LEFT = 0;
    if (plr_tub_tilt < 0) {
        LEFT = 1;
    } else {
        RIGHT = (plr_tub_tilt > 0);
    }
    if (plr_tub_tilttarget < 0) {
        plr->obj.direction = 3;
    } else if (plr_tub_tilttarget > 0) {
        plr->obj.direction = 4;
    } else {
        plr->obj.direction = 0;
    }
    MoveTub(&PlrTub, LEFT, RIGHT);
    if (Level == 0x1C) {
        VEHICLECONTROL = 1;
        MoveTub(&OppTub, 0, 0);
        d.x = OppTub.c->obj.pos.x - OppTub.c->obj.oldpos.x;
        d.z = OppTub.c->obj.pos.z - OppTub.c->obj.oldpos.z;
        OppTub.c->obj.xz_distance = NuFsqrt(d.x * d.x + d.z * d.z);
        pos.x = OppTub.c->obj.pos.x;
        pos.y = OppTub.c->obj.pos.y +
                (OppTub.c->obj.bot + OppTub.c->obj.top) * OppTub.c->obj.SCALE * 0.5f;
        pos.z = OppTub.c->obj.pos.z;
        GetLights(&pos, &OppTub.c->lights, 1);
        NuVecSub(&d, &OppTub.c->obj.pos, &PlrTub.c->obj.pos);
        if (d.x * d.x + d.y * d.y + d.z * d.z < 1.0f) {
            spl = SplTab[63].spl;
            if (spl != 0) {
                p0 = (struct nuvec_s *)spl->pts;
                p1 = (struct nuvec_s *)(spl->pts + spl->ptsize);
            } else {
                p1 = 0;
                p0 = 0;
            }
            if (PlrTub.lap_position >= OppTub.lap_position) {
                tub[0] = &PlrTub;
                tub[1] = &OppTub;
            } else {
                tub[0] = &OppTub;
                tub[1] = &PlrTub;
            }
            if (p0 != 0 && p1 != 0 &&
                (tub[0]->c->obj.pos.x - p0->x) * (p1->z - p0->z) +
                (tub[0]->c->obj.pos.z - p0->z) * (p0->x - p1->x) >= 0.0f &&
                !((tub[1]->c->obj.pos.x - p0->x) * (p1->z - p0->z) +
                  (tub[1]->c->obj.pos.z - p0->z) * (p0->x - p1->x) >= 0.0f)) {
                tub[2] = tub[0];
                tub[0] = tub[1];
                tub[1] = tub[2];
            }
            tub[1]->stall = 1;
            if (race_finished == 0) {
                if (tub[0] == &PlrTub) {
                    tub[0]->boost = (s32)TUBBOOSTFRAMES;
                    NewRumble(&player->rumble, 0xFF);
                    NewBuzz(&player->rumble, 0x32);
                } else {
                    NewRumble(&player->rumble, 0x7F);
                    NewBuzz(&player->rumble, 0x19);
                }
                if (tub_overlap == 0) {
                    GameSfx(0x70, &plr->obj.pos);
                    NewBuzz(&plr->rumble, 10);
                }
            }
            tub_overlap = 1;
        } else {
            tub_overlap = 0;
        }
        if (race_finished == 0) {
            if (PlrTub.laps > OppTub.laps ||
                (PlrTub.laps == OppTub.laps && PlrTub.lap_position >= OppTub.lap_position)) {
                PlrTub.place = 1;
                OppTub.place = 2;
            } else {
                OppTub.place = 1;
                PlrTub.place = 2;
            }
        }
    }
    obj = PlrTub.c->obj;
    ratio = (f32)plr_tub_tilt / (f32)plr_tub_tiltlock;
    obj.pos.x += NuTrigTable[(u16)(obj.hdg + 0x4000)] * 0.5f * 2.0f * ratio;
    obj.pos.y += (f32)abs(plr_tub_tilt) / (f32)plr_tub_tiltlock * 0.25f;
    obj.pos.z += NuTrigTable[(u16)(obj.hdg - 0x8000)] * 0.5f * 2.0f * ratio;
    TempTubPos = obj.pos;
    HitItems(&obj);
    if (HitCrates(&obj, 1) != 0 && (temp_crate_type == 0x10 || temp_crate_type == 9)) {
        KillPlayer(&plr->obj, 0xB);
    }
    WumpaCollisions(&obj);
    plr->obj.attack = 0;
    if (plr->obj.mask != 0 && plr->obj.mask->active >= 3) {
        plr->obj.attack = 0x80;
    }
    plr->obj.attack |= 0x400;
    if (Level == 4 || GameTimer.frame >= 150) {
        gamesfx_effect_volume = (s32)(plr->obj.xyz_distance * 32766.0f) / 0.099999994f;
        if (gamesfx_effect_volume > 0x7FFE) {
            gamesfx_effect_volume = 0x7FFE;
        }
        gamesfx_pitch = (s32)(plr->obj.xyz_distance * 235.0f) / 0.099999994f + 470.0f;
        GameSfxLoop(0x73, &PlrTub.c->obj.pos);
    }
}


void AnimateMINETUB(struct creature_s *plr) {
    struct nuvec_s pos;
    s32 left;
    s32 right;
    s32 left_ang;
    s32 right_ang;

    if ((u32)plr->obj.dead >= 2) {
        plr->obj.anim.newaction = plr->obj.die_action;
    } else if (plr->obj.direction == 3) {
        plr->obj.anim.newaction = 0x65;
    } else if (plr->obj.direction == 4) {
        plr->obj.anim.newaction = 0x67;
    } else {
        plr->obj.anim.newaction = 0x68;
    }
    UpdateCharacterIdle(plr, 0);
    if (plr->obj.anim.newaction != plr->obj.die_action) {
        left = 0x5C;
        right = 0x5C;
        left_ang = 0x4000;
        right_ang = -0x4000;
        if (plr_tub_tilt < 0) {
            left_ang = -0x4000;
            right = 0x5D;
        } else if (plr_tub_tilt > 0) {
            left = 0x5C;
            right = 0x5D;
            right_ang = 0x4000;
        } else if (PlrTub.spark != 0) {
            left = 0x5D;
            right = 0x5D;
        }
        if (Level != 0x1C || race_finished == 0) {
            pos.x = plr->obj.pos.x + NuTrigTable[(plr->obj.hdg + left_ang) & 0xFFFF] * 0.45f;
            pos.y = plr->obj.pos.y;
            pos.z = plr->obj.pos.z + NuTrigTable[(plr->obj.hdg + left_ang + 0x4000) & 0xFFFF] * 0.45f;
            AddVariableShotDebrisEffect(GDeb[left].i, &pos, 1, 0, (s16)plr->obj.hdg);
            pos.x = plr->obj.pos.x + NuTrigTable[(plr->obj.hdg + right_ang) & 0xFFFF] * 0.45f;
            pos.y = plr->obj.pos.y;
            pos.z = plr->obj.pos.z + NuTrigTable[(plr->obj.hdg + right_ang + 0x4000) & 0xFFFF] * 0.45f;
            AddVariableShotDebrisEffect(GDeb[right].i, &pos, 1, 0, (s16)plr->obj.hdg);
        }
    }
    if (Level == 0x1C) {
        if (opptub_wait != 0 && --opptub_wait == 0) {
            if (opptub_action == 0x62) {
                opptub_action = (qrand() <= 0x7FFF) ? 0x5F : 0x60;
                opptub_wait = (s32)(ModelAnimDuration(OppTub.c->obj.character, opptub_action,
                                                      0.0f, 0.0f) * 50.0f);
            } else {
                opptub_action = 0x62;
                opptub_wait = (qrand() * 50) / 0x10000 + 50;
            }
        }
        OppTub.c->obj.anim.oldaction = OppTub.c->obj.anim.action;
        OppTub.c->obj.anim.newaction = opptub_action;
        UpdateAnimPacket(OppTub.c->obj.model, &OppTub.c->obj.anim, 0.59999996f,
                         OppTub.c->obj.xz_distance);
    }
}
