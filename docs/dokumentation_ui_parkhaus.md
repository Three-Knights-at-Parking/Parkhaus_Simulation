Dokumentation der Projektumsetzung – Benutzeroberfläche der
Parkhaus-Simulation

1.  Lösung der Aufgabenstellung

Im Rahmen des Projekts wurde eine Parkhaus-Simulation in der
Programmiersprache C entwickelt. Der Schwerpunkt dieses Beitrags liegt
auf der Konzeption und Implementierung der terminalbasierten
Benutzeroberfläche (UI), welche als zentrale Schnittstelle zwischen
Benutzer und Simulationsbackend dient.

Die Benutzeroberfläche ermöglicht es, die Simulation zu konfigurieren,
auszuführen sowie Simulationsergebnisse darzustellen und gespeicherte
Simulationen zu laden. Ziel der Implementierung war eine robuste,
übersichtliche und erweiterbare Bedienstruktur.

Architektur der Benutzeroberfläche

Die UI wurde modular aufgebaut und in mehrere spezialisierte Module
unterteilt. Jedes Modul übernimmt eine klar definierte Aufgabe:

ui.c – Zentrale Steuerung der Benutzeroberfläche sowie Implementierung
der UI-State-Machine.

ui_home.c – Startmenü und Navigation innerhalb der Anwendung.

ui_config.c – Konfiguration der Simulationseinstellungen.

ui_simulation.c – Starten der Simulation.

ui_statistics.c – Formatierung und Darstellung der Simulationsergebnisse
im Terminal.

ui_storage.c – Laden gespeicherter Simulationen aus CSV-Dateien.

ui_help.c – Bereitstellung erklärender Hilfetexte zur Anwendung und zu
den Simulationseinstellungen.

Die Navigation zwischen diesen Modulen basiert auf einer
Zustandsmaschine (State Machine). Der aktuelle Zustand wird durch ein
Enum (ui_state) repräsentiert. Abhängig vom Zustand wird das
entsprechende Menü aufgerufen und anschließend der nächste Zustand
zurückgegeben.

Der Einstiegspunkt der UI ist die Funktion ui_start(), welche eine
zentrale Steuerschleife enthält. Diese Struktur sorgt für eine klar
definierte Navigation zwischen den einzelnen Programmteilen und
erleichtert zukünftige Erweiterungen. Gleichzeitig wird dadurch der
Programmfluss zentral kontrolliert, wodurch unerwartete Zustandswechsel
vermieden werden.

Interaktion mit dem Simulationsbackend

Die eigentliche Simulation ist vollständig vom UI-Code getrennt
implementiert. Die Benutzeroberfläche übernimmt lediglich folgende
Aufgaben:

- Konfiguration der Simulationseinstellungen
- Starten der Simulation
- Darstellung der Tick-Statistiken sowie der abschließenden
  Zusammenfassung
- Laden gespeicherter Simulationsergebnisse

Die Simulation selbst erzeugt statistische Daten pro Tick (StatsTick).
Zusätzlich wird am Ende der Simulation eine aggregierte Zusammenfassung
(StatsSummary) berechnet.

Diese Daten werden vom Statistikmodul der UI formatiert und im Terminal
ausgegeben. Während der laufenden Simulation werden die entsprechenden
Ausgabefunktionen vom Backend aufgerufen. Beim Laden gespeicherter
Simulationen werden die aus der Datei gelesenen Datenstrukturen
(StatList) an die UI übergeben und dort ausgegeben.

Die Darstellung kann abhängig vom gewählten Output-Modus variieren (z.
B. NORMAL, VERBOSE oder DEBUG). Zusätzlich wird eine visuelle
Darstellung über ASCII-basierte Balkenanzeigen verwendet, beispielsweise
zur Anzeige der aktuellen Parkhausauslastung.

Eingabeverarbeitung und Validierung

Ein besonderer Fokus lag auf der robusten Verarbeitung von
Benutzereingaben. Anstelle der häufig fehleranfälligen Funktion scanf()
wurde eine Kombination aus fgets(), strtol() und strtof() verwendet.

Dabei wird zunächst eine vollständige Eingabezeile gelesen und
anschließend kontrolliert in numerische Werte umgewandelt. Dadurch
können ungültige Eingaben (z. B. Buchstaben statt Zahlen oder Werte
außerhalb des erlaubten Bereichs) zuverlässig erkannt werden.

Zusätzlich wurden mehrere Hilfsfunktionen implementiert, um Eingaben zu
validieren, beispielsweise Funktionen zur Prüfung von Ganzzahlen,
Prozentwerten oder Strings.

Diese Strategie erhöht die Stabilität der Anwendung und verhindert
unerwartete Programmzustände durch fehlerhafte Eingaben. Gleichzeitig
ermöglicht sie eine konsistente Fehlerbehandlung für sämtliche
Benutzereingaben.

2.  Diskutierte alternative Lösungsansätze

Während der Entwicklung wurden mehrere alternative
Implementierungsstrategien diskutiert und miteinander verglichen.

Monolithische UI-Struktur

Eine mögliche Lösung wäre gewesen, die gesamte Benutzeroberfläche in
einer einzigen Datei zu implementieren. Dieser Ansatz hätte zwar den
initialen Implementierungsaufwand reduziert, wäre jedoch schnell
unübersichtlich geworden und hätte die Wartbarkeit des Codes erheblich
verschlechtert.

Direkte Ausgabe aus dem Simulationsbackend

