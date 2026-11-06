# NOTES.md – Projekt-Logbuch Micro Maniacs Recomp

## Status
| Phase | Thema                       | Status       | Datum |
|-------|-----------------------------|--------------|-------|
| 0     | Vorbereitung / Docs lesen   | erledigt     | 2026-11-05 |
| 1     | Framework bauen             | erledigt     | 2026-11-05 |
| 2     | Spielprojekt anlegen        | erledigt     | 2026-11-05 |
| 3     | Lauffähigkeit               | erledigt     | 2026-11-05 |
| 4     | Netplay 2 Spieler           | in Arbeit    | 2026-11-06 |
| 5     | Multitap / mehr Spieler     | offen        |       |
| 6     | Grafikverbesserungen        | offen        |       |
| 7     | Später / optional           | offen        |       |

Status-Werte: offen · in Arbeit · blockiert · erledigt

## Aktueller Stand
<!-- 2–5 Sätze: Wo stehen wir gerade, was ist der nächste Schritt? -->
Phase 3 erledigt und vom Nutzer abgenommen (2026-11-06). Cheats als Paket
mmr.cheats eingebaut und geprüft. Phase 4 (Netplay 2 Spieler) läuft.

## Entscheidungen
<!-- Format: Datum – Entscheidung – Begründung -->
- 2026-11-05 – Framework-Referenz-Checkout unter ~/Projects/psxrecomp (getrennt
  vom gepinnten Submodul im Spiel-Repo) – zum Testen/Bauen ohne Projekt-Pin.
- 2026-11-05 – Scaffold mit --players 8 – Multitap aktiv erst ab 5 (einfach)
  bzw. 8 (doppelt); vermeidet Neu-Konfiguration in Phase 5.
- 2026-11-05 – Scaffold im Zwischenordner, ohne Generate/Build, dann nach
  ~/Projects/MicroManRecomp verschoben; Generate + rebuild erst am Zielort –
  CMake-Builds enthalten absolute Pfade.
- 2026-11-05 – Kein Retail-BIOS (OpenBIOS), Netplay an, Lobby-Standard
  ws://netplay.retcomm.net:8765, recomp-ui + Setup-Wizard an, keine CI, kein
  GitHub-Repo, keine Boxart (nachholbar).
- 2026-11-05 – Referenzmaterial nach docs/reference/; Video und psx-spx-Kopie
  per .gitignore ausgeschlossen (Fremdmaterial, groß).

- 2026-11-05 (Nutzer): Nichts Urheberrechtliches ins Repo. Ganz generated/
  (Spiel-C + AOT-Overlay-C, aus der Disc abgeleitet) und docs/reference/
  (Cheats u. ä.) sind gitignored – Abweichung vom Scaffold, das generated/
  committen will. Jeder Spieler stellt seine eigene Disc bereit; beim ersten
  Start fragt der Setup-Assistent danach, erzeugt generated/ lokal und baut
  neu. Folge: kein Prebuilt-/CI-Release ohne Disc. Committet werden nur
  Konfiguration/Fakten (game.toml, Seeds, Symbole, aot/overlays.json mit
  Hashes/Offsets/Dateinamen, Mods) und das Framework.
- game.toml `disc = "disc/game.cue"` (Platzhalter, disc/ gitignored); die
  Disc-Wahl des Assistenten landet in den Benutzereinstellungen. Lokal immer
  `--disc <cue>` an generate/rebuild/Spiel übergeben.
- Getestet: frischer Klon ohne generated/ baut den Setup-Host; Launcher zeigt
  "First-run setup", Disc "Needed", Generate & rebuild erst nach Disc-Wahl.

## Erkenntnisse
<!-- Technische Fakten zum Spiel/Framework, die sich bestätigt haben.
     Z.B. RAM-Adressen, Overlay-Dateien, Besonderheiten des Vs-Modus. -->

### Framework (psxrecomp @ d9605697, gelesen 2026-11-05)
- Spiel-Repo-Layout: psxrecomp/ + recomp-ui/ als Root-Submodule (gepinnt per
  Gitlink). Spielcode (game.toml, seeds/, codegen_setup.c, mods/preloaded/) im Root.
- generated/ (Spiel-C) WIRD laut Framework committet (Release-CI baut daraus).
  Disc, BIOS-Dumps, Logs, overlay_captures.json, Memory Cards bleiben gitignored.
