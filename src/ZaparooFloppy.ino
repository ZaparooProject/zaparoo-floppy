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
Adafruit_MFM_Floppy mfm_floppy(&floppy, DEFAULT_DISK_FORMAT);

FatVolume fatfs;

File32 root;
File32 file;
bool newDisk = true;
String lastCommand = "";
volatile bool readyPinChanged = false;
volatile int lastReadyState = HIGH;
volatile bool indexInterruptEnabled = true;
volatile bool indexTriggered = false;
volatile bool waitingForReadyAfterStep = false;
enum SystemState { WAITING_FOR_INDEX, WAITING_FOR_READY };
volatile SystemState currentState = WAITING_FOR_INDEX;

void readyPinTrigger() {
  int currentReadyState = digitalRead(READY_PIN);
  if (currentReadyState != lastReadyState) {
    readyPinChanged = true;
    lastReadyState = currentReadyState;
    if (waitingForReadyAfterStep) {
      waitingForReadyAfterStep = false;
      currentState = WAITING_FOR_INDEX;
      indexInterruptEnabled = true;
      indexTriggered = false;
    }
  }
}

void indexPinTrigger() {
  if (indexInterruptEnabled && !indexTriggered && currentState == WAITING_FOR_INDEX) {
    indexTriggered = true;
    indexInterruptEnabled = false;
    currentState = WAITING_FOR_READY;
    waitingForReadyAfterStep = true;
    floppy.step(true, 1);
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    yield();
  }
  mfm_floppy.begin();
  mfm_floppy.inserted(DEFAULT_DISK_FORMAT);
  
#if USE_INDEX_POLLING
  lastReadyState = digitalRead(READY_PIN);
  attachInterrupt(digitalPinToInterrupt(READY_PIN), readyPinTrigger, CHANGE);
  attachInterrupt(digitalPinToInterrupt(INDEX_PIN), indexPinTrigger, RISING);
#else
  floppy.spin_motor(false);
#endif
}

void loop() {
#if USE_INDEX_POLLING
  bool inserted = lastReadyState == HIGH;
#else
  bool inserted = floppy.get_write_protect() == WRITE_FLAG;
#endif
  if(newDisk && inserted){
  #if RE_DETECT
    mfm_floppy.inserted(AUTODETECT);
  #endif
    floppy.spin_motor(true);
    fatfs.begin(&mfm_floppy);
    newDisk = false;
    bool couldReadDisk = root.open("/");
    if(!couldReadDisk){
      fatfs.end();
      mfm_floppy.inserted(AUTODETECT);
      fatfs.begin(&mfm_floppy);
      couldReadDisk = root.open("/");
    }
    if (!couldReadDisk) {
      Serial.println("open root failed, check disk");
    }else{
      parseZaparoo();
      root.close();
    }
    fatfs.end();
    floppy.goto_track(TARGET_TRACK);
    floppy.spin_motor(false);
  }else if(!newDisk && !inserted){
    newDisk = true;
    lastCommand = "";
  }
  
#if USE_INDEX_POLLING
  if(readyPinChanged){
    readyPinChanged = false;
    if(lastReadyState == HIGH){
      floppy.spin_motor(false);
    }else{
      floppy.spin_motor(true);
    }
  }
  if(indexTriggered){
    indexTriggered = false;
  }
#endif
  
  if(lastCommand.length()){
    Serial.print(lastCommand);
    Serial.flush();
  }
  delay(200);
}

void parseZaparoo() {
  lastCommand = "";
  while (file.openNext(&root, O_RDONLY)) {
    char name[25];
    file.getName(name, 25);
    String filename(name);
    file.close();
    if (filename.equalsIgnoreCase("tapto.txt") || filename.equalsIgnoreCase("zaparoo.txt")) {
      File32 dataFile = fatfs.open(filename, FILE_READ);
      lastCommand.reserve(dataFile.size() + 10); 
      lastCommand = "SCAN\t";
      char buffer[128];
      while (dataFile.available()) {
        int bytesRead = dataFile.read(buffer, sizeof(buffer));
        if (bytesRead > 0) {
          for (int i = 0; i < bytesRead; i++) {
            lastCommand += buffer[i];
          }
        }
      }
      dataFile.close();
      lastCommand += "\n";
      return;
    }
  }
}