/*
 * general_tasks.c
 *
 *  Created on: May 24, 2023
 *      Author: Z0180064
 */
#include "main.h"
#include "vanttec_canlib_tx_task.h"
#include "vanttec_canlib_rx_task.h"
#include <vanttec_sdv_ids.h>
#include "general_tasks.h"


osThreadId_t eStopTaskHandle;
const osThreadAttr_t eStopTaskAttributes = {
    .name = "estop_general",
    .stack_size = 128 * 4};

osThreadId_t reverseTaskHandle;
const osThreadAttr_t reverseTaskAttributes = {
    .name = "reverse_task",
    .stack_size = 128 * 4};

osThreadId_t driveModeTaskHandle;
const osThreadAttr_t driveModeTaskAttributes = {
    .name = "drive_mode_task",
    .stack_size = 128 * 4};

osThreadId_t seatSensorTaskHandle;
const osThreadAttr_t seatSensorTaskAttributes = {
    .name = "seat_sensor_task",
    .stack_size = 128 * 4};

// Declaracion task emergency stop

void estop_task(void *args)
{
    GPIO_PinState emergency_button;
    uint8_t prev_emergency_button = 2;

    for (;;)
    {
        emergency_button = HAL_GPIO_ReadPin(I_BotonEmergencia_GPIO_Port, I_BotonEmergencia_Pin);           // Se lee el estado del botón
        //emergency_button_aux = HAL_GPIO_ReadPin(I_BotonEmergenciaAux_GPIO_Port, I_BotonEmergenciaAux_Pin); // Se lee el estado del botón
        if (prev_emergency_button != emergency_button)
        {
            if (emergency_button )
            {
                HAL_GPIO_WritePin(GPIOC, DEBUG_1_Pin, GPIO_PIN_SET);
                // envío de ID de can
            }
            else
            {
                HAL_GPIO_WritePin(GPIOC, DEBUG_1_Pin, GPIO_PIN_RESET);
                // envío de ID de can
            }
            canlib_send_byte(VANTTEC_CAN_ID_ESTOP, (uint8_t)emergency_button); // Lectura constante de botón de emergencia
        }
        prev_emergency_button = emergency_button;
        osDelay(100);
    }
}

void reverse_task(void *args)
{
    GPIO_PinState reverse_state;
    uint8_t prev_reverse = 2;

    for (;;)
    {
        reverse_state = HAL_GPIO_ReadPin(I_BotonDeReversa_GPIO_Port, I_BotonDeReversa_Pin); // Se lee el estado del botón
        if (prev_reverse != reverse_state)
        {
            if (reverse_state)
            {
                HAL_GPIO_WritePin(GPIOC, DEBUG_2_Pin, GPIO_PIN_SET);
                // envío de ID de can
            }
            else
            {
                HAL_GPIO_WritePin(GPIOC, DEBUG_2_Pin, GPIO_PIN_RESET);
                // envío de ID de can
            }
            canlib_send_byte(VANTTEC_CAN_ID_REVERSE, (uint8_t)reverse_state); // Lectura constante de botón de emergencia
        }
        prev_reverse = reverse_state;
        osDelay(100);
    }
}

void drive_mode_task(void *args)
{
    GPIO_PinState drive_mode;
    uint8_t prev_drive_mode = 2;
    uint8_t prev_drive_mode_aux = 2;
    for (;;)
    {
        drive_mode = HAL_GPIO_ReadPin(I_BotonAutonomo_Manual_GPIO_Port, I_BotonAutonomo_Manual_Pin);       // Se lee el estado del botón
        if (prev_drive_mode != drive_mode)
        {
            if (drive_mode)
            {
                HAL_GPIO_WritePin(GPIOC, DEBUG_3_Pin, GPIO_PIN_SET);
                // envío de ID de can
            }
            else
            {
                HAL_GPIO_WritePin(GPIOC, DEBUG_3_Pin, GPIO_PIN_RESET);
                // envío de ID de can
            }
            canlib_send_byte(VANTTEC_CAN_ID_DRIVE_MODE, (uint8_t)drive_mode); // Lectura constante de botón de emergencia
        }
        prev_drive_mode = drive_mode;
        osDelay(100);
    }
}

// Declaracion asiento sensor

void asientoSensor_general_task(void *args)
{
    GPIO_PinState seat_sensor;
    uint8_t prev_seat_sensor = 2;
    for (;;)
    {
        seat_sensor = HAL_GPIO_ReadPin(I_AsientoSensor_GPIO_Port, I_AsientoSensor_Pin); // Se lee el estado del botón
        if (prev_seat_sensor != seat_sensor)
        {
            if (seat_sensor)
            {
                HAL_GPIO_WritePin(GPIOC, DEBUG_4_Pin, GPIO_PIN_SET);
                // envío de ID de can
            }
            else
            {
                HAL_GPIO_WritePin(GPIOC, DEBUG_4_Pin, GPIO_PIN_RESET);
                // envío de ID de can
            }
            canlib_send_byte(VANTTEC_CAN_ID_DRIVER_PRESENT, (uint8_t)seat_sensor); // Lectura constante de botón de emergencia
        }
        prev_seat_sensor = seat_sensor;
        osDelay(100);
    }
}

void init_general_task()
{
    eStopTaskHandle = osThreadNew(estop_task, NULL, &eStopTaskAttributes);
    reverseTaskHandle = osThreadNew(reverse_task, NULL, &reverseTaskAttributes);
    driveModeTaskHandle = osThreadNew(drive_mode_task, NULL, &driveModeTaskAttributes);
    seatSensorTaskHandle = osThreadNew(asientoSensor_general_task, NULL, &seatSensorTaskAttributes);
}

/*

// Habilitar interrupt

void interrupt_EmergencyStop(){
    RCC -> APBENR |= 0x1;
    SYSCFG -> EXTICR[1] &= ~(0x7 << 12); // Se seleccionan 3 bits no masked y se shitfean 12 bits
    SYSCFG -> EXTICR[1] |= ~(0x1 << 12); // Se seleccionan 3 bits no masked y se shitfean 12 bits

    EXTI -> IMR1 |= 1 << BUTTON_Pin_Pos; //Se usa para habilitar posición 7 del IMR1
    EXTI -> RTSR1 |= 1 << BUTTON_Pin_Pos;
    EXTI -> FTSR1 |= 1 << BUTTON_Pin_Pos;

}
*/
