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

// We want to recaeive the exact same packet that we have send here
struct __attribute__((packed)) TeleCmdPacket {
  uint32_t timestamp;
  uint16_t seq;
  int16_t throttle;
  int16_t steer;
  uint8_t mode;
  uint8_t flags;
  uint16_t battery_mv;
  int16_t speed_cm_s;
  int16_t temp_cx10;
  uint8_t rssi_est;
  uint8_t reserved[11];
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
    for (int i=0;i<8;i++) {
      if (crc & 0x8000) crc = (crc << 1) ^ 0x1021;
      else crc <<= 1;
    }
  }
  return crc;
}


uint32_t lastSeq = 0;
// counter of received packets
uint32_t received = 0;
// counter of lost packets
uint32_t lost = 0;

unsigned long lastPrint = 0;

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
  // joning channel provided by sending device
  radio.openReadingPipe(0, address);
  // start listening for incomming packets
  radio.startListening();

  Serial.println("== NRF24 RECEIVER TEST STARTED ==");
}

void loop() {
  // check if a packet received
  if (radio.available()) {
    // defining empty packet struct
    TeleCmdPacket pkt;
    // received 32-byte datablock will be copied to the struct
    radio.read(&pkt, sizeof(pkt));

    // Calculating checksum of the received packet
    uint16_t calc = crc16_ccitt((uint8_t*)&pkt, 30);
    // if send checksum and calculated checksum are not equal -> drop packet
    if (calc != pkt.crc16) {
      Serial.println("Bad CRC!");
      return;
    }

    // one more packet successfully received
    received++;

    // pkt.seq is a constant increasing number, if the last number is more than one smaller than the current, at least one packet got lost during the transmission
    if (pkt.seq > lastSeq + 1) {
      lost += (pkt.seq - lastSeq - 1);
    }
    lastSeq = pkt.seq;

    // Calculate latency -> the time the packet needed to be received 
    uint32_t now = millis();
    uint32_t latency = now - pkt.timestamp;

    // Print resdults every 500 milliseconds
    if (millis() - lastPrint > 500) {
      lastPrint = millis();
      Serial.print("Seq=");
      Serial.print(pkt.seq);
      Serial.print("  Latency=");
      Serial.print(latency);
      Serial.print("ms  Loss=");
      if (received + lost > 0)
        Serial.println((float)lost / (received + lost) * 100.0);
      else
        Serial.println("0");
    }
  }
}
