Libraries einbinden in C ist ziemlich großer schmerz auf Windows. (Man müsste ein separates Projekt mit dem Source Code compilieren und bauen) und die Chance dass das Projekt dann nicht mehr auf Codespaces kompiliert ist zu groß. Zur Entwicklung gibt es daher jetzt 2 Optionen:

---
### 1. Umstieg auf WSL
Windows Subsystem for Linux ist eine Art "VM" die auf Windows läuft. Sie ermöglicht es ein Linux OS (standardmäßig Ubuntu) parallel zu Windows zu haben (die auch parallel existieren und auf Dateien zugreifen können).
Das Setup ist etwas Zeitaufwand, aber an sich worth it, da wir so nativ entwickeln können (was auf WSL kompiliert, kompiliert auch auf Codespaces). Die installation funktioniert wie folgt:

Schließe VSCode oder CLion.

[öffne cmd oder Powershell]
Installiere WSL auf deiner Maschine
```wsl --install```
Es sollte sich danach eine Konsole öffnen, in der du deinen Nutzer und das Passwort eingeben musst. Bitte beende das Setup.

Nachdem du das Setup beendet hast, in der Konsole in der WSL geöffnet ist (es sieht aus wie git bash)
```LucisNotebook:/mnt/host/c/Users/busin #so sieht der pfad bei mir aus```
führst du diesen Befehl aus:
```sudo apt update && sudo apt install build-essential cmake doxygen libjson-c-dev pkg-config gnuplot```
Du wirst gefragt nach einem "Sudo" passwort, das ist dein Benutzerkennwort. Führe nun die Installation durch (gib überall Y für Yes an).

Nachdem alles installiert ist, müssen wir WSL erlauben, Dateien auf Windowspfaden zu bearbeiten:
```sudo nano /etc/wsl.conf```
Das öffnet eine Datei, in die wir diese Zeile oben hineinkopieren:
```
[automount]
options = "metadata,umask=22,fmask=11"
```
Um die Datei wieder zu schließen dann:
STRG + o
Enter
STRG + x

Nun öffnest du erneut ein CMD/Powershell Fenster und gibst diesen Befehl ein:
```wsl --shutdown```
Schließe alle CMD/Powershell oder Terminal Fenster. Falls noch offen MUSST du nun CLion oder VSCode schließen. Da wir alle CLion verwendet reduziere ich die Einstellung hier auf CLion, falls jemand von euch doch nochmal auf VSCode umspringen möchte ist Option 2 wahrscheinlich eh schlauer.

1. Klicke oben in der Leiste auf das CMakeProfil
   ![Toolchain Setup1](./docs/attachments/migration/img1.png)

2. Klicke "Edit CMake Profiles"

3. Es öffnet sich ein Fenster, gehe in der Leiste links auf Toolchains:
4.    ![Toolchain Setup1](./docs/attachments/migration/img2.png)

4. Klicke auf das Plussymbol und wähle WSL aus
   ![Toolchain Setup1](./docs/attachments/migration/img3.png)

5. Clion übernimmt alle Einstellungen für dich. Du musst in der List nur auf das WSL Profil klicken und das Profil nach oben verschieben (über das MinGW Profil)
   ![Toolchain Setup1](./docs/attachments/migration/img4.png)

6. Drücke unten rechts auf apply.

7. Links in der Liste, wähle nun CMake aus:
   ![Toolchain Setup1](./docs/attachments/migration/img5.png)

8. Wähle als Toolchain "Use Default" oder spezifisch das WSL Profil:
   ![Toolchain Setup1](./docs/attachments/migration/img6.png)

9. (Optional) Wähle als Generator "Let CMake decide" aus oder belass es bei ninja.exe falls das nicht geht.

10. Apply und Ok. Dein CLion ist nun aufgesetzt.

