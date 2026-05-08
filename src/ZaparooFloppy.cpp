#include "ZaparooFloppy.hpp"
#include "FloppyPins.hpp"
#include <Adafruit_Floppy.h>
#include <Arduino.h>
#include <SPI.h>
#include <SdFat.h>

void setup();
void loop();
void readyPinTrigger();
void indexPinTrigger();
void readDisk();
void parseZaparoo();
void indexPoll();
bool getInserted();

Adafruit_Floppy floppy(DENSITY_PIN, INDEX_PIN, SELECT_PIN, MOTOR_PIN, DIR_PIN, STEP_PIN, WRDATA_PIN, WRGATE_PIN,
                       TRK0_PIN, PROT_PIN, READ_PIN, SIDE_PIN, READY_PIN);
Adafruit_MFM_Floppy mfm_floppy(&floppy, DEFAULT_DISK_FORMAT);

enum SystemState { WAITING_FOR_INDEX, WAITING_FOR_READY };
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
#if defined(FLOPPY_DIRECTION_PIN)
  pinMode(FLOPPY_DIRECTION_PIN, OUTPUT);
  digitalWrite(FLOPPY_DIRECTION_PIN, HIGH);
#endif
#if defined(FLOPPY_ENABLE_PIN)
  pinMode(FLOPPY_ENABLE_PIN, OUTPUT);
  digitalWrite(FLOPPY_ENABLE_PIN, LOW); // do second after setting direction
#endif
  mfm_floppy.begin();
  mfm_floppy.inserted(DEFAULT_DISK_FORMAT);
#if USE_INDEX_POLLING
  lastReadyState = digitalRead(READY_PIN);
  attachInterrupt(digitalPinToInterrupt(READY_PIN), readyPinTrigger, CHANGE);
  attachInterrupt(digitalPinToInterrupt(INDEX_PIN), indexPinTrigger, RISING);
#endif
}

void loop() {
  bool inserted = getInserted();
  if (newDisk && inserted) {
    readDisk();
  } else if (!newDisk && !inserted) {
    newDisk = true;
    lastCommand = "";
    floppy.spin_motor(true);
  }
  indexPoll();
  if (lastCommand.length()) {
    Serial.print(lastCommand);
    Serial.flush();
  }
  delay(200);
}

inline bool getInserted() {
#if USE_INDEX_POLLING
  return lastReadyState == HIGH;
#else
  return floppy.get_write_protect() == WRITE_FLAG;
#endif
}

inline void indexPoll() {
#if USE_INDEX_POLLING
  if (readyPinChanged) {
    readyPinChanged = false;
    if (lastReadyState == HIGH) {
      floppy.spin_motor(false);
    } else {
      floppy.spin_motor(true);
    }
  }
  if (indexTriggered) {
    indexTriggered = false;
  }
#endif
}

void readDisk() {
#if RE_DETECT
  mfm_floppy.inserted(AUTODETECT);
#endif
  fatfs.begin(&mfm_floppy);
  newDisk = false;
  bool couldReadDisk = root.open("/");
  if (!couldReadDisk) {
    fatfs.end();
    mfm_floppy.inserted(AUTODETECT);
    fatfs.begin(&mfm_floppy);
    couldReadDisk = root.open("/");
  }
  if (!couldReadDisk) {
    Serial.println("open root failed, check disk");
  } else {
    parseZaparoo();
    root.close();
  }
  fatfs.end();
  floppy.goto_track(TARGET_TRACK);
  floppy.spin_motor(false);
}

void parseZaparoo() {
  lastCommand = "";
  
  while (file.openNext(&root, O_RDONLY)) {
    char name[25];
    file.getName(name, 25);
    if (strcasecmp(name, "tapto.txt") == 0 || strcasecmp(name, "zaparoo.txt") == 0) {
      lastCommand.reserve(file.size() + 10);
      lastCommand = "SCAN\t";
      char buffer[513]; // Extra byte for null terminator
      while (file.available()) {
        int bytesRead = file.read(buffer, 512);
        if (bytesRead > 0) {
          buffer[bytesRead] = '\0';
          lastCommand += buffer;
        }
      }
      file.close();
      lastCommand += "\n";
      break;
    }
    
    file.close();
  }
}