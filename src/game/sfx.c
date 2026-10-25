/*
 * Unit: game/sfx
 *
 * Functions:
 *   0x0024f318 InitLevelSfxTables
 *   0x0024f680 InitGlobalSfx
 *   0x0024f778 InitLocalSfx
 *   0x0024fa08 UpdateGameSfx
 *   0x0024fa90 GameSfx
 *   0x0024fd30 GameSfxLoop
 *   0x0024fe88 GameAudioUpdate
 *   0x00250298 ResetGameSfx
 *   0x00250310 GameMusic
 *   0x00250458 PauseGameAudio
 *   0x002504f0 ResumeGameAudio
 *   0x00250518 TestLocalSfx
 */

#include "numath/nuvec.h"
#include "game/gametypes.h"
#include "game/creature.h"
#include "nusound/nusound.h"
#include "game/sfx.h"


extern void NuStrCpy(char *dst, const char *src);
extern void NuStrCat(char *dst, const char *src);
extern void NuSoundSetGlobalDat(void *dat);
extern void NuSoundInit(NUSOUND_FILENAME_INFO *info);

s32 sfxcount_global = 197;
int sfxcount_local = 0;

/* Defined after InitLocalSfx, where retail emits them. */
extern GameSound *CurSfxTabLocal;
extern s32 SFXCOUNT_ALL;

extern struct game_s Game;
extern s32 PLAYERCOUNT;
extern struct cammtx_s GameCam[];

extern void NuSoundPlay3d(struct nuvec_s *pos, s32 sfx, s32 volL, s32 volR,
                          s32 pitch);
extern void NuSoundPlayChan(s32 track, s32 volL, s32 volR, s32 pitch, s32 channel);
extern void NuSoundPlay(s32 sfx, s32 volL, s32 volR, s32 pitch);
extern int NuSoundSetChannelPitch(s32 chan, s32 pitch, s32 stereo);
extern void NuSoundUpdate(void);
extern int SOUND_StopSound(s32 chan);
extern s32 NuSoundStopStream(s32 channel);
extern void NuSoundPlayStereo(s32 track, s32 sfx2, s32 volL, s32 volR, s32 pitch);
extern void NuSoundPlayStereo2(s32 track, s32 sfx2, s32 volL, s32 volR, s32 pitch);
extern void NuSoundPlay3dLoopSfx(struct nuvec_s *pos, s32 sfx, s32 volL, s32 volR,
                                 s32 pitch);
extern void InitLocalSfx(GameSound *sfxTab, s32 totalCount);
extern void NuSoundKillAllAudio(void);

extern struct ldata_s LData[];

