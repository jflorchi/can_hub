#include "dispatcher.h"
#include "toyota.h"

CAN_message_t messages[15];
uint32_t lastSent[15] = {0};

Dispatcher::Dispatcher(CanBus* canBus) {
    can = canBus;
}


void Dispatcher::run() {
    for (size_t i = 0; i < 15; i++) {
        CAN_message_t msg = messages[i];
        if (msg.id == 0) {
            continue;
        }
        uint32_t last = lastSent[i];
        uint32_t currentTime = millis();
        if (currentTime - last >= 1000.0 / msg.frequency) {
            // dispatch event
            if (msg.checksum) {
                attachChecksum(msg.id, msg.len, msg.buf);
            }
            Serial.println(currentTime - last);
            can->write(msg.mbus, msg);
            lastSent[i] = millis();
        }
        
    }
    
}

void Dispatcher::schedule(uint8_t bus, uint16_t id, uint8_t* buff, uint8_t len, uint8_t freqeuncy, bool checksum) {
    CAN_message_t msg;
    msg.mbus = bus;
    msg.id = id;
    for (size_t i = 0; i < len; i++) {
        msg.buf[i] = buff[i];
    }
    msg.len = len;
    msg.frequency = freqeuncy;
    msg.checksum = checksum;
    messages[index] = msg;
    index += 1;
}
