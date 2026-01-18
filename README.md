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
The STM32 Blackpill is a very powerful and low profile microcontroller based on the STM32F411CEU6 processor. I chose this board compared to the ESP32 or Arduino to gain experience in developing with the STM32 board.

<img width="425" height="247" alt="image" src="https://github.com/user-attachments/assets/0922252b-bd32-4584-8224-7a9963c0a52f" />

## Schematic
Below is the schematic for the demo including two STM32 Blackpill boards, an OLED screen, and a DHT11 temperature and humidity sensor. Refer to the attached files in the repository for the transmitter and receiver code.

<img width="3000" height="1829" alt="circuit_image (1)" src="https://github.com/user-attachments/assets/1f9da096-edb5-4659-ad2d-d9654f5292cf" />


# Finished Product
Once the header is packed on the transmitter side, the header is sent in a packet via UART between the two STM32 boards. On the receiver side, the packet is decoded and shown via the serial monitor that shows the various flags and data sent from the transmitter.

``` cpp
Version=0 Packet Type=0 PID=1 Secondary Flag=0 Sequence=91 Temperature=23 C Humidity=36 %
Version=0 Packet Type=0 PID=1 Secondary Flag=0 Sequence=92 Temperature=23 C Humidity=37 %
Version=0 Packet Type=0 PID=1 Secondary Flag=0 Sequence=93 Temperature=23 C Humidity=36 %
```

Then, the temperature and humidity is displayed on the OLED screen, showing a sucessful data transfer.

# Conclusion
This repository showed an implementation of the CCSDS Space Packet Protocol for data transmission between two devices. This project is intended as a learning reference for understanding CCSDS packets and how to apply this concept to embedded systems. 

Feel free to take what I did and expand on it.