Eine weitere diskutierte Option bestand darin, dass das
Simulationsbackend die Statistiken direkt über printf() im Terminal
ausgibt. Diese Lösung hätte weniger Schnittstellen benötigt, hätte
jedoch zu einer starken Kopplung zwischen Simulation und
Benutzeroberfläche geführt. Darüber hinaus würden dadurch die
Verantwortungsbereiche der einzelnen Programmkomponenten vermischt, da
die Interaktion mit dem Benutzer klar in den Zuständigkeitsbereich der
UI fällt.

Speicherung von Tick-Statistiken in Arrays

Für die Speicherung der Statistikdaten wurde auch eine arraybasierte
Lösung diskutiert. Da die Anzahl der Simulationsticks jedoch zur
Laufzeit variieren kann, wurde stattdessen eine dynamische Datenstruktur
(verkettete Liste) bevorzugt.

Direkte Eingabe mit scanf()

Eine einfache Implementierung der Benutzereingaben hätte auf scanf()
basieren können. Aufgrund der eingeschränkten Fehlerbehandlung und
möglicher Probleme mit Eingabepuffern wurde diese Lösung jedoch
verworfen.

3.  Begründung der gewählten Architektur

Die finale Architektur wurde gewählt, weil sie mehrere zentrale
Anforderungen erfüllt.

Modularität

Die klare Aufteilung der Benutzeroberfläche in mehrere Module verbessert
die Lesbarkeit und Wartbarkeit des Codes. Jedes Modul besitzt eine klar
definierte Aufgabe und Verantwortlichkeit.

Geringe Kopplung zwischen UI und Backend

Die Simulation selbst ist vollständig von der Benutzeroberfläche
getrennt implementiert. Dadurch bleibt das Backend unabhängig von der
konkreten Darstellungslogik und kann prinzipiell auch mit anderen
Benutzeroberflächen verwendet werden.

Erweiterbarkeit

Die modulare Struktur ermöglicht es, neue Funktionen relativ einfach zu
ergänzen, beispielsweise zusätzliche Statistikformate, weitere
Konfigurationsoptionen oder alternative Ausgabemethoden.

Auch die Menüstruktur lässt sich problemlos erweitern. Ein praktisches
Beispiel hierfür ist das nachträglich implementierte Help-Menü, das dem
Benutzer zusätzliche Informationen zur Bedienung des Programms
bereitstellt. Die Erweiterung konnte ohne größere Änderungen an der
bestehenden Architektur erfolgen, da lediglich eine neue C-Datei ergänzt
und die Menüauswahl im Hauptmenü erweitert werden musste.

Robuste Benutzereingaben

Durch die Implementierung eigener Validierungsfunktionen konnte eine
stabile und kontrollierte Eingabeverarbeitung realisiert werden.

4.  Herausforderungen während der Zusammenarbeit

Während der Projektarbeit traten mehrere organisatorische und technische
Herausforderungen auf.

Abstimmung der Schnittstellen

Da Simulation, Statistiksystem und Benutzeroberfläche von
unterschiedlichen Teammitgliedern entwickelt wurden, mussten die
Schnittstellen zwischen diesen Komponenten mehrfach abgestimmt werden.
Besonders die Übergabe der Statistikdaten erforderte eine klare
Definition der verwendeten Datenstrukturen.

Synchronisation der Projektstruktur

Mehrere Personen arbeiteten parallel an unterschiedlichen Modulen. Daher
war es notwendig, eine konsistente Struktur der Header-Dateien,
Datentypen und Funktionssignaturen sicherzustellen.

Umgang mit komplexen Datenstrukturen

Die Simulation verwendet mehrere miteinander verbundene Strukturen wie
Settings, Simulation, StatList, StatsTick und StatsSummary. Das korrekte
Zusammenspiel dieser Strukturen musste im Verlauf der Entwicklung
mehrfach angepasst und abgestimmt werden.

5.  Positive Aspekte der Teamarbeit

Trotz der genannten Herausforderungen verlief die Zusammenarbeit im Team
insgesamt sehr erfolgreich.

Klare Aufgabenverteilung

Die Entwicklung wurde in mehrere Teilbereiche aufgeteilt, darunter
Simulation, Benutzeroberfläche, Statistiksystem und Dateiverwaltung.
Jedes Teammitglied erhielt einen klar definierten Aufgabenbereich.
Dadurch konnten mehrere Teammitglieder parallel und größtenteils
unabhängig voneinander arbeiten.

Modulare Softwarearchitektur

Die modulare Struktur des Programms erleichterte sowohl die parallele
Entwicklung als auch die spätere Integration der einzelnen Komponenten
sowie zusätzlicher Funktionen.

Gemeinsame Architekturentscheidungen

Wichtige Designentscheidungen wurden im Team gemeinsam diskutiert und
bewertet. Dadurch konnten verschiedene Lösungsansätze miteinander
verglichen und anschließend die sinnvollste Variante ausgewählt werden.

Iterative Verbesserung

Während der Entwicklung wurde der Code mehrfach überarbeitet und
vereinfacht. Diese iterative Vorgehensweise führte zu einer stabileren
und besser wartbaren finalen Implementierung.

Zusammenfassung

Die entwickelte Benutzeroberfläche erfüllt die Anforderungen der
Aufgabenstellung und stellt eine strukturierte Schnittstelle zur
Parkhaus-Simulation dar. Durch den modularen Aufbau, die klare Trennung
zwischen UI und Backend sowie eine robuste Eingabeverarbeitung konnte
eine stabile und erweiterbare Lösung realisiert werden.
