/* Micro Maniacs (PAL, SLES-01921) widescreen as a trusted static plugin.
 *
 * Package: mods/preloaded/packages/mmr.widescreen. Activation only selects the
 * host display aspect; the framework's native-wide path renders the wider
 * view. No guest code is patched (no cull sites), so the game computes the
 * same as at 4:3. The plugin carries no netplay class on purpose: in Vs the
 * visible area is part of the match (NOTES.md, netplay principle), so the
 * host's plan, aspect included, applies to every peer. */

#include <string.h>

#include "mod_plugins.h"

static void widescreen_activate(void)
{
    char value[16] = "16:9";
    psx_mod_current_option_value("aspect", value, sizeof value);
    psx_mod_set_fixed_display_aspect(16, 9);
    if (strcmp(value, "fit") == 0)
        psx_mod_set_adaptive_display_aspect(0, 0);
}

PSX_MOD_CONSTRUCTOR(mmr_widescreen_register)
{
    psx_mod_register_activation_plugin("mmr.widescreen", widescreen_activate);
}
