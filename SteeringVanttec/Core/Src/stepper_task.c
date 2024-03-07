#include <stdlib.h>
#include "stepper_task.h"
#include "vanttec_canlib_rx_task.h"
#include "stepper.h"
#include "cmsis_os.h"

// TODO Move this device id definition somewhere else
#define DEVICE_ID 0x10
#define ENCODER_UPDATE_DELAY 20

void stepper_task(uint8_t stepper_id, Stepper *stepper, int32_t *encoder_value){
    // First 4 bits of message are from stepper_id.
    // RX:
    // 0xI0 -> enable disable uint8_t
    // 0xI1 -> step setpoint float, mechanisim radians
    // TX:
    // 0xI2 -> fault status send
    uint8_t base_message_id = (stepper_id & 0b11) << 2;

    //0xI0, enable
    register_canlib_rx(DEVICE_ID, base_message_id | 0, VANTTEC_CANLIB_BYTE, &(stepper->enabled), 1);

    // 0xI1, setpoint
    float* mechanisim_setpoint = malloc(sizeof(float));
    register_canlib_rx(DEVICE_ID, base_message_id | 1, VANTTEC_CANLIB_FLOAT, mechanisim_setpoint, 4);

    for(;;){
        //stepper_update(stepper);
        // stepper_update(stepper, *encoder_value);
        osDelay(ENCODER_UPDATE_DELAY);
    }
}
