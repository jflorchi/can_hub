#include <Arduino.h>
#include "can.h"

void attachChecksum(uint16_t id, uint8_t len, uint8_t *msg);
int getChecksum(uint8_t *msg, uint8_t len, uint16_t addr);
void Can1Callback(const CAN_message_t &msg);
void Can2Callback(const CAN_message_t &msg);
void Can3Callback(const CAN_message_t &msg);
bool isAllowedMessage(uint16_t messageId);

const uint8_t ALLOWED_MESSAGES_SIZE = 5;
uint16_t ALLOWED_MESSAGES[ALLOWED_MESSAGES_SIZE] = {0xb4, 0x3b1, 0x2c1, 0x399, 0x24};
uint8_t ANGLE[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t WHEEL_SPEEDS[8] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};

void canSniff(const CAN_message_t &msg) {
  Serial.print("MB "); Serial.print(msg.mb);
  Serial.print("  OVERRUN: "); Serial.print(msg.flags.overrun);
  Serial.print("  LEN: "); Serial.print(msg.len);
  Serial.print(" EXT: "); Serial.print(msg.flags.extended);
  Serial.print(" TS: "); Serial.print(msg.timestamp);
  Serial.print(" ID: "); Serial.print(msg.id, HEX);
  Serial.print(" Buffer: ");
  for ( uint8_t i = 0; i < msg.len; i++ ) {
    Serial.print(msg.buf[i], HEX); Serial.print(" ");
  } Serial.println();
}

CanBus canBus(Can1Callback, Can2Callback, Can3Callback);

void setup() {
  Serial.begin(115200);
  canBus.enableCan(1);
  canBus.enableCan(2);
  canBus.enableCan(3);

}

void loop() {
  canBus.eventLoop();

  CAN_message_t msg;
  msg.id = random(0x1, 0x7FE);
  for ( uint8_t i = 0; i < 8; i++ ) msg.buf[i] = i + 1;
  canBus.write(1, msg);
  canBus.write(2, msg);
  canBus.write(3, msg);

}

// Vehicle CAN Bus
void Can1Callback(const CAN_message_t &msg) {
  if (msg.id == 0xb0) {
      uint8_t dat[8];
      WHEEL_SPEEDS[0] = msg.buf[0] + 0x1a;
      WHEEL_SPEEDS[1] = msg.buf[1] + 0x6f;
      WHEEL_SPEEDS[2] = msg.buf[2] + 0x1a;
      WHEEL_SPEEDS[3] = msg.buf[3] + 0x6f;
  } else if (msg.id == 0xb2) {
      WHEEL_SPEEDS[4] = msg.buf[0] + 0x1a;
      WHEEL_SPEEDS[5] = msg.buf[1] + 0x6f;
      WHEEL_SPEEDS[6] = msg.buf[2] + 0x1a;
      WHEEL_SPEEDS[7] = msg.buf[3] + 0x6f;
  }

  if (isAllowedMessage(msg.id)) {
    canBus.write(2, msg); // Write the car's messages to 2017 bus
  }
  
  canSniff(msg);
}

// 2017 CAN Bus
void Can2Callback(const CAN_message_t &msg) {
  if (msg.id == 0x260 || msg.id == 0x262 || msg.id == 0x394) {
    canBus.write(1, msg); // Forward the 2017 PSCM messages to the car
  }
  canSniff(msg);
}

// Custom Module CAN Bus
void Can3Callback(const CAN_message_t &msg) {
  canSniff(msg);
}

void attachChecksum(uint16_t id, uint8_t len, uint8_t *msg) {
    msg[len -1] = getChecksum(msg, len - 1, id);
}

int getChecksum(uint8_t *msg, uint8_t len, uint16_t addr) {
    uint8_t checksum = 0;
    checksum = ((addr & 0xFF00) >> 8) + (addr & 0x00FF) + len + 1;
    for (int ii = 0; ii < len; ii++) {
        checksum += (msg[ii]);
    }
    return checksum;
}

bool isAllowedMessage(uint16_t messageId) {
  if (messageId >= 0x700) {
    return true;
  }
  for (uint8_t i = 0; i < ALLOWED_MESSAGES_SIZE; i++) {
    if (messageId == ALLOWED_MESSAGES[i]) {
      return true;
    }
  }
  return false;
}
