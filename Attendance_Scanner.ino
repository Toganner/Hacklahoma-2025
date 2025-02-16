// Writes multiple records to an NFC formatted tag. Note this erases all existing records.

#include <SPI.h>
#include <MFRC522.h>
#include "NfcAdapter.h"

#define CS_PIN 10

const int modePin = 4;
const int startPin = 5;
int buttonState = 0;
int flag = 0;
int previousModeState = LOW;
int previousStartState = LOW;
int modeState = LOW;
int startState = LOW;

MFRC522 mfrc522(CS_PIN, UINT8_MAX); // Create MFRC522 instance

NfcAdapter nfc = NfcAdapter(&mfrc522);


void write() {
    if (nfc.tagPresent()) {
        Serial.println("Writing multiple records to NFC tag");
        NdefMessage message = NdefMessage();
        message.addTextRecord("Hello, Arduino!");
        message.addUriRecord("https://arduino.cc");
        message.addTextRecord("Goodbye, Arduino!");
        boolean success = nfc.write(message);
        if (success) {
            Serial.println("\tSuccess. Try reading this tag with your phone.");
            delay(10000);
        } else {
            Serial.println("\tWrite failed");
        }
    }
}

void read() {
      if (nfc.tagPresent())
    {
        Serial.println("Reading NFC tag");
        NfcTag tag = nfc.read();
        Serial.println(tag.getTagType());
        Serial.print("UID: ");Serial.println(tag.getUidString());
    }
}

void setup() {
    Serial.begin(9600);
    Serial.println("NDEF multiple record writer\nPlace a formatted Mifare Classic or Ultralight NFC tag on the reader.");
    SPI.begin();        // Init SPI bus
    mfrc522.PCD_Init(); // Init MFRC522
    nfc.begin();

    pinMode(2, OUTPUT);
    pinMode(3, OUTPUT);
    pinMode(modePin, INPUT_PULLUP);
    pinMode(startPin, INPUT_PULLUP);

    digitalWrite(2,LOW);
    digitalWrite(3,HIGH);
}

void loop() {

    modeState = digitalRead(modePin);
    startState = digitalRead(startPin);

    if(previousModeState == HIGH && modeState == LOW) {
      if (flag == 0) {
        digitalWrite(2,HIGH);
        digitalWrite(3,LOW);
        flag = 1; // write mode
      }
      else {
        digitalWrite(2,LOW);
        digitalWrite(3,HIGH);
        flag = 0; // read mode
      }
      delay(100);
    }
    if(previousStartState == HIGH && startState == LOW) {
      if (flag == 0) {
        Serial.println("read");
        read();
      }
      else {
        Serial.println("write");
        write();
      }
      delay(100);
    }

    previousModeState = modeState;
    previousStartState = startState;
}
