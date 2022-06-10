#ifndef CAN_H
#define CAN_H

#include <FlexCAN_T4.h>


class CanBus {

    public:
        CanBus(void (*can1Callback)(const CAN_message_t &msg), 
                void (*can2Callback)(const CAN_message_t &msg), 
                void (*can3Callback)(const CAN_message_t &msg));
        ~CanBus();

        void eventLoop();
        void enableCan(uint8_t bus);
        void disableCan(uint8_t bus);
        int write(uint8_t bus, CAN_message_t msg);

    private:
        FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;
        FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> can2;
        FlexCAN_T4<CAN3, RX_SIZE_256, TX_SIZE_16> can3;


};


#endif