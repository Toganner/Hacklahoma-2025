#define CS_PIN 10
#define NDEF_USE_SERIAL



#include <SPI.h>
#include <MFRC522.h>
#include "NfcAdapter.h"
#include <NDEF.h>

const int modePin = 4;
const int startPin = 5;
int buttonState = 0;
int flag = 0;
int previousModeState = LOW;
int previousStartState = LOW;
int modeState = LOW;
int startState = LOW;

// Flag to track if "No tag present" has been printed
bool tagPresentMessagePrinted = false;

MFRC522 mfrc522(CS_PIN, UINT8_MAX); // Create MFRC522 instance

NfcAdapter nfc = NfcAdapter(&mfrc522);

String byteArrayToString(const byte* byteArray, size_t length) {
    String result = "";
    for (size_t i = 0; i < length; i++) {
        result += (char)byteArray[i];
    }
    return result;
}

MFRC522::StatusCode MIFARE_Ultralight_Read(byte page, byte *buffer, byte *bufferSize) {
    // Build command: 0x30 (READ) + page + 2-byte CRC
    byte cmd[4];
    cmd[0] = 0x30;
    cmd[1] = page;
    // Calculate the CRC for the first 2 bytes and store in cmd[2..3]
    mfrc522.PCD_CalculateCRC(cmd, 2, &cmd[2]);

    byte reply[18];    // Expect 16 bytes of data + 2 bytes of CRC from the card
    byte replySize = sizeof(reply);

    MFRC522::StatusCode status = mfrc522.PCD_TransceiveData(cmd, 4, reply, &replySize);
    if (status != MFRC522::STATUS_OK) {
        return status;
    }
    // We expect 18 bytes in response (16 data + 2 CRC)
    if (replySize != 18) {
        return MFRC522::STATUS_ERROR;
    }
    // Copy the 16 data bytes to the provided buffer
    for (byte i = 0; i < 16; i++) {
        buffer[i] = reply[i];
    }
    *bufferSize = 16;
    return MFRC522::STATUS_OK;
}

void convertIDtoBytes(const char id[10], byte byteData[16]) {
    memset(byteData, ' ', 16);  // Fill entire byte array with spaces (' ')
    strncpy((char*)byteData, id, 9);  // Copy up to 9 characters from id
}

void write(char id[]) {
 if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
        tagPresentMessagePrinted = false;
        // For Ultralight cards, no PICC_Select is needed.
        
        byte startPage = 4;  // Typically the first writable page on Ultralight
        // Prepare 16 bytes of data (4 pages x 4 bytes each)

      //char id[10] = "123456789"; // Changeable ID (9 digits + null terminator)
      byte data[16]; // 16-byte storage

      convertIDtoBytes(id, data); // Convert ID to bytes

        // byte data[16] = { 'D','u','m','m',
        //                   'y',' ','T','e',
        //                   'x','t',' ','1',
        //                   '2','3','4','5' };
        
        // Write data one page (4 bytes) at a time.
        for (byte i = 0; i < 4; i++) {
            byte pageBuffer[4];
            for (byte j = 0; j < 4; j++) {
                pageBuffer[j] = data[i * 4 + j];
            }
            MFRC522::StatusCode status = mfrc522.MIFARE_Ultralight_Write(startPage + i, pageBuffer, 4);
            if (status != MFRC522::STATUS_OK) {
                Serial.print("Write failed for page ");
                Serial.println(startPage + i);
                mfrc522.PICC_HaltA();
                return;
            }
        }
        Serial.println("Write success!");
        mfrc522.PICC_HaltA();
    } else {
        if (!tagPresentMessagePrinted) {
            // Serial.println("No tag present.");
            tagPresentMessagePrinted = true;
        }
    }
}

void read() {
      if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
        tagPresentMessagePrinted = false;
        // For Ultralight cards, no PICC_Select is needed.
        
        byte startPage = 4; // starting page for read (pages 4-7)
        byte readBuffer[18]; // buffer to hold 16 data bytes + 2 CRC bytes from the read function
        byte size = sizeof(readBuffer);
        
        MFRC522::StatusCode status = MIFARE_Ultralight_Read(startPage, readBuffer, &size);
        if (status != MFRC522::STATUS_OK) {
            // Serial.print("Read failed for page ");
            // Serial.println(startPage);
        } else {
            // Serial.print("Data read from pages ");
            // Serial.print(startPage);
            // Serial.print(" to ");
            // Serial.print(startPage + 3);
            // Serial.print(": ");
            // Print the 16 data bytes.
            // for (byte i = 0; i < 16; i++) {
            //     Serial.write(readBuffer[i]);
            // }
            // Serial.println();
            char idString[17];  // 16 bytes + null terminator
            memcpy(idString, readBuffer, 16); // Copy bytes into the string
            idString[16] = '\0'; // Ensure null termination

            Serial.println(idString); // Print the stored string
        }
        mfrc522.PICC_HaltA();
    } else {
        if (!tagPresentMessagePrinted) {
            // Serial.println("No tag present.");
            tagPresentMessagePrinted = true;
        }
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
        Serial.println("Write Mode");
      }
      else {
        digitalWrite(2,LOW);
        digitalWrite(3,HIGH);
        flag = 0; // read mode
        Serial.println("Read Mode");
      }
      delay(200);
    }

      if (flag == 0) {
        read();
      }
      else {
        char id[10] = "113602867";
        write(id);
      }
      delay(200);

    previousModeState = modeState;
    previousStartState = startState;
}
