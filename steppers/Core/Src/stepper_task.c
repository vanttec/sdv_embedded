// stepper_task.c
#include <stdlib.h>
#include "stepper_task.h"
#include "vanttec_canlib_rx_task.h"
#include "stepper.h"
#include "cmsis_os.h"
#include "vanttec_sdv_ids.h"
#include "vanttec_canlib_tx_task.h"
#include "main.h"
#include "math.h"

// TODO Move this device id definition somewhere else
#define DEVICE_ID 0x10
#define STEPPER_TASK_DELAY 20

/// TOOD better implementation??
#define map_range(a1,a2,b1,b2,s) (b1 + (s-a1)*(b2-b1)/(a2-a1))

// declare global variables
int32_t g_test_stepper_setpoint = 0;
int32_t g_test_stepper_position = 0;
bool g_test_stepper_arrived = false;
bool g_test_stepper_fault = false;

float g_mech_steer_angle = 0.0;
float g_mech_brake_angle = 0.0;
float g_steer_mechanisim_setpoint = 0.0;
float g_brake_mechanisim_setpoint = 0.0;

float g_period = 0.0f;

void steering_stepper_task(void *task_attrs){
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
    float* steer_mechanisim_setpoint = malloc(sizeof(float));
    *steer_mechanisim_setpoint = 0.0f;
    float old_steer_mechanisim_setpoint = *steer_mechanisim_setpoint;
    register_canlib_rx(DEVICE_ID, base_msg_id | VANTTEC_CAN_ID_STEPPER_SETPOINT_ID, VANTTEC_CANLIB_FLOAT, steer_mechanisim_setpoint, 4);

    uint8_t* drivemode = malloc(sizeof(uint8_t));
    *drivemode = 1U;

    register_canlib_rx(DEVICE_ID, VANTTEC_CAN_ID_DRIVE_MODE, VANTTEC_CANLIB_BYTE, drivemode, 1);

    float total_delta_period = 3500.0-2000.0;
    int32_t mechanism_angle_setpoint = 0.0f;
    int32_t initial_position = 0.0f;

    for(;;){
        attrs.stepper->setpoint = mechanisim_angle_to_steps(
            attrs.stepper->config.gear_reduction,
            attrs.stepper->config.degs_per_step,
            *steer_mechanisim_setpoint
        );

        float normalized_position =
            abs(attrs.stepper->position - attrs.stepper->setpoint)
            /
            abs(initial_position - attrs.stepper->setpoint);

        // clamp
        normalized_position = fmaxf(0.0f, fminf(1.0f, normalized_position));
        
        // Interpolate period from 5000 to 2000 based on the normalized position
        // g_period = 3500.0f - (normalized_position * total_delta_period);


        // __HAL_TIM_SET_AUTORELOAD(attrs.stepper->config.step_timer, (uint32_t)g_period - 1);
        // __HAL_TIM_SET_COUNTER(attrs.stepper->config.step_timer, 0);

        // si no tenemos el mismo setpoint
        // if ( old_steer_mechanisim_setpoint != *steer_mechanisim_setpoint ) {
        //     // update the old setpoint
        //     old_steer_mechanisim_setpoint = *steer_mechanisim_setpoint;

        //     mechanism_angle_setpoint = mechanisim_angle_to_steps(
        //         attrs.stepper->config.gear_reduction,
        //         attrs.stepper->config.degs_per_step,
        //         *steer_mechanisim_setpoint
        //     );

        //     initial_position = attrs.stepper->position;

        // }

        g_mech_steer_angle = attrs.stepper->mechanisim_angle;
        g_steer_mechanisim_setpoint = *steer_mechanisim_setpoint;

         if (*drivemode == 0) {      // [0] is manual mode
            stepper_disable(attrs.stepper);
         } else {                    // [1] is autonomous mode
            stepper_enable(attrs.stepper);
         }

        stepper_update(attrs.stepper, *(attrs.encoder_value), *(attrs.encoder_tick_value), attrs.stepper_id);

        if (attrs.stepper->has_fault){
            // TODO: Show stepper fault in LED.
            canlib_send_byte(base_msg_id | VANTTEC_CAN_ID_STEPPER_FAULT_ID, attrs.stepper->has_fault);
        }

        // canlib_send_long(base_msg_id | VANTTEC_CAN_ID_STEPPER_RAW_POSITION_ID, attrs.stepper->position);
        // canlib_send_float(base_msg_id | VANTTEC_CAN_ID_STEPPER_POSITION_ID, attrs.stepper->mechanisim_angle);

        osDelay(STEPPER_TASK_DELAY);

        HAL_GPIO_TogglePin(GPIOC, DEBUG_6_Pin);
    }
}

