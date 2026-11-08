# NOTES.md – Projekt-Logbuch Micro Maniacs Recomp

## Status
| Phase | Thema                       | Status       | Datum |
|-------|-----------------------------|--------------|-------|
| 0     | Vorbereitung / Docs lesen   | erledigt     | 2026-11-05 |
| 1     | Framework bauen             | erledigt     | 2026-11-05 |
| 2     | Spielprojekt anlegen        | erledigt     | 2026-11-05 |
| 3     | Lauffähigkeit               | erledigt     | 2026-11-05 |
| 4     | Netplay 2 Spieler           | in Arbeit    | 2026-11-06 |
| 5     | Multitap / mehr Spieler     | in Arbeit    | 2026-11-06 |
| 6     | Grafikverbesserungen        | in Arbeit    | 2026-11-06 |
| 7     | Später / optional           | offen        |       |

Status-Werte: offen · in Arbeit · blockiert · erledigt

## Aktueller Stand
<!-- 2–5 Sätze: Wo stehen wir gerade, was ist der nächste Schritt? -->
Phase 4 lokal fertig (2 Spieler), offen: Lobby/echtes Online (Nutzer).
Phase 5 lokal fertig: Netplay mit 3–5 Spielern ohne Desync, auf 5 Spieler
begrenzt (zwei Multitaps → Intro-Hänger im Framework-CD-Datenpfad), Tap-Pads
digital. Phase 6 lokal fertig: Grafikoptionen über eingebaute Mods,
Widescreen-Mod mmr.widescreen (Host-Wahl gilt im Netplay für alle).
FSR 1 (Framework, Image Filtering → Upscaling) und Cull-Fix für die
fehlenden Polygone am linken Rand gebaut, Framework-Änderungen noch nicht
committet. Wartet auf OK.

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
- 2026-11-08 (Nutzer) – Öffentliches Repo github.com/Taktloss/MicroManRecomp;
  Framework über Fork Taktloss/psxrecomp (Branch micro-maniacs), Submodul-URL
  zeigt dorthin. Release = Setup-Host-Kit ohne Spielcode (erzeugt beim ersten
  Start aus der eigenen Disc), keine fertige EXE.
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
  - Ursache (2026-11-06, gemessen, OFFLINE REPRODUZIERBAR):
    1. Auslöser ist die Pad-Bestückung, nicht Netplay. Offline-Standard:
       Multitap an Port 1 + P1 digital, Port 2 leer. Netplay (2 Spieler):
       kein Multitap, 2 direkte Pads, als konfigurierbare DualShock im
       Digitalmodus (psx_netplay.c setzt config_capable=1). Das Spiel probt
       ab SIO-Byte ~10360 per 0x43/0x47/0x4C auf DualShock → andere
       SIO-Laufzeit → ab Frame 383 +11 376 Takte, Logo-Ende 6 Frames früher.
       (Frühere Aussage "Pads identisch" war falsch – Vergleich endete bei
       Byte 9108, knapp vor der Abweichung.)
    2. Offline-Nachstellung mit lokalem Messpatch (PSX_NPX=64: digitale Pads
       konfigurierbar; settings.toml p2_device=keyboard, multitap=false):
       Takte EXAKT wie Netplay (CD-Befehl 256 @259357412, Setloc INTRO
       @429384378) und Intro hängt genauso. Ohne Patch (2 SCPH-1080-Pads):
       Setloc @429384787, Intro läuft. Analog-Modus: Intro läuft.
    3. Netplay-Sonderpfade (Present-Verschiebung gpu.c, Interrupt-Halte-
       regeln interrupts.c) per Messschalter einzeln abgeschaltet: kein
       Einfluss auf die Takte. Mods, Renderer, STR-Modus: kein Einfluss.
    4. Mechanismus: zwischen Setmode(0xC0) und ReadS trifft noch ein
       Logo-Sektor ein; sein INT1 wird nach dem INT3-Ack zugestellt, ReadS
       "queued". Das Spiel verarbeitet den veralteten INT1 und lässt BFRD
       (Request 0x80) gesetzt. Beim ersten INTRO-Sektor zieht ein laufender
       CD-DMA sofort 1312 Bytes, dann setzt der Handler BFRD 0→0x80
       (Position 0) und der DMA liest den Sektoranfang erneut → Bild kaputt
       → MDEC 319/320 Blöcke → Spiel wartet ewig.
    5. DuckStation (src/core/cdrom.cpp) verhält sich in genau dieser Lage
       gleich (INT1 nach Ack zugestellt, Befehl wartet; BFRD löschen setzt
       Position 0). Das CD-Modell ist also nicht offensichtlich falsch.
       Offene Frage: Läuft das Intro auf echter Hardware/DuckStation mit 2
       DualShock (Analog aus)? Wenn ja, stimmt irgendwo das Timing (SIO oder
       CD) nicht mit echter Hardware überein → Oracle-Vergleich nötig
       (PCSX-Redux nicht installiert; DuckStation als Flatpak vorhanden).
  - Oracle-Abgleich PCSX-Redux (2026-11-06; ~/.local/opt/pcsx-redux, dev
    build de62106d, OpenBIOS, Interpreter, 2× DualShock digital, Lua-Logger
    für CD-Register): Intro läuft. Gleicher Befehlsablauf wie bei uns, aber:
    1. Seek-Zeit: ReadS von LBA ~185655 (Logo-Ende) nach 527 (INTRO)
       dauert in Redux ~19,4 Mio Takte (~0,57 s), bei uns ~0,45 Mio
       (13 ms). Das Framework hat für ReadN/ReadS keinen distanzabhängigen
       Seek (initial_read_delay_cycles = feste 1 Sektorperiode bei 1x).
       DuckStation (GetTicksForSeek) rechnet für diesen Sled-Seek ~0,67 s.
    2. Befehlsantwort: Redux/DuckStation erzeugen INT3 erst nach Laufwerks-
       Latenz (DuckStation 25 000 Takte mit Disc; psx-spx: GetStat Ø 0xC4E1
       = 50 401, min 0x4A73 = 19 059). Ein in der Zeit eintreffender Sektor
       (INT1) kommt zuerst, der Befehl wartet. Framework: Befehl wird sofort
       ausgeführt, irq_flag sofort gesetzt, nur INTC-Zustellung +5000
       (CDROM_IRQ_PRESENT_DELAY). Dadurch kippt die Reihenfolge.
  - Versuchspatches (nicht committet, Framework zurückgesetzt):
    Scratchpad cd_timing_experiment.patch
    * PSX_NPX_ACK (nur INTC-Verzögerung länger): hängt weiter.
    * PSX_NPX_CMDLAT=20000 (Befehl verzögert ausgeführt): offline ok,
      Netplay hängt weiter (Rennen nur verschoben).
    * PSX_NPX_SEEK=1 (DuckStation-Seek-Modell für ReadN/ReadS nach
      Setloc): Netplay UND offline Intro läuft, kein Desync, Rennen tritt
      nicht mehr auf (0 overwrites).
  - Fix (2026-11-06, vom Nutzer freigegeben, psxrecomp noch nicht
    committet): cdrom.c read_seek_cycles() nach Beetle CalcSeekTime
    (Distanz * clk/324000, min 20000; >= 2250 Sektoren +0,3 s; Motor aus
    +1 s ab LBA 0), auf ReadN/ReadS nach Setloc addiert, über
    apply_read_speed (CD-Speed-Mods wirken weiter). Standardverhalten, kein
    game.toml-Schalter (psxrecomp/CLAUDE.md Rule -1). Zähler
    read_seek_count/read_seek_cycles in cdrom_timing.
    Geprüft: Netplay und offline Intro läuft, Netplay durch Sprachwahl,
    Memory Card, Hauptmenü, Figur, Strecke bis Rennvorspann, desync 0.
  - Offene Fragen: Beetle-Zufallsanteil (0–25000) bewusst weggelassen
    (Determinismus). Beetle-Strafzeit beim Fortsetzen aus Pause und
    SeekL/SeekP (weiter seek_complete_delay_cycles) noch nicht nach Beetle.
    Befehlsantwort-Latenz (2) bleibt offen, falls weitere Probleme auftreten.
  - Netplay-Tests nach dem Fix (2026-11-06, lokal 127.0.0.1, delay-Modus):
    Zweikampf-Rennen komplett, 2 Rennen nacheinander, gemischte Renderer
    (OpenGL/Software), Verbindungsabbruch: alles ohne Desync bzw. sauber.
    Offen: Lobby über den Launcher (manuell), echtes Online, Erzwingen
    logikrelevanter Einstellungen, Cheats im Netplay (TESTS G).
  - game.toml [netplay] content_negotiation = true (2026-11-06): nötig, damit
    Cheats im Netplay gelten (sonst Vanilla-Sitzung). Direktverbindung:
    gleicher Plan → Cheats auf beiden Peers aktiv, desync 0; ungleicher Plan
    → "LAN content differs between peers", Start verweigert, nach 20 s
    zurück zur Lobby. Lobby veröffentlicht den Host-Plan (ungeprüft).
  - Boot-Plan-Lücke behoben (2026-11-06, freigegeben; psxrecomp 5fbfb0ac
    auf lokalem Branch cdrom-read-seek-timing): Lobby-Host veröffentlicht
    bios_hle/fast_boot, Gast übernimmt sie nur für die Sitzung; jeder Peer
    faltet seinen tatsächlichen Boot-Plan (call_hle, boot_skip) ins
    Content-Gate; Delay-Sync vergleicht Core-Digests pro Tick, 3 abweichende
    Ticks beenden die Sitzung ("netplay_state_desync"). netplay_status:
    core_desync, core_mismatch_ticks, core_desync_tick.
    Geprüft: Gast PSX_BIOS_HLE=0 → content_mismatch, kein Frame läuft;
    normale Sitzung Intro+Menüs+Rennen ~10 000 Ticks, 0 Abweichungen;
    write_ram nur beim Gast im Rennen → beide Seiten melden Desync bei sim
    5623 und gehen zur Lobby. Unit-Tests exit_reason/content_gate/
    bios_hle_plan ok. Lobby-Übernahme selbst ungeprüft (braucht Lobby).
  - Bekannt (Framework-Design): Während 24-Bit-Video mit aktivem MDEC
    werden keine Core-Digests erzeugt; auch das Sprachmenü (Video-
    Hintergrund) ist damit ohne Desync-Erkennung.
  - Hinweis Debug-Befehl press/set_input: Tasten active-low (0xFFFF = nichts
    gedrückt, z. B. Kreuz = 0xBFFF).
  - Weitere Videos auf der Disc: SHOWREEL.STR, 13× Figuren-STR – können
    dasselbe Rennen treffen, ungeprüft.

