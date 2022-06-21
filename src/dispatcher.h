#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <Arduino.h>
#include "can.h"

extern CAN_message_t messages[15];
extern uint32_t lastSent[15];

class Dispatcher {

    public:
        Dispatcher(CanBus* can);

        void run();
        void runIndex(uint8_t msgIndex);
        void schedule(uint8_t bus, uint16_t id, uint8_t* buff, uint8_t len, uint8_t freqeuncy, bool checksum);

    private:
        CanBus* can;
        uint8_t index = 0;

};

#endif