void braking_stepper_task(void *task_attrs){
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
    // register_canlib_rx(DEVICE_ID, base_fmsg_id | VANTTEC_CAN_ID_STEPPER_ENABLE_ID, VANTTEC_CANLIB_BYTE, &(attrs.stepper->enabled), 1);

    // 0xI1, setpoint
   
    float* brake_mechanisim_setpoint = malloc(sizeof(float));
    *brake_mechanisim_setpoint = 0.f; //If no message is sent, return to zero at startup for security reasons
    register_canlib_rx(DEVICE_ID, base_msg_id | VANTTEC_CAN_ID_STEPPER_SETPOINT_ID, VANTTEC_CANLIB_FLOAT, brake_mechanisim_setpoint, 4);

    uint8_t* drivemode = malloc(sizeof(uint8_t));
    *drivemode = 0U;

    register_canlib_rx(DEVICE_ID, VANTTEC_CAN_ID_DRIVE_MODE, VANTTEC_CANLIB_BYTE, drivemode, 1);

    for(;;){

        float output_start = 0.0;
        float output_end = 1.8;
        float input_start = 0.0;
        float input_end = 1.0;

        double fixed_brake_mechanisim_setpoint = output_start + ((output_end - output_start) / (input_end - input_start)) * ( (*brake_mechanisim_setpoint)  - input_start);

        g_brake_mechanisim_setpoint = fixed_brake_mechanisim_setpoint;

        attrs.stepper->setpoint = mechanisim_angle_to_steps(
            attrs.stepper->config.gear_reduction,
            attrs.stepper->config.degs_per_step,
            fixed_brake_mechanisim_setpoint * 1. //Control sends a message from 0 to -1, and the maximum of the mechanism is when the value is set to -3.1
        );

        g_mech_brake_angle = attrs.stepper->mechanisim_angle;

        // if (*drivemode == 0) {      // [0] is manual mode
        stepper_disable(attrs.stepper);
        // } else {                    // [1] is autonomous mode
        //stepper_enable(attrs.stepper);
        // }

        g_test_stepper_arrived = stepper_at_setpoint(attrs.stepper);

        stepper_update(attrs.stepper, *(attrs.encoder_value), *(attrs.encoder_tick_value), attrs.stepper_id);

        g_test_stepper_setpoint = attrs.stepper->setpoint;
        g_test_stepper_position = attrs.stepper->position;

        if (attrs.stepper->has_fault){
            // TODO: Show stepper fault in LED.
            canlib_send_byte(base_msg_id | VANTTEC_CAN_ID_STEPPER_FAULT_ID, attrs.stepper->has_fault);
        }

        g_test_stepper_fault = attrs.stepper->has_fault;

        // canlib_send_long(base_msg_id | VANTTEC_CAN_ID_STEPPER_RAW_POSITION_ID, attrs.stepper->position);
        // canlib_send_float(base_msg_id | VANTTEC_CAN_ID_STEPPER_POSITION_ID, attrs.stepper->mechanisim_angle);

        osDelay(STEPPER_TASK_DELAY);
    }
}