### Phase 5 – Multitap / mehr Spieler (2026-11-06)
- Spielerzahl geklärt: Hauptmenü zeigt "Zweikampf 1 – 8 Spieler",
  "Teamzweikampf 3 – 8 Spieler" → PAL-Version kann 8 Spieler (zwei Multitaps).
- Beitreten: In der Figurenauswahl muss jeder weitere Spieler Start drücken
  (Hinweis "START – VERBINDEN SIE SPIEL"), sonst teilen sich die Pads eine
  Figur ("Gemeinsame Controller") und die übrigen Plätze fährt der Computer.
  Start zu früh (während des Ladens der Auswahl) bricht zurück ins Hauptmenü.
  Vor dem Rennen ("START DRÜCKEN") muss jeder Spieler Start drücken.
- Framework: LAN-Sitzung mit 3+ Plätzen = Host als Hub (Slot 0 ohne
  --net-peer, Gäste wählen 127.0.0.1:7777, PSX_NET_SLOTS=N auf allen),
  architecture=host_relay. Multitap wird erzwungen (Spiel sieht ID 0x80 an
  Port 1, Pads A–D, Anzeige "1-A".."1-D").
- 3 Spieler lokal: Rennen Mice N Easy komplett, ~31 900 Ticks, desync 0,
  core_mismatch 0 auf allen drei Peers.
