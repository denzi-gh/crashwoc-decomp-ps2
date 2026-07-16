/* WrathSDK: revive retail's stubbed debug line renderer.
 *
 * NuRndrLine3dDbg (0x0012FDF0) is `jr $ra; nop`. ~20 debug renderers call it
 * and therefore draw nothing. This mod supplants it with a queue that the
 * real NuRndrLine3d drains during the 3D pass.
 *
 * WHY A QUEUE AND NOT A DIRECT FORWARD
 * ------------------------------------
 * The obvious shim -- translate the args and call NuRndrLine3d immediately --
 * works only for callers that already run inside a draw pass. It was tried and
 * verified: with ChrisInTheHouse=1 and LocIndx=0 on level 0x18, the draw-phase
 * DrawWeatherBoss_a put a red cross on the boss's right hand. But the fire-boss
 * and jeep callers stayed silent, because:
 *
 *   NuRndrLine3d submits STRAIGHT into the render stream (NuRndrStreamLink,
 *   NuRndrStreamAddMtl, vpDmaTag_*), so it needs an open stream and a live
 *   camera -- it only works mid-draw.
 *
 *   Most NuRndrLine3dDbg callers are UPDATE-phase game logic:
 *     ProcessFireBoss  <- ProcessFireBossLevel
 *     JeepCam          <- MoveGameCamera
 *     MovePlayerJeep / ProcessBigGun / ProcessSpaceArenaLevel / ZoffaSmoke
 *   No stream is open when they run, so a direct submit goes nowhere.
 *
 * That is almost certainly why TT had two functions rather than one: the dev
 * build's NuRndrLine3dDbg BUFFERED, and something flushed it during rendering.
 * Its signature is the tell -- loose floats and no matrix is a queue API, not a
 * renderer's. So: buffer here, drain from a draw hook.
 *
 * ARG MAPPING (confirmed from both callers -- DrawLine 0x00220DB0 and
 * DrawCross 0x00220D04, which agree exactly):
 *
 *   NuRndrLine3dDbg(colour -> a0, x1,y1,z1 -> f12,f13,f14,
 *                                 x2,y2,z2 -> f15,f16,f17)
 *   NuRndrLine3d(line -> a0 (POINTER to two verts), unused -> a1, mtx -> a2)
 *
 * A straight redirect makes NuRndrLine3d read a colour (0xFFFFFFFF) as a vertex
 * pointer. The EABI gives us the translation for free: an int lands in a0 and
 * six floats in f12..f17, exactly the incoming registers.
 */

typedef struct { float x, y, z; } wrath_vec;

/* NuRndrVtx: 0x24 stride, pos at +0x00, colour at +0x18. Mirrors
 * src/gamelib/trigger.c, whose DebugRenderTriggers is the reference caller of
 * NuRndrLine3d and the source of the (line, 0, 0) argument pair below. */
struct wrath_rndrvtx {
    wrath_vec pos;          /* 0x00 */
    char pad0c[0x0C];       /* 0x0C */
    int col;                /* 0x18 */
    char pad1c[0x08];       /* 0x1C -- 0x24 total */
};

void NuRndrLine3d(struct wrath_rndrvtx *line, void *unused, void *mtx); /* 0x00128138 */
void orig_DrawLevel(void);                                              /* 0x001F0C80 */

extern int PLAYERCOUNT;   /* 0x00630AF0 -- 0 in the front-end menus */
extern int Level;         /* 0x00630B90 */

/* Retail debug overlays with NO callers: their call sites were compiled out,
 * but the bodies survive and print through the REAL font renderer (NuFntSet /
 * NuFntSetPen / NuFntScale / NuFntPrintEx), not the stubbed line one. All are
 * void and read their state from globals, so calling them is all it takes.
 *
 * Safe from here: DrawLevel runs at 0x001CA430, inside the same scene as
 * DrawExtraCreatures (0x001CA0EC) -- where DrawWeatherBoss_a's ChrisInTheHouse
 * text is verified to print -- and before NuRndrEndScene (0x001CA4F0). */
void DrawFireBossDebugStuff(void);  /* 0x0022A5A8 level 0x16; reads FireBossHoldPlayer,
                                     * WallOfFireOn/Attatched, SHEIGHT, player  */
void DrawJeepDebugStuff(void);      /* 0x00225D98 self-gates on JamesInTheHouse;
                                     * reads CurrentDebugJeep, JeepFrame        */
void DrawGliderDebugStuff(void);    /* 0x0021D2C8 glider sections               */
void DrawZoffaDebugStuff(void);     /* 0x00206F80 reads the EnemyZoffa array    */

