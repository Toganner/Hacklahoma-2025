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

MFRC522 mfrc522(CS_PIN, UINT8_MAX); // Create MFRC522 instance

NfcAdapter nfc = NfcAdapter(&mfrc522);

String byteArrayToString(const byte* byteArray, size_t length) {
    String result = "";
    for (size_t i = 0; i < length; i++) {
        result += (char)byteArray[i];
    }
    return result;
}

void write() {
  
    if (nfc.tagPresent()) {
        Serial.println("\n--- NFC Tag Detected ---");
        //Serial.println("Cleaning Card");
        //nfc.clean();
        //Serial.println("Formatting Card");
        nfc.format();

        Serial.println("Writing multiple records to NFC tag");
        NdefMessage message = NdefMessage();
        message.addTextRecord("Hello, Arduino!");
        // message.addUriRecord("https://arduino.cc");
        message.addTextRecord("Goodbye, Arduino!");
        boolean success = nfc.write(message);
        if (success) {
            Serial.println("\tSuccess. Try reading this tag with your phone.");
            delay(5000);
        } else {
            Serial.println("\tWrite failed");
        }
    }
    else {
    Serial.println("No NFC tag detected.");
    return;
    }
}

void read() {
  // Serial.println("Start Read Funtion");
    if (nfc.tagPresent()) {
        Serial.println("\n--- NFC Tag Detected ---");

        NfcTag tag = nfc.read();

        // Serial.print("Tag Type: ");
        // Serial.println(tag.getTagType());

        // Serial.print("UID: ");
        // Serial.println(tag.getUidString());

        if (tag.hasNdefMessage()) {
            NdefMessage message = tag.getNdefMessage();
            int recordCount = message.getRecordCount();

            // Serial.print("This NFC Tag contains ");
            // Serial.print(recordCount);
            // Serial.println(" NDEF record(s).");
            // Serial.println("-----------------------");

            // Loop through all records in the NDEF message
            for (int i = 0; i < recordCount; i++) {
                NdefRecord record = message.getRecord(i);
                int payloadLength = record.getPayloadLength();
                const byte* payload = record.getPayload();

                // Skip empty records
                if (payloadLength == 0 || payload == nullptr) {
                    // Serial.print("Skipping empty record ");
                    // Serial.println(i + 1);
                    continue;
                }

                // Serial.print("Record ");
                // Serial.println(i + 1);
                // Serial.print("TNF: ");
                // Serial.println(record.getTnf());

                // Convert and print record type
                // Serial.print("Type: ");
                String typeStr = "";
                for (int j = 0; j < record.getTypeLength(); j++) {
                    typeStr += (char)record.getType()[j];
                }
                // Serial.println(typeStr);

                // Print payload in HEX format
                // Serial.print("Payload (HEX): ");
                // for (int j = 0; j < payloadLength; j++) {
                //     if (payload[j] < 0x10) Serial.print("0");  // Add leading zero for formatting
                //     Serial.print(payload[j], HEX);
                //     Serial.print(" ");
                // }
                // Serial.println();

                // Convert and print payload as string (if applicable)
                // Serial.print("Payload (Text): ");
                String payloadAsString = "";
                
                // Skip NDEF text record prefix if present (first byte is language code length)
                int textStartIndex = (typeStr == "T" && payloadLength > 1) ? payload[0] + 1 : 0;
                
                for (int j = textStartIndex; j < payloadLength; j++) {
                    payloadAsString += (char)payload[j];
                }
                Serial.println(payloadAsString);
                // Serial.println("-----------------------");
            }
        } else {
            Serial.println("No NDEF message found on this tag.");
        }
    }
    delay(1000);  // Avoid rapid consecutive reads
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
    if(previousStartState == HIGH && startState == LOW) {
      //Serial.println("Start button pressed"); // Debugging
    //while (modeState == LOW) {
      if (flag == 0) {
//        Serial.println("read");
        read();
      }
      else {
//        Serial.println("write");
        write();
      }
      delay(200);
      }
    //}

    previousModeState = modeState;
    previousStartState = startState;
}
