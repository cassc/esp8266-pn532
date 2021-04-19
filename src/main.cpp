/**
 * Use PN532 on ESP8266 / Wemos D1 mini with SPI
 * Connect the following 6 wires from PN532 to D1:
 * - 5V
 * - GND
 * - SCK - D5
 * - MISO - D6
 * - MOSI - D7
 * - SSEL - D4
 * 
 * */

#include <Adafruit_PN532.h>
#include "Wire.h"

Adafruit_PN532 nfc(D5, D6, D7, D8);

// Adafruit_PN532 nfc(D4);

void setup(void)
{
  Serial.begin(115200);
  Serial.println("14CORE | NFC - SPI Test Code TAG Reader");
  Serial.println("Initializing please wait.......");
  delay(3000);
  nfc.begin();

  uint32_t versiondata;
  int i = 0;
  while (!(versiondata = nfc.getFirmwareVersion()))
  {
    Serial.print(".");
    i++;
    if (i % 80 == 0)
    {
      Serial.println();
    }
    delay(100);
  }
  // Got ok data, print it out!
  Serial.print("Device Found PN5 Chip");
  Serial.println((versiondata >> 24) & 0xFF, HEX);
  Serial.print("Firmware version > ");
  Serial.print((versiondata >> 16) & 0xFF, DEC);
  Serial.println((versiondata >> 8) & 0xFF, DEC);
  nfc.SAMConfig(); //Set to read RFID tags
  Serial.println("Waiting for an ISO14443A Card ...");
}

void loop(void)
{
  uint8_t success;
  uint8_t uid[] = {0, 0, 0, 0, 0, 0, 0}; // Buffer to store the returned UID
  uint8_t uidLength;                     // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

  if (success)
  {

    Serial.println("ISO14443A Card Detected > ");
    Serial.print(" UID Length: ");
    Serial.print(uidLength, DEC);
    Serial.println(" BYTES");
    Serial.print("  UID VAL: ");
    nfc.PrintHex(uid, uidLength);

    if (uidLength == 4)
    {
      Serial.println(" MIFACE CLASSIC CARD > (4 byte UID)");

      // Factory default keyA is 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF
      Serial.println("Authenticating block 4");
      uint8_t keya[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; //Default Key
      success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 4, 0, keya);

      if (success)
      {
        Serial.println("Sector 1 (Blocks 4..7) has been authenticated");
        uint8_t data[16];

        /* If you wanted to write something at block 4 you can, uncomment */
        // data = { 'a', 'd', 'a', 'f', 'r', 'u', 'i', 't', '.', 'c', 'o', 'm', 0, 0, 0, 0};
        // success = nfc.mifareclassic_WriteDataBlock (4, data);
        success = nfc.mifareclassic_ReadDataBlock(4, data);

        if (success)
        {
          Serial.println("Reading Block 4:");
          nfc.PrintHexChar(data, 16);
          delay(1000);
        }
        else
        {
          Serial.println("ERROR > Unable to read requested block");
        }
      }
      else
      {
        Serial.println("ERROR > Authentication Failed Please Try Again.");
      }
    }

    if (uidLength == 7)
    {

      Serial.println("MIFARE LIGHT TAG > (7 byte UID)");

      // Try to read the first general-purpose user page (#4)
      Serial.println("Getting page 4 > ");
      uint8_t data[32];
      success = nfc.mifareultralight_ReadPage(4, data);
      if (success)
      {
        nfc.PrintHexChar(data, 4);
        delay(1000);
      }
      else
      {
        Serial.println("ERROR > Unable to read requested page");
      }
    }
  }
}