/* Control surface: poke these over PINE (addresses are printed by the build /
 * readable from wrathsdk.elf's symtab). Default off.
 *
 * Deliberately NOT level-gated. Each overlay belongs to one level type, but the
 * glider/Zoffa levels are unverified and a wrong hardcoded gate would silently
 * never fire -- worse than letting the operator choose. Enabling one outside its
 * own level reads stale globals: that shows junk, which is a debug tool working
 * as intended. The PLAYERCOUNT guard below is the one real safety net, since
 * DrawFireBossDebugStuff dereferences `player`. */
int wrath_show_fireboss;
int wrath_show_jeep;
int wrath_show_glider;
int wrath_show_zoffa;

/* ---------------------------------------------------------------------------
 * The level editor
 *
 * Retail still contains TT's whole in-game editor (618 fns, 13 ed* TUs) and
 * `main` STILL CALLS IT every frame -- edmainProcess/edmainRender, both behind
 * `editor_active`. Only the setup calls were removed:
 *
 *   edmainRegister(desc)  -- no callers. Head-inserts a module descriptor into
 *                            the list at D_0062F8CC.
 *   edmainInit(a0, name)  -- no callers. Walks that list calling each
 *                            desc->0x0C, then NuCameraCreate, eduiInit,
 *                            edmainCreateMainMenu, edmainReadLevelFile.
 *
 * All eight descriptors are fully populated in the retail image -- init fn at
 * +0x0C, process fn at +0x2C, display name at +0x08 -- with next/prev NULL,
 * i.e. built but never registered. So: register the eight, init, flip the flag.
 *
 * Independently confirmed viable: the TWoC E3 Demo has community .pnach codes
 * that hand-assemble a stub jal'ing the same init path, and the editor works
 * there. Those target the demo build (its editor_active is 0x004694EC); this is
 * the retail PAL v1.03 port.
 *
 * edmainInit's a0 lands in D_0062F8E0, which edmainCreateMainMenu passes as a
 * menu-item callback. 0 is safe to construct with -- just do not pick that
 * entry.
 */
void edmainRegister(void *desc);        /* 0x001B7A00 */
int  edmainInit(void *cb, char *level); /* 0x001B7BB0 */
extern int editor_active;               /* 0x0062FA10 -- gates edmainProcess/Render in main */

extern char edptldesc[];    /* 0x00484EE0 "Particle Positioner" */
extern char edobjdesc[];    /* 0x00489DC0 "Object Editor"       */
extern char edanimdesc[];   /* 0x0049C660 "Anim Editor"         */
extern char edgradesc[];    /* 0x004BF510 "Grass Editor"        */
extern char edcrtdesc[];    /* 0x005C1820 "Crate Editor"        */
extern char edwmpdesc[];    /* 0x005CD178 "Wumpa Editor"        */
extern char edaidesc[];     /* 0x005CD1F0 "AI Editor"           */
extern char edlightdesc[];  /* 0x005CF588 "Light Editor"        */

void orig_UpdateLevel(void);            /* 0x001EF658 */

/* --- host filesystem -----------------------------------------------------
 * NuFileOpen builds every path as DEVTAB[D_0062E99C] + D_00293730 + filename,
 * where DEVTAB (0x00293720) is four pointers alternating "host0:" / "cdrom0:\".
 * The game was built to run from a devkit host or a disc; retail's main just
 * calls NuFileInitEx(1) at startup (0x001C8D80).
 *
 * Setting the index to 0 makes the whole game load from the PC filesystem under
 * PCSX2 (needs Settings -> Emulation -> Enable Host Filesystem, and the ELF must
 * sit in the extracted tree's root -- host access is sandboxed to the ELF's
 * directory). VERIFIED 2026-07-16: with it set, renaming SNOW.GSC on disk made
 * that level load with no geometry while its crates and AI -- separate files --
 * still worked. The disc image was untouched, so only a host read explains it.
 *
 * Why re-assert it here instead of patching main's boot argument to 0:
 * NuFileSifLoadModule ALSO reads this index, and the ELF's host IRX paths
 * ("host0:/usr/local/sce/iop/modules/*.irx") are dev leftovers that do not ship.
 * Booting on host0: would likely fail to load the IOP modules. So let startup
 * run on cdrom0: as retail does, and switch only afterwards -- which also makes
 * the setting survive anything that re-inits the file system.
 *
 * D_0062E99C is an unregistered D_ symbol (no .mdebug entry), so it has no name
 * to link against -- bind it by address, like the trigger flags. */