GameSound SfxTabGLOBAL[197] = {
    {"AkuakuTrapped1", 941, 0x3FFF, 0, 0, 50, 0, "GENERIC\\AKU_BKBN", 11025, 1, 0, 0xFF, 0x1000},
    {"LoseAkuaku", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\AKU_LSE", 11025, 1, 0, 0xFF, 0x1001},
    {"WoodCrateBoing", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\BCEWDCRT", 11025, 1, 0, 0xFF, 0x1002},
    {"BazookaLaser", 941, 0x3FFF, 0, 0, 50, 0, "GENERIC\\BZL_LSR", 11025, 1, 0, 0xFF, 0x1003},
    {"BazookaHit", 941, 0x3FFF, 10, 192, 0, 0, "GENERIC\\BZK_EXPL", 11025, 1, 0, 0xFF, 0x1004},
    {"BazookaLock", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\BZK_LOK", 11025, 1, 0, 0xFF, 0x1005},
    {"BazookaShoot", 941, 0x3FFF, 5, 127, 0, 0, "GENERIC\\BZK_SHT", 11025, 1, 0, 0xFF, 0x1006},
    {"CrashBurn", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\C_BURN", 11025, 1, 0, 0xFF, 0x1007},
    {"CrashFall", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\C_ENDFAL", 11025, 1, 0, 0xFF, 0x1008},
    {"CrashFrozen", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\C_FRZN", 11025, 1, 0, 0xFF, 0x1009},
    {"CrashSlide1", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\C_ICE1", 11025, 1, 0, 0xFF, 0x100A},
    {"CrashSlide2", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\C_ICE2", 11025, 1, 0, 0xFF, 0x100B},
    {"CrashSlip1", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\C_ICSLP1", 11025, 1, 0, 0xFF, 0x100C},
    {"CrashSlip2", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\C_ICSLP2", 11025, 1, 0, 0xFF, 0x100D},
    {"MtlCrateBounce", 941, 0x3FFF, 0, 191, 0, 0, "GENERIC\\C_JMETBX", 11025, 1, 0, 0xFF, 0x100E},
    {"CrashScratch", 941, 0xCCC, 0, 0, 0, 0, "GENERIC\\C_SCRACH", 11025, 1, 0, 0xFF, 0x100F},
    {"CrashShiver", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\C_SHIVER", 11025, 1, 0, 0xFF, 0x1010},
    {"CrashSlam", 941, 0x3FFF, 0, 191, 0, 0, "GENERIC\\C_SLAM", 11025, 1, 0, 0xFF, 0x1011},
    {"CrashSlide", 941, 0x3FFF, 0, 127, 0, 0, "GENERIC\\C_SLIDE", 11025, 1, 0, 0xFF, 0x1012},
    {"CrashSpin", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\C_SPIN", 11025, 1, 0, 0xFF, 0x1013},
    {"CrashKnockOut", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\C_SPINDI", 11025, 1, 0, 0xFF, 0x1014},
    {"CrashWoa", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\C_WHOA", 11025, 1, 0, 0xFF, 0x1015},
    {"CrashExplode", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\CC_DTH2", 11025, 1, 0, 0xFF, 0x1016},
    {"CheckPointOpen", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\CHK_PT", 11025, 1, 0, 0xFF, 0x1017},
    {"MenuMove", 1882, 0x3FFF, 0, 0, 0, 0, "GENERIC\\CLET_BP", 22050, 1, 0, 0xFF, 0x1018},
    {"AddWumpa", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\CLLCTFRT", 11025, 1, 0, 0xFF, 0x1019},
    {"CocoSpin", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\CO_SPIN", 11025, 1, 0, 0xFF, 0x101A},
    {"WoodDangle1", 941, 0x7FFE, 0, 0, 0, 0, "GENERIC\\CR_1HWD", 11025, 1, 0, 0xFF, 0x101B},
    {"WoodDangle2", 941, 0x7FFE, 0, 0, 0, 0, "GENERIC\\CR_2HWD", 11025, 1, 0, 0xFF, 0x101C},
    {"CrashStubToe", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\CR_ADTH", 11025, 1, 0, 0xFF, 0x101D},
    {"Invisible", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\CR_INVS", 11025, 1, 0, 0xFF, 0x101E},
    {"MetalFootStep1", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\CR_MFS1", 11025, 1, 0, 0xFF, 0x101F},
    {"MetalFootStep2", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\CR_MFS2", 11025, 1, 0, 0xFF, 0x1020},
    {"Visible", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\CR_RPRS", 11025, 1, 0, 0xFF, 0x1021},
    {"CrashSigh", 823, 0x3FFF, 0, 0, 0, 0, "GENERIC\\CR_SIGH", 11025, 1, 0, 0xFF, 0x1022},
    {"CrashChopped", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\CR_SLC", 11025, 1, 0, 0xFF, 0x1023},
    {"CrashSquashed", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\CR_SQSH", 11025, 1, 0, 0xFF, 0x1024},
    {"CrateSmash", 941, 0x3FFF, 5, 0, 16, 0, "GENERIC\\CRTCRASH", 11025, 1, 0, 0xFF, 0x1025},
    {"CollectCrystal", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\CRYSTAL", 11025, 1, 0, 0xFF, 0x1026},
    {"ScubaBreath", 450, 0x2AAA, 0, 0, 0, 0, "GENERIC\\CUW_BRTH", 11025, 1, 0, 0xFF, 0x1027},
    {"ScubaSpin", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\CUW_SPN", 11025, 1, 0, 0xFF, 0x1028},
    {"ScubaFlipper", 941, 0x1FFF, 0, 0, 0, 0, "GENERIC\\CUW_SWM2", 11025, 1, 0, 0xFF, 0x1029},
    {"WumpaSpin", 941, 0x3FFF, 0, 0, 5, 0, "GENERIC\\FRT_SPIN", 11025, 1, 0, 0xFF, 0x102A},
    {"ExtraLife", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\CLLCT2UP", 11025, 1, 0, 0xFF, 0x102B},
    {"MunchWumpa", 941, 0x3FFF, 0, 0, 10, 0, "GENERIC\\FRUITCHP", 11025, 1, 0, 0xFF, 0x102C},
    {"WaterFootStep1", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\FS_WAT1", 11025, 1, 0, 0xFF, 0x102D},
    {"WaterFootStep2", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\FS_WAT2", 11025, 1, 0, 0xFF, 0x102E},
    {"Jump", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\JUMP", 11025, 1, 0, 0xFF, 0x102F},
    {"MechGunCock", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\MCH_GC", 11025, 1, 0, 0xFF, 0x1030},
    {"MechGunExplode", 941, 0x3FFF, 16, 191, 0, 0, "GENERIC\\MCH_GEX", 11025, 1, 0, 0xFF, 0x1031},
    {"MechJump", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\MCH_JMP", 11025, 1, 0, 0xFF, 0x1032},
    {"NitroExclmatn", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\NIT_EXPL", 11025, 1, 0, 0xFF, 0x1033},
    {"NitroWobble", 941, 0x2AAA, 0, 0, 25, 0, "GENERIC\\NITROBOX", 11025, 1, 0, 0xFF, 0x1034},
    {"Exclamation", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\SBOXJMP", 11025, 1, 0, 0xFF, 0x1035},
    {"MenuSelect", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\SEL_BP", 11025, 1, 0, 0xFF, 0x1036},
    {"SpinOpponent", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\SPIN_NME", 11025, 1, 0, 0xFF, 0x1037},
    {"JmpOnRnfrcdCrt", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\ST_CRT", 11025, 1, 0, 0xFF, 0x1038},
    {"HitReinfrcdCrt", 941, 0x3FFF, 0, 0, 10, 0, "GENERIC\\STCR_HIT", 11025, 1, 0, 0xFF, 0x1039},
    {"SubEngine", 941, 0x2AAA, 0, 0, 0, 0, "GENERIC\\SUB_STD", 11025, 1, 0, 0xFF, 0x103A},
    {"TNTExplode", 941, 0x3FFF, 10, 255, 16, 0, "GENERIC\\TNTXPLOD", 11025, 1, 0, 0xFF, 0x103B},
    {"MenuBack", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\UDLT_BP", 11025, 1, 0, 0xFF, 0x103C},
    {"CollectAkuaku", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\UGGABUGA", 11025, 1, 0, 0xFF, 0x103D},
    {"Wade1", 941, 0x7FFE, 0, 0, 0, 0, "GENERIC\\WD1_WAT", 11025, 1, 0, 0xFF, 0x103E},
    {"Wade2", 941, 0x7FFE, 0, 0, 0, 0, "GENERIC\\WD2_WAT", 11025, 1, 0, 0xFF, 0x103F},
    {"BallBounce", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\BALL_BNC", 11025, 1, 0, 0xFF, 0x1040},
    {"BallRollWood", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\BALL_BRI", 11025, 1, 0, 0xFF, 0x1041},
    {"BallHitLight", 941, 0x3FFF, 0, 0, 16, 0, "GENERIC\\BL_HIT1", 11025, 1, 0, 0xFF, 0x1042},
    {"BallRollGlass", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\BL_STD", 11025, 1, 0, 0xFF, 0x1043},
    {"BallHitInTube", 941, 0x3FFF, 0, 0, 50, 0, "GENERIC\\BLTUB_HB", 11025, 1, 0, 0xFF, 0x1044},
    {"PotionExplode", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\BKR_BEX", 11025, 1, 0, 0xFF, 0x1045},
    {"CocoSweep", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\CC_SKCK", 11025, 1, 0, 0xFF, 0x1046},
    {"FeetLandInWatr", 941, 0x7FFE, 0, 0, 33, 0, "GENERIC\\CJMP_LND", 11025, 1, 0, 0xFF, 0x1047},
    {"LandInMech", 941, 0x3FFF, 0, 191, 10, 0, "GENERIC\\CR_MCH", 11025, 1, 0, 0xFF, 0x1048},
    {"BmpKnightsShld", 941, 0x3FFF, 0, 0, 50, 0, "GENERIC\\CR_SHLD", 11025, 1, 0, 0xFF, 0x1049},
    {"FlameThrower", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\EN_FLT", 11025, 1, 0, 0xFF, 0x104A},
    {"CyclonLasrFire", 1882, 0x3FFF, 0, 0, 10, 0, "GENERIC\\EN_LAS", 22050, 1, 0, 0xFF, 0x104B},
    {"CollectClock", 941, 0x3FFF, 0, 127, 0, 0, "GENERIC\\P_SWATCH", 11025, 1, 0, 0xFF, 0x104C},
    {"BurningTorch", 1882, 0x1FFF, 0, 0, 0, 0, "GENERIC\\SN_TORCH", 22050, 1, 0, 0xFF, 0x104D},
    {"ClockTick", 1882, 0x3FFF, 0, 0, 0, 0, "GENERIC\\SW_TICK", 22050, 1, 0, 0xFF, 0x104E},
    {"TNTBeep", 1882, 0x3FFF, 0, 0, 0, 0, "GENERIC\\TNT_BEEP", 22050, 1, 0, 0xFF, 0x104F},
    {"BalloonPop", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\BAL_POP", 11025, 1, 0, 0xFF, 0x1050},
    {"BananaHitGrnd", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\BAN_HIT", 11025, 1, 0, 0xFF, 0x1051},
    {"BatSqueak", 941, 0x3FFF, 0, 0, 100, 0, "GENERIC\\BAT_SQ", 11025, 1, 0, 0xFF, 0x1052},
    {"BatFlap", 941, 0x3FFF, 0, 0, 25, 0, "GENERIC\\BAT_WING", 11025, 1, 0, 0xFF, 0x1053},
    {"LandOnSnwBoard", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\CC_SB", 11025, 1, 0, 0xFF, 0x1054},
    {"FireGlderBulet", 941, 0x3FFF, 0, 0, 10, 0, "GENERIC\\CR_BULT", 11025, 1, 0, 0xFF, 0x1055},
    {"GliderExplode", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\CR_CRSH", 11025, 1, 0, 0xFF, 0x1056},
    {"LandInJeep", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\CR_JPLND", 11025, 1, 0, 0xFF, 0x1057},
    {"GliderPlummet", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\CRGL_DV", 11025, 1, 0, 0xFF, 0x1058},
    {"GliderCrshLand", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\CRGL_EXP", 11025, 1, 0, 0xFF, 0x1059},
    {"GliderTakeHit", 941, 0x3FFF, 0, 0, 25, 0, "GENERIC\\CRGL_HIT", 11025, 1, 0, 0xFF, 0x105A},
    {"GliderFloat", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\CRGL_SD2", 11025, 1, 0, 0xFF, 0x105B},
    {"GliderRoll", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\CRGL_WHO", 11025, 1, 0, 0xFF, 0x105C},
    {"DepthChrgRlse", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\DC_RLS", 11025, 1, 0, 0xFF, 0x105D},
    {"EnemyCrashLand", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\ENE_CRSH", 11025, 1, 0, 0xFF, 0x105E},
    {"EnemyExplode", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\ENE_EXPL", 11025, 1, 0, 0xFF, 0x105F},
    {"EnemyFire", 941, 0x3FFF, 0, 0, 5, 0, "GENERIC\\ENE_MGUN", 11025, 1, 0, 0xFF, 0x1060},
    {"EnemyEngine", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\ENE_STDY", 11025, 1, 0, 0xFF, 0x1061},
    {"LaserLoop", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\GLSR_STD", 11025, 1, 0, 0xFF, 0x1062},
    {"GiantPstonStmp", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\GT_PIST", 11025, 1, 0, 0xFF, 0x1063},
    {"HammerDown", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\HAM_DOWN", 11025, 1, 0, 0xFF, 0x1064},
    {"HammerUp", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\HAM_UP", 11025, 1, 0, 0xFF, 0x1065},
    {"JeepSuspension", 941, 0x3FFF, 0, 0, 10, 0, "GENERIC\\JP_BMP1", 11025, 1, 0, 0xFF, 0x1066},
    {"JeepTyreWood", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\JP_BRTYR", 11025, 1, 0, 0xFF, 0x1067},
    {"JeepSkidDirt", 941, 0x3FFF, 0, 0, 50, 0, "GENERIC\\JP_DISKD", 11025, 1, 0, 0xFF, 0x1068},
    {"JeepFinish", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\JP_END", 11025, 1, 0, 0xFF, 0x1069},
    {"JeepEngine", 941, 0x7FFE, 0, 0, 0, 0, "GENERIC\\JP_IDLE", 11025, 1, 0, 0xFF, 0x106A},
    {"JeepLand", 941, 0x3FFF, 0, 0, 10, 0, "GENERIC\\JP_LAND", 11025, 1, 0, 0xFF, 0x106B},
    {"JeepHitRamp", 941, 0x3FFF, 0, 0, 10, 0, "GENERIC\\JP_RAMP", 11025, 1, 0, 0xFF, 0x106C},
    {"JeepRev", 941, 0x3FFF, 0, 127, 25, 0, "GENERIC\\JP_REV1", 11025, 1, 0, 0xFF, 0x106D},
    {"KnifeSpin", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\KNF_SPNR", 11025, 1, 0, 0xFF, 0x106E},
    {"MammothRoar", 941, 0x3FFF, 0, 0, 50, 0, "GENERIC\\MAM_ROAR", 11025, 1, 0, 0xFF, 0x106F},
    {"MineTubCrash", 941, 0x3FFF, 0, 0, 50, 0, "GENERIC\\MC_CRSH", 11025, 1, 0, 0xFF, 0x1070},
    {"MineTubDeath", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\MC_DTH", 11025, 1, 0, 0xFF, 0x1071},
    {"MineTubTakeHit", 941, 0x3FFF, 0, 0, 100, 0, "GENERIC\\MC_HIT", 11025, 1, 0, 0xFF, 0x1072},
    {"MineTubOn2Rals", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\MC_STDY", 11025, 1, 0, 0xFF, 0x1073},
    {"MechFootStep", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\MCH_FS1", 11025, 1, 0, 0xFF, 0x1074},
    {"MonkeyChatter", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\MON_CHAT", 11025, 1, 0, 0xFF, 0x1075},
    {"MonkeyScreech", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\MON_SCRE", 11025, 1, 0, 0xFF, 0x1076},
    {"WalDevicesLoop", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\MW_HUM", 11025, 1, 0, 0xFF, 0x1077},
    {"NinjaAppear", 941, 0x7FFE, 0, 0, 0, 0, "GENERIC\\NIN_APR", 11025, 1, 0, 0xFF, 0x1078},
    {"NinjaSwish", 941, 0x7FFE, 0, 0, 0, 0, "GENERIC\\NIN_ARM", 11025, 1, 0, 0xFF, 0x1079},
    {"NinjaClap", 941, 0xBFFD, 0, 0, 0, 0, "GENERIC\\NIN_CLP1", 11025, 1, 0, 0xFF, 0x107A},
    {"NinjaVanish", 941, 0x7FFE, 0, 0, 0, 0, "GENERIC\\NIN_DAPR", 11025, 1, 0, 0xFF, 0x107B},
    {"ParrotDie", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\PAR_DIE", 11025, 1, 0, 0xFF, 0x107C},
    {"ParrotSquawk", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\PAR_SQWK", 11025, 1, 0, 0xFF, 0x107D},
    {"ParrotFlap", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\PAR_WING", 11025, 1, 0, 0xFF, 0x107E},
    {"PistonDown", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\PIS_DOWN", 11025, 1, 0, 0xFF, 0x107F},
    {"RobotSnap", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\ROB1_SNP", 11025, 1, 0, 0xFF, 0x1080},
    {"SealBark", 941, 0x3FFF, 0, 0, 50, 0, "GENERIC\\SL_SLP", 11025, 1, 0, 0xFF, 0x1081},
    {"SpikeBallClunk", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\SPBAL_CL", 11025, 1, 0, 0xFF, 0x1082},
    {"SpikeBallSwish", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\SPIK_SW1", 11025, 1, 0, 0xFF, 0x1083},
    {"SpikeDoorClose", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\SPIKE_CL", 11025, 1, 0, 0xFF, 0x1084},
    {"SteamValveBrst", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\SVLV_BST", 11025, 1, 0, 0xFF, 0x1085},
    {"TrapDoorClose", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\TDOOR_CL", 11025, 1, 0, 0xFF, 0x1086},
    {"TrapDoorOpen", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\TDOOR_OP", 11025, 1, 0, 0xFF, 0x1087},
    {"FireTorpedo", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\TOR_FIR", 11025, 1, 0, 0xFF, 0x1088},
    {"WireSpark", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\WIR_SPK1", 11025, 1, 0, 0xFF, 0x1089},
    {"PlatFormInOut", 941, 0x1555, 0, 0, 0, 0, "GENERIC\\WM_PLAT", 11025, 1, 0, 0xFF, 0x108A},
    {"WumpaShot", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\WMP_GUN", 11025, 1, 0, 0xFF, 0x108B},
    {"WumpaHitObject", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\WMP_HITS", 11025, 1, 0, 0xFF, 0x108C},
    {"SoftFootStep1", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\FS_SOFT1", 11025, 1, 0, 0xFF, 0x108D},
    {"SoftFootStep2", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\FS_SOFT2", 11025, 1, 0, 0xFF, 0x108E},
    {"FishSplash", 941, 0x5FFE, 0, 0, 0, 0, "GENERIC\\FISHSPLA", 11025, 1, 0, 0xFF, 0x108F},
    {"GyroEngine", 941, 0x7FFE, 0, 0, 0, 0, "GENERIC\\HOV_MOT", 11025, 1, 0, 0xFF, 0x1090},
    {"LavaBubble", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\IND_1LAV", 11025, 1, 0, 0xFF, 0x1091},
    {"JellyFishBuble", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\JFSH_1B", 11025, 1, 0, 0xFF, 0x1092},
    {"LavaFlow", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\LAVAFLOW", 11025, 1, 0, 0xFF, 0x1093},
    {"LavaSteam", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\LAVSTEAM", 11025, 1, 0, 0xFF, 0x1094},
    {"LaserBeamLoop", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\LB_STDY", 11025, 1, 0, 0xFF, 0x1095},
    {"Bzzzt", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\LB1_SHRT", 11025, 1, 0, 0xFF, 0x1096},
    {"GridBeepLoop", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\LGRD_LP", 11025, 1, 0, 0xFF, 0x1097},
    {"PrplLightPlse", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\PLT_PUL", 11025, 1, 0, 0xFF, 0x1098},
    {"PotionBubble", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\POT_BG", 11025, 1, 0, 0xFF, 0x1099},
    {"RobotMotor", 941, 0x3FFF, 0, 0, 0, 0, "GENERIC\\ROB1_MVT", 11025, 1, 0, 0xFF, 0x109A},
    {"MonitorBeep", 941, 0x7FFE, 0, 0, 0, 0, "GENERIC\\SCR_BP", 11025, 1, 0, 0xFF, 0x109B},
    {"SpellFizzle", 941, 0x5FFE, 0, 0, 0, 0, "GENERIC\\WM_FIZ", 11025, 1, 0, 0xFF, 0x109C},
    {"GyroBlade", 941, 0x5FFE, 0, 0, 0, 0, "GENERIC\\HOV_BLA", 11025, 1, 0, 0xFF, 0x109D},
    {"castle bonus l", 1882, 0x1BFF, 0, 0, 0, 0, "MUSIC\\CASTLBNL", 22050, 0, 1, 0xFF, 0x0},
    {"castle bonus r", 1882, 0x1BFF, 0, 0, 0, 0, "MUSIC\\CASTLBNR", 22050, 0, 1, 0xFF, 0x1},
    {"fire bonus l", 1882, 0x1BFF, 0, 0, 0, 0, "MUSIC\\FIREBNL", 22050, 0, 1, 0xFF, 0x2},
    {"fire bonus r", 1882, 0x1BFF, 0, 0, 0, 0, "MUSIC\\FIREBNR", 22050, 0, 1, 0xFF, 0x3},
    {"invincibility l", 1882, 0x1BFF, 0, 0, 0, 0, "MUSIC\\INVINCIL", 22050, 1, 1, 0xFF, 0x4},
    {"invincibility r", 1882, 0x1BFF, 0, 0, 0, 0, "MUSIC\\INVINCIR", 22050, 1, 1, 0xFF, 0x5},
    {"jungle bonus l", 1882, 0x1BFF, 0, 0, 0, 0, "MUSIC\\JUNGLBNL", 22050, 0, 1, 0xFF, 0x6},
    {"jungle bonus r", 1882, 0x1BFF, 0, 0, 0, 0, "MUSIC\\JUNGLBNR", 22050, 0, 1, 0xFF, 0x7},
    {"base bonus l", 1882, 0x1BFF, 0, 0, 0, 0, "MUSIC\\RESEABNL", 22050, 0, 1, 0xFF, 0x8},
    {"base bonus r", 1882, 0x1BFF, 0, 0, 0, 0, "MUSIC\\RESEABNR", 22050, 0, 1, 0xFF, 0x9},
    {"space bonus l", 1882, 0x1BFF, 0, 0, 0, 0, "MUSIC\\SPACEBNL", 22050, 0, 1, 0xFF, 0xA},
    {"space bonus r", 1882, 0x1BFF, 0, 0, 0, 0, "MUSIC\\SPACEBNR", 22050, 0, 1, 0xFF, 0xB},
    {"theme l", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\THEMEL", -21436, 0, 1, 0xFF, 0xC},
    {"theme r", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\THEMER", -21436, 0, 1, 0xFF, 0xD},
    {"logos cut", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ENG\\LOGOS", 22050, 1, 2, 0xFF, 0xE},
    {"intro1 cut", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ENG\\INTRO1", 22050, 1, 2, 0xFF, 0xF},
    {"intro2 cut", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ENG\\INTRO2", 22050, 1, 2, 0xFF, 0x10},
    {"outro1 cut", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ENG\\OUTRO1", 22050, 1, 2, 0xFF, 0x11},
    {"outro2 cut", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ENG\\OUTRO2", 22050, 1, 2, 0xFF, 0x12},
    {"intro1 cut", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_FRE\\FRINTRO1", 22050, 1, 2, 0xFF, 0x13},
    {"intro2 cut", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_FRE\\FRINTRO2", 22050, 1, 2, 0xFF, 0x14},
    {"outro1 cut", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_FRE\\FROUTRO1", 22050, 1, 2, 0xFF, 0x15},
    {"outro2 cut", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_FRE\\FROUTRO2", 22050, 1, 2, 0xFF, 0x16},
    {"intro1 cut", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_GER\\DEINTRO1", 22050, 1, 2, 0xFF, 0x17},
    {"intro2 cut", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_GER\\DEINTRO2", 22050, 1, 2, 0xFF, 0x18},
    {"outro1 cut", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_GER\\DEOUTRO1", 22050, 1, 2, 0xFF, 0x19},
    {"outro2 cut", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_GER\\DEOUTRO2", 22050, 1, 2, 0xFF, 0x1A},
    {"intro1 cut", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ITA\\ITINTRO1", 22050, 1, 2, 0xFF, 0x1B},
    {"intro2 cut", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ITA\\ITINTRO2", 22050, 1, 2, 0xFF, 0x1C},
    {"outro1 cut", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ITA\\ITOUTRO1", 22050, 1, 2, 0xFF, 0x1D},
    {"outro2 cut", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ITA\\ITOUTRO2", 22050, 1, 2, 0xFF, 0x1E},
    {"intro1 cut", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_SPA\\ESINTRO1", 22050, 1, 2, 0xFF, 0x1F},
    {"intro2 cut", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_SPA\\ESINTRO2", 22050, 1, 2, 0xFF, 0x20},
    {"outro1 cut", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_SPA\\ESOUTRO1", 22050, 1, 2, 0xFF, 0x21},
    {"outro2 cut", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_SPA\\ESOUTRO2", 22050, 1, 2, 0xFF, 0x22},
    {"intro1 cut", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_DUT\\NLINTRO1", 22050, 1, 2, 0xFF, 0x23},
    {"intro2 cut", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_DUT\\NLINTRO2", 22050, 1, 2, 0xFF, 0x24},
    {"outro1 cut", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_DUT\\NLOUTRO1", 22050, 1, 2, 0xFF, 0x25},
    {"outro2 cut", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_DUT\\NLOUTRO2", 22050, 1, 2, 0xFF, 0x26},
};
static GameSound SfxTabESKIMO[7] = {
    {"music left", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\ESKIMORL", -21436, 0, 1, 0xFF, 0xC5},
    {"music right", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\ESKIMORR", -21436, 0, 1, 0xFF, 0xC6},
    {"ColdWindLoop", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\CWND_LP", 11025, 1, 0, 0xFF, 0x10C5},
    {"IceBlockFall1", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\IBLK_FL1", 11025, 1, 0, 0xFF, 0x10C6},
    {"IceBlockFall2", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\IBLK_FL2", 11025, 1, 0, 0xFF, 0x10C7},
    {"IcyWind", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\IHL_WIND", 11025, 1, 0, 0xFF, 0x10C8},
    {"HitBoostPad", 941, 0x3FFF, 10, 255, 0, 0, "LEVSPEC\\DB_TRB", 11025, 1, 0, 0xFF, 0x10C9},
};
static GameSound SfxTabBURN[13] = {
    {"music left", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\CANDBURL", -21436, 0, 1, 0xFF, 0xC5},
    {"music right", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\CANDBURR", -21436, 0, 1, 0xFF, 0xC6},
    {"Burn_RadarDish", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\TRANMISS", 11025, 1, 0, 0xFF, 0x10C5},
    {"Burn_Waterlap", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\WAT_LAP2", 11025, 1, 0, 0xFF, 0x10C6},
    {"FireBall1", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\FB_WHSH1", 11025, 1, 0, 0xFF, 0x10C7},
    {"FireBall2", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\FB_WHSH2", 11025, 1, 0, 0xFF, 0x10C8},
    {"FireBall3", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\FB_WHSH3", 11025, 1, 0, 0xFF, 0x10C9},
    {"FireBall4", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\FB_WHSH4", 11025, 1, 0, 0xFF, 0x10CA},
    {"LavaBubbleFizz", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\FF_LAV", 11025, 1, 0, 0xFF, 0x10CB},
    {"FireJet", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\FR_JETS", 11025, 1, 0, 0xFF, 0x10CC},
    {"MisileHitIlnd1", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\HIT_ISL1", 11025, 1, 0, 0xFF, 0x10CD},
    {"MisileHitIlnd2", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\HIT_ISL2", 11025, 1, 0, 0xFF, 0x10CE},
    {"MisileHitIlnd3", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\HIT_ISL3", 11025, 1, 0, 0xFF, 0x10CF},
};
static GameSound SfxTabOHNO[7] = {
    {"music left", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\H2OHNOL", -21436, 0, 1, 0xFF, 0xC5},
    {"music right", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\H2OHNOR", -21436, 0, 1, 0xFF, 0xC6},
    {"ElctrfidWtrlop", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\EW_LP", 11025, 1, 0, 0xFF, 0x10C5},
    {"TubeAmbentLoop", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\UW_TUBE", 11025, 1, 0, 0xFF, 0x10C6},
    {"BigFishSwim", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\BF_SWM", 11025, 1, 0, 0xFF, 0x10C7},
    {"SmallFishSwim", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\SF_SWM", 11025, 1, 0, 0xFF, 0x10C8},
    {"MinePing", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\MINEFALL", 11025, 1, 0, 0xFF, 0x10C9},
};
static GameSound SfxTabSMOKEY[4] = {
    {"music left", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\SMOKEYL", -21436, 0, 1, 0xFF, 0xC5},
    {"music right", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\SMOKEYR", -21436, 0, 1, 0xFF, 0xC6},
    {"BuggyBump", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\DB_BMP", 11025, 1, 0, 0xFF, 0x10C5},
    {"HitBoostPad", 941, 0x3FFF, 10, 255, 0, 0, "LEVSPEC\\DB_TRB", 11025, 1, 0, 0xFF, 0x10C6},
};
static GameSound SfxTabREACTOR[5] = {
    {"music left", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\COMPACTL", -21436, 0, 1, 0xFF, 0xC5},
    {"music right", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\COMPACTR", -21436, 0, 1, 0xFF, 0xC6},
    {"Reactor_AmbLop", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\CVRN_BG", 11025, 1, 0, 0xFF, 0x10C5},
    {"CavernRiver", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\C_RIVER", 11025, 1, 0, 0xFF, 0x10C6},
    {"ConveyorBelt", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\CNV_BLT", 11025, 1, 0, 0xFF, 0x10C7},
};
static GameSound SfxTabAVALANCHE[9] = {
    {"music left", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\AVALANCL", -21436, 0, 1, 0xFF, 0xC5},
    {"music right", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\AVALANCR", -21436, 0, 1, 0xFF, 0xC6},
    {"AvalancheLoop", 941, 0x7FFE, 0, 0, 0, 0, "LEVSPEC\\AVL_LP", 11025, 1, 0, 0xFF, 0x10C5},
    {"FlagBreak", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\FLG_BRK", 11025, 1, 0, 0xFF, 0x10C6},
    {"ColdWindLoop", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\CWND_LP", 11025, 1, 0, 0xFF, 0x10C7},
    {"CableCar", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\GOND_BY", 11025, 1, 0, 0xFF, 0x10C8},
    {"IceBlockFall1", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\IBLK_FL1", 11025, 1, 0, 0xFF, 0x10C9},
    {"IceBlockFall2", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\IBLK_FL2", 11025, 1, 0, 0xFF, 0x10CA},
    {"IcyWind", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\IHL_WIND", 11025, 1, 0, 0xFF, 0x10CB},
};
static GameSound SfxTabCORAL[6] = {
    {"music left", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\CORALCAL", -21436, 0, 1, 0xFF, 0xC5},
    {"music right", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\CORALCAR", -21436, 0, 1, 0xFF, 0xC6},
    {"TentacleInOut", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\TENT_SW", 11025, 1, 0, 0xFF, 0x10C5},
    {"BigFishSwim", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\BF_SWM", 11025, 1, 0, 0xFF, 0x10C6},
    {"SmallFIshSwim", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\SF_SWM", 11025, 1, 0, 0xFF, 0x10C7},
    {"MinePing", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\MINEFALL", 11025, 1, 0, 0xFF, 0x10C8},
};
static GameSound SfxTabARCTIC[7] = {
    {"music left", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\ARCTICAL", -21436, 0, 1, 0xFF, 0xC5},
    {"music right", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\ARCTICAR", -21436, 0, 1, 0xFF, 0xC6},
    {"ColdWindLoop", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\CWND_LP", 11025, 1, 0, 0xFF, 0x10C5},
    {"IceBlockFall1", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\IBLK_FL1", 11025, 1, 0, 0xFF, 0x10C6},
    {"IceBlockFall2", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\IBLK_FL2", 11025, 1, 0, 0xFF, 0x10C7},
    {"IcyWind", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\IHL_WIND", 11025, 1, 0, 0xFF, 0x10C8},
    {"WaterLapping", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\WAT_LAPS", 11025, 1, 0, 0xFF, 0x10C9},
};
static GameSound SfxTabJUNGLE[7] = {
    {"music left", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\JURUMBLL", -21436, 0, 1, 0xFF, 0xC5},
    {"music right", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\JURUMBLR", -21436, 0, 1, 0xFF, 0xC6},
    {"BirdsTweeting", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\BIRDS1", 11025, 1, 0, 0xFF, 0x10C5},
    {"FlyBuzz", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\DKFL_BUZ", 11025, 1, 0, 0xFF, 0x10C6},
    {"ThunderRoll", 941, 0x3FFF, 0, 0, 150, 0, "LEVSPEC\\THR_ROLL", 11025, 1, 0, 0xFF, 0x10C7},
    {"AvalancheLoop", 941, 0x7FFE, 0, 0, 0, 0, "LEVSPEC\\AVL_LP", 11025, 1, 0, 0xFF, 0x10C8},
    {"AmbientLoop", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\WTR_BG", 11025, 1, 0, 0xFF, 0x10C9},
};
static GameSound SfxTabNATURE[7] = {
    {"music left", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\FORCEL", -21436, 0, 1, 0xFF, 0xC5},
    {"music right", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\FORCER", -21436, 0, 1, 0xFF, 0xC6},
    {"ColdWindLoop", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\CWND_LP", 11025, 1, 0, 0xFF, 0x10C5},
    {"CableCar", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\GOND_BY", 11025, 1, 0, 0xFF, 0x10C6},
    {"IceBlockFall1", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\IBLK_FL1", 11025, 1, 0, 0xFF, 0x10C7},
    {"IceBlockFall2", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\IBLK_FL2", 11025, 1, 0, 0xFF, 0x10C8},
    {"IceyWind", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\IHL_WIND", 11025, 1, 0, 0xFF, 0x10C9},
};
static GameSound SfxTabGAUNTLET[4] = {
    {"music left", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\GAUNTLEL", -21436, 0, 1, 0xFF, 0xC5},
    {"music right", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\GAUNTLER", -21436, 0, 1, 0xFF, 0xC6},
    {"ChndalierCreak", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\CHN_CRK", 11025, 1, 0, 0xFF, 0x10C5},
    {"FirePlace", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\FIRP_STD", 11025, 1, 0, 0xFF, 0x10C6},
};
static GameSound SfxTabBAMBOO[5] = {
    {"music left", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\ROKNROLL", -21436, 0, 1, 0xFF, 0xC5},
    {"music right", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\ROKNROLR", -21436, 0, 1, 0xFF, 0xC6},
    {"ElevatorDown", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\ELEV_DO", 11025, 1, 0, 0xFF, 0x10C5},
    {"ElevatorUp", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\ELEV_UP", 11025, 1, 0, 0xFF, 0x10C6},
    {"FLyLamp", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\FLY_LAMP", 11025, 1, 0, 0xFF, 0x10C7},
};
static GameSound SfxTabBANZAI[9] = {
    {"music left", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\BANZAIBL", -21436, 0, 1, 0xFF, 0xC5},
    {"music right", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\BANZAIBR", -21436, 0, 1, 0xFF, 0xC6},
    {"AmbientLoop", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\JGARD_BG", 11025, 1, 0, 0xFF, 0x10C5},
    {"CraneSnap", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\FL_SNAP", 11025, 1, 0, 0xFF, 0x10C6},
    {"PnkPltformFall", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\PINKDOWN", 11025, 1, 0, 0xFF, 0x10C7},
    {"TorchFlameLoop", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\TORCHFLA", 11025, 1, 0, 0xFF, 0x10C8},
    {"GongBong", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\HIT_GONG", 11025, 1, 0, 0xFF, 0x10C9},
    {"FlyLamp", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\FLY_LAMP", 11025, 1, 0, 0xFF, 0x10CA},
    {"WaterLapping", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\WAT_LAPS", 11025, 1, 0, 0xFF, 0x10CB},
};
static GameSound SfxTabTORNADO[8] = {
    {"music left", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\TORNADOL", -21436, 0, 1, 0xFF, 0xC5},
    {"music right", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\TORNADOR", -21436, 0, 1, 0xFF, 0xC6},
    {"coco", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ENG\\US_COC25", 22050, 1, 2, 0xFF, 0xC7},
    {"coco", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_FRE\\FRCOC25", 22050, 1, 2, 0xFF, 0xC8},
    {"coco", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_GER\\DECOC25", 22050, 1, 2, 0xFF, 0xC9},
    {"coco", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ITA\\ITCOC25", 22050, 1, 2, 0xFF, 0xCA},
    {"coco", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_SPA\\ESCOC25", 22050, 1, 2, 0xFF, 0xCB},
    {"coco", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_DUT\\NLCOC25", 22050, 1, 2, 0xFF, 0xCC},
};
static GameSound SfxTabTSUNAMI[16] = {
    {"music left", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\TSUNAMIL", -21436, 0, 1, 0xFF, 0xC5},
    {"music right", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\TSUNAMIR", -21436, 0, 1, 0xFF, 0xC6},
    {"BoatCreak", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\BAOT_CRE", 11025, 1, 0, 0xFF, 0x10C5},
    {"WheelaHorn", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\CART_HRN", 11025, 1, 0, 0xFF, 0x10C6},
    {"WheelaLoop", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\CART_STD", 11025, 1, 0, 0xFF, 0x10C7},
    {"Lightning", 941, 0x3FFF, 0, 0, 150, 0, "LEVSPEC\\LT3_STRK", 11025, 1, 0, 0xFF, 0x10C8},
    {"RickShawLoop", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\RKSH_STD", 11025, 1, 0, 0xFF, 0x10C9},
    {"ScooterJump", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\RZR_JMP", 11025, 1, 0, 0xFF, 0x10CA},
    {"ScooterLand", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\RZR_LND", 11025, 1, 0, 0xFF, 0x10CB},
    {"ScooterSpin", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\RZR_SPN", 11025, 1, 0, 0xFF, 0x10CC},
    {"ScooterNormal", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\RZR_STD", 11025, 1, 0, 0xFF, 0x10CD},
    {"ScooterOnWood", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\RZR_WD", 11025, 1, 0, 0xFF, 0x10CE},
    {"WaveLoop", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\WAV_STD", 11025, 1, 0, 0xFF, 0x10CF},
    {"WaveStart", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\WAV_STRT", 11025, 1, 0, 0xFF, 0x10D0},
    {"ThunderRoll", 941, 0x7FFE, 0, 0, 150, 0, "LEVSPEC\\THR_ROLL", 11025, 1, 0, 0xFF, 0x10D1},
    {"AmbientLoop", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\WTR_BG", 11025, 1, 0, 0xFF, 0x10D2},
};
static GameSound SfxTabFRENZY[10] = {
    {"music left", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\FARENHEL", -21436, 0, 1, 0xFF, 0xC5},
    {"music right", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\FARENHER", -21436, 0, 1, 0xFF, 0xC6},
    {"XrayLoop", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\XRY_AMB", 11025, 1, 0, 0xFF, 0x10C5},
    {"ConveyorBelt", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\CNV_BLT", 11025, 1, 0, 0xFF, 0x10C6},
    {"FireBall1", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\FB_WHSH1", 11025, 1, 0, 0xFF, 0x10C7},
    {"FireBall2", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\FB_WHSH2", 11025, 1, 0, 0xFF, 0x10C8},
    {"VireBall3", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\FB_WHSH3", 11025, 1, 0, 0xFF, 0x10C9},
    {"FireBall4", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\FB_WHSH4", 11025, 1, 0, 0xFF, 0x10CA},
    {"LavaBubbleFizz", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\FF_LAV", 11025, 1, 0, 0xFF, 0x10CB},
    {"FireJet", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\FR_JETS", 11025, 1, 0, 0xFF, 0x10CC},
};
static GameSound SfxTabMADNESS[3] = {
    {"music left", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\GAUNTL2L", -21436, 0, 1, 0xFF, 0xC5},
    {"music right", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\GAUNTL2R", -21436, 0, 1, 0xFF, 0xC6},
    {"BirdsTweeting", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\BIRDS1", 11025, 1, 0, 0xFF, 0x10C5},
};
static GameSound SfxTabLIZARDS[11] = {
    {"music left", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\WIZARDSL", -21436, 0, 1, 0xFF, 0xC5},
    {"music right", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\WIZARDSR", -21436, 0, 1, 0xFF, 0xC6},
    {"ArmourBreak", 941, 0x3FFF, 10, 255, 0, 0, "LEVSPEC\\ARMCRSH", 11025, 1, 0, 0xFF, 0x10C5},
    {"BarrelBreak", 941, 0x3FFF, 10, 255, 0, 0, "LEVSPEC\\BARBRK2", 11025, 1, 0, 0xFF, 0x10C6},
    {"BridgeBreak", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\BRCRSH", 11025, 1, 0, 0xFF, 0x10C7},
    {"BridgeWoodFall", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\BRWFALL", 11025, 1, 0, 0xFF, 0x10C8},
    {"DragonDie", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\DRDIE", 11025, 1, 0, 0xFF, 0x10C9},
    {"DragonFootStep", 941, 0xBFFD, 2, 127, 0, 0, "LEVSPEC\\DRFS", 11025, 1, 0, 0xFF, 0x10CA},
    {"DragonGrowl", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\DRGRWL2", 11025, 1, 0, 0xFF, 0x10CB},
    {"ChndalierCreak", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\CHN_CRK", 11025, 1, 0, 0xFF, 0x10CC},
    {"FirePlace", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\FIRP_STD", 11025, 1, 0, 0xFF, 0x10CD},
};
static GameSound SfxTabSINKING[21] = {
    {"music left", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\SINKINGL", -21436, 0, 1, 0xFF, 0xC5},
    {"music right", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\SINKINGR", -21436, 0, 1, 0xFF, 0xC6},
    {"BombHitShip", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\BMB_HIT", 11025, 1, 0, 0xFF, 0x10C5},
    {"BombRleaseScrm", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\BMB_SCRM", 11025, 1, 0, 0xFF, 0x10C6},
    {"BombSpladoosh", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\BMB_SPL", 11025, 1, 0, 0xFF, 0x10C7},
    {"MsqitoEngneLop", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\DFLY_STD", 11025, 1, 0, 0xFF, 0x10C8},
    {"GunBoatFire", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\ENMS_GN", 11025, 1, 0, 0xFF, 0x10C9},
    {"ShipExplode", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\ENSH_EXP", 11025, 1, 0, 0xFF, 0x10CA},
    {"MissileDrop", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\MS_DRP", 11025, 1, 0, 0xFF, 0x10CB},
    {"MsqitoBombRlse", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\RLS_BOMB", 11025, 1, 0, 0xFF, 0x10CC},
    {"ShipFire1", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\SHP_GN1", 11025, 1, 0, 0xFF, 0x10CD},
    {"ShipFire2", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\SHP_GN2", 11025, 1, 0, 0xFF, 0x10CE},
    {"ShipFire3", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\SHP_GN3", 11025, 1, 0, 0xFF, 0x10CF},
    {"TargetEngine", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\TGT_ENG", 11025, 1, 0, 0xFF, 0x10D0},
    {"FireBall1", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\FB_WHSH1", 11025, 1, 0, 0xFF, 0x10D1},
    {"FireBall2", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\FB_WHSH2", 11025, 1, 0, 0xFF, 0x10D2},
    {"FireBall3", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\FB_WHSH3", 11025, 1, 0, 0xFF, 0x10D3},
    {"FireBall4", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\FB_WHSH4", 11025, 1, 0, 0xFF, 0x10D4},
    {"MisileHitIlnd1", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\HIT_ISL1", 11025, 1, 0, 0xFF, 0x10D5},
    {"MisileHitIlnd2", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\HIT_ISL2", 11025, 1, 0, 0xFF, 0x10D6},
    {"MisileHitIlnd3", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\HIT_ISL3", 11025, 1, 0, 0xFF, 0x10D7},
};
static GameSound SfxTabGOLD[13] = {
    {"music left", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\GOLDRUSL", -21436, 0, 1, 0xFF, 0xC5},
    {"music right", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\GOLDRUSR", -21436, 0, 1, 0xFF, 0xC6},
    {"VultureFlap1", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\BUZ_FLP1", 11025, 1, 0, 0xFF, 0x10C5},
    {"VultureFlap2", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\BUZ_FLP2", 11025, 1, 0, 0xFF, 0x10C6},
    {"VultureSquawk", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\BUZ_SQWK", 11025, 1, 0, 0xFF, 0x10C7},
    {"MineCrtSqueak1", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\HC_1SQK", 11025, 1, 0, 0xFF, 0x10C8},
    {"MineCrtSqueak2", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\HC_2SQK", 11025, 1, 0, 0xFF, 0x10C9},
    {"MineCrtSqueak3", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\HC_3SQK", 11025, 1, 0, 0xFF, 0x10CA},
    {"MineCrtSqueak4", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\HC_4SQK", 11025, 1, 0, 0xFF, 0x10CB},
    {"MineCartLoop", 941, 0xBFFD, 0, 0, 0, 0, "LEVSPEC\\HC_ROLL", 11025, 1, 0, 0xFF, 0x10CC},
    {"CavernRiver", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\C_RIVER", 11025, 1, 0, 0xFF, 0x10CD},
    {"DesertWindGust", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\DES_WND1", 11025, 1, 0, 0xFF, 0x10CE},
    {"FlyBuzz", 941, 0x7FFE, 0, 0, 0, 0, "LEVSPEC\\DKFL_BUZ", 11025, 1, 0, 0xFF, 0x10CF},
};
static GameSound SfxTabVOID[5] = {
    {"music left", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\DROIDVOL", -21436, 0, 1, 0xFF, 0xC5},
    {"music right", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\DROIDVOR", -21436, 0, 1, 0xFF, 0xC6},
    {"AmbientLoop", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\DRVD_BG", 11025, 1, 0, 0xFF, 0x10C5},
    {"TltPltfrmReset", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\GPL_RST", 11025, 1, 0, 0xFF, 0x10C6},
    {"TltPltfrmTilt", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\GPL_TLT", 11025, 1, 0, 0xFF, 0x10C7},
};
static GameSound SfxTabROKS[17] = {
    {"music left", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\ROKRUMBL", -21436, 0, 1, 0xFF, 0xC5},
    {"music right", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\ROKRUMBR", -21436, 0, 1, 0xFF, 0xC6},
    {"ActivateRock", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\CR_ARK", 11025, 1, 0, 0xFF, 0x10C5},
    {"NeutraliseRock", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\CR_NTR", 11025, 1, 0, 0xFF, 0x10C6},
    {"CrashBallLand1", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\CRB_1LD", 11025, 1, 0, 0xFF, 0x10C7},
    {"CrashBallLand2", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\CRB_2LD", 11025, 1, 0, 0xFF, 0x10C8},
    {"CrshBallTkeHit", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\CRB_HT", 11025, 1, 0, 0xFF, 0x10C9},
    {"CrunchBallRoll", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\CRN_RKR", 11025, 1, 0, 0xFF, 0x10CA},
    {"RocksAtkCrnchA", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\RCK_ATK1", 11025, 1, 0, 0xFF, 0x10CB},
    {"RocksAtkCrnchB", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\RCK_ATK", 11025, 1, 0, 0xFF, 0x10CC},
    {"RocksSpnAndHit", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\RCK_SH", 11025, 1, 0, 0xFF, 0x10CD},
    {"AmbientLoop", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\RR_BG", 11025, 1, 0, 0xFF, 0x10CE},
    {"SmalRockTumble", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\SRK_TMB", 11025, 1, 0, 0xFF, 0x10CF},
    {"TnyActivateRck", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\TIN_ARK", 11025, 1, 0, 0xFF, 0x10D0},
    {"TnyNeutrlseRck", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\TIN_NTR", 11025, 1, 0, 0xFF, 0x10D1},
    {"TnyRfrmIntoRck", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\TIN_RFR", 11025, 1, 0, 0xFF, 0x10D2},
    {"TinyWarpDeath", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\TNY_WRP", 11025, 1, 0, 0xFF, 0x10D3},
};
static GameSound SfxTabASHES[18] = {
    {"music left", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\CRAASHEL", -21436, 0, 1, 0xFF, 0xC5},
    {"music right", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\CRAASHER", -21436, 0, 1, 0xFF, 0xC6},
    {"ThrwBigBoulder", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\FIR_AT1A", 11025, 1, 0, 0xFF, 0x10C5},
    {"BigBoulderLoop", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\FIR_AT1B", 11025, 1, 0, 0xFF, 0x10C6},
    {"ThrwSmlBoulder", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\FIR_AT2A", 11025, 1, 0, 0xFF, 0x10C7},
    {"SmlBoulderLoop", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\FIR_AT2B", 11025, 1, 0, 0xFF, 0x10C8},
    {"WaterAttckLong", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\FIR_ATM1", 11025, 1, 0, 0xFF, 0x10C9},
    {"WaterAtckShort", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\FIR_ATM2", 11025, 1, 0, 0xFF, 0x10CA},
    {"AmbientLoop", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\FIR_BG", 11025, 1, 0, 0xFF, 0x10CB},
    {"CrunchFootStep", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\FIR_CFS1", 11025, 1, 0, 0xFF, 0x10CC},
    {"CrunchBrkApart", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\FIR_DIE", 11025, 1, 0, 0xFF, 0x10CD},
    {"MechBreakApart", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\FIR_MECB", 11025, 1, 0, 0xFF, 0x10CE},
    {"MechBrdgeFtStp", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\FIR_MFB1", 11025, 1, 0, 0xFF, 0x10CF},
    {"MechDirtFtStep", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\FIR_MFS1", 11025, 1, 0, 0xFF, 0x10D0},
    {"TorchLoop", 1882, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\TORCLOOP", 22050, 1, 0, 0xFF, 0x10D1},
    {"Scream", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\CRN04F", 11025, 1, 0, 0xFF, 0x10D2},
    {"Angry", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\CRN05D", 11025, 1, 0, 0xFF, 0x10D3},
    {"Breath", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\CRN05A", 11025, 1, 0, 0xFF, 0x10D4},
};
static GameSound SfxTabDRAIN[11] = {
    {"music left", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\DRAINDAL", -21436, 0, 1, 0xFF, 0xC5},
    {"music right", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\DRAINDAR", -21436, 0, 1, 0xFF, 0xC6},
    {"WaterLapping", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\WAT_LAPS", 11025, 1, 0, 0xFF, 0x10C5},
    {"LowAttack", 941, 0x7FFE, 5, 127, 0, 0, "LEVSPEC\\WTR_AT1", 11025, 1, 0, 0xFF, 0x10C5},
    {"AttackHit", 941, 0x7FFE, 0, 0, 0, 0, "LEVSPEC\\WTR_AT1H", 11025, 1, 0, 0xFF, 0x10C6},
    {"HighAttack", 941, 0x7FFE, 5, 127, 0, 0, "LEVSPEC\\WTR_AT2", 11025, 1, 0, 0xFF, 0x10C7},
    {"PunchAttack", 941, 0x7FFE, 10, 255, 0, 0, "LEVSPEC\\WTR_AT3", 11025, 1, 0, 0xFF, 0x10C8},
    {"FallInWater", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\WTR_BFC", 11025, 1, 0, 0xFF, 0x10C9},
    {"AmbientLoop", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\WTR_BG", 11025, 1, 0, 0xFF, 0x10CA},
    {"CrunchTakeHit", 941, 0x3FFF, 10, 255, 0, 0, "LEVSPEC\\WTR_CHIT", 11025, 1, 0, 0xFF, 0x10CB},
    {"Lightning", 941, 0x3FFF, 0, 0, 150, 0, "LEVSPEC\\LT3_STRK", 11025, 1, 0, 0xFF, 0x10CC},
};
static GameSound SfxTabPRESSURE[8] = {
    {"music left", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\ATMOSPHL", -21436, 0, 1, 0xFF, 0xC5},
    {"music right", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\ATMOSPHR", -21436, 0, 1, 0xFF, 0xC6},
    {"BallAttack", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\AIR_AT1", 11025, 1, 0, 0xFF, 0x10C5},
    {"FallingAttack", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\AIR_AT4", 11025, 1, 0, 0xFF, 0x10C6},
    {"AmbientLoop", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\AIR_BG", 11025, 1, 0, 0xFF, 0x10C7},
    {"CrunchDie", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\AIR_DIE", 11025, 1, 0, 0xFF, 0x10C8},
    {"CrnchElctrclNs", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\AIR_ELEC", 11025, 1, 0, 0xFF, 0x10C9},
    {"GlderHitCrunch", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\AIR_HIT1", 11025, 1, 0, 0xFF, 0x10CA},
};
static GameSound SfxTabCRUNCH[7] = {
    {"music left", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\CRTIMEL", -21436, 0, 1, 0xFF, 0xC5},
    {"music right", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\CRTIMER", -21436, 0, 1, 0xFF, 0xC6},
    {"BackGrndNoise", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\HUB_BG", 11025, 1, 0, 0xFF, 0x10C5},
    {"EarthAmbient", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\CVRN_BG", 11025, 1, 0, 0xFF, 0x10C6},
    {"WaterAmbient", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\SNOW_AMB", 11025, 1, 0, 0xFF, 0x10C7},
    {"FireAmbient", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\FIRE_AMB", 11025, 1, 0, 0xFF, 0x10C8},
    {"WeatherAmbient", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\BUB_AMB", 11025, 1, 0, 0xFF, 0x10C9},
};
static GameSound SfxTabOIDS[22] = {
    {"music left", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\CRASHTEL", -21436, 0, 1, 0xFF, 0xC5},
    {"music right", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\CRASHTER", -21436, 0, 1, 0xFF, 0xC6},
    {"aku aku", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ENG\\AKU_INTR", 22050, 1, 2, 0xFF, 0xC7},
    {"aku aku", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_FRE\\FRAKUINT", 22050, 1, 2, 0xFF, 0xC8},
    {"aku aku", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_GER\\DEAKUINT", 22050, 1, 2, 0xFF, 0xC9},
    {"aku aku", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ITA\\ITAKUINT", 22050, 1, 2, 0xFF, 0xCA},
    {"aku aku", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_SPA\\ESAKUINT", 22050, 1, 2, 0xFF, 0xCB},
    {"aku aku", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_DUT\\NLAKUINT", 22050, 1, 2, 0xFF, 0xCC},
    {"AstroidExplsn", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\AST_EXP2", 11025, 1, 0, 0xFF, 0x10C5},
    {"ShipEngine", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\COCO_SHP", 11025, 1, 0, 0xFF, 0x10C6},
    {"ShipFire", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\COCO_SHT", 11025, 1, 0, 0xFF, 0x10C7},
    {"ShipDeath", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\COSH_DTH", 11025, 1, 0, 0xFF, 0x10C8},
    {"EnemyEngine", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\EN_SHP", 11025, 1, 0, 0xFF, 0x10C9},
    {"EnemyFire", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\EN2_SHT", 11025, 1, 0, 0xFF, 0x10CA},
    {"HitEnemy", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\LSHT_EN", 11025, 1, 0, 0xFF, 0x10CB},
    {"HitSatellite", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\LZR_SAT", 11025, 1, 0, 0xFF, 0x10CC},
    {"HitSpaceStatn", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\LZR_SPST", 11025, 1, 0, 0xFF, 0x10CD},
    {"SatelliteLoop", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\SAT_TON", 11025, 1, 0, 0xFF, 0x10CE},
    {"HitShield", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\SHLD_HT", 11025, 1, 0, 0xFF, 0x10CF},
    {"SpceStatnExpld", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\SPST_EXP", 11025, 1, 0, 0xFF, 0x10D0},
    {"Coco Hit", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\USCOC20C", 11025, 1, 0, 0xFF, 0x10D1},
    {"SpaceAmbeint", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\SPC_AMB", 11025, 1, 0, 0xFF, 0x10D2},
};
static GameSound SfxTabKNIGHT[3] = {
    {"music left", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\KNIGHTTL", -21436, 0, 1, 0xFF, 0xC5},
    {"music right", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\KNIGHTTR", -21436, 0, 1, 0xFF, 0xC6},
    {"BugLoop", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\CC_FBUZ", 11025, 1, 0, 0xFF, 0x10C5},
};
static GameSound SfxTabTOWN[3] = {
    {"music left", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\GHOSTTOL", -21436, 0, 1, 0xFF, 0xC5},
    {"music right", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\GHOSTTOR", -21436, 0, 1, 0xFF, 0xC6},
    {"DesertWindGust", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\DES_WND1", 11025, 1, 0, 0xFF, 0x10C5},
};
static GameSound SfxTabICE[6] = {
    {"music left", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\ICESTATL", -21436, 0, 1, 0xFF, 0xC5},
    {"music right", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\ICESTATR", -21436, 0, 1, 0xFF, 0xC6},
    {"ColdWindLoop", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\CWND_LP", 11025, 1, 0, 0xFF, 0x10C5},
    {"IceBlockFall1", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\IBLK_FL1", 11025, 1, 0, 0xFF, 0x10C6},
    {"IceBlockFall2", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\IBLK_FL2", 11025, 1, 0, 0xFF, 0x10C7},
    {"IcyWind", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\IHL_WIND", 11025, 1, 0, 0xFF, 0x10C8},
};
static GameSound SfxTabHEIGHTS[2] = {
    {"music left", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\WEATHEIL", -21436, 0, 1, 0xFF, 0xC5},
    {"music right", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\WEATHEIR", -21436, 0, 1, 0xFF, 0xC6},
};
static GameSound SfxTabBALLS[6] = {
    {"music left", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\CRATEBAL", -21436, 0, 1, 0xFF, 0xC5},
    {"music right", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\CRATEBAR", -21436, 0, 1, 0xFF, 0xC6},
    {"DoorOpen", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\SDR_OP", 11025, 1, 0, 0xFF, 0x10C5},
    {"TltPltfrmReset", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\GPL_RST", 11025, 1, 0, 0xFF, 0x10C6},
    {"TiltPltfrmTilt", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\GPL_TLT", 11025, 1, 0, 0xFF, 0x10C7},
    {"AvalancheLoop", 941, 0x7FFE, 0, 0, 0, 0, "LEVSPEC\\AVL_LP", 11025, 1, 0, 0xFF, 0x10C8},
};
static GameSound SfxTabBOWLER[3] = {
    {"music left", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\SOLARBOL", -21436, 0, 1, 0xFF, 0xC5},
    {"music right", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\SOLARBOR", -21436, 0, 1, 0xFF, 0xC6},
    {"SpaceAmbient", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\SPC_AMB", 11025, 1, 0, 0xFF, 0x10C5},
};
static GameSound SfxTabVORTEX[3] = {
    {"music left", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\CORTEXVL", -21436, 0, 1, 0xFF, 0xC5},
    {"music right", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\CORTEXVR", -21436, 0, 1, 0xFF, 0xC6},
    {"AmbientLoop", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\CVOR_BG", 11025, 1, 0, 0xFF, 0x10C5},
};
static GameSound SfxTabSEA[5] = {
    {"music left", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\SEASHELL", -21436, 0, 1, 0xFF, 0xC5},
    {"music right", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\SEASHELR", -21436, 0, 1, 0xFF, 0xC6},
    {"BigFishSwim", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\BF_SWM", 11025, 1, 0, 0xFF, 0x10C5},
    {"SmallFishSwim", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\SF_SWM", 11025, 1, 0, 0xFF, 0x10C6},
    {"MinePing", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\MINEFALL", 11025, 1, 0, 0xFF, 0x10C7},
};
GameSound SfxTabFRONTEND[80] = {
    {"FrtEnd_USCOR45", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ENG\\USCOR45", 22050, 1, 2, 0xFF, 0xC5},
    {"FrtEnd_USCOR46", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ENG\\USCOR46", 22050, 1, 2, 0xFF, 0xC6},
    {"FrtEnd_USCOR47", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ENG\\USCOR47", 22050, 1, 2, 0xFF, 0xC7},
    {"FrtEnd_USCOR49", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ENG\\USCOR49", 22050, 1, 2, 0xFF, 0xC8},
    {"FrtEnd_USCOR50", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ENG\\USCOR50", 22050, 1, 2, 0xFF, 0xC9},
    {"FrtEnd_USCOR51", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ENG\\USCOR51", 22050, 1, 2, 0xFF, 0xCA},
    {"FrtEnd_USCRN01", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ENG\\USCRN01", 22050, 1, 2, 0xFF, 0xCB},
    {"FrtEnd_USCRN06", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ENG\\USCRN06", 22050, 1, 2, 0xFF, 0xCC},
    {"FrtEnd_USCRN08", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ENG\\USCRN08", 22050, 1, 2, 0xFF, 0xCD},
    {"FrtEnd_USCRN09", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ENG\\USCRN09", 22050, 1, 2, 0xFF, 0xCE},
    {"FrtEndUSUKA017", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ENG\\USUKA017", 22050, 1, 2, 0xFF, 0xCF},
    {"FrtEndUSUKA018", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ENG\\USUKA018", 22050, 1, 2, 0xFF, 0xD0},
    {"FrtEnd_USCOR45", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_FRE\\FRCOR45", 22050, 1, 2, 0xFF, 0xD1},
    {"FrtEnd_USCOR46", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_FRE\\FRCOR46", 22050, 1, 2, 0xFF, 0xD2},
    {"FrtEnd_USCOR47", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_FRE\\FRCOR47", 22050, 1, 2, 0xFF, 0xD3},
    {"FrtEnd_USCOR49", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_FRE\\FRCOR49", 22050, 1, 2, 0xFF, 0xD4},
    {"FrtEnd_USCOR50", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_FRE\\FRCOR50", 22050, 1, 2, 0xFF, 0xD5},
    {"FrtEnd_USCOR51", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_FRE\\FRCOR51", 22050, 1, 2, 0xFF, 0xD6},
    {"FrtEnd_USCRN01", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_FRE\\FRCRN01", 22050, 1, 2, 0xFF, 0xD7},
    {"FrtEnd_USCRN06", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_FRE\\FRCRN06", 22050, 1, 2, 0xFF, 0xD8},
    {"FrtEnd_USCRN08", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_FRE\\FRCRN08", 22050, 1, 2, 0xFF, 0xD9},
    {"FrtEnd_USCRN09", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_FRE\\FRCRN09", 22050, 1, 2, 0xFF, 0xDA},
    {"FrtEndUSUKA017", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_FRE\\FRUKA17", 22050, 1, 2, 0xFF, 0xDB},
    {"FrtEndUSUKA018", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_FRE\\FRUKA18", 22050, 1, 2, 0xFF, 0xDC},
    {"FrtEnd_USCOR45", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_GER\\DECOR45", 22050, 1, 2, 0xFF, 0xDD},
    {"FrtEnd_USCOR46", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_GER\\DECOR46", 22050, 1, 2, 0xFF, 0xDE},
    {"FrtEnd_USCOR47", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_GER\\DECOR47", 22050, 1, 2, 0xFF, 0xDF},
    {"FrtEnd_USCOR49", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_GER\\DECOR49", 22050, 1, 2, 0xFF, 0xE0},
    {"FrtEnd_USCOR50", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_GER\\DECOR50", 22050, 1, 2, 0xFF, 0xE1},
    {"FrtEnd_USCOR51", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_GER\\DECOR51", 22050, 1, 2, 0xFF, 0xE2},
    {"FrtEnd_USCRN01", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_GER\\DECRN01", 22050, 1, 2, 0xFF, 0xE3},
    {"FrtEnd_USCRN06", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_GER\\DECRN06", 22050, 1, 2, 0xFF, 0xE4},
    {"FrtEnd_USCRN08", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_GER\\DECRN08", 22050, 1, 2, 0xFF, 0xE5},
    {"FrtEnd_USCRN09", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_GER\\DECRN09", 22050, 1, 2, 0xFF, 0xE6},
    {"FrtEndUSUKA017", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_GER\\DEUKA017", 22050, 1, 2, 0xFF, 0xE7},
    {"FrtEndUSUKA018", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_GER\\DEUKA018", 22050, 1, 2, 0xFF, 0xE8},
    {"FrtEnd_USCOR45", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ITA\\ITCOR45", 22050, 1, 2, 0xFF, 0xE9},
    {"FrtEnd_USCOR46", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ITA\\ITCOR46", 22050, 1, 2, 0xFF, 0xEA},
    {"FrtEnd_USCOR47", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ITA\\ITCOR47", 22050, 1, 2, 0xFF, 0xEB},
    {"FrtEnd_USCOR49", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ITA\\ITCOR49", 22050, 1, 2, 0xFF, 0xEC},
    {"FrtEnd_USCOR50", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ITA\\ITCOR50", 22050, 1, 2, 0xFF, 0xED},
    {"FrtEnd_USCOR51", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ITA\\ITCOR51", 22050, 1, 2, 0xFF, 0xEE},
    {"FrtEnd_USCRN01", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ITA\\ITCRN01", 22050, 1, 2, 0xFF, 0xEF},
    {"FrtEnd_USCRN06", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ITA\\ITCRN06", 22050, 1, 2, 0xFF, 0xF0},
    {"FrtEnd_USCRN08", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ITA\\ITCRN08", 22050, 1, 2, 0xFF, 0xF1},
    {"FrtEnd_USCRN09", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ITA\\ITCRN09", 22050, 1, 2, 0xFF, 0xF2},
    {"FrtEndUSUKA017", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ITA\\ITUKA017", 22050, 1, 2, 0xFF, 0xF3},
    {"FrtEndUSUKA018", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ITA\\ITUKA018", 22050, 1, 2, 0xFF, 0xF4},
    {"FrtEnd_USCOR45", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_SPA\\ESCOR45", 22050, 1, 2, 0xFF, 0xF5},
    {"FrtEnd_USCOR46", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_SPA\\ESCOR46", 22050, 1, 2, 0xFF, 0xF6},
    {"FrtEnd_USCOR47", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_SPA\\ESCOR47", 22050, 1, 2, 0xFF, 0xF7},
    {"FrtEnd_USCOR49", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_SPA\\ESCOR49", 22050, 1, 2, 0xFF, 0xF8},
    {"FrtEnd_USCOR50", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_SPA\\ESCOR50", 22050, 1, 2, 0xFF, 0xF9},
    {"FrtEnd_USCOR51", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_SPA\\ESCOR51", 22050, 1, 2, 0xFF, 0xFA},
    {"FrtEnd_USCRN01", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_SPA\\ESCRN01", 22050, 1, 2, 0xFF, 0xFB},
    {"FrtEnd_USCRN06", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_SPA\\ESCRN06", 22050, 1, 2, 0xFF, 0xFC},
    {"FrtEnd_USCRN08", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_SPA\\ESCRN08", 22050, 1, 2, 0xFF, 0xFD},
    {"FrtEnd_USCRN09", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_SPA\\ESCRN09", 22050, 1, 2, 0xFF, 0xFE},
    {"FrtEndUSUKA017", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_SPA\\ESUKA17", 22050, 1, 2, 0xFF, 0xFF},
    {"FrtEndUSUKA018", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_SPA\\ESUKA18", 22050, 1, 2, 0xFF, 0x100},
    {"FrtEnd_USCOR45", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_DUT\\NLCOR45", 22050, 1, 2, 0xFF, 0x101},
    {"FrtEnd_USCOR46", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_DUT\\NLCOR46", 22050, 1, 2, 0xFF, 0x102},
    {"FrtEnd_USCOR47", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_DUT\\NLCOR47", 22050, 1, 2, 0xFF, 0x103},
    {"FrtEnd_USCOR49", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_DUT\\NLCOR49", 22050, 1, 2, 0xFF, 0x104},
    {"FrtEnd_USCOR50", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_DUT\\NLCOR50", 22050, 1, 2, 0xFF, 0x105},
    {"FrtEnd_USCOR51", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_DUT\\NLCOR51", 22050, 1, 2, 0xFF, 0x106},
    {"FrtEnd_USCRN01", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_DUT\\NLCRN01", 22050, 1, 2, 0xFF, 0x107},
    {"FrtEnd_USCRN06", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_DUT\\NLCRN06", 22050, 1, 2, 0xFF, 0x108},
    {"FrtEnd_USCRN08", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_DUT\\NLCRN08", 22050, 1, 2, 0xFF, 0x109},
    {"FrtEnd_USCRN09", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_DUT\\NLCRN09", 22050, 1, 2, 0xFF, 0x10A},
    {"FrtEndUSUKA017", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_DUT\\NLUKA017", 22050, 1, 2, 0xFF, 0x10B},
    {"FrtEndUSUKA018", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_DUT\\NLUKA018", 22050, 1, 2, 0xFF, 0x10C},
    {"WeatherAmbient", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\BUB_AMB", 11025, 1, 0, 0xFF, 0x10C5},
    {"FireAmbient", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\FIRE_AMB", 11025, 1, 0, 0xFF, 0x10C6},
    {"BackGrndNoise", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\HUB_BG", 11025, 1, 0, 0xFF, 0x10C7},
    {"VrScreenUp", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\LUP_BG", 11025, 1, 0, 0xFF, 0x10C8},
    {"BnsPltfrmWrpUp", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\PLT_WPDN", 11025, 1, 0, 0xFF, 0x10C9},
    {"SnowAmbient", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\SNOW_AMB", 11025, 1, 0, 0xFF, 0x10CA},
    {"", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\SPC_AMB", 11025, 1, 0, 0xFF, 0x10CB},
    {"Lightning", 941, 0x3FFF, 0, 0, 150, 0, "LEVSPEC\\LT3_STRK", 11025, 1, 0, 0xFF, 0x10CC},
};
static GameSound SfxTabGAMEOVER[9] = {
    {"face my wrath", 3763, 0x3FFF, 0, 0, 0, 0, "VCE_ENG\\COR09", -21436, 1, 2, 0xFF, 0xC5},
    {"giving up", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ENG\\COR013", 22050, 1, 2, 0xFF, 0xC6},
    {"curse you", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ENG\\COR014", 22050, 1, 2, 0xFF, 0xC7},
    {"world is ours", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ENG\\COR015", 22050, 1, 2, 0xFF, 0xC8},
    {"so soon", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ENG\\COR016", 22050, 1, 2, 0xFF, 0xC9},
    {"it's game over", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ENG\\COR017", 22050, 1, 2, 0xFF, 0xCA},
    {"world is ours", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ENG\\COR018", 22050, 1, 2, 0xFF, 0xCB},
    {"reluctant hero", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ENG\\COR019", 22050, 1, 2, 0xFF, 0xCC},
    {"AmbientLoop", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\DRVD_BG", 11025, 1, 0, 0xFF, 0x10C5},
};
static GameSound SfxTabGAMEOVERFre[9] = {
    {"face my wrath", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_FRE\\FRCOR09", 22050, 1, 2, 0xFF, 0xC5},
    {"giving up", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_FRE\\FRCOR13", 22050, 1, 2, 0xFF, 0xC6},
    {"curse you", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_FRE\\FRCOR14", 22050, 1, 2, 0xFF, 0xC7},
    {"world is ours", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_FRE\\FRCOR15", 22050, 1, 2, 0xFF, 0xC8},
    {"so soon", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_FRE\\FRCOR16", 22050, 1, 2, 0xFF, 0xC9},
    {"it's game over", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_FRE\\FRCOR17", 22050, 1, 2, 0xFF, 0xCA},
    {"world is ours", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_FRE\\FRCOR18", 22050, 1, 2, 0xFF, 0xCB},
    {"reluctant hero", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_FRE\\FRCOR19", 22050, 1, 2, 0xFF, 0xCC},
    {"AmbientLoop", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\CVOR_BG", 11025, 1, 0, 0xFF, 0x10C5},
};
static GameSound SfxTabGAMEOVERGer[9] = {
    {"face my wrath", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_GER\\DECOR09", 22050, 1, 2, 0xFF, 0xC5},
    {"giving up", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_GER\\DECOR13", 22050, 1, 2, 0xFF, 0xC6},
    {"curse you", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_GER\\DECOR14", 22050, 1, 2, 0xFF, 0xC7},
    {"world is ours", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_GER\\DECOR15", 22050, 1, 2, 0xFF, 0xC8},
    {"so soon", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_GER\\DECOR16", 22050, 1, 2, 0xFF, 0xC9},
    {"it's game over", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_GER\\DECOR17", 22050, 1, 2, 0xFF, 0xCA},
    {"world is ours", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_GER\\DECOR18", 22050, 1, 2, 0xFF, 0xCB},
    {"reluctant hero", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_GER\\DECOR19", 22050, 1, 2, 0xFF, 0xCC},
    {"AmbientLoop", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\CVOR_BG", 11025, 1, 0, 0xFF, 0x10C5},
};
static GameSound SfxTabGAMEOVERIta[9] = {
    {"face my wrath", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ITA\\ITCOR09", 22050, 1, 2, 0xFF, 0xC5},
    {"giving up", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ITA\\ITCOR13", 22050, 1, 2, 0xFF, 0xC6},
    {"curse you", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ITA\\ITCOR14", 22050, 1, 2, 0xFF, 0xC7},
    {"world is ours", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ITA\\ITCOR15", 22050, 1, 2, 0xFF, 0xC8},
    {"so soon", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ITA\\ITCOR16", 22050, 1, 2, 0xFF, 0xC9},
    {"it's game over", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ITA\\ITCOR17", 22050, 1, 2, 0xFF, 0xCA},
    {"world is ours", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ITA\\ITCOR18", 22050, 1, 2, 0xFF, 0xCB},
    {"reluctant hero", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ITA\\ITCOR19", 22050, 1, 2, 0xFF, 0xCC},
    {"AmbientLoop", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\CVOR_BG", 11025, 1, 0, 0xFF, 0x10C5},
};
static GameSound SfxTabGAMEOVERSpa[9] = {
    {"face my wrath", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_SPA\\ESCOR09", 22050, 1, 2, 0xFF, 0xC5},
    {"giving up", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_SPA\\ESCOR13", 22050, 1, 2, 0xFF, 0xC6},
    {"curse you", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_SPA\\ESCOR14", 22050, 1, 2, 0xFF, 0xC7},
    {"world is ours", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_SPA\\ESCOR15", 22050, 1, 2, 0xFF, 0xC8},
    {"so soon", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_SPA\\ESCOR16", 22050, 1, 2, 0xFF, 0xC9},
    {"it's game over", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_SPA\\ESCOR17", 22050, 1, 2, 0xFF, 0xCA},
    {"world is ours", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_SPA\\ESCOR18", 22050, 1, 2, 0xFF, 0xCB},
    {"reluctant hero", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_SPA\\ESCOR19", 22050, 1, 2, 0xFF, 0xCC},
    {"AmbientLoop", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\CVOR_BG", 11025, 1, 0, 0xFF, 0x10C5},
};
static GameSound SfxTabGAMEOVERDut[9] = {
    {"face my wrath", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_DUT\\NLCOR09", 22050, 1, 2, 0xFF, 0xC5},
    {"giving up", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_DUT\\NLCOR13", 22050, 1, 2, 0xFF, 0xC6},
    {"curse you", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_DUT\\NLCOR14", 22050, 1, 2, 0xFF, 0xC7},
    {"world is ours", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_DUT\\NLCOR15", 22050, 1, 2, 0xFF, 0xC8},
    {"so soon", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_DUT\\NLCOR16", 22050, 1, 2, 0xFF, 0xC9},
    {"it's game over", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_DUT\\NLCOR17", 22050, 1, 2, 0xFF, 0xCA},
    {"world is ours", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_DUT\\NLCOR18", 22050, 1, 2, 0xFF, 0xCB},
    {"reluctant hero", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_DUT\\NLCOR19", 22050, 1, 2, 0xFF, 0xCC},
    {"AmbientLoop", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\CVOR_BG", 11025, 1, 0, 0xFF, 0x10C5},
};
static GameSound SfxTabSPACE[29] = {
    {"Space_USAIR01", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ENG\\USAIR01", 22050, 1, 2, 0xFF, 0xC5},
    {"Space_USAIR02", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ENG\\USAIR02", 22050, 1, 2, 0xFF, 0xC6},
    {"Space_USAIR04", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ENG\\USAIR04", 22050, 1, 2, 0xFF, 0xC7},
    {"Space_USAIR06", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ENG\\USAIR06", 22050, 1, 2, 0xFF, 0xC8},
    {"Space_USAKU24", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ENG\\USAKU24", 22050, 1, 2, 0xFF, 0xC9},
    {"Space_USCOR44", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ENG\\USCOR44", 22050, 1, 2, 0xFF, 0xCA},
    {"Space_USCOR52", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ENG\\USCOR52", 22050, 1, 2, 0xFF, 0xCB},
    {"Space_USCOR54", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ENG\\USCOR54", 22050, 1, 2, 0xFF, 0xCC},
    {"Space_USCOR56", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ENG\\USCOR56", 22050, 1, 2, 0xFF, 0xCD},
    {"Space_USCRN02", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ENG\\USCRN02", 22050, 1, 2, 0xFF, 0xCE},
    {"Space_USCRN07", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ENG\\USCRN07", 22050, 1, 2, 0xFF, 0xCF},
    {"Space_USCRN09", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ENG\\USCRN09", 22050, 1, 2, 0xFF, 0xD0},
    {"Space_USCRN10", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ENG\\USCRN10", 22050, 1, 2, 0xFF, 0xD1},
    {"Space_USCRN12", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ENG\\USCRN12", 22050, 1, 2, 0xFF, 0xD2},
    {"Space_USCRN19", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ENG\\USCRN19", 22050, 1, 2, 0xFF, 0xD3},
    {"Space_USCRN21", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ENG\\USCRN21", 22050, 1, 2, 0xFF, 0xD4},
    {"Space_USERTH01", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ENG\\USERTH01", 22050, 1, 2, 0xFF, 0xD5},
    {"Space_USERTH02", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ENG\\USERTH02", 22050, 1, 2, 0xFF, 0xD6},
    {"Space_USERTH05", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ENG\\USERTH05", 22050, 1, 2, 0xFF, 0xD7},
    {"Space_USERTH07", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ENG\\USERTH07", 22050, 1, 2, 0xFF, 0xD8},
    {"Space_USFIRE02", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ENG\\USFIRE02", 22050, 1, 2, 0xFF, 0xD9},
    {"Space_USFIRE05", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ENG\\USFIRE05", 22050, 1, 2, 0xFF, 0xDA},
    {"Space_USFIRE06", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ENG\\USFIRE06", 22050, 1, 2, 0xFF, 0xDB},
    {"Space_USFIRE07", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ENG\\USFIRE07", 22050, 1, 2, 0xFF, 0xDC},
    {"Space_USWA01", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ENG\\USWA01", 22050, 1, 2, 0xFF, 0xDD},
    {"Space_USWA02", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ENG\\USWA02", 22050, 1, 2, 0xFF, 0xDE},
    {"Space_USWA04", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ENG\\USWA04", 22050, 1, 2, 0xFF, 0xDF},
    {"Space_USWA08", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ENG\\USWA08", 22050, 1, 2, 0xFF, 0xE0},
    {"Ambient loop", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\VORTEXBG", 11025, 1, 2, 0xFF, 0x10C5},
};
static GameSound SfxTabSPACEFre[29] = {
    {"Space_USAIR01", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_FRE\\FRAIR01", 22050, 1, 2, 0xFF, 0xC5},
    {"Space_USAIR02", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_FRE\\FRAIR02", 22050, 1, 2, 0xFF, 0xC6},
    {"Space_USAIR04", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_FRE\\FRAIR04", 22050, 1, 2, 0xFF, 0xC7},
    {"Space_USAIR06", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_FRE\\FRAIR06", 22050, 1, 2, 0xFF, 0xC8},
    {"Space_USAKU24", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_FRE\\FRAKU24", 22050, 1, 2, 0xFF, 0xC9},
    {"Space_USCOR44", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_FRE\\FRCOR44", 22050, 1, 2, 0xFF, 0xCA},
    {"Space_USCOR52", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_FRE\\FRCOR52", 22050, 1, 2, 0xFF, 0xCB},
    {"Space_USCOR54", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_FRE\\FRCOR54", 22050, 1, 2, 0xFF, 0xCC},
    {"Space_USCOR56", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_FRE\\FRCOR56", 22050, 1, 2, 0xFF, 0xCD},
    {"Space_USCRN02", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_FRE\\FRCRN02", 22050, 1, 2, 0xFF, 0xCE},
    {"Space_USCRN07", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_FRE\\FRCRN07", 22050, 1, 2, 0xFF, 0xCF},
    {"Space_USCRN09", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_FRE\\FRCRN09", 22050, 1, 2, 0xFF, 0xD0},
    {"Space_USCRN10", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_FRE\\FRCRN10", 22050, 1, 2, 0xFF, 0xD1},
    {"Space_USCRN12", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_FRE\\FRCRN12", 22050, 1, 2, 0xFF, 0xD2},
    {"Space_USCRN19", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_FRE\\FRCRN19", 22050, 1, 2, 0xFF, 0xD3},
    {"Space_USCRN21", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_FRE\\FRCRN21", 22050, 1, 2, 0xFF, 0xD4},
    {"Space_USERTH01", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_FRE\\FRERTH01", 22050, 1, 2, 0xFF, 0xD5},
    {"Space_USERTH02", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_FRE\\FRERTH02", 22050, 1, 2, 0xFF, 0xD6},
    {"Space_USERTH05", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_FRE\\FRERTH05", 22050, 1, 2, 0xFF, 0xD7},
    {"Space_USERTH07", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_FRE\\FRERTH07", 22050, 1, 2, 0xFF, 0xD8},
    {"Space_USFIRE02", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_FRE\\FRFIRE02", 22050, 1, 2, 0xFF, 0xD9},
    {"Space_USFIRE05", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_FRE\\FRFIRE05", 22050, 1, 2, 0xFF, 0xDA},
    {"Space_USFIRE06", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_FRE\\FRFIRE06", 22050, 1, 2, 0xFF, 0xDB},
    {"Space_USFIRE07", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_FRE\\FRFIRE07", 22050, 1, 2, 0xFF, 0xDC},
    {"Space_USWA01", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_FRE\\FRWA01", 22050, 1, 2, 0xFF, 0xDD},
    {"Space_USWA02", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_FRE\\FRWA02", 22050, 1, 2, 0xFF, 0xDE},
    {"Space_USWA04", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_FRE\\FRWA04", 22050, 1, 2, 0xFF, 0xDF},
    {"Space_USWA08", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_FRE\\FRWA08", 22050, 1, 2, 0xFF, 0xE0},
    {"Ambient loop", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\VORTEXBG", 11025, 1, 2, 0xFF, 0x10C5},
};
static GameSound SfxTabSPACEGer[29] = {
    {"Space_USAIR01", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_GER\\DEAIR01", 22050, 1, 2, 0xFF, 0xC5},
    {"Space_USAIR02", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_GER\\DEAIR02", 22050, 1, 2, 0xFF, 0xC6},
    {"Space_USAIR04", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_GER\\DEAIR04", 22050, 1, 2, 0xFF, 0xC7},
    {"Space_USAIR06", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_GER\\DEAIR06", 22050, 1, 2, 0xFF, 0xC8},
    {"Space_USAKU24", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_GER\\DEAKU24", 22050, 1, 2, 0xFF, 0xC9},
    {"Space_USCOR44", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_GER\\DECOR44", 22050, 1, 2, 0xFF, 0xCA},
    {"Space_USCOR52", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_GER\\DECOR52", 22050, 1, 2, 0xFF, 0xCB},
    {"Space_USCOR54", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_GER\\DECOR54", 22050, 1, 2, 0xFF, 0xCC},
    {"Space_USCOR56", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_GER\\DECOR56", 22050, 1, 2, 0xFF, 0xCD},
    {"Space_USCRN02", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_GER\\DECRN02", 22050, 1, 2, 0xFF, 0xCE},
    {"Space_USCRN07", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_GER\\DECRN07", 22050, 1, 2, 0xFF, 0xCF},
    {"Space_USCRN09", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_GER\\DECRN09", 22050, 1, 2, 0xFF, 0xD0},
    {"Space_USCRN10", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_GER\\DECRN10", 22050, 1, 2, 0xFF, 0xD1},
    {"Space_USCRN12", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_GER\\DECRN12", 22050, 1, 2, 0xFF, 0xD2},
    {"Space_USCRN19", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_GER\\DECRN19", 22050, 1, 2, 0xFF, 0xD3},
    {"Space_USCRN21", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_GER\\DECRN21", 22050, 1, 2, 0xFF, 0xD4},
    {"Space_USERTH01", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_GER\\DEERTH01", 22050, 1, 2, 0xFF, 0xD5},
    {"Space_USERTH02", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_GER\\DEERTH02", 22050, 1, 2, 0xFF, 0xD6},
    {"Space_USERTH05", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_GER\\DEERTH05", 22050, 1, 2, 0xFF, 0xD7},
    {"Space_USERTH07", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_GER\\DEERTH07", 22050, 1, 2, 0xFF, 0xD8},
    {"Space_USFIRE02", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_GER\\DEFIRE02", 22050, 1, 2, 0xFF, 0xD9},
    {"Space_USFIRE05", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_GER\\DEFIRE05", 22050, 1, 2, 0xFF, 0xDA},
    {"Space_USFIRE06", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_GER\\DEFIRE06", 22050, 1, 2, 0xFF, 0xDB},
    {"Space_USFIRE07", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_GER\\DEFIRE07", 22050, 1, 2, 0xFF, 0xDC},
    {"Space_USWA01", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_GER\\DEWA01", 22050, 1, 2, 0xFF, 0xDD},
    {"Space_USWA02", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_GER\\DEWA02", 22050, 1, 2, 0xFF, 0xDE},
    {"Space_USWA04", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_GER\\DEWA04", 22050, 1, 2, 0xFF, 0xDF},
    {"Space_USWA08", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_GER\\DEWA08", 22050, 1, 2, 0xFF, 0xE0},
    {"Ambient loop", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\VORTEXBG", 11025, 1, 2, 0xFF, 0x10C5},
};
static GameSound SfxTabSPACEIta[29] = {
    {"Space_USAIR01", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ITA\\ITAIR01", 22050, 1, 2, 0xFF, 0xC5},
    {"Space_USAIR02", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ITA\\ITAIR02", 22050, 1, 2, 0xFF, 0xC6},
    {"Space_USAIR04", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ITA\\ITAIR04", 22050, 1, 2, 0xFF, 0xC7},
    {"Space_USAIR06", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ITA\\ITAIR06", 22050, 1, 2, 0xFF, 0xC8},
    {"Space_USAKU24", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ITA\\ITAKU24", 22050, 1, 2, 0xFF, 0xC9},
    {"Space_USCOR44", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ITA\\ITCOR44", 22050, 1, 2, 0xFF, 0xCA},
    {"Space_USCOR52", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ITA\\ITCOR52", 22050, 1, 2, 0xFF, 0xCB},
    {"Space_USCOR54", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ITA\\ITCOR54", 22050, 1, 2, 0xFF, 0xCC},
    {"Space_USCOR56", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ITA\\ITCOR56", 22050, 1, 2, 0xFF, 0xCD},
    {"Space_USCRN02", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ITA\\ITCRN02", 22050, 1, 2, 0xFF, 0xCE},
    {"Space_USCRN07", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ITA\\ITCRN07", 22050, 1, 2, 0xFF, 0xCF},
    {"Space_USCRN09", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ITA\\ITCRN09", 22050, 1, 2, 0xFF, 0xD0},
    {"Space_USCRN10", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ITA\\ITCRN10", 22050, 1, 2, 0xFF, 0xD1},
    {"Space_USCRN12", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ITA\\ITCRN12", 22050, 1, 2, 0xFF, 0xD2},
    {"Space_USCRN19", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ITA\\ITCRN19", 22050, 1, 2, 0xFF, 0xD3},
    {"Space_USCRN21", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ITA\\ITCRN21", 22050, 1, 2, 0xFF, 0xD4},
    {"Space_USERTH01", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ITA\\ITERTH01", 22050, 1, 2, 0xFF, 0xD5},
    {"Space_USERTH02", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ITA\\ITERTH02", 22050, 1, 2, 0xFF, 0xD6},
    {"Space_USERTH05", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ITA\\ITERTH05", 22050, 1, 2, 0xFF, 0xD7},
    {"Space_USERTH07", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ITA\\ITERTH07", 22050, 1, 2, 0xFF, 0xD8},
    {"Space_USFIRE02", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ITA\\ITFIRE02", 22050, 1, 2, 0xFF, 0xD9},
    {"Space_USFIRE05", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ITA\\ITFIRE05", 22050, 1, 2, 0xFF, 0xDA},
    {"Space_USFIRE06", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ITA\\ITFIRE06", 22050, 1, 2, 0xFF, 0xDB},
    {"Space_USFIRE07", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ITA\\ITFIRE07", 22050, 1, 2, 0xFF, 0xDC},
    {"Space_USWA01", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ITA\\ITWA01", 22050, 1, 2, 0xFF, 0xDD},
    {"Space_USWA02", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ITA\\ITWA02", 22050, 1, 2, 0xFF, 0xDE},
    {"Space_USWA04", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ITA\\ITWA04", 22050, 1, 2, 0xFF, 0xDF},
    {"Space_USWA08", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_ITA\\ITWA08", 22050, 1, 2, 0xFF, 0xE0},
    {"Ambient loop", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\VORTEXBG", 11025, 1, 2, 0xFF, 0x10C5},
};
static GameSound SfxTabSPACESpa[29] = {
    {"Space_USAIR01", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_SPA\\ESAIR01", 22050, 1, 2, 0xFF, 0xC5},
    {"Space_USAIR02", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_SPA\\ESAIR02", 22050, 1, 2, 0xFF, 0xC6},
    {"Space_USAIR04", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_SPA\\ESAIR04", 22050, 1, 2, 0xFF, 0xC7},
    {"Space_USAIR06", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_SPA\\ESAIR06", 22050, 1, 2, 0xFF, 0xC8},
    {"Space_USAKU24", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_SPA\\ESAKU24", 22050, 1, 2, 0xFF, 0xC9},
    {"Space_USCOR44", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_SPA\\ESCOR44", 22050, 1, 2, 0xFF, 0xCA},
    {"Space_USCOR52", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_SPA\\ESCOR52", 22050, 1, 2, 0xFF, 0xCB},
    {"Space_USCOR54", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_SPA\\ESCOR54", 22050, 1, 2, 0xFF, 0xCC},
    {"Space_USCOR56", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_SPA\\ESCOR56", 22050, 1, 2, 0xFF, 0xCD},
    {"Space_USCRN02", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_SPA\\ESCRN02", 22050, 1, 2, 0xFF, 0xCE},
    {"Space_USCRN07", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_SPA\\ESCRN07", 22050, 1, 2, 0xFF, 0xCF},
    {"Space_USCRN09", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_SPA\\ESCRN09", 22050, 1, 2, 0xFF, 0xD0},
    {"Space_USCRN10", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_SPA\\ESCRN10", 22050, 1, 2, 0xFF, 0xD1},
    {"Space_USCRN12", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_SPA\\ESCRN12", 22050, 1, 2, 0xFF, 0xD2},
    {"Space_USCRN19", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_SPA\\ESCRN19", 22050, 1, 2, 0xFF, 0xD3},
    {"Space_USCRN21", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_SPA\\ESCRN21", 22050, 1, 2, 0xFF, 0xD4},
    {"Space_USERTH01", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_SPA\\ESERTH01", 22050, 1, 2, 0xFF, 0xD5},
    {"Space_USERTH02", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_SPA\\ESERTH02", 22050, 1, 2, 0xFF, 0xD6},
    {"Space_USERTH05", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_SPA\\ESERTH05", 22050, 1, 2, 0xFF, 0xD7},
    {"Space_USERTH07", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_SPA\\ESERTH07", 22050, 1, 2, 0xFF, 0xD8},
    {"Space_USFIRE02", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_SPA\\ESFIRE02", 22050, 1, 2, 0xFF, 0xD9},
    {"Space_USFIRE05", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_SPA\\ESFIRE05", 22050, 1, 2, 0xFF, 0xDA},
    {"Space_USFIRE06", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_SPA\\ESFIRE06", 22050, 1, 2, 0xFF, 0xDB},
    {"Space_USFIRE07", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_SPA\\ESFIRE07", 22050, 1, 2, 0xFF, 0xDC},
    {"Space_USWA01", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_SPA\\ESWA01", 22050, 1, 2, 0xFF, 0xDD},
    {"Space_USWA02", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_SPA\\ESWA02", 22050, 1, 2, 0xFF, 0xDE},
    {"Space_USWA04", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_SPA\\ESWA04", 22050, 1, 2, 0xFF, 0xDF},
    {"Space_USWA08", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_SPA\\ESWA08", 22050, 1, 2, 0xFF, 0xE0},
    {"Ambient loop", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\VORTEXBG", 11025, 1, 2, 0xFF, 0x10C5},
};
static GameSound SfxTabSPACEDut[29] = {
    {"Space_USAIR01", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_DUT\\NLAIR01", 22050, 1, 2, 0xFF, 0xC5},
    {"Space_USAIR02", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_DUT\\NLAIR02", 22050, 1, 2, 0xFF, 0xC6},
    {"Space_USAIR04", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_DUT\\NLAIR04", 22050, 1, 2, 0xFF, 0xC7},
    {"Space_USAIR06", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_DUT\\NLAIR06", 22050, 1, 2, 0xFF, 0xC8},
    {"Space_USAKU24", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_DUT\\NLAKU24", 22050, 1, 2, 0xFF, 0xC9},
    {"Space_USCOR44", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_DUT\\NLCOR44", 22050, 1, 2, 0xFF, 0xCA},
    {"Space_USCOR52", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_DUT\\NLCOR52", 22050, 1, 2, 0xFF, 0xCB},
    {"Space_USCOR54", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_DUT\\NLCOR54", 22050, 1, 2, 0xFF, 0xCC},
    {"Space_USCOR56", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_DUT\\NLCOR56", 22050, 1, 2, 0xFF, 0xCD},
    {"Space_USCRN02", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_DUT\\NLCRN02", 22050, 1, 2, 0xFF, 0xCE},
    {"Space_USCRN07", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_DUT\\NLCRN07", 22050, 1, 2, 0xFF, 0xCF},
    {"Space_USCRN09", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_DUT\\NLCRN09", 22050, 1, 2, 0xFF, 0xD0},
    {"Space_USCRN10", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_DUT\\NLCRN10", 22050, 1, 2, 0xFF, 0xD1},
    {"Space_USCRN12", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_DUT\\NLCRN12", 22050, 1, 2, 0xFF, 0xD2},
    {"Space_USCRN19", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_DUT\\NLCRN19", 22050, 1, 2, 0xFF, 0xD3},
    {"Space_USCRN21", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_DUT\\NLCRN21", 22050, 1, 2, 0xFF, 0xD4},
    {"Space_USERTH01", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_DUT\\NLERTH01", 22050, 1, 2, 0xFF, 0xD5},
    {"Space_USERTH02", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_DUT\\NLERTH02", 22050, 1, 2, 0xFF, 0xD6},
    {"Space_USERTH05", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_DUT\\NLERTH05", 22050, 1, 2, 0xFF, 0xD7},
    {"Space_USERTH07", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_DUT\\NLERTH07", 22050, 1, 2, 0xFF, 0xD8},
    {"Space_USFIRE02", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_DUT\\NLFIRE02", 22050, 1, 2, 0xFF, 0xD9},
    {"Space_USFIRE05", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_DUT\\NLFIRE05", 22050, 1, 2, 0xFF, 0xDA},
    {"Space_USFIRE06", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_DUT\\NLFIRE06", 22050, 1, 2, 0xFF, 0xDB},
    {"Space_USFIRE07", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_DUT\\NLFIRE07", 22050, 1, 2, 0xFF, 0xDC},
    {"Space_USWA01", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_DUT\\NLWA01", 22050, 1, 2, 0xFF, 0xDD},
    {"Space_USWA02", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_DUT\\NLWA02", 22050, 1, 2, 0xFF, 0xDE},
    {"Space_USWA04", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_DUT\\NLWA04", 22050, 1, 2, 0xFF, 0xDF},
    {"Space_USWA08", 1882, 0x3FFF, 0, 0, 0, 0, "VCE_DUT\\NLWA08", 22050, 1, 2, 0xFF, 0xE0},
    {"Ambient loop", 941, 0x3FFF, 0, 0, 0, 0, "LEVSPEC\\VORTEXBG", 11025, 1, 2, 0xFF, 0x10C5},
};
static GameSound SfxTabCREDITS[2] = {
    {"music left", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\SOLARBOL", -21436, 0, 1, 0xFF, 0xC5},
    {"music right", 3763, 0x1BFF, 0, 0, 0, 0, "MUSIC\\SOLARBOR", -21436, 0, 1, 0xFF, 0xC6},
};
NUSOUND_FILENAME_INFO SfxInfo[301] = {0};


extern struct cursor_s Cursor;
extern s32 Paused;
extern s32 GameMode;
extern s32 new_level;
extern s32 cutmovie;
extern u8 temp_music_volume;
extern u8 temp_surround;
extern s32 Bonus;
extern f32 bonus_duration;
extern f32 bonus_time;
extern s32 nusound_dpl_enabled;
extern f32 nusound_fade_end;
extern s32 Level;
extern u64 LBIT;
extern s32 VEHICLECONTROL;

extern s32 ChaseActive(void);
extern int SOUND_SetChannelVolume(unsigned int channel, int left, int right);
extern void edbitsSetSoundFxVolume(int volume);
extern void NuSoundLocalSet(NUSOUND_FILENAME_INFO* nfi, s32 SFX_COUNT, s32 GLOBAL_SFX_COUNT);


void InitLevelSfxTables(void) {
    s32 i;

    for (i = 0; i < 44; i++) {
        LData[i].pSFX = 0;
        LData[i].nSFX = 0;
    }

    LData[0].pSFX = SfxTabESKIMO;
    LData[0].nSFX = 0xCC;
    LData[1].pSFX = SfxTabBURN;
    LData[1].nSFX = 0xD2;
    LData[2].pSFX = SfxTabOHNO;
    LData[2].nSFX = 0xCC;
    LData[36].pSFX = SfxTabSMOKEY;
    LData[36].nSFX = 0xC9;
    LData[4].pSFX = SfxTabREACTOR;
    LData[4].nSFX = 0xCA;
    LData[5].pSFX = SfxTabAVALANCHE;
    LData[5].nSFX = 0xCE;
    LData[6].pSFX = SfxTabCORAL;
    LData[6].nSFX = 0xCB;
    LData[7].pSFX = SfxTabARCTIC;
    LData[7].nSFX = 0xCC;
    LData[8].pSFX = SfxTabJUNGLE;
    LData[8].nSFX = 0xCC;
    LData[9].pSFX = SfxTabNATURE;
    LData[9].nSFX = 0xCC;
    LData[10].pSFX = SfxTabGAUNTLET;
    LData[10].nSFX = 0xC9;
    LData[11].pSFX = SfxTabBAMBOO;
    LData[11].nSFX = 0xCA;
    LData[12].pSFX = SfxTabBANZAI;
    LData[12].nSFX = 0xCE;
    LData[13].pSFX = SfxTabTORNADO;
    LData[13].nSFX = 0xCD;
    LData[14].pSFX = SfxTabTSUNAMI;
    LData[14].nSFX = 0xD5;
    LData[15].pSFX = SfxTabFRENZY;
    LData[15].nSFX = 0xCF;
    LData[16].pSFX = SfxTabMADNESS;
    LData[16].nSFX = 0xC8;
    LData[17].pSFX = SfxTabLIZARDS;
    LData[17].nSFX = 0xD0;
    LData[18].pSFX = SfxTabSINKING;
    LData[18].nSFX = 0xDA;
    LData[19].pSFX = SfxTabGOLD;
    LData[19].nSFX = 0xD2;
    LData[20].pSFX = SfxTabVOID;
    LData[20].nSFX = 0xCA;
    LData[21].pSFX = SfxTabROKS;
    LData[21].nSFX = 0xD6;
    LData[22].pSFX = SfxTabASHES;
    LData[22].nSFX = 0xD7;
    LData[23].pSFX = SfxTabDRAIN;
    LData[23].nSFX = 0xD0;
    LData[24].pSFX = SfxTabPRESSURE;
    LData[24].nSFX = 0xCD;
    LData[25].pSFX = SfxTabCRUNCH;
    LData[25].nSFX = 0xCC;
    LData[26].pSFX = SfxTabOIDS;
    LData[26].nSFX = 0xDB;
    LData[27].pSFX = SfxTabKNIGHT;
    LData[27].nSFX = 0xC8;
    LData[28].pSFX = SfxTabTOWN;
    LData[28].nSFX = 0xC8;
    LData[29].pSFX = SfxTabICE;
    LData[29].nSFX = 0xCB;
    LData[30].pSFX = SfxTabHEIGHTS;
    LData[30].nSFX = 0xC7;
    LData[31].pSFX = SfxTabBALLS;
    LData[31].nSFX = 0xCB;
    LData[32].pSFX = SfxTabBOWLER;
    LData[32].nSFX = 0xC8;
    LData[33].pSFX = SfxTabVORTEX;
    LData[33].nSFX = 0xC8;
    LData[34].pSFX = SfxTabSEA;
    LData[34].nSFX = 0xCA;
    LData[3].pSFX = SfxTabSMOKEY;
    LData[3].nSFX = 0xC9;
    LData[37].pSFX = SfxTabFRONTEND;
    LData[37].nSFX = 0x115;
    LData[38].pSFX = SfxTabGAMEOVER;
    LData[38].nSFX = 0xCE;
    LData[40].pSFX = SfxTabSPACE;
    LData[40].nSFX = 0xE2;
    LData[43].pSFX = SfxTabCREDITS;
    LData[43].nSFX = 0xC7;
}


void InitGlobalSfx(void) {
    s32 i;

    for (i = 0; i < 0xC5; i++) {
        NuStrCpy(SfxInfo[i].Filename, "SFX\\");
        NuStrCat(SfxInfo[i].Filename, SfxTabGLOBAL[i].path);
        NuStrCat(SfxInfo[i].Filename, ".VAG");
        SfxInfo[i].Pitch = SfxTabGLOBAL[i].frequency;
        SfxInfo[i].LoopInfo = SfxTabGLOBAL[i].stream;
        SfxInfo[i].ID = SfxTabGLOBAL[i].id;
    }
    NuStrCpy(SfxInfo[i].Filename, "");
    SfxInfo[i].Pitch = 0;
    SfxInfo[i].LoopInfo = 1;
    SfxInfo[i].ID = -1;
    NuSoundSetGlobalDat("SFX.DAT");
    NuSoundInit(SfxInfo);
    SFXCOUNT_ALL = i;
}

void InitLocalSfx(GameSound *sfxTab, s32 count) {
    s32 i;

    if (sfxTab == 0) {
        printf("No local SFX table available!\n");
        return;
    }

    if (sfxTab == SfxTabSPACE) {
        switch (Game.language) {
        case 0:
            break;
        case 1:
            sfxTab = SfxTabSPACEFre;
            break;
        case 2:
            sfxTab = SfxTabSPACEGer;
            break;
        case 3:
            sfxTab = SfxTabSPACEIta;
            break;
        case 4:
            sfxTab = SfxTabSPACESpa;
            break;
        case 5:
            sfxTab = SfxTabSPACEDut;
            break;
        }
    }

    if (sfxTab == SfxTabGAMEOVER) {
        switch (Game.language) {
        case 0:
            break;
        case 1:
            sfxTab = SfxTabGAMEOVERFre;
            break;
        case 2:
            sfxTab = SfxTabGAMEOVERGer;
            break;
        case 3:
            sfxTab = SfxTabGAMEOVERIta;
            break;
        case 4:
            sfxTab = SfxTabGAMEOVERSpa;
            break;
        case 5:
            sfxTab = SfxTabGAMEOVERDut;
            break;
        }
    }

    if (count - 0xC5 < 1) {
        printf("No local SFX in table!\n");
        return;
    }

    if (count > 0x12B) {
        printf("SFX table not large enough for all local SFX...\n");
        count = 0x12B;
    }

    count -= 0xC5;
    sfxcount_local = count;
    for (i = 0; i < count; i++) {
        NuStrCpy(SfxInfo[i].Filename, "SFX\\");
        NuStrCat(SfxInfo[i].Filename, sfxTab[i].path);
        NuStrCat(SfxInfo[i].Filename, ".VAG");
        SfxInfo[i].Pitch = sfxTab[i].frequency;
        SfxInfo[i].LoopInfo = sfxTab[i].stream;
        SfxInfo[i].ID = sfxTab[i].id;
    }
    NuStrCpy(SfxInfo[i].Filename, "");
    SfxInfo[i].Pitch = 0;
    SfxInfo[i].LoopInfo = 1;
    SfxInfo[i].ID = -1;
    NuSoundLocalSet(SfxInfo, count, 0xC5);
    CurSfxTabLocal = sfxTab;
    SFXCOUNT_ALL = count + 0xC5;
    for (i = 0; i < count; i++) {
        CurSfxTabLocal[i].wait = 0;
    }
}

s32 game_music = -1;
s32 gamesfx_channel = -1;
s32 gamesfx_effect_volume = -1;
s32 gamesfx_volume = -1;
s32 gamesfx_pitch = -1;
s32 gamesfx_edbits = -1;
s32 testglobalsfx = -1;
s32 last_testglobalsfx = -1;
s32 testsfx = -1;
f32 musicvol_mul = 0.0f;
s32 soundtestcount = 0;
s32 SFXCOUNT_ALL = 0;
GameSound *CurSfxTabLocal = 0;


void UpdateGameSfx(void) {
    GameSound *p;
    s32 i;

    p = SfxTabGLOBAL;
    do {
        if (p->wait != 0) {
            p->wait--;
        }
        p++;
    } while ((nusintptr)p < (nusintptr)&SfxTabGLOBAL[0xC5]);

    if (CurSfxTabLocal == 0) {
        return;
    }
    if (SFXCOUNT_ALL <= 0xC5) {
        return;
    }
    for (i = 0; i < SFXCOUNT_ALL - 0xC5; i++) {
        if (CurSfxTabLocal[i].wait != 0) {
            CurSfxTabLocal[i].wait--;
        }
    }
}

void ResetGameSfx(void) {
    s32 i;

    for (i = 0xC4; i >= 0; i--) {
        SfxTabGLOBAL[i].wait = 0;
    }

    if (CurSfxTabLocal == 0) {
        return;
    }
    if (SFXCOUNT_ALL <= 0xC5) {
        return;
    }
    for (i = 0; i < SFXCOUNT_ALL - 0xC5; i++) {
        CurSfxTabLocal[i].wait = 0;
    }
}

void PauseGameAudio(s32 pause) {
    s32 i;

    if (pause != 0) {
        NuSoundSetChannelPitch(4, 0, 0);
    }
    for (i = 6; i < 0x18; i++) {
        if ((i & 7) == 0) {
            NuSoundUpdate();
        }
        SOUND_StopSound(i);
    }
    for (i = 0x18; i < 0x30; i++) {
        if ((i & 7) == 0) {
            NuSoundUpdate();
        }
        NuSoundSetChannelPitch(i, 0, 0);
    }
}

void ResumeGameAudio(void) {
    NuSoundSetChannelPitch(4, 0x75A, 0);
}

void GameSfx(s32 sfx, struct nuvec_s *pos) {
    GameSound *info;
    struct nuvec_s camPos;
    s32 vol;
    s32 pitch;
    s32 proximity;
    s32 type;

    camPos = *(struct nuvec_s *)&GameCam[0].m._30;
    proximity = 0;
    if (sfx < 0) {
        goto cleanup;
    }
    if (sfx >= SFXCOUNT_ALL) {
        goto cleanup;
    }
    if (sfx < 0xC5) {
        info = &SfxTabGLOBAL[sfx];
    } else {
        info = &CurSfxTabLocal[sfx - 0xC5];
    }
    if (sfx == 0x81 || sfx == 0x53) {
        if (pos != 0) {
            f32 dist1 = NuVecDist(&camPos, &info->Pos, 0);
            f32 dist2 = NuVecDist(&camPos, pos, 0);
            if (dist2 + 2.0f < dist1) {
                proximity = 1;
            }
        }
    }
    if (info->wait != 0) {
        if (proximity == 0) {
            goto cleanup;
        }
    }
    type = info->type;
    if (type == 1) {
        vol = info->volume * Game.music_volume / 100;
    } else {
        if (gamesfx_volume == -1) {
            vol = Game.sfx_volume;
        } else {
            vol = gamesfx_volume;
        }
        if (gamesfx_effect_volume == -1) {
            vol = info->volume * vol / 100;
        } else {
            vol = gamesfx_effect_volume * vol / 100;
        }
    }
    if (gamesfx_pitch == -1) {
        pitch = info->pitch;
    } else {
        pitch = gamesfx_pitch;
    }
    if (pos != 0) {
        NuSoundPlay3d(pos, sfx, vol, vol, pitch);
    } else if (gamesfx_channel != -1) {
        NuSoundPlayChan(sfx, vol, vol, pitch, gamesfx_channel);
    } else {
        NuSoundPlay(sfx, vol, vol, pitch);
    }
    if (PLAYERCOUNT != 0 && gamesfx_edbits == 0) {
        if (info->buzz != 0) {
            NewBuzz(&player->rumble, info->buzz);
        }
        if (info->rumble != 0) {
            NewRumble(&player->rumble, info->rumble);
        }
    }
    info->wait = info->delay;
    if (pos != 0) {
        info->Pos = *pos;
    } else {
        info->Pos = camPos;
    }
cleanup:
    gamesfx_edbits = 0;
    gamesfx_effect_volume = -1;
    gamesfx_channel = -1;
    gamesfx_volume = -1;
    gamesfx_pitch = -1;
}

void GameSfxLoop(s32 sfx, struct nuvec_s *pos) {
    GameSound *info;
    s32 vol;
    s32 pitch;

    if (sfx < 0) {
        goto cleanup;
    }
    if (sfx >= SFXCOUNT_ALL) {
        goto cleanup;
    }
    if (sfx < 0xC5) {
        info = &SfxTabGLOBAL[sfx];
    } else {
        info = &CurSfxTabLocal[sfx - 0xC5];
    }
    if (info->wait != 0) {
        goto cleanup;
    }
    if (gamesfx_volume == -1) {
        vol = Game.sfx_volume;
    } else {
        vol = gamesfx_volume;
    }
    if (gamesfx_effect_volume == -1) {
        vol = info->volume * vol / 100;
    } else {
        vol = gamesfx_effect_volume * vol / 100;
    }
    if (gamesfx_pitch == -1) {
        pitch = info->pitch;
    } else {
        pitch = gamesfx_pitch;
    }
    if (pos != 0) {
        NuSoundPlay3dLoopSfx(pos, sfx, vol, vol, pitch);
    }
    if (PLAYERCOUNT != 0 && gamesfx_edbits == 0) {
        if (info->buzz != 0) {
            NewBuzz(&player->rumble, info->buzz);
        }
        if (info->rumble != 0) {
            NewRumble(&player->rumble, info->rumble);
        }
    }
    info->wait = info->delay;
cleanup:
    gamesfx_edbits = 0;
    gamesfx_effect_volume = -1;
    gamesfx_channel = -1;
    gamesfx_volume = -1;
    gamesfx_pitch = -1;
}

void GameAudioUpdate(void) {
    s32 testMode;
    s32 vol;
    f32 volScale;
    f32 fadeTarget;
    double dist;

    if (Cursor.menu == 5) {
        testMode = (Cursor.y == 2) ? 2 : 1;
    } else {
        testMode = 0;
    }

    if ((Paused == 0 && (player->obj.mask == 0 || player->obj.mask->active < 3) &&
         GameMode != 1 && new_level == -1 && Cursor.menu != 0x21) ||
        (Paused != 0 && testMode == 2) || cutmovie == 0) {
        volScale = 1.0f;
    } else {
        volScale = 0.0f;
    }

    if (musicvol_mul > volScale) {
        musicvol_mul -= 0.02f;
        if (musicvol_mul < volScale) {
            musicvol_mul = volScale;
        }
    } else if (musicvol_mul < volScale) {
        musicvol_mul += 0.02f;
        if (musicvol_mul > volScale) {
            musicvol_mul = volScale;
        }
    }

    if (game_music != -1) {
        vol = (testMode != 0) ? temp_music_volume : Game.music_volume;
        vol = (f32)(vol * 0x1BFF / 100) * musicvol_mul;
        if (Bonus == 1 || Bonus == 3) {
            dist = bonus_duration * 0.5f - bonus_time;
            if (dist < 0.0) {
                dist = 0.0 - dist;
            }
            volScale = dist;
            if (volScale < 1.0f) {
                volScale *= (f32)(25 - Paused) / 25.0f;
                vol = (f32)vol * volScale;
            }
        }
        SOUND_SetChannelVolume(0, vol, 0);
        SOUND_SetChannelVolume(1, 0, vol);
    }

    if (Cursor.menu == 5) {
        nusound_dpl_enabled = temp_surround;
    } else {
        nusound_dpl_enabled = Game.surround;
    }

    fadeTarget = 12.5f;
    if (ChaseActive() != -1 || Level == 0x17) {
        fadeTarget = 50.0f;
    } else if (LBIT & 0x3E00000) {
        fadeTarget = 25.0f;
    } else if (Level == 9 || Level == 3) {
        fadeTarget = 50.0f;
    } else if (VEHICLECONTROL == 2) {
        fadeTarget = 25.0f;
    } else if (VEHICLECONTROL == 1) {
        s16 vehicle = player->obj.vehicle;

        if (vehicle == 0x53 || vehicle == 0x20) {
            fadeTarget = 25.0f;
        } else if (vehicle == 0x36 || vehicle == 0x81 || vehicle == 0x8B || vehicle == 0x99) {
            fadeTarget = 100.0f;
        }
    }

    if (nusound_fade_end > fadeTarget) {
        nusound_fade_end -= 0.5f;
        if (nusound_fade_end < fadeTarget) {
            nusound_fade_end = fadeTarget;
        }
    } else if (nusound_fade_end < fadeTarget) {
        nusound_fade_end += 0.5f;
        if (nusound_fade_end > fadeTarget) {
            nusound_fade_end = fadeTarget;
        }
    }

    if (testsfx != -1) {
        GameSfx(testsfx + 0xC7, 0);
        testsfx = -1;
    }
    if (testglobalsfx != -1) {
        last_testglobalsfx = testglobalsfx;
        GameSfx(testglobalsfx, 0);
        testglobalsfx = -1;
    }

    edbitsSetSoundFxVolume(Game.sfx_volume);
}


void GameMusic(s32 sfx, s32 i) {
    GameSound *info;
    s32 vol;

    if (sfx < 0) {
        return;
    }
    if (sfx >= SFXCOUNT_ALL - 1) {
        return;
    }
    if (sfx < 0xC5) {
        info = &SfxTabGLOBAL[sfx];
    } else {
        info = &CurSfxTabLocal[sfx - 0xC5];
    }
    if (info->type == 1) {
        vol = Game.music_volume * 0x1BFF / 100;
    } else {
        vol = info->volume * Game.sfx_volume / 100;
    }
    if (i == 0) {
        NuSoundStopStream(0);
        NuSoundStopStream(1);
        NuSoundPlayStereo(sfx, sfx + 1, vol, vol, info->pitch);
    } else {
        NuSoundStopStream(2);
        NuSoundStopStream(3);
        NuSoundPlayStereo2(sfx, sfx + 1, vol, vol, info->pitch);
    }
    NuSoundUpdate();
    game_music = sfx;
}

void TestLocalSfx(void) {
    for (;;) {
        InitLocalSfx(SfxTabLIZARDS, 0xD0);
        NuSoundKillAllAudio();
        InitLocalSfx(SfxTabGOLD, 0xD2);
        NuSoundKillAllAudio();
        InitLocalSfx(SfxTabBANZAI, 0xCE);
        NuSoundKillAllAudio();
        soundtestcount++;
    }
}
