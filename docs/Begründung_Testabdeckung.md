# Testabdeckung und bewusste Ausnahmen

Im Projekt wurden die fachlich relevanten Kernbereiche der Simulation mit Unit-Tests abgedeckt.  
Dabei liegt der Fokus bewusst auf den Teilen des Systems, die das Verhalten der Simulation direkt beeinflussen: Settings-Verarbeitung, Zufallsgenerierung, Nachfrageverteilung, Queue-Verhalten, Parkhaus-Logik, Statistik und die zentrale Simulationssteuerung.

Nicht jede einzelne Funktion des Projekts wurde mit einem eigenen isolierten Test versehen. Das ist **bewusst so entschieden worden**, weil einige Funktionen nur Hilfslogik darstellen, stark von I/O abhängen oder bereits sinnvoll über höhere Ebenen indirekt mitgeprüft werden.

Ziel der Teststrategie ist daher nicht eine künstliche „100%-Testabdeckung auf jeder Hilfsfunktion“, sondern eine **sinnvolle, wartbare und fachlich relevante Abdeckung**.

---

## Bereits getestete Kernbereiche

Folgende Bereiche sind durch eigene Testdateien abgedeckt:

- `Settings`
- `Queue`
- `Parkhaus`
- `Simulation`
- `SaveHandler`
- `ConfigFileHandler`
- `RNG`
- `demand`
- `gate_routing`
- `StatList`
- `Stats`
- `Car`
- die UI-Module (`ui`, `ui_home`, `ui_help`, `ui_config`, `ui_simulation`, `ui_statistics`, `ui_storage`)
- generische Fahrzeug-/Listenlogik über `test_generics.c`

---

## Nicht bzw. nur indirekt getestete Dateien und Funktionen

### `src/utils/SafteyUtils.c`
Diese Datei enthält vor allem **allgemeine Hilfs- und Loggingfunktionen**.

#### Funktionen:
- `checkNull`
- `checkEmptyString`
- `print_error`
- `print_warning`
- `print_error_s_internal`
- `print_warning_s_internal`
- `print_log_s_internal`

#### Begründung:
Diese Funktionen dienen hauptsächlich als einfache Prüf- und Ausgabehilfen.  
Sie enthalten kaum fachliche Simulationslogik und sind stark an Konsolen-Ausgabe gebunden.  
Ausgabetests wären daher vergleichsweise fragil, da sie von Formatierung, Zeilenumbrüchen und Laufzeitumgebung abhängen.

Die eigentliche Relevanz dieser Funktionen liegt darin, dass sie von vielen anderen Modulen intern verwendet werden. Deshalb wurde hier auf eine separate, tiefe Testabdeckung verzichtet.

---

### `src/generic/GenericVehicle.c`
#### Funktionen:
- `generic_vehicle_init`
- `get_vehicle_minimum_space`
- `get_vehicle_space_needed`

#### Begründung:
Diese Funktionen bilden vor allem eine generische Datenbasis für Fahrzeugobjekte.  
Sie enthalten wenig eigenständige Logik und werden indirekt durch die Fahrzeug-, Queue- und Parkhaus-Tests mitabgedeckt.

---

### `src/generic/SimulationObject.c`
#### Funktionen:
- `tick`
- `simulation_object_set_tick`
- `simulation_object_get_tick`
- `free_simulation_object`

#### Begründung:
Diese Funktionen sind Infrastruktur für das polymorphe Objektverhalten der Simulation.  
Sie werden indirekt über die konkreten Objekte mitbenutzt und sind daher nicht als isolierter Schwerpunkt getestet worden.

---

### `src/generic/VehicleList.c`
#### Funktionen:
- `vehicle_list_append`
- `vehicle_list_pop_front`
- `vehicle_list_remove`
- `vehicle_list_remove_all`
- `vehicle_list_count`

#### Begründung:
Diese Liste wird inzwischen durch eigene Unit-Tests abgesichert und ist damit nicht mehr nur indirekt über Queue und Parkhaus mitgeprüft.  
Die Funktionen bilden eine grundlegende interne Fahrzeugverwaltung und sind vor allem als technische Hilfsstruktur gedacht.  
Trotz eigener Tests bleibt der Fokus darauf, das Listenverhalten sinnvoll und praxisnah zu prüfen, statt jede interne Zustandsvariante künstlich auszutesten.
---

### `src/generic/StatList.c`
#### Funktionen:
- `statlist_append`
- `statlist_clear`
- `statlist_compute_summary`

#### Begründung:
Diese Funktionen hängen sehr eng mit Simulation und Statistik zusammen.  
Die eigentliche Aussagekraft entsteht erst im Gesamtkontext der laufenden Simulation.  
Ein isolierter Test jeder Mikro-Operation würde hier nur begrenzt zusätzlichen Nutzen bringen.

---

### `src/generic/Stats.c`
#### Funktionen:
- Statistik- und Zusammenfassungslogik des Statistiksystems

#### Begründung:
Die Statistikfunktionen sind eng mit den während der Simulation erzeugten Daten verknüpft.  
Viele Werte sind nur im Gesamtkontext sinnvoll prüfbar.  
Darum wurde hier die für das Projekt relevante Kernlogik priorisiert statt jede interne Hilfsoperation separat zu testen.