#define NU_FILE_DEVICE (*(volatile int *)0x0062E99Cu)
#define NU_FILE_DEV_HOST  0
#define NU_FILE_DEV_CDROM 1

/* Poke 1 over PINE to load everything from the host filesystem. */
int wrath_host;

/* --- editor UI font scale ------------------------------------------------
 * main hardcodes NuFntScale(12, 12) immediately before edmainRender
 * (0x001CA4C8..0x001CA4D0, two addiu immediates). That constant is the reason
 * the editor UI renders wrong on retail -- the community's E3 Demo code set
 * ships a "Font Scale Corrector" patching the same site, so it is a known-bad
 * value rather than something we broke.
 *
 * Patching those two immediates would work but bakes in a guess. Instead
 * re-apply the scale from the edmainRender hook: it runs AFTER main's call, so
 * ours wins, and both numbers stay pokeable over PINE
 * (`wrathsdk.ctl flag wrath_font_w 8`) to dial in by eye. 12/12 reproduces
 * retail exactly, so the default changes nothing until asked. */
void NuFntScale(int w, int h);          /* 0x00113638-ish; a0/a1 are ints */
void orig_edmainRender(void);           /* 0x001B7AC0 */

int wrath_font_w = 12;
int wrath_font_h = 12;

void wrath_edmain_render(void)
{
    NuFntScale(wrath_font_w, wrath_font_h);
    orig_edmainRender();
}

/* Poke to 1 over PINE to boot the editor. Once armed it re-inits on every level
 * change (see wrath_update_level) -- eduiInit builds the UI's materials, and a
 * level load reallocates the VRAM under them, which is what makes the editor's
 * font and selection highlight render as garbage in a level after booting it in
 * the hub. */
int wrath_editor_arm;
int wrath_editor_booted;   /* readable: 1 = init ran, 2 = init reported failure */
int wrath_editor_level = -1;   /* level the current init belongs to */
int wrath_editor_inits;        /* how many times we have (re-)inited */

/* Register all eight, then init. NEVER init without registering first:
 * edmainInit calls edmainClose when already inited, and edmainClose EMPTIES the
 * module list (D_0062F8CC = 0) before edmainInit walks it -- so a bare re-init
 * yields an editor with zero modules. edmainRegister closes too, so the first
 * of these eight tears the old session down and the rest rebuild the list. */
static void wrath_editor_boot(int activate)
{
    edmainRegister(edptldesc);
    edmainRegister(edobjdesc);
    edmainRegister(edanimdesc);
    edmainRegister(edgradesc);
    edmainRegister(edcrtdesc);
    edmainRegister(edwmpdesc);
    edmainRegister(edaidesc);
    edmainRegister(edlightdesc);

    /* cb = 0: the menu constructs fine with a NULL callback -- just do not pick
     * that entry. level = 0: edmainReadLevelFile bails on an empty name, which
     * only skips loading previously-SAVED editor data. It does not stop the
     * modules editing live game state (the AI Editor adds and edits enemies
     * with this NULL -- verified in-game). */
    if (edmainInit(0, 0) == 0) {
        wrath_editor_booted = 2;
        return;
    }
    wrath_editor_booted = 1;
    wrath_editor_inits++;
    /* Only open the editor when asked. A re-init is bookkeeping -- rebuilding
     * the UI materials against the new level -- and must not shove the menu in
     * the player's face on every level load. Whatever editor_active was before
     * the re-init is what it stays. */
    if (activate)
        editor_active = 1;
}

/* One frame's worth of debug lines.
 *
 * This was 512 on the theory that a frame emitting more was a runaway caller
 * rather than a view worth seeing. That held for DrawCross on a boss and was
 * badly wrong for the editor: with all eight modules registered it wants ~800+
 * lines/frame (light radii, AI waypoints, object cursors, ...) and g_line_dropped
 * hit 904k inside a couple of minutes -- roughly 40% of the editor's rendering
 * silently thrown away. 8192 * 28B = 224KB, which is nothing against the heap
 * hole the blob is carved from.
 *
 * Size it from g_line_peak, not by guessing: peak is the true per-frame demand
 * including lines that did not fit. If peak ever approaches WRATH_MAX_LINES,
 * raise this. */
#define WRATH_MAX_LINES 8192

struct wrath_line {
    wrath_vec a;
    wrath_vec b;
    int col;
};

static struct wrath_line g_lines[WRATH_MAX_LINES];
static int g_line_count;

/* Diagnostics, all readable over PINE (`wrathsdk.ctl status`).
 *
 * g_line_want counts every line ATTEMPTED this frame, dropped ones included, so
 * g_line_peak is the real high-water demand. g_line_count alone cannot show it:
 * once the buffer saturates it just pins at WRATH_MAX_LINES and hides how much
 * is being lost. */
