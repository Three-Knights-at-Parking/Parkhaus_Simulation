# Parkhaus‑Simulation -- UI Dokumentation

**Projekt:** Programmieren I -- Parkhaus‑Simulation\
**Komponente:** Terminalbasierte Benutzeroberfläche (UI)

------------------------------------------------------------------------

## Inhaltsverzeichnis

1.  [Lösung der Aufgabenstellung](#lösung-der-aufgabenstellung)
2.  [Architektur der
    Benutzeroberfläche](#architektur-der-benutzeroberfläche)
3.  [Interaktion mit dem
    Simulationsbackend](#interaktion-mit-dem-simulationsbackend)
4.  [Eingabeverarbeitung und
    Validierung](#eingabeverarbeitung-und-validierung)
5.  [Diskutierte alternative
    Lösungsansätze](#diskutierte-alternative-lösungsansätze)
6.  [Begründung der gewählten
    Architektur](#begründung-der-gewählten-architektur)
7.  [Herausforderungen während der
    Zusammenarbeit](#herausforderungen-während-der-zusammenarbeit)
8.  [Positive Aspekte der Teamarbeit](#positive-aspekte-der-teamarbeit)
9.  [Zusammenfassung](#zusammenfassung)

------------------------------------------------------------------------

# Lösung der Aufgabenstellung

Im Rahmen des Projekts wurde eine **Parkhaus‑Simulation in C**
entwickelt.\
Der Schwerpunkt dieser Dokumentation liegt auf der **Konzeption und
Implementierung der terminalbasierten Benutzeroberfläche (UI)**.

Die Benutzeroberfläche dient als zentrale Schnittstelle zwischen:

-   Benutzer
-   Simulationsbackend
-   Statistiksystem
-   Speicher‑ bzw. Dateiverwaltung

Die UI ermöglicht:

-   Konfiguration der Simulation
-   Starten der Simulation
-   Darstellung der Simulationsergebnisse
-   Laden gespeicherter Simulationen

Ziel der Implementierung war eine **robuste, übersichtliche und
erweiterbare Bedienstruktur**.

------------------------------------------------------------------------

# Architektur der Benutzeroberfläche

Die Benutzeroberfläche wurde **modular aufgebaut** und in mehrere
Komponenten unterteilt.

  -----------------------------------------------------------------------
  Modul                               Aufgabe
  ----------------------------------- -----------------------------------
  `ui.c`                              Zentrale Steuerung der
                                      Benutzeroberfläche und
                                      Implementierung der State Machine

  `ui_home.c`                         Hauptmenü und Navigation

  `ui_config.c`                       Konfiguration der Simulation

  `ui_simulation.c`                   Start und Steuerung der Simulation

  `ui_statistics.c`                   Darstellung der
                                      Simulationsergebnisse

  `ui_storage.c`                      Laden gespeicherter Simulationen

  `ui_help.c`                         Hilfetexte und
                                      Programmdokumentation
  -----------------------------------------------------------------------

Die Navigation basiert auf einer **State Machine**.

``` c
ui_state ui_start(Settings *settings, Simulation *simulation)
```

Die Funktion enthält die zentrale Programmschleife und ruft abhängig vom
aktuellen Zustand das entsprechende Menü auf.

Vorteile dieser Architektur:

-   klare Programmstruktur
-   einfache Erweiterbarkeit
-   zentral kontrollierter Programmfluss
-   geringere Fehleranfälligkeit

------------------------------------------------------------------------

# Interaktion mit dem Simulationsbackend

Die Simulation selbst ist **vollständig vom UI-Code getrennt**.

Die Benutzeroberfläche übernimmt lediglich:

-   Konfiguration der Simulation
-   Starten der Simulation
-   Darstellung der Tick‑Statistiken
-   Anzeige der finalen Zusammenfassung
-   Laden gespeicherter Simulationsergebnisse

Die Simulation erzeugt statistische Daten pro Tick:

``` c
StatsTick
```

Am Ende der Simulation wird eine Zusammenfassung erzeugt:

``` c
StatsSummary
```

Diese Daten werden vom Modul **`ui_statistics`** formatiert und im
Terminal ausgegeben.

Während der Simulation:

-   werden die Ausgabefunktionen vom Backend aufgerufen

Beim Laden gespeicherter Simulationen:

-   werden Datenstrukturen (`StatList`) aus CSV-Dateien geladen
-   anschließend über die UI dargestellt

Unterstützte **Output‑Modi**:

-   `NORMAL`
-   `VERBOSE`
-   `DEBUG`

Zusätzlich verwendet die UI **ASCII‑Balkenanzeigen**, z. B. für die
Darstellung der aktuellen Parkhausauslastung.

------------------------------------------------------------------------

# Eingabeverarbeitung und Validierung

Ein wichtiger Schwerpunkt lag auf der **robusten Verarbeitung von
Benutzereingaben**.

Anstelle von

``` c
scanf()
```

wurde folgende Strategie verwendet:

``` c
fgets()
strtol()
strtof()
```

Vorgehensweise:

1.  Eingabezeile mit `fgets()` einlesen
2.  Umwandlung mit `strtol()` oder `strtof()`
3.  Validierung des Wertebereichs

Dadurch können zuverlässig erkannt werden:

-   ungültige Zeichen
-   falsche Datentypen
-   Werte außerhalb erlaubter Bereiche

Zusätzlich wurden **Hilfsfunktionen zur Validierung** implementiert.

Vorteile:

-   höhere Stabilität
-   bessere Fehlerbehandlung
-   konsistente Eingabelogik

------------------------------------------------------------------------

# Diskutierte alternative Lösungsansätze

Während der Entwicklung wurden mehrere alternative Implementierungen
diskutiert.

## Monolithische UI

Eine Möglichkeit wäre gewesen, die gesamte UI in einer einzigen Datei zu
implementieren.

Nachteile:

-   schlechte Wartbarkeit
-   unübersichtlicher Code
-   erschwerte Teamarbeit

------------------------------------------------------------------------

## Ausgabe direkt im Backend

Alternative:

``` c
printf()
```

direkt aus der Simulation heraus.

Probleme:

-   starke Kopplung zwischen Simulation und UI
-   Vermischung der Verantwortlichkeiten
-   geringere Flexibilität

------------------------------------------------------------------------

## Speicherung der Statistikdaten in Arrays

Alternative Struktur:

``` c
StatsTick ticks[MAX_TICKS];
```

Problem:

Die Anzahl der Ticks ist zur Laufzeit variabel.

Daher wurde eine **verkettete Liste** verwendet.

------------------------------------------------------------------------

## Eingabe mit scanf()

Diese Variante wurde verworfen wegen:

-   schlechter Fehlerbehandlung
-   Probleme mit Eingabepuffern
-   höherer Fehleranfälligkeit

------------------------------------------------------------------------

# Begründung der gewählten Architektur

## Modularität

Die modulare Struktur verbessert:

-   Lesbarkeit
-   Wartbarkeit
-   Erweiterbarkeit

Jedes Modul besitzt eine klar definierte Aufgabe.

------------------------------------------------------------------------

## Geringe Kopplung

Simulation und UI sind klar getrennt.

Dadurch bleibt das Backend unabhängig von der Darstellung.

------------------------------------------------------------------------

## Erweiterbarkeit

Neue Funktionen können leicht ergänzt werden, z. B.:

-   zusätzliche Statistikformate
-   neue Konfigurationsoptionen
-   alternative Ausgabemethoden

Ein Beispiel ist das **nachträglich implementierte Help‑Menü**.

Dieses konnte ergänzt werden durch:

-   eine zusätzliche `.c` Datei
-   Erweiterung des Hauptmenüs

------------------------------------------------------------------------

## Robuste Benutzereingaben

Die Validierungsfunktionen sorgen für eine stabile und kontrollierte
Eingabeverarbeitung.

------------------------------------------------------------------------

# Herausforderungen während der Zusammenarbeit

## Abstimmung der Schnittstellen

Simulation, Statistiksystem und UI wurden von unterschiedlichen
Teammitgliedern entwickelt.

Daher mussten Schnittstellen mehrfach abgestimmt werden.

Besonders relevant war:

-   Definition gemeinsamer Datenstrukturen
-   Übergabe der Statistikdaten

------------------------------------------------------------------------

## Synchronisation der Projektstruktur

Mehrere Entwickler arbeiteten parallel am Code.

Daher mussten folgende Punkte konsistent gehalten werden:

-   Header‑Dateien
-   Datentypen
-   Funktionssignaturen

------------------------------------------------------------------------

## Komplexe Datenstrukturen

Das System nutzt mehrere miteinander verbundene Strukturen:

-   `Settings`
-   `Simulation`
-   `StatList`
-   `StatsTick`
-   `StatsSummary`

Das Zusammenspiel dieser Strukturen erforderte wiederholte Anpassungen.

------------------------------------------------------------------------

# Positive Aspekte der Teamarbeit

## Klare Aufgabenverteilung

Das Projekt wurde in mehrere Bereiche aufgeteilt:

-   Simulation
-   Benutzeroberfläche
-   Statistiksystem
-   Dateiverwaltung

Dadurch konnten Teammitglieder **parallel arbeiten**.

------------------------------------------------------------------------

## Modulare Softwarearchitektur

Die Architektur erleichterte:

-   parallele Entwicklung
-   Integration neuer Funktionen
-   Wartung des Codes

------------------------------------------------------------------------

## Iterative Verbesserung

Während der Entwicklung wurde der Code mehrfach überarbeitet und
vereinfacht.

Diese iterative Vorgehensweise führte zu einer stabileren und besser
wartbaren finalen Implementierung.

------------------------------------------------------------------------

# Zusammenfassung

Die entwickelte Benutzeroberfläche erfüllt die Anforderungen der
Aufgabenstellung und stellt eine strukturierte Schnittstelle zur
Parkhaus‑Simulation dar.

Wichtige Eigenschaften der Lösung:

-   modulare Architektur
-   klare Trennung von UI und Backend
-   robuste Eingabeverarbeitung
-   einfache Erweiterbarkeit

Dadurch konnte eine **stabile und wartbare Lösung** für die Steuerung
der Simulation entwickelt werden.
