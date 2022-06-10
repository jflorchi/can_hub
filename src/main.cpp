#include <Arduino.h>
#include "can.h"
#include "dispatcher.h"

void Can1Callback(const CAN_message_t &msg);
void Can2Callback(const CAN_message_t &msg);
void Can3Callback(const CAN_message_t &msg);
bool isAllowedMessage(uint16_t messageId);

const uint8_t ALLOWED_MESSAGES_SIZE = 5;
uint16_t ALLOWED_MESSAGES[ALLOWED_MESSAGES_SIZE] = {0xb4, 0x3b1, 0x2c1, 0x399, 0x24};

uint8_t ANGLE[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t WHEEL_SPEEDS[8] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
uint8_t GEAR_MSG[8] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x80, 0x0, 0x0};
uint8_t STEERING_LEVER_MSG[8] = {0x29, 0x0, 0x01, 0x0, 0x0, 0x0, 0x76};

uint8_t MSG7[2] = {0x06, 0x00};
uint8_t MSG8[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x17, 0x00};
uint8_t MSG10[7] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t MSG11[7] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t MSG12[8] = {0x66, 0x06, 0x08, 0x0a, 0x02, 0x00, 0x00, 0x00};
uint8_t MSG13[8] = {0x1C, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00};
uint8_t MSG15[8] = {0x05, 0xea, 0x1b, 0x08, 0x00, 0x00, 0xc0, 0x9f};

CanBus canBus(Can1Callback, Can2Callback, Can3Callback);
Dispatcher dispatcher(&canBus);

void setup() {
  Serial.begin(115200);
  pinMode(2, INPUT);
  
  canBus.enableCan(1);
  canBus.enableCan(2);
  canBus.enableCan(3);

  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  // bus, id, buffer, length, frequency, checksum
  // dispatcher.schedule(2, 0x25, ANGLE, 8, 84, true);
  // dispatcher.schedule(2, 0xAA, WHEEL_SPEEDS, 8, 84, false);
  // dispatcher.schedule(2, 0x3BC, GEAR_MSG, 9, 84, false);
  // dispatcher.schedule(2, 0x1C4, MSG15, 8, 84, false);
  // dispatcher.schedule(2, 0x414, MSG8, 7, 84, false);
  // dispatcher.schedule(2, 0x489, MSG10, 7, 84, false);
  // dispatcher.schedule(2, 0x48A, MSG11, 7, 84, false);
  // dispatcher.schedule(2, 0x48B, MSG12, 8, 84, false);
  // dispatcher.schedule(2, 0x4D3, MSG13, 8, 84, false);
  // dispatcher.schedule(2, 0x367, MSG7, 2, 40, false);

  dispatcher.schedule(3, 0x614, STEERING_LEVER_MSG, 7, 2, false);
  // dispatcher.schedule(1, 0x3BC, GEAR_MSG, 9, 84, false);

}

void loop() {
  canBus.eventLoop();
  dispatcher.run();
}

// Vehicle CAN Bus
void Can1Callback(const CAN_message_t &msg) {
  if (msg.id == 0xb0) {
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
  Serial.println(msg.id);
}

// 2017 CAN Bus
void Can2Callback(const CAN_message_t &msg) {
  if (msg.id == 0x260 || msg.id == 0x262 || msg.id == 0x394) {
    canBus.write(1, msg); // Forward the 2017 PSCM messages to the car
  }
  // Serial.println(msg.id);
}

// Custom Module CAN Bus
void Can3Callback(const CAN_message_t &msg) {
  // if (msg.id == 0x23) {
  //   uint8_t p1 = msg.buf[6];
  //   uint8_t p2 = msg.buf[7];
  //   Serial.println(((uint16_t)p1 << 8) | p2);
  //   Serial.println((msg.buf[0] << 24) | (msg.buf[1] << 16) | (msg.buf[2] << 8) | msg.buf[3]);
  // }
  // Serial.println(msg.id);
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
