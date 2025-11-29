// Including libaries
#include <SPI.h> //nRF works ove the SPI-Interface
#include <nRF24L01.h>
#include <RF24.h>

//TODO: Modify pin values!!
// Defining pins for nRF-Module
#define CE_PIN 4
#define CSN_PIN 5


RF24 radio(CE_PIN, CSN_PIN);

//nRF-Modules use 5-Byte adresses
//they can only reach out to eachother, if both use the same address
//Will only be definied for testing, proper name will be defined in main-sketch
const byte address[5] = {'T','e','s','t','1'};


// We want to send a compressed packet, which consists of 32 Bytes (we should be able to fit all relevant informations into this packet)
// Note, that this packet will only be send by the car, the receiver will send a significantly smaller packet, to reduce the traffic (that is the plan for now)
// parameter "packed" means, the struct will exactly be 32 Bytes, otherwise it could be filled up with more bytes-to fit better into the storage-units
// We will call this packet: TeleCmdPacket
// NOTE: I hope there will be time, to test sending smaller packets shortly after each-other to speed up the process. But 32-Bytes should probably be small enough to not cause a lot of trouble
struct __attribute__((packed)) TeleCmdPacket {
  // When will the data be send (maybe it will be part in the final sketch, but for now it is just for debugging)
  uint32_t timestamp;
  // Specified Number for packet, to determine if packets got lost (maybe implemented in final sketch, further tests needed)
  uint16_t seq;
  // Dummy-Values to fill the rest of the struckt with values, that represent the data, that will be send later
  int16_t throttle;
  int16_t steer;
  uint8_t mode;
  uint8_t flags;
  uint16_t battery_mv;
  int16_t speed_cm_s;
  int16_t temp_cx10;
  uint8_t rssi_est;
  // For now we have 11 unused bytes in the packet, they will be filled later
  uint8_t reserved[11];
  // checksum, to validate the values that are send by the device (it is possible that values get corrupted due to interference while transmittig) -> Preventing faulty values
  uint16_t crc16;
};
// make shure, that send packet is exactly 32 byte long
// if the size of the send packets is constant, the handling will get a lot easier (that is the reason why maybe both datapackets will be 32 byte large)
static_assert(sizeof(TeleCmdPacket) == 32, "Packet must be 32 bytes");

// function to calculate a 16 bit (2 Byte) checksum over the other 30 bytes of the packet
uint16_t crc16_ccitt(const uint8_t *data, size_t len) {
  uint16_t crc = 0xFFFF;
  while (len--) {
    crc ^= (uint16_t)(*data++) << 8;
    for (int i = 0; i < 8; ++i) {
      if (crc & 0x8000)
        crc = (crc << 1) ^ 0x1021;
      else
        crc <<= 1;
    }
  }
  return crc;
}

uint16_t seqCounter = 0;

void setup() {
  // Just for Debugg-Output
  Serial.begin(115200);
  // initializing the nRF-Module
  radio.begin();
  // setting channel to 100, should theoretically prevent WiFi-interferance
  radio.setChannel(100);
  // setting speed for transmission, 1mb/s; should be stable enough
  radio.setDataRate(RF24_1MBPS);
  radio.setPALevel(RF24_PA_HIGH);
  // setting pyload size (every message is 32 bytes long)
  radio.setPayloadSize(32);
  // setting address for transmission only between our modules
  radio.openWritingPipe(address);
  // setting nRF-module to sending
  radio.stopListening();

  Serial.println("== NRF24 SENDER TEST STARTED ==");
}

void loop() {
  // creats empty packet
  TeleCmdPacket pkt;

  // -defining rondom dummy values for the packet 
  pkt.timestamp = millis();
  pkt.seq = seqCounter++;
  pkt.throttle = random(-1000, 1000);
  pkt.steer = random(-1000, 1000);
  pkt.mode = 1;
  pkt.flags = 0;
  pkt.battery_mv = 12000 + random(-50, 50);
  pkt.speed_cm_s = random(0, 500);
  pkt.temp_cx10 = 230 + random(-30, 30);
  pkt.rssi_est = random(0, 255);

  memset(pkt.reserved, 0xAB, sizeof(pkt.reserved));

  // setting the checksum
  pkt.crc16 = crc16_ccitt((uint8_t*)&pkt, 30);

  // Send as fast as possible (without much delay)
  radio.write(&pkt, sizeof(pkt));
}
