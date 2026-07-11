/* Retail symbols mod code uses; each is bound to its fixed address at
 * link time (gen_symbols.py). Grow as needed -- src/ is the ground truth
 * for signatures. Plain extern only: mod code is compiled -G0.
 *
 * Verify a global's SEMANTICS from the consuming asm before writing it,
 * not from its name: WUMPACOUNT is the live count of Wumpa[] entities
 * (blindly incrementing it made the engine walk past the array end and
 * corrupt memory at Crash's death); the HUD collected counter is
 * plr_wumpas, whose >=100 handling is a tolerant while-loop. */
#ifndef MODSDK_RETAIL_H
#define MODSDK_RETAIL_H

struct nuvec_s; /* defined in creature.h; only used by pointer below */
struct obj_s;   /* defined in creature.h; only used by pointer below */

extern int Level;
extern int PLAYERCOUNT;             /* 0/1 flag: player exists this level */
extern unsigned short plr_wumpas;   /* HUD wumpa counter, player 0 */
extern int Paused;                  /* 0 = running; ramps 1..0x19 while in-game paused */
extern int VEHICLECONTROL;          /* 0 on foot / 1 riding vehicle / 2 swimming */
extern float vtog_time;             /* vehicle mount-transition timer (== vtog_duration when complete) */
extern float vtog_duration;         /* mount-transition length */
extern int warp_level;              /* hub (Level 0x25) teleport target; -1 = not
                                     * warping. Set when Crash steps on a hub
                                     * teleporter, before the level loads. */
extern int in_finish_range;         /* ramps 1..0x32 while Crash stands in a
                                     * teleport zone (hub node / level exit);
                                     * 0 = not in a zone. Goes >0 at zone entry,
                                     * before warp_level commits. */
extern float in_finish_pos[3];      /* world position of the teleport node Crash
                                     * is in range of; set while in_finish_range>0.
                                     * The teleporter (JonProbe) is drawn here. */

/* Spawn the finite warp-debris effect at an object's mid-body (the "Crash
 * dissolves into sparks" teleport-out look). Fire-and-forget: the effect
 * self-animates in the game's debris pass. 0x00260CD8 */
void AddWarpDebris(struct obj_s *obj);

/* Play a game sound effect. pos != NULL routes through NuSoundPlay3d, which
 * spatialises + attenuates it by distance from the listener (so it is only
 * audible when in range); pos == NULL plays it non-positional. gamesfx_effect_volume
 * is the base volume (-1 = use the sfx's default); set it before the call, the game
 * resets it to -1 afterwards. SFX 0x1E is the warp/dissolve sound. 0x0024FA90 */
void GameSfx(int id, struct nuvec_s *pos);
extern int gamesfx_effect_volume;   /* 0x00632960 */

/* --- Teleporter prop (the spinning hub/level-exit ring) -----------------
 * The teleporter you see is NOT a placed object: JonProbe (0x1DB150) draws it
 * every frame from global probe state via Draw3DCharacter, using the model for
 * character 0xB1 (CModel[CRemap[0xB1]]). We reuse that same draw primitive to
 * render a teleporter at the puppet's position, with our own isolated state, so
 * it never touches the local player's singleton probe globals. CModel/CRemap
 * are declared in creature.h. */
#define COOP_TELEPORTER_CHAR 0xB1

/* Draw one character model at a world pos with Euler rotation. scale must be
 * != 0 (0 early-returns). action = -1 => static pose (no anim eval); rotorder
 * 0 => X,Y,Z. EABI: pos=a0, rot=a1..a3, scale=f12, anim_time=f13, model=t0,
 * action=t1, rotorder=t2. 0x001EE410 */
void Draw3DCharacter(struct nuvec_s *pos, int rotx, int roty, int rotz,
                     float scale, float anim_time, void *model,
                     int action, int rotorder);

void UpdateLevel(void);   /* per-frame, gameplay levels only */
void DoInput(void);       /* per-frame, menus included */

/* 2D text in normalized screen space (game HUD/menu font); align 0=left,
 * 8=centre, 2=right. x,y ~ -1..1, z = 1.0, sx/sy/sz = scale. 0x00238280 */
void Text3D(char *text, int align, int colour, float x, float y, float z,
            float sx, float sy, float sz);
/* Project count world points (nuvec, stride 12) to screen space through the
 * global screen matrix when mtx is NULL. 0x00114048 */
void NuCameraTransformScreen(struct nuvec_s *dest, struct nuvec_s *src,
                             int count, void *mtx);
/* Transform count world points (nuvec, stride 12) into CAMERA/view space
 * through the global view matrix when mtx is NULL (affine, no perspective
 * divide). dest.z is the signed depth along the camera axis: its sign is the
 * front/back test and its magnitude is the true perspective distance. 0x00113D18 */
void NuCameraTransformView(struct nuvec_s *dest, struct nuvec_s *src,
                           int count, void *mtx);

#endif