- BUG gefunden: Mit multitap_analog = true (Launcher-Standard, steht in
  settings.toml und wird in die Build-game.toml geschrieben) meldet Pad 1-A
  am Multitap dauerhaft 0xF3 (Konfigurationsmodus). Das Spiel schickt jedes
  Frame 43 01 / 45 / 4C an Slot A, verlässt den Modus nie → Tasten von
  Spieler 1 kommen nicht an (Menü reagiert nur auf 1-B/1-C). Mit
  multitap_analog = false läuft alles. Ursache (Spiel vs. Framework-
  Emulation von DualShock am Multitap) noch offen → gegen Beetle prüfen.
  Für den Test settings.toml + build-release/game.toml vorübergehend auf
  false (Sicherung im Scratchpad).
- 4 Spieler lokal: Rennen Mice N Easy komplett (Punkte, Sieg, Ergebnis),
  ~42 000 Ticks, desync 0, core_mismatch 0 auf allen vier Peers.
- 8 Spieler lokal (zwei Multitaps): Intro hängt wieder (schwarzes Bild, CD
  steht nach 3840 Sektoren, MDEC 1 Decode/319 Blöcke, bei Frame 748 ReadS
  nach Setmode(0xC0) "queued") – gleiches Muster wie der alte Intro-Hänger.
  Der Seek-Fix hat das Zeitfenster nur verschoben; die längere Pad-Abfrage
  mit zwei Taps trifft es erneut. Echte Ursache vermutlich die fehlende
  Befehlsantwort-Latenz: Beetle (cdc.cpp) führt Befehle erst nach
  10500+rand(0..3000)+1815 Takten aus, verschiebt sie weiter, solange ein
  Sektor-IRQ noch nicht quittiert ist (CDCReadyReceiveCounter), und stellt
  asynchrone IRQs (AsyncIRQPending) erst nach der Quittierung zu. Das
  Framework führt Befehle sofort aus. Nutzer: 8 Spieler nicht nötig.
  Offen: Framework-Fix nach Beetle (braucht Zustimmung); dieselbe Lücke
  kann auch mit weniger Spielern unter anderen Pad-/Zeitbedingungen
  auftreten.
