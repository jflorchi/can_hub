#include "can.h"

CanBus::CanBus(void (*can1Callback)(const CAN_message_t &msg), 
                void (*can2Callback)(const CAN_message_t &msg), 
                void (*can3Callback)(const CAN_message_t &msg)) {
  can1.begin();
  can1.setBaudRate(500000);
  can1.setMaxMB(16);
  can1.enableFIFO();
  can1.enableFIFOInterrupt();
  can1.onReceive(can1Callback);
  can1.mailboxStatus();

  can2.begin();
  can2.setBaudRate(500000);
  can2.setMaxMB(16);
  can2.enableFIFO();
  can2.enableFIFOInterrupt();
  can2.onReceive(can2Callback);
  can2.mailboxStatus();

  can3.begin();
  can3.setBaudRate(500000);
  can3.setMaxMB(16);
  can3.enableFIFO();
  can3.enableFIFOInterrupt();
  can3.onReceive(can3Callback);
  can3.mailboxStatus();

  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
}

CanBus::~CanBus() {

}

void CanBus::eventLoop() {
  can1.events();
  can2.events();
  can3.events();
}

void CanBus::disableCan(uint8_t bus) {
  if (bus == 1) {
    digitalWrite(3, HIGH);
  } else if (bus == 2) {
    digitalWrite(4, HIGH);
  } else if (bus == 3) {
    digitalWrite(5, HIGH);
  } 
}

void CanBus::enableCan(uint8_t bus) {
  if (bus == 1) {
    digitalWrite(3, LOW);
  } else if (bus == 2) {
    digitalWrite(4, LOW);
  } else if (bus == 3) {
    digitalWrite(5, LOW);
  } 
}

void CanBus::write(uint8_t bus, CAN_message_t msg) {
  if (bus == 1) {
    can1.write(msg);
  } else if (bus == 2) {
    can2.write(msg);
  } else if (bus == 3) {
    can3.write(msg);
  } 
}

