# Anleitung: Parkhaus-Simulation ausführen

Diese Anleitung beschreibt, wie die Hauptanwendung der Parkhaus-Simulation in der GitHub Codespaces-Umgebung (oder einer kompatiblen Linux/WSL-Umgebung) kompiliert und gestartet wird. Theoretisch funktioniert der Code auch in einer Windows-Umgebung, jedoch muss das Installieren der externen Bibliotheken selbst erfolgen.

## 1. Voraussetzungen & Dateisystem

Im Gegensatz zu den automatisierten Tests, die eine strikte Datei-Isolation erfordern, ist die Hauptanwendung so konzipiert, dass sie benötigte Dateien und Ordner bei Bedarf selbstständig verwaltet. Dennoch gibt es ein paar Dinge zu beachten:

### Datei- und Ordnerstruktur:
* **Konfigurationsdatei (`config.json`):** Die Simulation sucht beim Start im Hauptverzeichnis nach einer `config.json`.
    * *Ist sie vorhanden*, werden die darin gespeicherten Einstellungen (Kapazität, Gates, etc.) geladen.
    * *Ist sie nicht vorhanden*, startet das Programm mit vordefinierten Standardwerten (Defaults) und bietet über die UI an, diese abzuspeichern.
* **Statistik-Ordner (`./stats/`):** Die Simulation speichert ihre Ergebnisse standardmäßig im Ordner `./stats/` ab. Falls dieser Ordner noch nicht existiert, wird er von der Middleware (SaveHandler) automatisch während der Laufzeit erstellt.
* **Terminal-Größe:** Da unsere Anwendung eine terminalbasierte Benutzeroberfläche (CUI) nutzt, sollte man sicherstellen, dass das Terminal-Fenster im Codespace groß genug gezogen ist, damit alle Menüs und Tabellen fehlerfrei und übersichtlich gerendert werden können.

---

## 2. Programm bauen (CMake)

Wir nutzen CMake, um den Build-Prozess sauber vom Quellcode zu trennen. Falls man zuvor bereits die Tests kompiliert hat und sich noch im `build`-Ordner befindet, einfach direkt bei *Schritt 3* weitermachen. Im integrierten Terminal des Codespaces (ausgehend vom Hauptverzeichnis) müssen folgende Befehle ausgeführt werden:

**Schritt 1: Build-Verzeichnis erstellen und betreten**
```bash
mkdir -p build
cd build
```

**Schritt 2: CMake konfigurieren**
(Dieser Schritt generiert die Makefiles und bindet externe Bibliotheken wie json-c und die Math-Library ein).
```bash
cmake ..
```

**Schritt 3: Kompilieren**
```bash
make
```

## 3. Programm starten
Nach dem erfolgreichen Build-Prozess befindet sich die ausführbare Datei Parkhaus_Simulation im build-Ordner.
Es kann einfach mit diesem Befehl ausgeführt werden:
```bash
./Parkhaus_Simulation
```
Sobald das Programm läuft, befindet man sich im interaktiven Hauptmenü der Benutzeroberfläche. Von hier aus kann man:

- Die Einstellungen (Settings) prüfen und anpassen. 
- Die Simulation manuell oder automatisch starten.
- Sich im Anschluss die generierten Statistiken ansehen.
- Historische Simulationsläufe über das Storage-Menü laden.
- Beendet wird das Programm regulär über die entsprechende Option im Hauptmenü. Ein Hard-Stop wie Ctrl+C gibt es nicht.

Einstellungen so wie Statistiken, und was sie bedeuten, sind im Programm beschrieben und unter dem Help Menü verfügbar. Die Navigation erfolgt über die Eingabe der entsprechenden Nummern.