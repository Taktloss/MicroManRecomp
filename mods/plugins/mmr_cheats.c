/* Micro Maniacs (PAL, SLES-01921) cheats as trusted static plugins.
 *
 * Package: mods/preloaded/packages/mmr.cheats. Each feature is a guest-VBlank
 * write, the same effect as the corresponding Action Replay constant-write
 * code. Both addresses lie in the EXE's resident data and were verified on the
 * running game (NOTES.md, Phase 3 cheats):
 *   0x80065A22 u8  player 1 Challenge lives (30065A22 nn)
 *   0x80065DA0 u16 secret-options unlock mask (80065DA0 FFFF)
 *
 * The writes depend only on the committed mod plan, never on host state, so
 * they are deterministic under rollback netplay; the plugins are not
 * host_only, so the host's plan decides for every peer. */

#include <stdint.h>
#include <stdlib.h>

#include "mod_plugins.h"

#define MMR_LIVES_ADDR   0x80065A22u
#define MMR_SECRET_ADDR  0x80065DA0u
#define MMR_SECRET_ALL   0xFFFFu
#define MMR_LIVES_DEFAULT 9u

static uint8_t g_lives = MMR_LIVES_DEFAULT;

static void lives_activate(void)
{
    char value[16];
    g_lives = MMR_LIVES_DEFAULT;
    if (psx_mod_current_option_value("count", value, sizeof value)) {
        long n = strtol(value, NULL, 10);
        if (n >= 1 && n <= 99) g_lives = (uint8_t)n;
    }
}

static void lives_vblank(void)
{
    if (!psx_mod_game_started()) return;
    psx_mod_write_byte(MMR_LIVES_ADDR, g_lives);
}

static void secret_vblank(void)
{
    if (!psx_mod_game_started()) return;
    psx_mod_write_half(MMR_SECRET_ADDR, MMR_SECRET_ALL);
}

PSX_MOD_CONSTRUCTOR(mmr_cheats_register)
{
    psx_mod_register_activation_plugin("mmr.cheats.infinite-lives", lives_activate);
    psx_mod_register_vblank_plugin("mmr.cheats.infinite-lives", lives_vblank);
    psx_mod_register_vblank_plugin("mmr.cheats.secret-options", secret_vblank);
}
