#include <Arduino.h>
#include "can.h"
#include "dispatcher.h"

void Can1Callback(const CAN_message_t &msg);
void Can2Callback(const CAN_message_t &msg);
void Can3Callback(const CAN_message_t &msg);
bool isAllowedMessage(uint16_t messageId);

const uint8_t ALLOWED_MESSAGES_SIZE = 5;
uint16_t ALLOWED_MESSAGES[ALLOWED_MESSAGES_SIZE] = {0xb4};

uint8_t ANGLE[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t WHEEL_SPEEDS[8] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
uint8_t GEAR_MSG[8] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x80, 0x0, 0x0};
uint8_t STEERING_LEVER_MSG[2] = {0x00, 0x00};
uint8_t IGNITION_MSG[3] = {0x00, 0x00, 0x00};

uint8_t MSG7[2] = {0x06, 0x00};
uint8_t MSG8[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x17, 0x00};
uint8_t MSG10[7] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t MSG11[7] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t MSG12[8] = {0x66, 0x06, 0x08, 0x0a, 0x02, 0x00, 0x00, 0x00};
uint8_t MSG13[8] = {0x1C, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00};
uint8_t MSG15[8] = {0x05, 0xea, 0x1b, 0x08, 0x00, 0x00, 0xc0, 0x9f};

uint16_t raw_wheel_speeds[4] = {0x00, 0x00, 0x00, 0x00};

CanBus canBus(Can1Callback, Can2Callback, Can3Callback);
Dispatcher dispatcher(&canBus);
bool ignition = false;

void setup() {
  Serial.begin(115200);
  pinMode(2, INPUT);
  
  canBus.enableCan(1);
  canBus.enableCan(2);
  canBus.enableCan(3);

  pinMode(13, OUTPUT);
  
  // bus, id, buffer, length, frequency, checksum // 2
  dispatcher.schedule(2, 0x25, ANGLE, 8, 84, true);
  dispatcher.schedule(2, 0xAA, WHEEL_SPEEDS, 8, 84, false);
  //dispatcher.schedule(2, 0x3BC, GEAR_MSG, 8, 84, false);
  dispatcher.schedule(2, 0x1C4, MSG15, 8, 84, false);
  //dispatcher.schedule(2, 0x414, MSG8, 7, 84, false);
  //dispatcher.schedule(2, 0x489, MSG10, 7, 84, false);
  //dispatcher.schedule(2, 0x48A, MSG11, 7, 84, false);
  dispatcher.schedule(2, 0x48B, MSG12, 8, 84, false);
  //dispatcher.schedule(2, 0x4D3, MSG13, 8, 84, false);
  //dispatcher.schedule(2, 0x367, MSG7, 2, 40, false);

  // dispatcher.schedule(1, 0x614, STEERING_LEVER_MSG, 2, 4, false); // 1
  dispatcher.schedule(1, 0x3BC, GEAR_MSG, 8, 84, false); // 1

  dispatcher.schedule(3, 0x001, IGNITION_MSG, 3, 12, true);
}

void loop() {
  uint8_t ign = digitalRead(2);
  if (ign == HIGH) {
    ignition = true;
  } else {
    ignition = false;
  }
  if (ignition) {
    digitalWrite(13, HIGH);

    IGNITION_MSG[0] = ((uint16_t) (((raw_wheel_speeds[0] + raw_wheel_speeds[1] + raw_wheel_speeds[2] + raw_wheel_speeds[3]) / 4.0) + 0.5)) & 0xFF;

    canBus.eventLoop();
    dispatcher.run();
  } else {
    digitalWrite(13, LOW);
    delay(500);
  }
}

// Vehicle CAN Bus
void Can1Callback(const CAN_message_t &msg) {
  if (msg.id == 0xb0) {
      raw_wheel_speeds[0] = ((msg.buf[0] << 8) | (msg.buf[1] & 0xFF)) * 0.01;
      raw_wheel_speeds[1] = ((msg.buf[2] << 8) | (msg.buf[3] & 0xFF)) * 0.01;
      WHEEL_SPEEDS[0] = msg.buf[0] + 0x1a;
      WHEEL_SPEEDS[1] = msg.buf[1] + 0x6f;
      WHEEL_SPEEDS[2] = msg.buf[2] + 0x1a;
      WHEEL_SPEEDS[3] = msg.buf[3] + 0x6f;
  } else if (msg.id == 0xb2) {
      raw_wheel_speeds[2] = ((msg.buf[0] << 8) | (msg.buf[1] & 0xFF)) * 0.01;
      raw_wheel_speeds[3] = ((msg.buf[2] << 8) | (msg.buf[3] & 0xFF)) * 0.01;
      WHEEL_SPEEDS[4] = msg.buf[0] + 0x1a;
      WHEEL_SPEEDS[5] = msg.buf[1] + 0x6f;
      WHEEL_SPEEDS[6] = msg.buf[2] + 0x1a;
      WHEEL_SPEEDS[7] = msg.buf[3] + 0x6f;
  } else if (msg.id == 0x224) {
    IGNITION_MSG[2] = (msg.buf[0] & 0x20) >> 5;
  }

  if (isAllowedMessage(msg.id)) {
    canBus.write(2, msg); // Write the car's messages to 2017 bus
  }
}

// 2017 CAN Bus
void Can2Callback(const CAN_message_t &msg) {
  if (msg.id == 0x260 || msg.id == 0x262 || msg.id == 0x394) {
    canBus.write(1, msg); // Forward the 2017 PSCM messages to the car
    if (msg.id == 0x262) {
      IGNITION_MSG[1] = (msg.buf[3] & 0xFE) >> 1;
    }
  }
}

// Custom Module CAN Bus
void Can3Callback(const CAN_message_t &msg) {

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