- Nachstellung (2026-11-06): Hänger auch mit 5 Spielern (beide Taps aktiv).
  Vergleich Frame 740–748: 4 Spieler → Setloc, Setmode(C0), ReadS sofort
  ausgeführt, Intro läuft (167 Decodes). 5 Spieler → zwischen Setmode und
  ReadS kommt noch ein Logo-Sektor (INT1), ReadS "queued", Intro hängt.
- 8 Spieler zusätzlich: einmal Stillstand aller Peers bei Tick 877
  (stall=wait_confirm), dann "other player left" (Logs scratchpad/logs8).
  Nicht weiter verfolgt (8 Spieler nicht nötig).
- Analyse gegen Beetle cdc.cpp: Befehlsverzögerung allein erklärt es nicht
  (mit Beetle-Latenz wäre das Fenster eher größer). Unterschied im
  Datenpfad: Beetle kopiert den Sektor NUR beim Schreiben von BFRD (0x80)
  in den DMA-Puffer (und nur, wenn der leer ist); neue Sektoren landen im
  Sektorpuffer und sind erst nach dem nächsten 0x80 lesbar. Das Framework
  lädt bei gesetztem BFRD neue Sektoren automatisch nach (load_data_fifo(0)
  bei Zustellung und DMA-Fortsetzung) → laufender DMA zieht 1312 Bytes vom
  ersten INTRO-Sektor. Versuch "kein Auto-Nachladen" (nur Testschalter):
  Spiel hängt schon vor dem Intro (Framework-DMA-Modell setzt das
  Nachladen voraus). Echte Lösung = CD-Datenpfad (BFRD/DMA) nach Beetle
  umbauen – größerer Framework-Eingriff, betrifft alle Spiele.
- Entscheidung Nutzer (2026-11-06): auf 5 Spieler begrenzen (players = 5 in
  game.toml, MAX_PLAYERS 5 in CMakeLists.txt). Unter 5 gibt es im Framework
  keinen Multitap (sio.c: PSX_MAX_PLAYERS >= 5), mit 8 werden zwei Taps
  benutzt (Hänger). Mit 5: ein Tap an Port 1 (1-A..1-D) + Pad an Port 2.
  Geprüft: Intro läuft mit 5 Spielern (451 Decodes, ReadS sofort), Rennen
  mit 5 Spielern ~5 min, desync 0, core_mismatch 0. CD-Datenpfad-Umbau
  bleibt offen (Framework).
- multitap_analog-Ursache (2026-11-06): Mitschnitt mit DualShock an Tap:
  Spiel schickt direkt an Slot A 43 01, 45, 4C, 47 und nie 43 00; im
  Sammelmodus nur 42 pro Slot (kein versteckter Exit). An direkten Ports
  läuft die volle Folge (43 01, 45, 4C 01, 47, 4C 00, 46 00, 46 01, …) und
  endet. Abweichungen der Framework-Antworten (sio.c, modern SM) von Beetle
  (frontio.c DualShock):
  * 45: Framework 03,<analog>,01,02,01,00 – Beetle 01,02,<analog>,02,01,00
  * 46 Index 1: Framework wie Index 0 (00,01,02,00,0A) – Beetle 00,01,01,01,14
  * 47 Index ≠ 0: Framework 00,02,00,01,00 – Beetle nur Nullen
  * 4C Index 1: Framework 04 – Beetle 07
  Ob diese Abweichungen den Abbruch am Tap auslösen, ist noch nicht
  bewiesen (Fix + Test nötig, Framework → Zustimmung).
