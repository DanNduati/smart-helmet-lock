#include <SoftwareSerial.h>
#include <SPI.h>
#include <MFRC522.h>
#define testLed A0
#define lock 4

//RFID
constexpr uint8_t RST_PIN = 9;
constexpr uint8_t SS_PIN = 10;
MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;
//byte array for verified NUID
byte nuidStored[2][4] = {{83, 232, 253, 20}, {189, 217, 217, 115}};
byte nuidReadCard[4];

//BLUETOOTH
SoftwareSerial BTSerial(6, 5); // RX | TX 5/6
char inputByte;

boolean doorOpened = false;
boolean successRead = false;
void setup() {
  Serial.begin(9600);
  SPI.begin();
  BTSerial.begin(9600);
  pinMode(testLed, OUTPUT);
  pinMode(lock, OUTPUT);
  mfrc522.PCD_Init();
  mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max);
}

void loop() {
  while (!doorOpened) {
    //check for bt first
    if (BTSerial.available() > 0) {
      if (verifyBt(BTSerial.read())) {
        doorOpened = true;
        digitalWrite(testLed, 1);
        digitalWrite(lock,1);
        delay(1000);
        digitalWrite(testLed, 0);
        digitalWrite(lock,0);
        doorOpened = false;
      }
    }
    
    //check for rfid
    else if (getID()) {
      if (verifyRfid()) {
        doorOpened = true;
        digitalWrite(testLed, 1);
        digitalWrite(lock,1);
        delay(1000);
        digitalWrite(testLed, 0);
        digitalWrite(lock,0);
        doorOpened = false;
      }
    }
    else {
      return;
    }
  }

}

uint8_t getID() {
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return 0;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return 0;
  }
  for ( uint8_t i = 0; i < 4; i++) {
    nuidReadCard[i] = mfrc522.uid.uidByte[i];
  }
  mfrc522.PICC_HaltA(); // Stop reading
  return 1;
}

bool verifyRfid() {
  if ((mfrc522.uid.uidByte[0] == nuidStored[0][0] && mfrc522.uid.uidByte[1] == nuidStored[0][1] && mfrc522.uid.uidByte[2] == nuidStored[0][2] && mfrc522.uid.uidByte[3] == nuidStored[0][3]) ||
      (mfrc522.uid.uidByte[0] == nuidStored[1][0] && mfrc522.uid.uidByte[1] == nuidStored[1][1] && mfrc522.uid.uidByte[2] == nuidStored[1][2] && mfrc522.uid.uidByte[3] == nuidStored[1][3])) {
    //clear stored nuid
    for (int i = 0; i < 4; i++) {
      mfrc522.uid.uidByte[i] = 0;
    }
    return 1;
  }
  else {
    return 0;
  }
}

bool verifyBt(char inByte) {
  if (inByte == 49) {
    return 1;
  }
}

