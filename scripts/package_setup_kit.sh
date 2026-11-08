#!/usr/bin/env bash
# Self-compile kit (setup host, no game code) for Micro Maniacs Recompiled.
# The zip holds the runtime setup host, the framework sources/emitters and
# OpenBIOS; the player's first start generates the game C from their own disc
# and builds it locally. Nothing disc-derived is packaged.
# Usage (repo root, fresh clone with submodules, no generated/):
#   scripts/package_setup_kit.sh [linux-x64]
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "${ROOT}"
ARTIFACT="${1:-linux-x64}"
if [[ -d generated ]]; then
  echo "error: generated/ exists; package the kit from a clean checkout" >&2
  exit 1
fi
psxrecomp/tools/ci/build_emitters.sh --build-dir build-recompiler
cmake -S . -B build-ci -G Ninja -DCMAKE_BUILD_TYPE=Release \
  -DPSX_SETUP_WIZARD=ON -DPSXRECOMP_FORCE_SETUP_HOST=ON -DPSX_SDL_BACKEND=SDL3
cmake --build build-ci
args=()
for f in CMakeLists.txt game.toml game_options.toml VERSION codegen_setup.c \
         codegen_setup.h psx_symbols.h symbols.toml catalog_identity.json \
         disc_probe.json README.md .recomp.json assets/psxrecomp.ico \
         assets/psxrecomp.png; do
  args+=(--project-file "$f")
done
for d in seeds aot; do args+=(--project-dir "$d"); done
MMR_RELEASE_VERSION="$(cat VERSION)" psxrecomp/tools/package_setup_host.sh \
  --build-dir build-ci --artifact "${ARTIFACT}" \
  --zip-prefix MicroManiacsRecomp --exe-name Micro_Maniacs__Recompiled \
  --display-name "Micro Maniacs Recompiled" \
  --recompiler-build build-recompiler --version-env MMR_RELEASE_VERSION \
  --exclude-dev-mods \
  --disc-hint "your legally owned Micro Maniacs (Europe) disc, SLES-01921, CUE/BIN" \
  "${args[@]}"