- Fix (2026-11-06, Nutzer freigegeben, psxrecomp sio.c, nicht committet):
  Antworten 45/46/47/4C nach Beetle (45-Layout, Index-abhängige Tails für
  46/47/4C im modernen Zustandsautomaten; Legacy-Pfad unverändert). Neuer
  Unit-Test sio_dualshock_config_replies_test ok; rumble/jogcon/negcon-
  Tests ok. 2 Spieler direkt: Intro 457 Decodes, Menü, desync 0.
  Ergebnis am Tap: Antworten jetzt Beetle-gleich, Schleife bleibt. Voller
  Mitschnitt: Erste Einrichtung (Slot A über Tap-Methode 1) läuft komplett
  inkl. 43 00. Sobald der Tap im Sammelmodus ist, startet das Spiel eine
  zweite Einrichtung: 43 01, 45, 4C 00, 47 01 je einmal pro Frame, nie
  43 00. Ursache offen (Spielcode/libpad oder Tap-Verhalten) → bräuchte
  Disassembly oder Oracle mit Multitap+DualShock (DuckStation/Redux).
- Entscheidung: game.toml [controller] multitap_analog = false (Tap-Pads
  digital). Achtung: settings.toml des Launchers überschreibt game.toml;
  steht dort true, ist Pad 1 am Tap wieder tot. Netplay übernimmt den
  Host-Wert (match_caps.multitap_analog).

### Phase 6 – Grafik (2026-11-06)
- Vorhandene Optionen (Framework, eingebaute Mods, alle "host_only" = pro
  Spieler, nicht im Netplay-Abgleich): Internal Resolution (Native…8K,
  Mod-Standard 4x), Image Filtering (Glättung, Texturfilter Nearest/Bilinear/
  xBR, FMV-Filter, Dithering Aus/Native/Fein), CRT-Filter, Post Processing
  (FXAA, Bloom, .pfx-Shader), PGXP (perspektivische Texturen).
- Vergleich vom selben Rennen-Savestate (Mice N Easy, OpenGL): Dithering aus
  wirkt (238k Pixel leicht verändert, Muster weg), perspective_texturing
  ändert nur große Bodenflächen leicht, Bilinear/xBR wirken. Keine Artefakte.
- Standardwerte: game.toml bleibt originalgetreu (4:3, keine Filter, kein
  PGXP); Verbesserungen wählt der Spieler über die Mods. geometry_correction
  bleibt aus/verborgen (Framework: bekannt kaputt, Risse).
- FSR 1 (mit Zustimmung generisch ins Framework, psxrecomp uncommittet):
  EASU als Present-Modus (nur wenn das Bild kleiner als die Ausgabe ist),
  RCAS als erste Post-Stufe. Mod Image Filtering: Option "upscaling"
  (default/off/fsr1) und "fsr_sharpness" (0–100, Std. 90); game.toml
  `[video] upscaler`/`fsr_sharpness`; Debug-Befehl `upscaler`. Pro Spieler
  (host_only, rein visuell). Wirkt nur bei niedriger Internal Resolution
  (1x/2x); bei 4x greift es nicht (nichts hochzuskalieren). Verstärkt das
  PS1-Dithermuster → mit Dithering Aus empfohlen. GL-Test gl_fsr 21/21.
- Widescreen: game.toml `[video] aspect_ratio` allein wirkt nicht mehr
  (Widescreen gehört seit S1 dem Mod-Katalog). Neues Paket mmr.widescreen
  (mods/preloaded/packages, Plugin mods/plugins/mmr_widescreen.c): setzt bei
  Aktivierung 16:9 oder "An Fenster anpassen"; Framework rendert native-wide.
  game.toml `[widescreen] gte_game_mode = true` nötig, sonst wird das Rennen
  als 2D-Szene erkannt und 4:3 mit Rändern gezeigt (game_mode 0).
