# tapto-floppy
A ardunio based project to launch games on the [TapTo](https://tapto.wiki/Main_Page) platform via the contents of a floppy disc using the Adafruit Floppy FeatherWing.

## How It Works
The M4 (or RP2040) microcontroller interfaces the the FeatherWing to connect a 34 pin floppy drive. The supporting libaries allow the controller to read the discs from any 3.5 or 5.25 IBM PC compatible FAT floppy disc (so pretty much any 90s floppy drive will work). When a write-protected disc is inserted into the drive, the controller will look at the root of the disk for a file called "tapto.txt". The contents of this file are the same as the text in a stanard TapTo nfc tag (See [TapScript](https://tapto.wiki/TapScript)).

## Required Hardware
1. Adafruit Floppy FeatherWing ([Buy](https://www.adafruit.com/product/5679) or [Build](https://github.com/adafruit/Adafruit_Floppy_FeatherWing_PCB)).
2. Adafruit Feather M4 Express ([Buy](https://www.adafruit.com/product/3857)) or a RP2040*
3. Floppy Drive (Tested using a Sony mpf920)
4. Ribbon 34 Pin Floppy Ribbon Connector
5. A power supply for the floppy drive**
6. (Optional) Female pin headers to easily connector the two boards. You can source these cheaply on alliexpress (or if using an Adafruit Feather Board, they sell the [correct size headers](https://www.adafruit.com/product/2940) but they are more expensive).

\* During testing, a [Adafruit Feather RP2040](https://www.adafruit.com/product/4884) was used. The exists some hardware bug that allows the Feather Wing to backpower the RP2040. As a result, if the floppy drive is powered before the microcontroller, the RP2040 will enter boot_sel mode.

\** Many 3.5 only require a 5v supply (the documentation suggests at least 2amps). Check you floppy pinnout to see what your drive requires. You can use a 4-pin AT / ATX / IDE Molex to Berg Floppy Drive Power Cable (adafruit as sells [one](https://www.adafruit.com/product/425)) that you can use to plug into a apporpriate supply or use to solder you own barrel connectors.

##Quick Start
