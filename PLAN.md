# Projekt: [Micro Maniacs] (PS1, [SSLES-01921]) mit PSXRecomp

## Spiel
Micro Maniacs (PS1, Codemasters 2000), meine Disc: [SLES-01921]
Ziel: native Version mit Online-Multiplayer für Freunde.
Beim Setup-Skript Netplay aktivieren (-DPSX_NETPLAY=ON), Lobby-Standard
übernehmen.

## Ziel
Ein spielbares natives Build von Micro Maniacs (PS1, Codemasters 2000) für
Linux (Nobara/Fedora), später auch Windows, auf Basis des Frameworks
PSXRecomp (https://github.com/RetroPortingToolKit/psxrecomp) – mit
Online-Multiplayer für Freunde und sinnvollen Grafikverbesserungen.
Disc-Image (meine eigene Kopie): [~/dumps/Micro Maniacs(Europe)(En,Fr,De,Es,It)/Micro Maniacs (Europe) (En,Fr,De,Es,It).cue]
Projektordner: [~/Projects/MicroManRecomp/]

## Vorbereitung – zuerst lesen, nichts ändern
Lies im psxrecomp-Repo: CLAUDE.md, AGENTS.md, CONTRIBUTING.md,
docs/GETTING_STARTED.md, docs/GAME_PROJECT_SETUP.md,
docs/EXECUTION_MODEL.md, docs/AOT_SHARDING.md und docs/BUILDING.md.
Fasse die Regeln, die für ein neues Spielprojekt gelten, kurz zusammen und
halte dich strikt daran. Prüfe außerdem, ob es für dieses Spiel schon ein
Community-Projekt gibt (README, Games-Tabelle).


## Arbeitsweise
- Arbeite in Phasen. Nach jeder Phase kurzer Statusbericht, dann auf mein
  OK warten, bevor die nächste beginnt.
- Diese Datei (PLAN.md) ist der Plan. Fortschritt, Erkenntnisse, offene
  Fragen und unsichere Annahmen kommen in NOTES.md. Erledigte Phasen dort
  abhaken.
- Zu Beginn jeder Sitzung: PLAN.md und NOTES.md lesen und bei der
  aktuellen Phase weitermachen.
- Kein Raten: Unsichere Annahmen dokumentieren statt stillschweigend treffen.

## Regeln
- Generierten Code NIE von Hand ändern – Ursache im Recompiler bzw. in
  game.toml/Seeds beheben und neu generieren.
- Spielspezifisches gehört ins Spiel-Repo, nicht ins Framework. Änderungen
  am Framework nur generisch und nach Rücksprache mit mir.
- Die CLAUDE.md im psxrecomp-Ordner gehört dem Framework – nicht
  überschreiben, aber befolgen.
- Keine Spieldaten, BIOS-Dumps oder overlay_captures.json ins Git-Repo
  (.gitignore prüfen). Lizenz von PSXRecomp: PolyForm Noncommercial.
- Fehler gegen den Referenz-Emulator (Beetle-Oracle laut CONTRIBUTING.md)
  abgleichen, nicht raten.
  
---
 
## Phase 0 – Vorbereitung (nur lesen, nichts ändern)
- Im psxrecomp-Repo lesen: CLAUDE.md, AGENTS.md, CONTRIBUTING.md,
  docs/GETTING_STARTED.md, docs/GAME_PROJECT_SETUP.md,
  docs/EXECUTION_MODEL.md, docs/AOT_SHARDING.md, docs/BUILDING.md,
  docs/NETPLAY.md, docs/WIDESCREEN.md, docs/ENHANCEMENTS.md
- Regeln für ein neues Spielprojekt kurz zusammenfassen
- Prüfen, ob es bereits ein Projekt für Micro Maniacs gibt (README,
  Games-Tabelle)
- Ergebnis: Zusammenfassung + Bestätigung dieses Plans oder
  Änderungsvorschläge
  
## Phase 1 – Framework bauen
- Abhängigkeiten für Fedora/Nobara per dnf ermitteln (README nennt nur apt),
  alternativ mitgeliefertes Toolchain-Paket (tools/fetch_toolchain.sh)
- Recompiler bauen, OpenBIOS-Backend mit tools/regen_bios.sh erzeugen,
  Runtime bauen – genau in der Reihenfolge aus der README
- Ergebnis: Framework baut fehlerfrei

## Phase 2 – Spielprojekt anlegen
- tools/new_project_layout/setup_project.sh mit meinem .cue ausführen,
  Generate bestätigen, OpenBIOS (kein Retail-BIOS)
- Netplay beim Setup aktivieren (-DPSX_NETPLAY=ON), Lobby-Standard übernehmen
- Mit build.sh bauen
- Ergebnis: Spiel bootet bis zum Titelbild

## Phase 3 – Lauffähigkeit
- Menüs, Einzelspieler-Rennen und Vs-Modus lokal testen
- Overlays erfassen lassen, AOT-Sharding nach docs/AOT_SHARDING.md für
  bekannte Overlays einrichten
- Speichern/Laden (Memory Card) prüfen
- Ergebnis: Spiel lokal stabil spielbar

## Phase 4 – Netplay mit 2 Spielern
- docs/NETPLAY.md zusammenfassen: Rollback, Disc Gates, was jeder
  Mitspieler braucht (eigenes identisches Disc-Image?)
- Vs-Modus mit 2 Spielern lokal testen, dann online
- Desyncs dokumentieren und Ursache suchen statt Workarounds bauen
- Ergebnis: stabiles 2-Spieler-Online-Rennen

## Phase 5 – Mehr als 2 Spieler (Multitap)
- Micro Maniacs unterstützt per Multitap bis zu 8 Spieler
- Zuerst klären: Unterstützt die Runtime (SIO0) bereits einen Multitap?
  Wenn nein: Konzept vorschlagen, wie Multitap generisch im Framework
  ergänzt werden kann (nicht spielspezifisch!), und auf mein OK warten
- Danach Netplay auf 3–4, später bis 8 Spieler erweitern
- Ergebnis: Mehrspieler online mit mehr als 2 Spielern

## Phase 6 – Grafikverbesserungen
- Strikt unterscheiden: rein visuell (pro Spieler frei wählbar) vs.
  logikrelevant (beim Netplay für alle gleich erzwingen)
- Visuell: supersampling, geometry_correction, perspective_texturing in
  game.toml testen und sinnvolle Standardwerte festlegen
- Prüfen, ob Dithering abschaltbar ist (24-Bit-Ausgabe), sonst generisch
  im Renderer ergänzen
- Optional: Texturfilter und CRT-Shader als Post-Processing-Optionen
- FSR 1 nur als optionaler Upscaler für schwache Hardware; kein FSR 2+
  (keine Bewegungsvektoren/Tiefe auf der PS1)
- Widescreen: Cull-Sites finden; prüfen, ob ein breiteres Bild im Vs-Modus
  das Gameplay verändert (Ausscheiden am Bildrand) – dann im Vs-Modus
  abschaltbar und beim Netplay synchronisiert
- Ergebnis: Grafikoptionen im Launcher, Netplay weiterhin synchron

## Phase 7 – Später / optional
- 60 FPS: nur per Zwischenbild-Interpolation, niemals Logik-Timing ändern
- Windows-Build
- Installer-Paket: Setup-Programm (ohne Spielcode) + psxrecomp-Werkzeuge +
  Konfiguration; fragt beim ersten Start nach der eigenen Disc, erzeugt den
  Spielcode lokal, holt bei Bedarf die Toolchain (cmake-clang-v1) und baut
  das Spiel. Spieler braucht nur Python 3. Für Linux und Windows zusammen.
- Overlay-Abdeckung Richtung 100 % nativ treiben