- Ablauf: setup_project.sh (--disc) → probe_disc.py füllt game.toml, Seeds,
  catalog_identity.json → psxrecomp_cli.py generate → psxrecomp_cli.py rebuild
  (nicht nur cmake --build, sonst fehlt overlay_toolchain/).
- 3 Ausführungs-Tiers: statisch (EXE+BIOS) → native Overlays (Capture → Compile
  → Cache) → Dirty-RAM-Interpreter als Sicherheitsnetz.
- AOT-Sharding: deklaratives Profil aot/overlays.json ("psxrecomp AOT methods
  v1"), tools/aot_overlay_pipeline.py; mit "static_output" wird ein
  overlays_static.c beim Generate mitgebaut. Erst Lade-Mechanismus der Overlays
  belegen (Datei, Ladeadresse, Transform), keine Captures als Input.
- Oracle: psx-beetle (Beetle PSX, eigener Prozess, TCP 4380) vs psx-runtime
  (TCP 4370), gleiches JSON-Protokoll. Debug-Server nur in RelWithDebInfo/Debug.
- Keine printf/Logfile-Diagnose im Framework-Code (TCP-Debugserver nutzen).
- Release-Build: Standard OpenBIOS; Retail-BIOS optional.
- Netplay: -DPSX_NETPLAY=ON, Rollback Standard (Delay-Sync abschaltbar),
  Lobby-Standard ws://netplay.retcomm.net:8765. Netplay nur bei players >= 2.
  Bei Multi-Track-Disc: [netplay] require_cue, required_tracks, required_disc_fp.
  Jeder Peer braucht eigenes identisches Disc-Image (gleiche Track-Geometrie,
  TOC-Fingerprint) und denselben Release-Stand. BIOS wird beim Start abgestimmt
  (Retail nur wenn alle denselben Dump haben, sonst OpenBIOS).
- Netplay-Grafik: Dual-Raster – SW-Renderer 1x ist die Autorität (Digests,
  Rollback), OpenGL-Auflösung pro Spieler frei. Grafik-Optionen sind seit
  2026-10-24 eingebaute "host_only"-Mods (CRT-Filter, Image Filtering,
  Internal Resolution, Enhanced Audio, Rewind) → automatisch pro Spieler.
- Widescreen in Netplay: "local_view"-Mods; Cull-Margin in der gemeinsamen
  Simulation bleibt 0 (stock), nur die eigene Ansicht ist breit.
- Multitap: SIO0 unterstützt SCPH-1070 bereits – einzeln ab PSX_MAX_PLAYERS >= 5,
  doppelt (8 Pads) ab PSX_MAX_PLAYERS >= 8 (runtime/src/sio.c). Netplay mit
  >2 Sitzen erzwingt Multitap. game.toml: multitap_port.
- Grafik: perspective_texturing ist freigegeben (über PGXP-Precision-Mod);
  geometry_correction ist bekannt kaputt (Risse) und darf NICHT angeboten oder
  als Standard gesetzt werden. supersampling/internal_resolution = Spiel-Baseline,
  Spieler wählt per Internal-Resolution-Mod.

### Spielprojekt (Scaffold 2026-11-05)
- Pins: psxrecomp d9605697, recomp-ui 839a6f9, recomp-net 7a4d330,
  retcomm-rbengine 28a2a47.
- probe_disc: SLES-01921, Boot-EXE SLES_019.21 (763904 Bytes), load 0x80010000,
  entry 0x80050ED0, text 0xBA000, 648 JAL-Seeds, 2 Tracks,
  disc_fp 58b1ce19…; game.toml [netplay] automatisch gefüllt.
- Track 1: MD5 4c99e78b0218cbf75967a9a33daae8e5, CRC32 b63b5ce0.
- Generate: 1523 Funktionen (1208 via jr $ra, 71 per JAL, 5 Pointer-Tabelle,
  235 Alias-Einträge), 36 Shards, 1,92 Mio. Zeilen, Dispatch 1678 Einträge,
  generated/ = 56 MB. Warnungen nur in vendored fmt/toml11 (clang).
- Es gibt kein build.sh mehr (PLAN.md veraltet): Build =
  `python3 psxrecomp/psxrecomp_cli.py rebuild --config game.toml
  --project-root . --build-dir build-release --no-pgo`.
- Emitter-Build im Spiel-Repo: build-recompiler/ (clang, statisch).

### Erster Boot (2026-11-05, Release-Build, OpenGL, OpenBIOS)
- Start: `build-release/Micro_Maniacs__Recompiled --no-launcher --game game.toml
  --disc <cue>`. Release-Build hat keinen TCP-Debugserver (dafür RelWithDebInfo).
- Ablauf ohne Eingabe: Intro-FMV (MDEC) läuft ~3 min, dann Hauptmenü auf
  Deutsch (Herausforderung, Zeitfahren, Zweikampf, Teamzweikampf [ausgegraut],
  Optionen, Berichte, Spiel laden; "1 Spieler"). Kein Absturz in 190 s.
- Log: PAL erkannt (50 Hz), BIOS-Boot HLE (Shell übersprungen), BIOS-Kernel LLE,
  Thread-Scheduler HLE, Internal Resolution 4x, Overlay-Autocompile aktiv (gcc),
  overlay_captures.json in build-release/ (ignoriert).
- GPU laut Log: AMD Radeon RX 9070 XT (radeonsi), Panel 175 Hz – CLAUDE.md
  nennt RX 6800 XT.
- Screenshots: spectacle -b -n -a (KDE Wayland) funktioniert.

### Phase 3 – Tests (2026-11-05)
- Debugserver auch im Release-Build: `PSX_DEBUG_SERVER=1`, Port 4370 (nur
  127.0.0.1). Hilfsskript im Scratchpad (dbg.py): press/seq/shot/ram.
  `press` nimmt das PS1-Tastenwort aktiv-low (Start 0xFFF7, X 0xBFFF).
- `rebuild --cmake-extra=-DCMAKE_BUILD_TYPE=RelWithDebInfo` → build-dev/, aber
  PSX_DEBUG_TOOLS bleibt OFF (CLI setzt es); für Tracing ggf. später klären.
- Start drückt das Intro weg → Sprachauswahl (X = vorgewählt Englisch) →
  Hauptmenü englisch. Ohne Eingabe landete das Spiel auf Deutsch.
- Challenge: Charakterwahl, Streckenwahl (Mice N Easy, Training), Gegner-
  Vorstellung (3D-Modelle), PRESS START, Rennen mit HUD/Zeit/Positionen,
  Race Results, "Training Round Failed", nächstes Laden – alles ohne Absturz.
- Vor AOT: dispatch_native 1,99 Mio. / interp_fallback 0,90 Mio. (~31 %).
- Erster AOT-Versuch: INTRO.BIN (3093) und SCRIPT.BIN (2) UNSUPPORTED im
  Generated-C-Audit – Heuristik lief in Daten. Behoben im Profil mit
  supplemental_entries=false für diese zwei Dateien → 12/12 auditiert.

- AOT eingebunden: CMake `GAME_OVERLAY_STATIC_C` mit absolutem Pfad
  (`${CMAKE_CURRENT_SOURCE_DIR}/generated/...`) und VOR `CODEGEN_SETUP_SOURCES`
  – dahinter wird es als Quelldatei geschluckt (Linkfehler codegen_host_*),
  relativ wird es gegen build-release/ aufgelöst.
- Nach AOT im Vs-Rennen: dispatch_native 205 427 / interp_fallback 700
  (~0,3 %); Dirty-RAM-Interpreter 24 Instruktionen. Log ohne Fehler.
- Vs-Modus: Spieler 2 per Start auf Port 2 ("1-B"), Charakterwahl geteilt,
  Streckenwahl, Gegnervorstellung (beide PRESS START), Fly-by, gemeinsamer
  Bildschirm, "POINT WIN!" – läuft. VS TEAMS ausgegraut (braucht vermutlich
  Multitap/mehr Spieler → Phase 5).
- Memory Card: LOAD GAME zeigt Slot 1-A und Slot 2 "No Micro Maniacs game data
  found". Karten liegen beim normalen Start in <Projekt>/saves/card1.mcd,
  card2.mcd (gitignored), nicht neben der EXE; Save-Eintrag
  "BESLES-01921-MANAICS" (Schreibweise vom Spiel).
  Optionen-OK speichert nicht. Speichern/Laden vom Nutzer manuell bestätigt
  (2026-11-05).
- Ton: vom Nutzer bestätigt (Intro hörbar).

### Cheats (2026-11-06)
- Nutzbar aus docs/reference/Cheats.txt nur die zwei PS1-Codes; Rest ist
  Mega-Drive ("Micro Machines"). Die Tastenkombinationen (Secret Options)
  kennt das Spiel selbst, kein Einbau nötig.
- 30065A22 nn = u8 Leben Spieler 1 (Challenge). Geprüft: Spiel setzt beim
  Challenge-Start 3; 9 geschrieben → nach verlorener Trainingsrunde "LEBEN 8".
- 80065DA0 FFFF = u16 Freischaltmaske Geheimoptionen. Geprüft: vorher leere
  Liste, danach 12 Einträge (Unlock all characters/tracks, Infinite power-ups,
  Motion blur, Slow CPUs, Tanks, Tough guys, Ultimate AI, Aussie mode, Turbo
  vs freeplay, Nanomaniacs, Super slippy), jeweils "Aus".
- Umsetzung: Paket mods/preloaded/packages/mmr.cheats (2 Features, Standard
  aus, Option Lives 1–99, Standard 9) + trusted static plugin
  mods/plugins/mmr_cheats.c (VBlank-Schreibzugriff, erst nach Spielstart;
  per target_sources in CMakeLists.txt). Nicht host_only → im Netplay gilt
  der Plan des Hosts für alle (Netplay-Grundsatz).
- Test mit Plugin: Lives=5 → RAM 5, nach verlorener Runde "LEBEN 5";
  Maske FFFF. Lokal per build-release/mods/state.toml aktiviert und wieder
  zurückgesetzt.
- Secret Options schalten echte Spielmodi frei (z. B. Turbo, Slow CPUs);
  Speichern auf Memory Card behält die Freischaltung vermutlich (ungeprüft).

### Phase 4 – Netplay (2026-11-06)
- Lokaler 2-Peer-Test: zwei Instanzen, `--net-slot 0/1 --net-bind
  127.0.0.1:7777/7778 --net-peer … --net-session-id 4242 --debug-port
  4370/4371`. Verbindung steht, 2 Spieler, Ticks gleich, desync 0. CLI
  startet Delay-Sync; Rollback per `PSX_NET_MODE=rollback`.
- Im Netplay: "mods cleared for netplay (vanilla session)" – Cheats sind im
  LAN/CLI-Netplay also aus (Host-Plan nur online mit content_negotiation).
- **BLOCKER: Intro hängt im Netplay** (Delay und Rollback, beide Peers
  identisch → kein Desync, sondern deterministischer Hänger). Erstes Video
  (24-Bit) läuft, ab dem zweiten schwarz (Nutzer bestätigt). Messung:
  - offline: MDEC dekodiert laufend (1069 Decodes), gp0_copy steigt stetig,
    zweites Video ab LBA ~975, Modus 0xC0.
  - netplay: ab Frame ~639 keine GPU-Befehle mehr; mdec_decode_count 1;
    DMA ch1 (MDEC→RAM) 20× gestartet, 19× fertig – letzter Start von
    INTRO.BIN 0x80086034 wird nie fertig (MDEC output_size 0); ch0 (RAM→MDEC)
    nur 3×. CD liest weiter dieselben Sektoren im selben Tempo wie offline.
  - Pad-Abfrage normal (beide Ports digitales Pad). Kein CD-Speed-Mod
    offline aktiv. Start im ersten Video überspringt nicht.
  - Ursachenkette (2026-11-06, gemessen; Pad-Hypothese verworfen):
    1. Pads sind NICHT die Ursache: kompletter SIO-Mitschnitt (9108 Bytes)
       offline vs. Netplay identisch. Frühere Aussage "Pad-Bestückung" war
       falsch (beruhte auf nur 64 Trace-Einträgen).
    2. Emulierte Zeit identisch bis Frame 376; ab Frame 383 setzt das Spiel
       seine CD-Befehle im Netplay +11 376 Takte später ab. DMA-Abfolge in
       Frame 360–384 identisch; Software-Renderer offline ändert nichts;
       Offline-Selfcheck (PSX_RB_SELFCHECK=1) und PSX_SAME_THREAD_RESTORE=1
       im Netplay ändern nichts.
    3. Zweite, größere Abweichung: das Logo-Video (CODELOGO.STR) wird im
       Netplay 6 Frames früher beendet (Setloc für INTRO in Frame 634 nach
       LBA 185655, offline Frame 640 ~18 Sektoren später), obwohl der
       Sektorstrom nur 11k Takte versetzt ist. Was das Spiel dort zählt,
       ist noch offen.
    4. Kandidaten für die Zeitverschiebung: netplay-only Pfade im Framework
       (gpu.c: vblank_callback wird an die nächste BB-Kante verschoben statt
       sofort; interrupts.c: Halte-Regeln für MotK-PCs 0x8006CD54/0x800768C8,
       Present-Flush an BB-Kanten). Ohne Framework-Instrumentierung nicht
       weiter eingrenzbar.
    5. Folge der Verschiebung: zwischen Setmode(0xC0) und ReadS trifft noch
       ein Logo-Sektor ein; sein INT1 wird gepuffert und nach dem INT3-Ack
       zugestellt, ReadS wird "queued". Das Spiel verarbeitet den veralteten
       INT1, lässt BFRD (Request-Reg 0x80) gesetzt. Beim ersten INTRO-Sektor
       zieht ein bereits laufender CD-DMA sofort 1312 Bytes, dann setzt der
       INT1-Handler BFRD 0→0x80 (Position zurück auf 0) und der DMA liest den
       Sektoranfang erneut → Videobild kaputt → MDEC 319/320 Blöcke → Hänger.
    6. Abgleich DuckStation (src/core/cdrom.cpp, master): gleiches Verhalten
       in dieser Lage (async INT1 wird nach Ack zugestellt, Befehl wartet;
       BFRD löschen setzt Sektorposition auf 0; ReadS verwirft nur einen
       NOCH NICHT zugestellten INT1). Das CD-Modell ist hier also nicht
       offensichtlich falsch; DuckStation trifft das Zeitfenster nur nicht.
  - Weitere Videos auf der Disc: SHOWREEL.STR, 13× Figuren-STR – können
    dasselbe Rennen treffen, ungeprüft.

### Disc
- Redump-Dump, 2 Tracks: Track 1 MODE2/2352 (Daten), Track 2 AUDIO.
  → für Netplay: require_cue = true, required_tracks = 2.
- PAL → 50 Hz.

### Phase-1-Build (Framework ~/Projects/psxrecomp @ d9605697)
- Fedora-Pakete (dnf) zusätzlich zur README nötig/empfohlen:
  - vorhanden: gcc-c++, cmake, ninja-build, python3, pkgconf, git,
    SDL3-devel 3.4.18 (System-SDL3 wird genutzt), zlib-ng-compat-devel,
    libglvnd-devel (reicht für OpenGL), vulkan-headers, vulkan-loader-devel,
    openssl-devel, libcurl-devel
  - nachinstalliert (2026-11-05): glibc-static (Pflicht für Test-Fixtures: fixture_link.py liest
    libc.a, sonst werden libc-Funktionen zu abort()-Stubs → rc=-6),
    glslc (Vulkan-Shader; ohne → Vulkan nur Software-Stub), ccache (optional)
- Recompiler: 0 Warnungen. Runtime-Binary heißt runtime/build/PSXRecomp
  (Target psx-runtime). 1 Warnung, nur in third_party/stb_image.h (GCC-Fehlalarm).
- regen_bios OpenBIOS: EMIT OK, 651 Funktionen, 4 übersprungen (Datenwörter
  bei 0xBFC095F8/0xBFC22674/0xBFC227D4/0xBFC227E4); Ausgabe byte-identisch
  zum committeten generated/ → reproduzierbar.
- Build schreibt in getrackte Dateien unter overlay_toolchain/ (Staging,
  compiler.txt → /usr/bin/gcc). Nie im Framework committen.
- `PSXRecomp --help` gibt keine Hilfe aus, sondern startet die Runtime.
- ctest (161 registriert): Upstream-CI führt ctest NICHT aus (nur BIOS-Stamps,
  CLI-Release). Fehlschläge:
  - durch glibc-static behoben: dma_gpu_list_cpu_hold,
    syscall_change_thread_self, syscall_closed_thread_context
  - weiter rc=-6, aber Upstream-Drift (Fixture stellt neue Mod-Seams nicht
    bereit): interp_cop0_read_sample (psx_mod_immediate_patch),
    interp_refused_game_text (psx_mod_try_function_replacement)
  - Upstream-Drift (Tests passen nicht zum Quelltext / Harness-Linkfehler):
    bios_module_build (undefined g_psx_func_override_hook), overlay_segment_keys
    + overlay_segment_gate (undefined psx_mod_memory_address),
    bundled_release_layout (package_title_release.sh statt package_game_release.sh),
    mod_owned_display_controls, frame_interpolation_flip_source,
    gl_internal_scale_guards, native_wide_line_batching,
    overlay_candidate_capacity, aot_overlay_discovery
  - compile_overlays_static_tail: Fehler verschwand nach Runtime-Build
    (Codegen-Hash-Header fehlte vorher).
- Upstream-Commit 2026-11-05 "Expose dithering in the Image Filtering mod" –
  Dithering ist abschaltbar (relevant für Phase 6).

### Host
- Nobara, vorhanden: dnf, gcc/g++, clang, cmake 4.3.0, ninja, python 3.14.8,
  pkg-config, git.

### Speicherkarte / RAM-Adressen
| Adresse | Bedeutung | Quelle (Cheat-Code, Debugger, ...) | bestätigt? |
|---------|-----------|------------------------------------|------------|

### Overlays
| Datei auf Disc | Ladeadresse | Wann geladen | nativ kompiliert? |
|----------------|-------------|--------------|-------------------|
| (in EXE, Header-ID 5) | 0x80084B40 | Boot (eingelinkt) | statisch (EXE) |
| MANIACS/BIN/INTRO.BIN (84352) | 0x80084B40 (Slot 0) | Intro/FMV | AOT (35 Fkt., supplemental_entries=false) |
| MANIACS/BIN/FRONTEND.BIN (44288) | 0x80084B40 (Slot 0) | Hauptmenü | AOT |
| MANIACS/BIN/GAME.BIN (247840) | 0x80084B40 (Slot 0) | Rennen | AOT (im Rennen im RAM bestätigt) |
| MANIACS/BIN/CHARSEL.BIN (32024) | 0x80084B40 (Slot 0) | Charakterwahl | AOT |
| MANIACS/BIN/TRACKSEL.BIN (124328) | 0x80084B40 (Slot 0) | Streckenwahl | AOT |
| MANIACS/BIN/TOURNSEL.BIN (88800) | 0x80084B40 (Slot 0) | Turnierwahl | AOT |
| MANIACS/BIN/SCRIPT.BIN (87528) | 0x80084B40 (Slot 0) | Story/Texte | AOT (33 Fkt., supplemental_entries=false) |
| MANIACS/BIN/MEMOIRS.BIN (50456) | 0x80084B40 (Slot 0) | Memoirs | AOT |
| MANIACS/BIN/PRERACE.BIN (29976) | 0x800C1B60 (Slot 1) | vor dem Rennen (mit GAME) | AOT + Komposition GAME+PRERACE |
| MANIACS/BIN/RESULTS.BIN (14488) | 0x800C1B60 (Slot 1) | Ergebnis (mit GAME) | AOT + Komposition GAME+RESULTS |
| MANIACS/BIN/DOB_BIN.MNG, SA_BIN.MNG | ? | ? | ungeklärt (evtl. Code in 0x8016C000/0x801A8000) |

Overlay-Belege (SLES_019.21): Tabelle 0x800689B8, 16-Byte-Records {slot, BIN*,
MNG*, id}; Slot-Basistabelle 0x8006331C = [0x80084B40, 0x800C1B60]; Loader-Code
0x80047444..0x80047A68. Dateien werden komplett inkl. 8-Byte-Header (u32 id, 0)
geladen. Daten-Track-SHA256 ddfdbf86…a62166.

## Offene Fragen / unsichere Annahmen
<!-- Alles, was noch nicht geklärt ist. Erledigtes nach "Erkenntnisse"
     oder "Entscheidungen" verschieben. -->
- ~~Unterstützt die Runtime (SIO0) bereits einen Multitap?~~ → Ja (siehe
  Erkenntnisse). Offen: Wie viele Spieler/Multitaps unterstützt Micro Maniacs
  PAL genau (4 an einem Tap? 8 an zwei?) – am Original in DuckStation prüfen.
- ~~Braucht jeder Netplay-Mitspieler ein eigenes identisches Disc-Image?~~ → Ja.
- setup_project.sh bricht ab, wenn der Zielordner existiert – unser Ordner
  MicroManRecomp/ existiert schon (PLAN/NOTES/…). Scaffold-Ort klären.
- --players beim Setup: Multitap greift erst ab 5 (einzeln) bzw. 8 (doppelt).
  Vorschlag: gleich --players 8, sonst später Neu-Konfiguration nötig.
- Framework-CLAUDE.md enthält Owner-interne Workflows (F:/-Pfade, Ghidra-MCP,
  ChatGPT-Chat, Beads-DB, "BIOS first / kein Spiel bis Phase 5" der v4-Entwicklung).
  Annahme: gilt für Framework-Entwicklung, nicht für unser Spiel-Repo.
- Referenz-Emulator: Framework verlangt Beetle (psx-beetle), CLAUDE.md nennt
  DuckStation. Annahme: Beetle für Oracle-Abgleich, DuckStation/PCSX-Redux für
  visuellen Vergleich und Debugging.
- Cheats.txt: Die "FFxxxx:yyyy"-Codes sind Mega-Drive-Format ("Micro Machines"),
  nicht PS1 – unbrauchbar. Nur 80065DA0 / 30065A22 sind PS1-Format, unbestätigt.
- ~~game.toml enthält den absoluten Disc-Pfad~~ → erledigt 2026-11-05:
  Platzhalter disc/game.cue.
- ~~Git-Historie enthält entfernte Dateien~~ → erledigt 2026-11-06: per
  filter-branch generated/ und docs/reference/ aus allen Commits entfernt,
  reflog/gc bereinigt (Pack 633 KB). Alle Commit-Hashes haben sich geändert.
- Ist Track 1 Redump-verifiziert? (MD5 oben gegen redump.org abgleichen)
- FMV-Bild endet rechts ~25 px (bei 1127 px Fensterbreite) vor dem Rand,
  links nicht → mit DuckStation vergleichen (Display-Bereich/Offset?).
- Sprache: Spiel startete direkt auf Deutsch ohne Sprachauswahl – woher kommt
  die Sprachwahl (BIOS-Spracheinstellung von OpenBIOS? Memory Card?) →
  gegen DuckStation prüfen.
- ~~Ton im Intro nicht von mir prüfbar~~ → erledigt: Nutzer hat Ton bestätigt (2026-11-05).
- PLAN.md nennt einmal "SSLES-01921" – vermutlich Tippfehler für SLES-01921.
- Scheiden Spieler im Vs-Modus am Bildrand aus (relevant für Widescreen)?

## Bekannte Bugs
| Nr. | Beschreibung | Wo/wann | Status |
|-----|--------------|---------|--------|

## Sitzungsprotokoll
<!-- Kurz pro Sitzung, neueste oben. Ältere Einträge gelegentlich
     zusammenfassen, damit die Datei übersichtlich bleibt. -->
- 2026-11-05 – Phase 2 erledigt: Scaffold (--players 8, Netplay, OpenBIOS),
  in Projektordner verschoben, Generate (1523 Funktionen) + rebuild ok,
  Boot bis Hauptmenü per Screenshots belegt. Noch kein Commit.
- 2026-11-05 – Phase 1 erledigt: Pakete nachinstalliert, Runtime mit Vulkan
  neu gebaut, ctest 149/161 (Rest = Upstream-Drift, dokumentiert).
- 2026-11-05 – Phase 1 begonnen: Framework geklont + gebaut (Recompiler,
  OpenBIOS, Runtime), ctest 146/161. Wartet auf dnf install glibc-static glslc ccache.
- 2026-11-05 – Phase 0: psxrecomp-Doku gelesen (CLAUDE, AGENTS, CONTRIBUTING,
  GETTING_STARTED, GAME_PROJECT_SETUP, EXECUTION_MODEL, AOT_SHARDING, BUILDING,
  NETPLAY, WIDESCREEN, ENHANCEMENTS), Games-Tabelle + Websuche: kein
  Micro-Maniacs-Projekt. Nichts am Projekt geändert außer NOTES.md.
