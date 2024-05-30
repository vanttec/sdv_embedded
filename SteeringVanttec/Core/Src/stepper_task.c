#include <stdlib.h>
#include "stepper_task.h"
#include "vanttec_canlib_rx_task.h"
#include "stepper.h"
#include "cmsis_os.h"
#include "vanttec_sdv_ids.h"
#include "vanttec_canlib_tx_task.h"

// TODO Move this device id definition somewhere else
#define DEVICE_ID 0x10
#define STEPPER_TASK_DELAY 20

int32_t g_test_setpoint;

void stepper_task(void *task_attrs){
    stepper_task_attrs attrs =  *((stepper_task_attrs*) task_attrs);
    
    // MSB 42 bits of message are from stepper_id.
    // RX:
    // 0xI0 -> enable disable uint8_t
    // 0xI1 -> step setpoint float, mechanisim radians
    // TX:
    // 0xI2 -> fault status send
    uint8_t base_msg_id = (attrs.stepper_id & 0b11) << 6;

    // [DEBUG]
    // base_msg_id is used to address individual steppers
    // if you want to address the board as a whole, dont use it
    // for example, setting the mode vs. setting a setpoint 

    //0xI0, enable
    // register_canlib_rx(DEVICE_ID, base_msg_id | VANTTEC_CAN_ID_STEPPER_ENABLE_ID, VANTTEC_CANLIB_BYTE, &(attrs.stepper->enabled), 1);

    // 0xI1, setpoint
    float* mechanisim_setpoint = malloc(sizeof(float));
    *mechanisim_setpoint = 0.0f;

    int32_t* test_setpoint = malloc(sizeof(int32_t));
    *test_setpoint = 0;
    register_canlib_rx(DEVICE_ID, base_msg_id | VANTTEC_CAN_ID_STEPPER_SETPOINT_ID, VANTTEC_CANLIB_FLOAT, mechanisim_setpoint, 4);

    uint8_t* drivemode = malloc(sizeof(uint8_t));
    *drivemode = 0U;

    register_canlib_rx(DEVICE_ID, VANTTEC_CAN_ID_DRIVE_MODE, VANTTEC_CANLIB_BYTE, drivemode, 1);

    for(;;){
        attrs.stepper->setpoint = mechanisim_angle_to_steps(attrs.stepper->config.gear_reduction, attrs.stepper->config.degs_per_step, *mechanisim_setpoint);
        g_test_setpoint = attrs.stepper->setpoint;

        if (*drivemode == 0) {      // [0] is manual mode
            stepper_disable(attrs.stepper);
        } else {                    // [1] is autonomous mode
            stepper_enable(attrs.stepper);
        }

        stepper_update(attrs.stepper, *(attrs.encoder_value), *(attrs.encoder_tick_value));

        if (attrs.stepper->has_fault){
            // TODO: Show stepper fault in LED.
            // canlib_send_byte(base_msg_id | VANTTEC_CAN_ID_STEPPER_FAULT_ID, attrs.stepper->has_fault);
        }

        // canlib_send_long(base_msg_id | VANTTEC_CAN_ID_STEPPER_RAW_POSITION_ID, attrs.stepper->position);
        // canlib_send_float(base_msg_id | VANTTEC_CAN_ID_STEPPER_POSITION_ID, attrs.stepper->mechanisim_angle);

        osDelay(STEPPER_TASK_DELAY);
    }
}
