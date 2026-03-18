# Parkhaus Rauenegg – Simulation (Teil II: Implementierung)

> **Kurs:** Programmieren 1 (TSA/TSL 25, Embedded Systems)  
> **Dozent:** Prof. Dr. Christian Braunagel  
> **Abgabe:** 18.03.2026 (Teil II – Programmentwurf / Implementierung)  
> **Sprache:** C (Standard: C17)  
> **Team:** Luca Perri, Simon Ibach, Daniel Weimer

---

## Projektbeschreibung & Anforderungen

Dieses Repository enthält die vollständige Implementierung (Teil II) einer diskreten Parkhaus-Simulation für das fiktive Parkhaus *Rauenegg*. Das Projekt überführt die in Teil I erstellten Konzepte (Strukturen, Pseudocode, Diagramme) in eine voll funktionsfähige, modulare Softwarearchitektur in C.

**Zentrale Anforderungen der Aufgabenstellung:**
* **Tick-basierte Simulation:** Deterministischer Ablauf in diskreten Zeitschritten.
* **Dynamische Konfiguration:** Einlesen und Anpassen von Parametern (Kapazität, Gates, Ein-/Ausfahrtsraten, Zeiteinheiten).
* **Datenhaltung & Statistik:** Erfassung und persistente Speicherung von mindestens fünf Statistiken pro Zeitschritt sowie einer abschließenden Gesamtauswertung.
* **CUI (Console User Interface):** Interaktive, terminalbasierte Benutzeroberfläche zur Steuerung und Datenauswertung.
* **Robustheit:** Saubere Trennung von Logik, sicheres Memory-Management und Fehlerbehandlung.

---

## Lösungsansatz & Architektur

Um diese Anforderungen zu erfüllen, wurde ein stark modulares und erweiterbares Design gewählt (angelehnt an das *Separation of Concerns* und das *Open/Closed Principle*):

1. **Objektorientierter Ansatz in C (`SimulationObject` & `GenericVehicle`):** Durch Basis-Pointer und "Destructive Casting" werden Fahrzeuge extrem flexibel verwaltet. Das Parkhaus und die Queues arbeiten gegen abstrakte Schnittstellen, was zukünftige Erweiterungen (z. B. Motorräder, E-Autos) nahtlos ermöglicht.
2. **Middleware-Architektur (`SaveHandler`):** Die Berechnungslogik (Simulation) ist vollständig von der Ausgabe (UI) und der Dateispeicherung entkoppelt. Der Datenfluss wird zentral über eine Middleware gesteuert.
3. **Defensive Programmierung:** Sämtliche Eingaben werden über eigene Validierungsroutinen (`SafetyUtils`) geprüft. Fehlende Pfade oder Ordner werden bei der Speicherung automatisch abgefangen und generiert.

---

## Ordnerstruktur

Die Dateistruktur trennt streng zwischen Konfiguration, Header-Definitionen, Implementierung und Tests:

```text
.
├── .devcontainer/        # Konfiguration für die GitHub Codespaces Referenzplattform
├── docs/                 # Projektdokumentation (Entwurfs-Dokus, Migration, Architektur)
│   └── attachments/      # Bilder und Diagramme
├── include/              # Header-Dateien (.h) - Schnittstellen / Prototypen
│   ├── io/               # Dateiverwaltung und Middleware (ConfigFileHandler, SaveHandler)
│   ├── ui/               # Module der Terminal-Benutzeroberfläche
│   └── utils/            # Hilfsfunktionen, Routing, SafteyUtils, StatList
├── src/                  # Quellcode (.c) - Implementierungen
│   ├── generic/          # Basisklassen (SimulationObject, GenericVehicle, RNG)
│   ├── io/               # Implementierung Datei-I/O
│   ├── ui/               # Implementierung der UI-Module
│   └── utils/            # Hilfslogiken (Demand, Gate-Routing)
├── test/                 # CTest-Suite zur Validierung der Module
├── CMakeLists.txt        # Zentrale Build-Konfiguration
└── README.md             # Dieses Dokument
```

Hinweis: Temporäre Build-Verzeichnisse wie cmake-build-debug oder IDE-Ordner wie .idea werden via .gitignore von der Versionskontrolle ausgeschlossen).

---
## Kompilieren, Ausführen & Testen
Als Build-System wird CMake verwendet. Detaillierte Schritt-für-Schritt-Anleitungen zur Ausführung in GitHub Codespaces oder WSL sind in den separaten Dokumenten zu finden:

- [Anleitung: Programm ausführen](./docs/Anleitung_Programm.md) – Wie die Simulation gebaut und gestartet wird.
- [Anleitung: Tests ausführen](./docs/Anleitung_Tests.md) – Voraussetzungen und Ausführung der Test-Suite.
---

## Dokumentation & Sonstiges
Ideen, Mindmaps, Architekturdiskussionen und sonstige Konzept-Artefakte werden teilweise gebündelt im Planungs-Repository gesammelt:

[Three-Knights-at-Parking / Teil1_Documentation](https://github.com/Three-Knights-at-Parking/Teil1_Documentation)

Dort liegen auch die Transkripte und Zusammenfassungen der Team-Meetings, die den gemeinsamen Entwicklungsprozess dokumentieren. In unserem "docs"-Ordner finden sich auch weitere Dokumente, mit denen wir unsere Architektur und Entwicklungsentscheidungen gemäß den Requirements aus Aufgabe 2 dokumentieren und justifizieren.
- [Dokumentation Testabdeckung](./docs/Begründung_Testabdeckung.md)