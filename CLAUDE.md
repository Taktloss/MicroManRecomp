# CLAUDE.md – Micro Maniacs Recomp

## Bei jedem Sitzungsstart
1. PLAN.md lesen (Ziel, Phasen, Regeln)
2. NOTES.md lesen (aktueller Stand, offene Fragen)
3. Bei der aktuellen Phase weitermachen und kurz sagen, wo wir stehen
4. Bei Änderungen am Verhalten: TESTS.md durchgehen

## Umgebung
- Betriebssystem: Linux, Nobara (Fedora-basiert) – Pakete per dnf, nicht apt
- GPU: AMD Radeon RX 9070 XT (Mesa radeonsi, laut Runtime-Log), 175-Hz-Panel, 32 GB RAM
- Referenz-Emulator zum Vergleich: DuckStation
- Debugger: PCSX-Redux (nativ, per Lua/GDB-Server automatisierbar –
  bevorzugt nutzen), no$psx (über Wine, nur manuell durch mich)
- Disc-Image: [~/dumps/Micro Maniacs(Europe)(En,Fr,De,Es,It)/Micro Maniacs (Europe) (En,Fr,De,Es,It).cue] – Seriennummer: [SLES-01921]

## Projektstruktur
- psxrecomp/ und recomp-ui/ sind Submodule (Framework). Deren eigene
  CLAUDE.md gilt dort ebenfalls und wird befolgt.
- Spielcode (game.toml, Seeds, CMake, Mods) liegt im Wurzelverzeichnis.
- docs/reference/ enthält Hilfsmaterial: psx-spx, Cheat-Codes,
  Screenshots/Videos vom Original, Handbuch.

## Feste Regeln
- Generierten Code NIE von Hand ändern. Ursache im Recompiler oder in
  game.toml/Seeds beheben und neu generieren.
- Keine Änderungen an psxrecomp/ oder recomp-ui/ ohne meine Zustimmung.
  Framework-Änderungen nur generisch, nie spielspezifisch.
- Keine Spieldaten, BIOS-Dumps, Disc-Images oder overlay_captures.json
  committen. Vor jedem Commit .gitignore und git status prüfen.
- Nicht raten: Unsichere Annahmen in NOTES.md unter "Offene Fragen"
  eintragen. Fehler gegen DuckStation/Beetle-Oracle abgleichen.
- Nach jeder Phase Statusbericht und auf mein OK warten.

## Netplay-Grundsatz
- Rein visuelle Einstellungen: pro Spieler frei wählbar.
- Alles, was Spiellogik, Timing oder Bildausschnitt im Vs-Modus berührt:
  beim Netplay für alle Spieler identisch erzwingen.

## Kommunikation
- Antworten und Notizen auf Deutsch.
- Kurze Statusberichte: was gemacht, was geprüft, was offen.
