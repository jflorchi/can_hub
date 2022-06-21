#include "dispatcher.h"
#include "toyota.h"

CAN_message_t messages[15];
uint32_t lastSent[15] = {0};

Dispatcher::Dispatcher(CanBus* canBus) {
    can = canBus;
}

void Dispatcher::runIndex(uint8_t msgIndex) {
    CAN_message_t msg = messages[msgIndex];
    if (msg.id == 0) {
        return;
    } 
    uint32_t last = lastSent[msgIndex];
    uint32_t currentTime = millis();
    if (currentTime - last >= 1000.0 / msg.frequency) {
        // dispatch event
        for (size_t i = 0; i < msg.len; i++) {
            msg.buf[i] = msg.bufRef[i];
        }
        if (msg.checksum) {
            attachChecksum(msg.id, msg.len, msg.buf);
        }
        can->write(msg.mbus, msg);
        lastSent[msgIndex] = millis();
    }
}

void Dispatcher::run() {
    for (size_t i = 0; i < 15; i++) {
        runIndex(i);
    }
}

void Dispatcher::schedule(uint8_t bus, uint16_t id, uint8_t* buff, uint8_t len, uint8_t freqeuncy, bool checksum) {
    CAN_message_t msg;
    msg.mbus = bus;
    msg.id = id;
    msg.bufRef = buff;
    msg.len = len;
    msg.frequency = freqeuncy;
    msg.checksum = checksum;
    messages[index] = msg;
    index += 1;
}