---

### `src/io/SaveHandler.c`
#### Funktionen:
- `savehandler_save_tick`
- `savehandler_save_summary`
- `savehandler_load_and_print`
- `savehandler_resolve_stats_path`
- `savehandler_write_header_if_new`
- `savehandler_init_stats_file`

#### Begründung:
Diese Datei arbeitet stark mit Datei-I/O und erzeugt bzw. liest Ausgabedateien.  
Solche Tests sind grundsätzlich möglich, aber deutlich wartungsintensiver als reine Logiktests, da sie von Dateisystemzustand, Pfaden und Cleanup abhängen.

Die wichtigsten Schreib- und Ladepfade wurden geprüft; eine vollständige Format- oder Randfallabdeckung jeder möglichen Datei-/Pfadkonstellation wurde bewusst nicht angestrebt.

---

### `src/utils/gate_routing.c`
#### Funktionen:
- Verteilungslogik für die Nachfrage auf mehrere Eingänge

#### Begründung:
Die Routinglogik ist zwar fachlich relevant, hängt aber stark von Queue- und Statistikobjekten ab.  
Getestet wurden die zentralen Verteilungspfade, nicht jede theoretische Randkombination.  
Für ein Projekt dieser Größe ist diese Testtiefe ein sinnvoller Kompromiss zwischen Aussagekraft und Wartbarkeit.

---

### `src/Simulation.c`
#### Funktionen:
- `simulation_init`
- `simulation_run`
- `simulation_start`
- `simulation_tick`
- `simulation_end`
- `free_simulation`

#### Begründung:
Die Simulationssteuerung ist das Kernstück des Projekts und wurde getestet.  
Trotzdem hängen viele Pfade von externen Zuständen ab:
- Settings
- StatList
- Parkhaus
- Queue-Verhalten
- Zufallsinitialisierung

Nicht jede denkbare Pfadkombination wurde separat isoliert geprüft, da dies schnell zu sehr komplexen und fragilen Tests führen würde.

---

### `src/Parkhaus.c`
#### Funktionen:
- Parkhaus-Initialisierung
- Kapazitätslogik
- Ein-/Austritt
- Tick-Verarbeitung
- Verteilungs- und Zustandsfunktionen

#### Begründung:
Das Parkhaus-Modul ist weitgehend abgedeckt, jedoch nicht bis in jede theoretische Nischenkombination hinein.  
Einige Pfade entfalten ihre volle Aussagekraft erst im Zusammenspiel mit Queue, Fahrzeugen und Simulation.  
Die wichtigsten fachlichen Verhaltenswege wurden priorisiert.

---

### `src/Queue.c`
#### Funktionen:
- Queue-Initialisierung
- Einfügen/Entfernen
- FIFO-Verhalten
- Nachfrageverwaltung
- Freigabe/Leeren

#### Begründung:
Die Queue ist getestet, aber nicht jede interne Grenzkonstellation wurde separat und explizit modelliert.  
Wichtig waren vor allem:
- korrektes FIFO-Verhalten
- Verhalten bei leerer/voller Queue
- sauberes Entfernen und Freigeben
- Nachfrageverwaltung

---

### `src/ui/*`
Betroffen sind:

- `ui.c`
- `ui_home.c`
- `ui_help.c`
- `ui_config.c`
- `ui_simulation.c`
- `ui_statistics.c`
- `ui_storage.c`

#### Begründung:
Die UI ist naturgemäß stark von Benutzereingaben und Konsolenausgaben abhängig.  
Das macht sie schwerer und fragiler testbar als reine Logikmodule.  
Deshalb wurden die wichtigsten Menü- und Zustandswechsel getestet, aber nicht jede einzelne Ausgabezeile oder jede mögliche Input-Fehlersituation vollständig isoliert.

---

## Warum nicht jede Funktion gleich tief getestet wurde?

Die Entscheidung basiert auf mehreren Punkten:

1. **Hilfsfunktionen haben oft wenig Eigenlogik**  
   Viele Funktionen sind nur Wrapper, Prüfer oder Ausgabehelfer.

2. **I/O ist schwer stabil zu testen**  
   Datei- und Konsolenausgabe sind vom Systemumfeld abhängig.

3. **Einige Funktionen werden sinnvoller indirekt geprüft**  
   Beispielsweise über Queue-, Parkhaus- oder Simulationsverhalten.

4. **Zufallsfunktionen brauchen Seed-basierte statt statistischer Tests**  
   Bei `RNG` und `demand` ist reproduzierbares Verhalten wichtiger als „perfekte Statistik“.

5. **Der Fokus liegt auf fachlich relevanter Logik**  
   Also auf dem Verhalten, das die Simulation tatsächlich bestimmt.

---

## Fazit

Die Teststrategie des Projekts ist bewusst pragmatisch:  
Es werden die **fachlich entscheidenden und risikoreichen Bereiche** der Simulation geprüft, während reine Hilfslogik, stark I/O-abhängige Funktionen und indirekt genutzte Basisroutinen nur dort separat getestet werden, wo es einen klaren Mehrwert bringt.

Damit wird ein guter Kompromiss aus:

- Aussagekraft,
- Wartbarkeit,
- Testaufwand
- und praktischer Relevanz

erreicht.

---