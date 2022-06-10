#ifndef MESSAGE_H
#define MESSAGE_H

#include <Arduino.h>

class Message {

    public:
        uint8_t _bus;
        uint16_t _id;
        uint8_t* _buff;
        uint8_t _len;
        uint8_t _frequency;
        bool _checksum;

        Message(uint8_t bus, uint16_t id, uint8_t* buff, uint8_t len, uint8_t freqeuncy, bool checksum);
        Message();

};

#endif