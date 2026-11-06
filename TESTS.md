# TESTS.md – Testcheckliste Micro Maniacs Recomp

Nach jeder Änderung, die Verhalten oder Darstellung betreffen kann, die
passenden Abschnitte durchgehen. Ergebnis kurz in NOTES.md
(Sitzungsprotokoll) festhalten. Bei Abweichungen: mit DuckStation
vergleichen und Screenshots aus docs/reference/ heranziehen.

## A – Basis (immer)
- [ ] Projekt baut ohne Fehler und ohne neue Warnungen
- [ ] Generierter Code ist unverändert reproduzierbar (Regenerieren ergibt
      keinen Diff)
- [ ] Spiel bootet bis zum Titelbild
- [ ] Hauptmenü ist bedienbar (Tastatur und Gamepad)
- [ ] git status: keine Spieldaten, Disc-Images, BIOS oder
      overlay_captures.json zum Commit vorgemerkt

## B – Spielablauf
- [ ] Einzelspieler-Rennen startet, läuft durch, Ergebnisbildschirm erscheint
- [ ] Mindestens zwei verschiedene Strecken getestet
- [ ] Waffen/Items funktionieren
- [ ] Musik und Soundeffekte laufen, kein Knacksen
- [ ] Zwischensequenzen/FMVs (falls vorhanden) laufen
- [ ] Speichern und Laden auf Memory Card funktioniert
- [ ] Kein Absturz und kein Hänger nach 15 Minuten Spielzeit

## C – Lokaler Mehrspieler
- [ ] Vs-Modus mit 2 Gamepads
- [ ] Vs Teams
- [ ] (ab Phase 5) Vs mit 3–4 Spielern per Multitap
      (2026-11-06 nur über lokales Netplay geprüft: 3, 4 und 5 Spieler,
      weitere Spieler drücken in der Figurenauswahl Start)
- [ ] (ab Phase 5) Vs mit bis zu 8 Spielern
      (entfällt: auf 5 Spieler begrenzt, zwei Multitaps lassen das Intro
      hängen – Framework-CD-Datenpfad, siehe NOTES.md)

## D – Netplay (ab Phase 4)
- [ ] Lobby erstellen und beitreten funktioniert
- [ ] Komplettes 2-Spieler-Rennen online ohne Desync
      (2026-11-06 lokal über 127.0.0.1 ok: Zweikampf Mice N Easy bis
      Siegerbild/Ergebnis, desync 0; echtes Online noch offen)
- [ ] Mehrere Rennen hintereinander ohne Desync
      (2026-11-06 lokal: 2 Rennen nacheinander, ~4 min, desync 0)
- [x] Verhalten bei Verbindungsabbruch sauber (kein Absturz)
      (2026-11-06: Gast per kill -9 beendet, Host meldet "The match ended:
      the other player left or stopped responding." und beendet sich ohne
      Absturz/Coredump)
- [ ] Logikrelevante Einstellungen werden für alle erzwungen
      (2026-11-06: Mod-Plan und BIOS-Boot-Plan werden abgeglichen, bei
      Abweichung startet die Sitzung nicht (content_mismatch); Übernahme der
      Host-Werte über die Lobby noch manuell zu prüfen)
- [ ] Unterschiedliche visuelle Einstellungen bei den Spielern verursachen
      keinen Desync
      (2026-11-06 teilweise: Host OpenGL, Gast Software-Renderer, ganzes
      Rennen desync 0; Vulkan fällt im Netplay auf Software zurück)

- [ ] (ab Phase 5) Netplay mit 3–5 Spielern ohne Desync
      (2026-11-06 lokal über 127.0.0.1, Host als Hub: 3 Spieler Rennen
      komplett, 4 Spieler Rennen komplett mit Ergebnis, 5 Spieler ~5 min;
      jeweils desync 0, core_mismatch 0; echtes Online noch offen)
- [ ] Pad 1 am Multitap reagiert (multitap_analog = false; mit true tot)

## G – Cheats (Paket mmr.cheats)
- [ ] Infinite Lives an: Leben bleiben nach verlorener Runde auf dem
      eingestellten Wert (Option "Lives")
- [ ] Secret Options an: alle 12 Einträge unter Optionen > Geheimoptionen
- [ ] Beide aus: Spiel verhält sich wie das Original (Leben sinken)
- [ ] (ab Phase 4) Netplay: Cheat-Auswahl des Hosts gilt für alle, kein Desync
      (2026-11-06 Direktverbindung ok: gleicher Plan → Leben 5 und Maske ffff
      auf beiden Peers, desync 0; Host-Plan über Lobby noch ungeprüft)

## E – Grafik (ab Phase 6)
- [ ] Supersampling 1x / 2x / 4x: Darstellung korrekt, keine Artefakte
- [ ] geometry_correction an/aus: kein Polygonwackeln mehr, nichts kaputt
- [ ] perspective_texturing an/aus: Bodentexturen nicht mehr verzerrt
      (2026-11-06: wirkt leicht auf großen Bodenflächen, keine Artefakte)
- [ ] Dithering aus (Mod Image Filtering): Rastermuster weg
      (2026-11-06: wirkt, Vergleich vom Savestate)
- [ ] 4:3-Ausgabe bei ausgeschalteten Verbesserungen identisch zum Original
- [ ] Widescreen: keine fehlenden Objekte am Bildrand (Culling)
      (2026-11-06 Mod mmr.widescreen: Strecke weitgehend vollständig,
      einzelne fehlende Polygone links oben)
- [ ] Widescreen im Vs-Modus: verhält sich wie in NOTES.md entschieden
      (2026-11-06: Ausscheiden an der 4:3-Grenze wie Original; Netplay:
      Host-Wahl gilt für alle, beide 16:9 ganzes Rennen desync 0,
      ungleiche Wahl → Sitzung startet nicht)
- [ ] Optionen über den Launcher änderbar und gespeichert

## F – Performance
- [ ] Volle Bildrate im Rennen (50 Hz PAL bzw. 60 Hz NTSC)
- [ ] Keine Ruckler beim Laden neuer Bereiche (Overlays)
- [ ] Anteil Interpreter vs. nativ notiert (Ziel: Interpreter möglichst leer)