Für andere Projekte kann es sein, dass Ihr in Zukunft (je nach euren Anforderungen) einfach das Windows MinGW Profil wieder als default (also ganz oben in der Toolchain List) setzen müsst. Wichtig ist, wenn ihr eine Bibliothek jetzt einbinden wollt, müsst ihr sie auf WSL installieren (also den sudo apt install command ausführen). Um WSL wieder zu starten (damit ihr mit WSL interagieren könnt), müsst ihr einfach "wsl" in CMD oder Powershell eingeben. **Das gilt nur für externe Bibliotheken, die Standardlibs sind immernoch normal gebündelt**.

---

### 2. Verbinden mit GitHub Codespaces (Remote Development)

Es besteht auch die Möglichkeit, direkt auf GitHub Codespaces zu entwickeln. Sowohl CLion als auch VSCode unterstützen die Entwicklung über ihre Remote Funktion. Das Setup (gerade für CLion) ist jedoch nicht sehr einfach und ich würde diese Weise zu entwickeln auch nicht empfehlen, da wir nur wenig Uptime mit GitHub Codespaces haben. Wenn ihr also lange Coded, dann kann es sein dass Herr Braunagel am Ende nicht mehr das Codespace verwenden kann. Nichtsdesto trotz:

#### A. Setup für VSCode (Am einfachsten)
VSCode hat die nativste Integration für Codespaces.

1. Installiere die Extension "GitHub Codespaces" aus dem Marketplace.

2. Klicke unten links auf das grüne Icon (Remote-Window) oder drücke F1 und gib Codespaces: Connect to Codespace ein.

3. Wähle dein Repository aus. VSCode öffnet ein neues Fenster, das direkt mit Ubuntu in der Cloud verbunden ist.

Wichtig: Du musst im Codespace-Terminal einmalig sudo apt install libjson-c-dev ausführen (oder wir hinterlegen es in der devcontainer.json, damit es automatisch passiert).

#### B. Setup für CLion (Gateway)
1. JetBrains nutzt den "JetBrains Gateway", um CLion mit Codespaces zu verbinden.

2. Voraussetzung: Installiere den [GitHub Codespaces Plugin für JetBrains](https://www.google.com/search?q=https://plugins.jetbrains.com/plugin/18525-github-codespaces).

3. Öffne CLion und wähle im Willkommensbildschirm Remote Development -> GitHub Codespaces.

4. Klicke auf "Connect to Codespace". Du wirst aufgefordert, dich bei GitHub zu autorisieren.

5. Wähle deinen aktiven Codespace aus oder erstelle einen neuen.

Der Prozess: JetBrains lädt nun ein "Backend" (eine schlanke Version von CLion ohne GUI) auf den Codespace hoch. Dein lokales CLion fungiert nur noch als "Thin Client" (Monitor/Tastatur).

Vorteil: Kompilierung passiert zu 100% in der Cloud
Nachteil: Hoher RAM-Verbrauch im Codespace-Container.

---
Egal welche Methode ihr wählt, hier sind die goldenen Regeln für unser Projekt:

- CMake-Pfad: Nutzt in der CMakeLists.txt niemals absolute Pfade wie C:\libs. Nutzt immer find_package oder pkg_check_modules, ansonsten wird es im Build nicht aufgelöst.

- Libraries installieren: Wenn ihr eine neue Library braucht (z.B. für Grafik oder Simulation/Multithreading was auch immer), muss diese immer via sudo apt install im WSL oder im Codespace installiert werden.

- Codespace ist ein Linuxsystem und verhält sich Grundsätzlich anders! Da die Dateien immer noch auf dem Windowspfad liegen, müsst ihr (damit der Code bei euch funktioniert) trotzdem die Windows-Variante von eurem Code verwenden. (Auch wenn das für euch wahrscheinlich weniger relevant ist da ihr kaum Kontakt mit dem Betriebssystem habt).

- Solltet ihr Option 2 wählen, vergesst bitte nicht den Codespace wieder zu stoppen. Wenn wir keine Minuten mehr übrig haben wirds teuer und ich würde ungern für meine Note die fair und unparteiisch bewertet wird trotzdem Geld zahlen müssen : ) (Zum Protokoll das ist ein Joke und ich würde niemals Bestechungen durchführen)






