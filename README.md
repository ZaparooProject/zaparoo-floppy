# Zaparoo Floppy Serial Interface
A ardunio based project to launch games on the [Zaparoo](https://wiki.zaparoo.org/Main_Page) platform via the contents of a floppy disc using the Adafruit Floppy FeatherWing.

## How It Works
A microcontroller interfaces with the FeatherWing to connect a 34 pin floppy drive. The supporting libaries allow the controller to read from any 3.5 or 5.25 IBM PC compatible FAT floppy disk (so pretty much any 90s floppy drive will work). When a write-protected disc is inserted into the drive, the controller will look at the root of the disk for a file called "zaparoo.txt". The contents of this file are the same as the text in a standard Zaparoo nfc tag (See [ZapScript](https://wiki.zaparoo.org/ZapScript)). Until another disk is inserted, the motor of the drive will not spin.

## Why Only Write Protected Disks?
The floppy drive bus does not have a dedicated pin that triggers when a disk is inserted. Other singals like the "READY" and "DISK CHANGE" only change when a new disk is inserted and the motor spins up. Polling those signals would cause the drive to constantly spin, adding additional wear and tear on the device. "WRITE PROTECT" is active only when a protected disk is in the drive and it will change when removed, allowing for motor free detection.

## Required Hardware
1. Adafruit Floppy FeatherWing ([Buy](https://www.adafruit.com/product/5679) or [Build](https://github.com/adafruit/Adafruit_Floppy_FeatherWing_PCB)).
2. Adafruit Feather M4 Express ([Buy](https://www.adafruit.com/product/3857)) or a RP2040*
3. Floppy Drive (Tested using a Sony mpf920)
4. 34 Pin Floppy Ribbon Connector
5. A power supply for the floppy drive**
6. (Optional). It is easier to use a cheap Another way to write floppy disks (Cheap usb drives should be more than enough)
7. (Optional) Female pin headers to easily connector the two boards. You can source these c.heaply on alliexpress (or if using an Adafruit Feather Board, they sell the [correct size headers](https://www.adafruit.com/product/2940) but they are more expensive).

<sup>\* During testing, a [Adafruit Feather RP2040](https://www.adafruit.com/product/4884) was used. The exists some hardware bug that allows the Feather Wing to backpower the RP2040. As a result, if the floppy drive is powered before the microcontroller, the RP2040 will enter boot_sel mode. Due to this flaw, only M4 firmwares will be distributed.</sup>

<sup>\** Many 3.5 only require a 5v supply (the documentation suggests at least 2amps). Check you floppy pinnout to see what your drive requires. You can use a 4-pin AT / ATX / IDE Molex to Berg Floppy Drive Power Cable (adafruit as sells [one](https://www.adafruit.com/product/425)) that you can use to plug into a apporpriate supply or use to solder you own barrel connectors.</sup>

## Quick Start (For M4 Feather Express)
After you've soldered your boards, connect the microcontroller to your computer and enter boolsel mode (on the M4, double click reset) (you should see a new drive connected). Download the latest .uf2 from the releases page and copy it to the microcontroller drive. Now that your board's firmware is flashed, you can connect everything to your system via the usb of your microcontroller. Configure Zaparoo to use [Simple Serial](https://wiki.zaparoo.org/Reader_Drivers#Simple_Serial) and insert a write-protected disk to launch a game.

## Developer Setup (Windows)
If you want to compile from source instead of the prebuilt firmware.
1. Download and install the Ardunio IDE.
2. Follow the instructions to add your board to the IDE (For the M4, see this [page](https://learn.adafruit.com/adafruit-feather-m4-express-atsamd51/setup) and this [page](https://learn.adafruit.com/adafruit-feather-m4-express-atsamd51/using-with-arduino-ide)).
3. Install the "Adafruit_Floppy" library from the Library Maanger and install any of its dependencies.
4. Select your board model to flash and select the com port.
5. Change the USB Stack to "TinyUSB"
6. Update your board settings to the recommened values in the [Adafruit_Floppy](https://github.com/adafruit/Adafruit_Floppy) libary page.
   -RP2040: "Overclock to 200MHz and select -O3 optimization for best performance"
   -M4: "Overclock to 180MHz, select Fastest optimization"
7. Compile and upload the firmware.
