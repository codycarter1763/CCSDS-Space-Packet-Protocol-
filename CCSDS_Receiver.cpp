/******************************************
 * CCSDS Space Packet Protocol Transmitter
 * Author: Cody Carter
 * Date: January 2026
 * Version: 1.0.0
 * 
 * This firmware handles the data collection of the DHT11 temperature and humidity
 * sensor, the building of the space packet protocol header, and data transmission
 * via UART.
 ******************************************/

#include <Arduino.h>
#include <stdint.h>
#include <Wire.h>
#include "ssd1306.h"

uint16_t g_temperatureC = 0.0;
uint16_t g_humidity = 0.0;

// Packet ID
#define version_Shift 13
#define packet_Type_Shift 12
#define secondary_Header_Flag_Shift 11
#define process_ID_Mask 0x07FF // Keeps lower 11 bits and clears everything else

// Sequence Control
#define sequence_Flag_Shift 14
#define sequence_CountName_Mask 0x3FFF // Keeps lower 14 bits and clears everything else

struct Primary_Header {
  uint16_t packet_ID; // Version num (3 bits) + type (1 bit) + secondary flag (1 bit) + process ID (11 bits)
  uint16_t sequence_Control; // Sequence flags (2 bits) + sequence count or name (14 bits)
  uint16_t data_Length; // Data length (14 bits)
};

// For use with temperature and humidity sensor
struct Secondary_Header { 
  uint16_t temperature_C; 
  uint16_t humidity; 
} __attribute__((packed)); // Makes sure the header size is exactly 32 bits or 4 bytes

// Packed primary + secondary packet
struct Telemetry_Packet {
  Primary_Header primary;
  Secondary_Header secondary;
} __attribute__((packed));

void decodePacket(const Telemetry_Packet& packet) {
  uint8_t version =
    (packet.primary.packet_ID >> version_Shift) & 0x07;

  uint8_t packet_Type =
    ((packet_Type & 0x1) >> packet_Type_Shift);

  uint8_t secondary_Header_Flag = 
    ((secondary_Header_Flag & 0x1) >> secondary_Header_Flag_Shift);

  uint16_t process_ID = 
    packet.primary.packet_ID & process_ID_Mask;

  uint8_t sequence_Flags =
    (packet.primary.sequence_Control >> sequence_Flag_Shift) & 0x03;

  uint16_t sequence_CountName =
    packet.primary.sequence_Control & sequence_CountName_Mask;

  g_temperatureC = packet.secondary.temperature_C / 100.0;
  g_humidity = packet.secondary.humidity / 100.0;

  Serial.printf(
    "Version=%u Packet Type=%u PID=%u S Sequence=%u Temperature=%u C Humidity=%u %%\n",
    version, packet_Type, process_ID, secondary_Header_Flag, sequence_CountName, g_temperatureC, g_humidity
  );
}

bool receivePacket(Telemetry_Packet &packet) {
  static uint16_t expectedLen = 0;

  // Read length prefix
  if (expectedLen == 0) {
    if (Serial1.available() >= sizeof(expectedLen)) {
      Serial1.readBytes((uint8_t*)&expectedLen, sizeof(expectedLen));

      if (expectedLen != sizeof(Telemetry_Packet)) {
        Serial.printf("Bad length: %u\n", expectedLen);
        expectedLen = 0;
        return false;
      }
    }
  }

  // Read packet
  if (expectedLen > 0 && Serial1.available() >= expectedLen) {
    Serial1.readBytes((uint8_t*)&packet, expectedLen);
    expectedLen = 0;
    return true;
  }

  return false;
}

void sendPacket(const Telemetry_Packet& packet) {
  uint16_t len = sizeof(Telemetry_Packet);

  Serial1.write((uint8_t*)&len, sizeof(len));      // length prefix
  Serial1.write((uint8_t*)&packet, len);           // CCSDS packet
}

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);  // UART link

  Wire.begin();        // PB7 SDA, PB6 SCL
  ssd1306_setFixedFont(ssd1306xled_font6x8);
  ssd1306_128x64_i2c_init();
  ssd1306_clearScreen();
}

void loop() {
  Telemetry_Packet packet;
  
  if (receivePacket(packet)) {
    decodePacket(packet);

    char line1[24];
    char line2[24];

    snprintf(line1, sizeof(line1), "Temperature: %u C", g_temperatureC);
    snprintf(line2, sizeof(line2), "Humidity : %u %%", g_humidity);

    ssd1306_clearScreen();
    ssd1306_printFixed(0,  0, line1, STYLE_NORMAL);
    ssd1306_printFixed(0, 16, line2, STYLE_NORMAL);
  }
}