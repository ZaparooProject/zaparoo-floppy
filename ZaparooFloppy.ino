#include <SPI.h>
#include <SdFat.h>
#include <Adafruit_Floppy.h>
#include "ZaparooFloppy.hpp"

#if defined(ADAFRUIT_FEATHER_M4_EXPRESS)
#define DENSITY_PIN A1 // IDC 2
#define INDEX_PIN A5   // IDC 8
#define SELECT_PIN A0  // IDC 12
#define MOTOR_PIN A2   // IDC 16
#define DIR_PIN A3     // IDC 18
#define STEP_PIN A4    // IDC 20
#define WRDATA_PIN 13  // IDC 22
#define WRGATE_PIN 12  // IDC 24
#define TRK0_PIN 10    // IDC 26
#define PROT_PIN 11    // IDC 28
#define READ_PIN 9     // IDC 30
#define SIDE_PIN 6     // IDC 32
#define READY_PIN 5    // IDC 34
#elif defined(ARDUINO_ADAFRUIT_FEATHER_RP2040)
#define DENSITY_PIN A1 // IDC 2
#define INDEX_PIN 25   // IDC 8
#define SELECT_PIN A0  // IDC 12
#define MOTOR_PIN A2   // IDC 16
#define DIR_PIN A3     // IDC 18
#define STEP_PIN 24    // IDC 20
#define WRDATA_PIN 13  // IDC 22
#define WRGATE_PIN 12  // IDC 24
#define TRK0_PIN 10    // IDC 26
#define PROT_PIN 11    // IDC 28
#define READ_PIN 9     // IDC 30
#define SIDE_PIN 8     // IDC 32
#define READY_PIN 7    // IDC 34
#ifndef USE_TINYUSB
#error "Please set Adafruit TinyUSB under Tools > USB Stack"
#endif
#elif defined(ARDUINO_RASPBERRY_PI_PICO)
#define DENSITY_PIN 2 // IDC 2
#define INDEX_PIN 3   // IDC 8
#define SELECT_PIN 4  // IDC 12
#define MOTOR_PIN 5   // IDC 16
#define DIR_PIN 6     // IDC 18
#define STEP_PIN 7    // IDC 20
#define WRDATA_PIN 8  // IDC 22 (not used during read)
#define WRGATE_PIN 9  // IDC 24 (not used during read)
#define TRK0_PIN 10   // IDC 26
#define PROT_PIN 11   // IDC 28
#define READ_PIN 12   // IDC 30
#define SIDE_PIN 13   // IDC 32
#define READY_PIN 14  // IDC 34
#ifndef USE_TINYUSB
#error "Please set Adafruit TinyUSB under Tools > USB Stack"
#endif
#else
#error "Please set up pin definitions!"
#endif

Adafruit_Floppy floppy(DENSITY_PIN, INDEX_PIN, SELECT_PIN,
                       MOTOR_PIN, DIR_PIN, STEP_PIN,
                       WRDATA_PIN, WRGATE_PIN, TRK0_PIN,
                       PROT_PIN, READ_PIN, SIDE_PIN, READY_PIN);
Adafruit_MFM_Floppy mfm_floppy(&floppy);

FatVolume fatfs;

File32 root;
File32 file;
bool newDisk = true;
bool inserted = true;
String lastCommand = "";

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    yield();
  }
  inserted = mfm_floppy.begin();
  floppy.spin_motor(false);
}

void loop() {
  bool inserted = floppy.get_write_protect();
  if(newDisk && inserted){
    floppy.spin_motor(true);
    fatfs.begin(&mfm_floppy);
    newDisk = false;
    bool couldReadDisk = root.open("/");
     if (!couldReadDisk) {
      Serial.println("open root failed, check disk");
    }else{
      parseTapTo();
      root.close();
    }
    fatfs.end();
    floppy.goto_track(TARGET_TRACK);
    floppy.spin_motor(false);
  }else if(!newDisk && !inserted){
    newDisk = true;
    lastCommand = "";
  }
  if(!lastCommand.equals("")){
    Serial.print(lastCommand);
    Serial.flush();
  }
  delay(200);
}

void parseTapTo(){
  lastCommand = "";
  while (file.openNext(&root, O_RDONLY)) {
    int length = 25;
    char name[length];
    file.getName(name, length);
    String filename = String(name);
    file.close();
    if(filename.equalsIgnoreCase("tapto.txt") || filename.equalsIgnoreCase("zaparoo.txt")){
      File32 dataFile = fatfs.open(filename, FILE_READ);
      while (dataFile.available()) {
        char c = dataFile.read();
        lastCommand += c;
      }
      lastCommand = "SCAN\t" + lastCommand + "\n";
      return;
    }
  }
}
