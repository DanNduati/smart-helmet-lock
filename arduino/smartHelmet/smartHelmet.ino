#include <SoftwareSerial.h>
#include <SPI.h>
#include <MFRC522.h>

//verification led
const int led = A0;
const int Lock = 4;
// Create MFRC522 instance.
constexpr uint8_t RST_PIN = 9;     // Configurable, see typical pin layout above
constexpr uint8_t SS_PIN = 10;

MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

//byte array for verified NUID
byte nuidStored[2][4] = {{83, 232, 253, 20}, {189, 217, 217, 115}};
//byte array for read card
byte nuidReadCard[4];
//bluetooth
SoftwareSerial BTSerial(6, 5); // RX | TX 5/6
char inputByte;

//unlock flag
bool unlock = false;


void setup() {
  pinMode(lock,OUTPUT);
  pinMode(led, OUTPUT);
  Serial.begin(9600);
  SPI.begin();
  BTSerial.begin(9600);
  mfrc522.PCD_Init();
  //If you set Antenna Gain to Max it will increase reading distance
  mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max);
  //digitalWrite(led,0);
}


void loop() {
  lock();
  //(!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) ||
  //!(mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) ||
  //!BTSerial.available()
  //if (!BTSerial.available() && !(mfrc522.PICC_IsNewCardPresent()))
  //return;
  

  if (btPresent) {
    if (checkBt()) {
      unlock = true;
      openlock();
      return;
    }
    else {
      lock();
      unlock = false;
    }
  }
  else if (rfidPresent()) {
    if (checkRfid()) {
      unlock = true;
      openlock();
    }
    else {
      lock();
      unlock = false;
    }
  }
  else {
    return;
  }
		
  }

bool btPresent() {
  if (BTSerial.available()) {
    return 1;
  }
  else {
    return 0;
  }
}
bool rfidPresent() {
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    return 1;
  }
  else {
    return 0;
  }

}
bool checkRfid() {
  if (!mfrc522.PICC_IsNewCardPresent())
    return;
  //digitalWrite(led,HIGH);
  //ensure the NUID has been read
  if (!mfrc522.PICC_ReadCardSerial())
    return;
  Serial.println("Card Read!!");
  //verify if the card read is a valid card(2)
  if ((mfrc522.uid.uidByte[0] == nuidStored[0][0] && mfrc522.uid.uidByte[1] == nuidStored[0][1] && mfrc522.uid.uidByte[2] == nuidStored[0][2] && mfrc522.uid.uidByte[3] == nuidStored[0][3]) ||
      (mfrc522.uid.uidByte[0] == nuidStored[1][0] && mfrc522.uid.uidByte[1] == nuidStored[1][1] && mfrc522.uid.uidByte[2] == nuidStored[1][2] && mfrc522.uid.uidByte[3] == nuidStored[1][3])) {
    //match
    Serial.println("Match!!");
    //granted();
    return 1;
  }
  else {
    //digitalWrite(led,0);
    //delay(1000);
    Serial.println("No Match!!");
    return 0;
  }
}

bool checkBt() {
  inputByte = BTSerial.read();
  //Serial.println(inputByte);
  if (inputByte == 49) {
    return 1;
  }
  else if (inputByte == 48) {
    return 0;
  }
}

void openlock() {
  digitalWrite(led, 1);
  digitalWrite(Lock, 1);
  delay(1000);
}


void lock() {
  digitalWrite(led, 0);
  digitalWrite(Lock, 0);
  delay(1000);
}