static int g_line_want;
int g_line_peak;
int g_line_dropped;    /* cumulative; write 0 over PINE to re-zero */

/* Replaces UpdateLevel (void, called once per frame from main at 0x001C9C44).
 * The editor boots from here rather than the DrawLevel hook because edmainInit
 * does file I/O (edmainReadLevelFile) and allocates a camera (NuCameraCreate) --
 * update-phase work that has no business running mid-render. Unhooked by anyone
 * else; the coop mod owns DoInput/DrawCreatures/PickupItem/DrawMenu/CrateOff/
 * GotoCheckpoint/ProcessFireBoss/ProcessJeepBalloon, none of which is this. */
void wrath_update_level(void)
{
    orig_UpdateLevel();

    /* Re-assert every frame rather than once: only NuFileInit/NuFileInitEx write
     * this, but a re-init would silently drop us back to the disc and the next
     * level would quietly load stale data -- a confusing failure to chase. One
     * store a frame is free. */
    if (wrath_host)
        NU_FILE_DEVICE = NU_FILE_DEV_HOST;

    if (!wrath_editor_arm)
        return;

    /* Re-init on every level change, not just once.
     *
     * edmainInit -> eduiInit builds the editor UI's materials against whatever
     * is loaded RIGHT NOW. A level load reallocates that memory underneath
     * them, so an editor booted in the hub renders a broken font and no
     * selection highlight once you are in a level -- the highlight is a
     * textured rect and dies first. Re-initing rebuilds them against the level
     * you are actually in.
     *
     * Runs on the first UpdateLevel of the new level, i.e. after the load has
     * finished -- exactly when the new material state exists. */
    if (Level == wrath_editor_level)
        return;

    /* Open it on the first arm only. After that a level change re-inits
     * silently and keeps whatever state you left it in: open if you were
     * editing across a warp, closed if you had exited. */
    wrath_editor_boot(wrath_editor_level < 0 || editor_active);
    wrath_editor_level = Level;
}

/* Supplants NuRndrLine3dDbg (0x0012FDF0). Callable from ANY phase: it only
 * writes to our own array and never touches the renderer. */
void wrath_line3d_dbg(int colour, float x1, float y1, float z1,
                      float x2, float y2, float z2)
{
    struct wrath_line *l;

    g_line_want++;
    if (g_line_count >= WRATH_MAX_LINES) {
        g_line_dropped++;
        return;
    }
    l = &g_lines[g_line_count++];
    l->a.x = x1;
    l->a.y = y1;
    l->a.z = z1;
    l->b.x = x2;
    l->b.y = y2;
    l->b.z = z2;
    l->col = colour;
}

/* Replaces DrawLevel (void, called once per frame from main at 0x001CA430,
 * late in the 3D pass with the world camera live -- and hooked by nobody else;
 * the coop mod owns DoInput/DrawCreatures/PickupItem/DrawMenu/CrateOff/
 * GotoCheckpoint/ProcessFireBoss/ProcessJeepBalloon, none of which is this).
 *
 * Drain after the original so the lines land on top of the frame's geometry.
 * mtx = 0 means world space: NuRndrLine3d resolves the current camera itself
 * and clip-tests both endpoints, so off-screen lines cost nothing.
 *
 * The queue is emptied every frame whether or not anything drew, so a caller
 * that stops emitting cannot leave stale lines on screen. Draw-phase callers
 * running after this point flush one frame late -- invisible in motion. */
void wrath_draw_level(void)
{
    struct wrath_rndrvtx line[2];
    int i;

    orig_DrawLevel();

    for (i = 0; i < g_line_count; i++) {
        line[0].pos = g_lines[i].a;
        line[0].col = g_lines[i].col;
        line[1].pos = g_lines[i].b;
        line[1].col = g_lines[i].col;
        NuRndrLine3d(line, 0, 0);
    }
    g_line_count = 0;
    if (g_line_want > g_line_peak)
        g_line_peak = g_line_want;
    g_line_want = 0;

    /* PLAYERCOUNT == 0 in the front-end menus, where `player` is not valid --
     * DrawFireBossDebugStuff reads it. */
    if (PLAYERCOUNT == 0)
        return;
    if (wrath_show_fireboss)
        DrawFireBossDebugStuff();
    if (wrath_show_jeep)
        DrawJeepDebugStuff();
    if (wrath_show_glider)
        DrawGliderDebugStuff();
    if (wrath_show_zoffa)
        DrawZoffaDebugStuff();
}
