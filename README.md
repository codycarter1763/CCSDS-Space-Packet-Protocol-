# CCSDS Space Packet Protocol Implementation
![IMG_6527](https://github.com/user-attachments/assets/5b1bc005-f0f8-4895-b93a-ca8dfa29fd1c)

# About
This repository shows an implementation of the CCSDS Space Packet Protocol for data transmission between two devices. Space Packet Protocol is a standardized format used to wrap data packets that are sent between spacecraft systems. To gain experience in Space Packet Protocol and use it to transfer useful information, this design is implemented on two STM32 Blackpill microcontrollers. One board transmits DHT11 temperature and humidity data via UART, and the other board collects this data and outputs this on an OLED screen.

This project is intended as a learning reference for understanding CCSDS packets and how to apply this concept to embedded systems. 

# Space Packet Protocol Structure
## Primary Header
<img width="723" height="300" alt="image" src="https://github.com/user-attachments/assets/743a0b24-9c1c-4f38-8d34-a411d2180c0a" />

Above shows the packet format for the primary header. According to the documentation:

### Packet Version Number
3 bit field that determines packet version. Used as a way to differentiate between different space packet structures.

### Packet Type
Indicates the packet function:

- 0 — Telemetry packet
- 1 — Telecommand packet

This field allows receiving systems to distinguish between data flowing from the spacecraft and commands sent to it.

### Secondary Header Flag
Denotes whether a secondary header is attached:

- 0 — No secondary header
- 1 — Secondary header present

### Application Process Identifier
Identifies the source, destination or subsystem associated with the packet.

### Sequence Flags
Define how the packet relates to other packets in a sequence:

- 00 — Continuation packet
- 01 — First packet
- 10 — Last packet
- 11 — Standalone packet

This enables packet segmentation and reassembly when larger data blocks are split across multiple packets. In the case for this project, all of the data is a standalone packet.

### Packet Sequence Count or Packet Name
Used to uniquely identify packets within a sequence or to provide a packet name.
For this project, this field is incremented with each transmitted packet.

### Packet Data Length
Specifies the length of the packet data field. Can be used to determine the total packet size, error detection, and payload extraction.

## Secondary Header
<img width="426" height="250" alt="image" src="https://github.com/user-attachments/assets/32b894dd-7f71-4127-9ae0-8d3f7c97878e" />

Above shows the packet format for the optional secondary header to transmit additional data. According to the documentation:

### Time Code Field
Mission specific data block where data such as timestamps like elapsed time, sent time, etc, can be sent. While not required for operation, the option is there for the mission if need be.

### Ancillary Data Field
Optional data block to transfer any additonal information not transmitted by the primary header if the mission requires it.

# Implementation on the STM32 Blackpill
## Schematic
## Code Analysis

# Finished Product

# Conclusion
