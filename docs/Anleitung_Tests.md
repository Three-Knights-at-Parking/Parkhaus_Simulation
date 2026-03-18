# Anleitung: Tests im Codespace ausführen

Diese Anleitung beschreibt, wie die automatisierten Tests der Parkhaus-Simulation in der GitHub Codespaces-Umgebung vorbereitet, kompiliert und ausgeführt werden. Unsere Test-Suite prüft die Kernlogik und das Datei-Handling. Dafür müssen vorab spezifische Dateibedingungen erfüllt sein.

## 1. Voraussetzungen & Dateisystem vorbereiten

Damit die Tests fehlerfrei durchlaufen und keine Konflikte mit realen Simulationsdaten entstehen, muss das Dateisystem in einen definierten Ausgangszustand gebracht werden:

### Dateien, die **NICHT** existieren dürfen:
Bevor die Tests gestartet werden, sollte sichergestellt werden, dass folgende Produktionsdateien **nicht** vorhanden sind (ggf. löschen oder verschieben):
* **Keine Standard-Konfiguration:** Es darf sich **keine** `config.json` im Hauptverzeichnis (Root) befinden. Die Tests müssen das Fallback-Verhalten prüfen können.
* **Keine bestehenden Statistiken:** Es darf **keine** `stats.csv` im Standard-Pfad (z. B. `./stats/stats.csv`) existieren. Diese Datei oder den gesamten `./stats/` Ordner löschen, um saubere Datei-I/O-Tests zu garantieren.

### Dateien, die **ZWINGEND VORHANDEN** sein müssen:
Die Test-Suite benötigt spezifische Dummy-Dateien, um Parsing und I/O-Logik isoliert zu validieren.

**1. Dummy-Konfiguration (`test_dummy_config.json`)**
Im **Hauptverzeichnis** (Root) die Datei `test_dummy_config.json` mit exakt folgendem Inhalt:

```json
{
  "name": "Raunegg Test",
  "src_path": "./test_dummy_config.json",
  "capacity": 100,
  "floors": 2,
  "gates": 3,
  "real_equivalent": 60,
  "output_mode": 1,
  "max_ticks": 1000,
  "rand_seed": 42,
  "gate_entry_inSec": 5,
  "tick_inSec": 60,
  "queue_max_length": 15,
  "max_parking_ticks": 1440,
  "min_parking_ticks": 1,
  "mode_select": 1,
  "entry_probability_perSec_prec": 5.0,
  "is_leavable": 1
}
```
2. Test-Statistik (test/test.csv)
   Für das korrekte Einlesen von historischen Statistiken muss eine vorgefertigte Dummy-CSV-Datei existieren. Dazu erstellt man im Ordner ./test/ die Datei test.csv mit folgendem Inhalt:
```csv
# Simulation Settings - Name: Raunegg Test
# Capacity: 100 | Floors: 2 | Gates: 2 | Mode: NORMAL
Tick,CapTotal,CapTaken,Enqueued,Entered,Departed,QueueLen
1,200,5,2,2,0,0
2,200,12,8,7,0,1
3,200,18,6,6,0,1
4,200,20,5,3,1,3
5,200,25,8,6,1,5
6,200,28,3,4,1,4
7,200,80,3,2,1,4
8,200,75,1,1,6,0
9,200,70,0,0,5,0
10,200,68,2,2,4,0
--- SIMULATION SUMMARY ---
Total Ticks,10
Total Arrivals,38
Total Entered,33
Total Departed,18
Avg Capacity Taken (%),20.05
Peak Capacity Taken (%),40.00
Avg Queue Length,1.8
Avg Wait Time (Ticks),3
Max Wait Time (Ticks),9
Bad Parking Share (%),2.0
```

## 2. Tests bauen und ausführen (CMake)
   Die Tests sind direkt in unsere CMakeLists.txt integriert und generieren ein eigenständiges Executable namens Parkhaus_Simulation_Tests.
 
Im integrierten Terminal des Codespaces (ausgehend vom Hauptverzeichnis) werden folgende Befehle ausgeführt:

#### Schritt 1: Build-Verzeichnis erstellen und betreten
Wir nutzen einen build-Ordner, um generierte Dateien strikt vom Quellcode zu trennen.

```bash
mkdir -p build
cd build
```

#### Schritt 2: CMake konfigurieren
(Dieser Schritt liest die CMakeLists.txt ein und bereitet die Build-Umgebung vor, inklusive der PkgConfig/json-c Abhängigkeiten).

```bash
cmake ..
make
```

#### Schritt 3: Tests ausführen
Da das Projekt mit enable_testing() konfiguriert ist, kann man den CMake-eigenen Test-Runner nutzen. Der Parameter -V (Verbose) sorgt dafür, dass detaillierter Output im Terminal gezeigt wird, falls ein Test fehlschlägt.

```bash
ctest -V
```
(Alternativ kann man die Test-Binary auch direkt ausführen, was beim Debuggen mit GDB/LLDB oft hilfreich ist: ./Parkhaus_Simulation_Tests)