- Fehlende Polygone am Rand (gelöst): Die Boden-/Strecken-Renderer im EXE
  (0x8001b6xx–0x8001eexx, 8 Funktionen) lesen die SXY-Wörter per mfc2,
  verknüpfen sie mit AND und verwerfen das Polygon bei `and mask 0xF3008600;
  bnez` (alle X < 0, alle X ≥ 512 oder alle Y außerhalb). Neue generische
  Cull-Art im Framework `[widescreen.cull] sxy_read_sites`: ein gelesenes X
  im aufgedeckten Band [-m,0) gilt als 0, in [W,W+m) als W-1; sonst
  unverändert, bei 4:3 (m=0) Identität. game.toml: 60 mfc2-Stellen,
  clip_edge_width = 512. Geprüft: Akkumulator nach dem Branch tot, keine
  weiteren Fold-Stellen im EXE, keine in den Overlays (Maske fehlt dort).
  A/B im selben Bild (Debug ws_margin 0 vs. 85): schwarzer Anteil im linken
  Rand 22,8 % → 2,2 %. Bei 4:3 ohne Mod x_margin 0.
  Nur Zeichnen betroffen (Polygonliste), Spiellogik unverändert; im Netplay
  gilt die Seitenverhältnis-Wahl des Hosts für alle → deterministisch.
  Alte Savestates passen nach dem Rebuild nicht mehr (Ausführungsprofil).
- Vs-Ausscheiden: Zurückfallender Spieler schied auf Mice N Easy am unteren
  Rand innerhalb des 4:3-Bereichs aus – Ausscheidegrenze bleibt die des
  Originals (Logik unverändert). Im breiten Bild sieht man seitlich mehr
  Strecke → betrifft den Bildausschnitt im Vs → Netplay-Grundsatz: Plugin
  ohne netplay-Klasse, Host-Plan gilt für alle.
- Netplay-Tests (2 Peers lokal, zweiter Peer aus build-release/peerB mit
  eigener mods/state.toml):
  - "local_view": mit content_negotiation = true trotzdem im Plan-Abgleich →
    content_mismatch (Framework zählt local_view-Features im Direkt-Modus mit).
  - "host_only" (Test): Host 16:9, Gast 4:3, ganzes Vs-Rennen desync 0 –
    technisch möglich, verletzt aber den Grundsatz → verworfen.
  - Endgültig (ohne Klasse): beide 16:9, ganzes Vs-Rennen bis Ergebnis,
    desync 0; Host 16:9 + Gast 4:3 → Sitzung startet nicht (content_mismatch).
- HUD bleibt an den 4:3-Positionen (nicht an die breiten Ecken verschoben).

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
- ~~Scheiden Spieler im Vs-Modus am Bildrand aus?~~ Ja, an der
  Original-4:3-Grenze (auch mit Widescreen, Logik unverändert). Nur auf
  Mice N Easy geprüft (Ausscheiden unten); seitliches Ausscheiden auf
  anderen Strecken ungeprüft.
- ~~Widescreen: fehlende Polygone am linken Rand~~ → gelöst über
  sxy_read_sites (Phase 6). Netplay mit dem neuen Cull geprüft
  (2026-11-06: Vs, beide 16:9, x_margin 85 auf beiden, ~4 min desync 0,
  core_mismatch 0, Bilder beider Peers gleich); andere Strecken ungeprüft.
- Framework-Werkzeug: wtrace sieht keine Schreibzugriffe aus nativem Code
  (hängt nur am langsamen Speicherpfad). Nicht behoben, Framework-Änderung.
- Framework-Tests: 13 Recompiler-Tests schlagen schon ohne meine Änderungen
  fehl (Upstream-Drift), eigenständiger Runtime-Build bricht an dma.c
  (timespec_get) ab – nicht von mir, nicht angefasst.
- Menüs/Figurenauswahl mit Widescreen nicht systematisch angesehen
  (Ladebild 3D breit, sah ok aus).

## Bekannte Bugs
| Nr. | Beschreibung | Wo/wann | Status |
|-----|--------------|---------|--------|

## Sitzungsprotokoll
<!-- Kurz pro Sitzung, neueste oben. Ältere Einträge gelegentlich
     zusammenfassen, damit die Datei übersichtlich bleibt. -->
- 2026-11-06 – Phase 6 Nachtrag: FSR 1 ins Framework (generisch), neue
  Cull-Art sxy_read_sites + 60 Stellen in game.toml → Rand-Polygone da.
  OpenBIOS-Stempel und docs/TCP_COMMANDS.md neu erzeugt. Wartet auf OK.
- 2026-11-06 – Phase 6: Grafikoptionen geprüft, Widescreen-Mod mmr.widescreen
  gebaut, Vs-Netplay mit Widescreen desync 0. Wartet auf OK.
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
