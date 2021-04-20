/**
 * Use PN532 on ESP8266 / Wemos D1 mini with SPI
 * Connect the following 6 wires from PN532 to D1:
 * - 5V
 * - GND
 * - SCK - D5
 * - MISO - D6
 * - MOSI - D7
 * - SSEL - D4
 * */

#include <Adafruit_PN532.h>
#include "Wire.h"

// clk, mi, mo, ss
// Adafruit_PN532 nfc(D5, D6, D7, D8);

// Need to Adafruit_PN532.cpp to make it reliable, see
// https://github.com/adafruit/Adafruit-PN532/issues/80#issuecomment-650817585
Adafruit_PN532 nfc(D4);

String bytes_to_string(byte *buffer, byte bufferSize)
{
  String r;
  for (byte i = 0; i < bufferSize; i++)
  {
    char buf[3];
    sprintf(buf, "%02X", *(buffer + i));
    r += buf;
  }
  return r;
}

void setup(void)
{
  Serial.begin(115200);
  Serial.println("Initializing please wait.......");

  pinMode(D1, OUTPUT);
  digitalWrite(D1, HIGH);

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
  // Got o
  Serial.print("Device Found PN5 Chip");
  Serial.println((versiondata >> 24) & 0xFF, HEX);
  Serial.print("Firmware version > ");
  Serial.print((versiondata >> 16) & 0xFF, DEC);
  Serial.println((versiondata >> 8) & 0xFF, DEC);
  nfc.SAMConfig(); //Set to read RFID tags
  digitalWrite(D1, LOW);
}

void loop(void)
{
  digitalWrite(D1, LOW);

  uint8_t uid[] = {0, 0, 0, 0, 0, 0, 0}; // Buffer to store the returned UID
  uint8_t uidLength;                     // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  auto success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A,  uid, &uidLength);

  if (success && uidLength > 0)
  {
    digitalWrite(D1, HIGH);
    String id = bytes_to_string(uid, uidLength);
    Serial.println(String("Found card: ") + id);
  }
}
