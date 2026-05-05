# Zaparoo Floppy Serial Interface
A ardunio based project to launch games on the [Zaparoo](https://wiki.zaparoo.org/Main_Page) platform via the contents of a floppy disc using the Adafruit Floppy FeatherWing.

## How It Works
A microcontroller interfaces with the FeatherWing to connect a 34 pin floppy drive. The supporting libaries allow the controller to read from any 3.5 or 5.25 IBM PC compatible FAT floppy disk (so pretty much any 90s floppy drive will work). When a disc is inserted into the drive, the controller will look at the root of the disk for a file called "zaparoo.txt". The contents of this file are the same as the text in a standard Zaparoo nfc tag (See [ZapScript](https://wiki.zaparoo.org/ZapScript)). Until another disk is inserted, the motor of the drive will not spin.

## M4 Firmware Versions

| Filename                                           | Redetect on Insert | Polling Type               |  
|----------------------------------------------------|--------------------|----------------------------|
| firmware_no_redetect_index_polling.uf2             | No                 | Index                      |
| firmware_no_redetect_write_polling.uf2             | No                 | Write Protect              |
| firmware_no_redetect_write_polling_inverse.uf2     | No                 | Write Protect (Inverse)    |
| firmware_redetect_index_polling.uf2                | Yes                | Index                      |
| firmware_redetect_write_polling.uf2                | Yes                | Write Protect              |
| firmware_redetect_write_polling_inverse.uf2        | Yes                | Write Protect (Invverse)   |


## Why Poll Using Write Protection?
The floppy drive bus does not have a dedicated pin that triggers when a disk is inserted. Other singals like the "READY" and "DISK CHANGE" only change when a new disk is inserted and the motor spins up. Polling those signals could cause the drive to constantly spin, adding additional wear and tear on the device. "WRITE PROTECT" is active only when a protected disk is in the drive and it will change when removed, allowing for motor free detection. Some drives will always return "write protected" true if no disk is inserted, other will aways return false. For this purpose, you can set the WRITE_FLAG value to which ever you drive expects (set to 1 your drive reports false by default, 0 if it reports true). As a result, your disk must be set to the opposite of the default value (ie if your drive reports false, your disk needs to be set to write-protected).


## Why Poll Using Index Pulse?
The floppy interface doesn’t provide a true “disk inserted” signal on its own. Signals like "READY" and "DISK CHANGE" only become valid after the drive has spun up and the controller has interacted with the media. Using the "INDEX" pulse works because it’s always generated when a disk is actually spinning. By asserting the motor signal and watching for index pulses, you can confirm that a disk is present. On many drives, the motor won’t spin at all if no disk is inserted, so you avoid unnecessary movement while still getting a reliable indication. Once the disk is spinning and index pulses are detected, stepping the head will properly update the "DISK CHANGE" signal. The "DISK CHANGE" signal will not change until the disk is ejected, allowing the mottor to spin down.
The main advantage of this method is faster detection, since the motor will spin sooner. The trade-off is potential wear if the motor doesn't shutdown if a disk is eject. In all drives,  there is an additional amount of head movement to refresh the "DISK CHANGE" state after a index pulse is detected.

# What is Redetect on Insert?
This setting controllers whether or not the drive will attempt to detect the floppy disk format on every insert, or if it will assume the disk is the same format as the last. The tradeoffs are speed. Redecticing the format each insert adds about 400ms to each insert, but is consitent between 720kb and 1.44mb disks. Without this enabled, inserting a different different formatted disk will take about an extra 2.5 seconds to load.

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

## Developer Setup (Test on Windows And Arch)
If you want to compile from source instead of the prebuilt firmware.
1. Download and install Visual Studio Code
2. Install the [Platformio Extension](https://platformio.org/)
3. Open the project folder within Visual Studio Code.
4. Once the project loads (first time takes a while), use the bottom bar to switch to your preferred environment configuration. Without changing this platforio will build and upload every environment on the next step. 
5. Upload the firmware (buttons are also found on the bottom bar).
6. Open a serial terminal to view the output.
