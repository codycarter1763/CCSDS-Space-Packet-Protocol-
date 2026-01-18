/******************************************
 * CCSDS Space Packet Protocol Receiver
 * Author: Cody Carter
 * Date: January 2026
 * Version: 1.0.0
 * 
 * This firmware handles the receiving of the space packet protocol header via UART
 * with displaying to the serial monitor and OLED display.
 ******************************************/

#include <Arduino.h>
#include <stdint.h>
#include "DHT.h"
#include <Wire.h>

uint16_t sequenceCount = 0;

// Packet ID
#define version_Shift 13
#define packet_Type_Shift 12
#define secondary_Header_Flag_Shift 11
#define process_ID_Mask 0x07FF // Keeps lower 11 bits and clears everything else

// Sequence Control
#define sequence_Flag_Shift 14
#define sequence_CountName_Mask 0x3FFF // Keeps lower 14 bits and clears everything else

// DHT 11 Sensor
#define DHT11_PIN PB0
DHT dht11(DHT11_PIN, DHT11);

struct Primary_Header {
  uint16_t packet_ID; // Version num (3 bits) + type (1 bit) + secondary flag (1 bit) + proccess ID (11 bits)
  uint16_t sequence_Control; // Sequence flags (2 bits) + sequence count or name (14 bits)
  uint16_t data_Length; // Data length (14 bits)
};

// For use with temperature and humidity sensor
struct Secondary_Header { 
  int16_t temperature_C; 
  uint16_t humidity; 
} __attribute__((packed)); // Makes sure the header size is exactly 32 bits or 4 bytes

// Packed primary + secondary packet
struct Telemetry_Packet {
  Primary_Header primary;
  Secondary_Header secondary;
} __attribute__((packed));

static inline uint16_t build_PacketID(
  uint8_t version,
  uint8_t packet_Type,
  uint8_t secondary_Header_Flag,
  uint16_t process_ID
) {
  return ((version & 0x7) << version_Shift) |
         ((packet_Type & 0x1) << packet_Type_Shift) |
         ((secondary_Header_Flag & 0x1) << secondary_Header_Flag_Shift) |
         (process_ID & process_ID_Mask);
}

static inline uint16_t build_Sequence_Control(
  uint8_t sequence_Flags, // Sequence flags
  uint16_t sequence_CountName // Packet #
) {
  return ((sequence_Flags & 0x3) << sequence_Flag_Shift) |
         (sequence_CountName & sequence_CountName_Mask);
}

void sendPacketUART(const Telemetry_Packet& packet) {
  uint16_t len = sizeof(Telemetry_Packet);

  Serial1.write((uint8_t*)&len, sizeof(len));   // length prefix
  Serial1.write((uint8_t*)&packet, len);        // packet bytes
}

void setup() {
  Serial.begin(115200);        // USB debug
  Serial1.begin(115200);       // UART TX
  sequenceCount = 0;
  
  Wire.begin();
  dht11.begin();
}

void loop() {
  Telemetry_Packet packet{};

  // Packet ID
  packet.primary.packet_ID = build_PacketID(
      0,   // CCSDS version
      0,   // Telemetry
      1,   // Secondary header present
      0x01 // APID
  );

  // Sequence control 
  packet.primary.sequence_Control = build_Sequence_Control(
      3,              // Standalone packet
      sequenceCount++
  );

  packet.primary.data_Length = sizeof(Secondary_Header) - 1;

  
  float temperature = dht11.readTemperature();
  float humidity = dht11.readHumidity();


  Serial.println(temperature);
  Serial.println(humidity);

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("DHT read failed");
    return;
  }

  packet.secondary.temperature_C = (int16_t)(temperature * 100);
  packet.secondary.humidity = (uint16_t)(humidity * 100);

  sendPacketUART(packet);

  delay(2000);
}