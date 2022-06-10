#include "message.h"

Message::Message(uint8_t bus, uint16_t id, uint8_t* buff, uint8_t len, uint8_t freqeuncy, bool checksum) {
    _bus = bus;
    _id = id;
    _buff = buff;
    _len = len;
    _frequency = freqeuncy;
    _checksum = checksum;
}

Message::Message() {
    _id = -1;
}
