/*
 * panel_tasks.c
 *
 *  Created on: May 9, 2023
 *      Author: books
 */
#include "main.h"
#include "vanttec_canlib_tx_task.h"
#include "vanttec_canlib_rx_task.h"

osThreadId_t panelMovTaskHandle;
const osThreadAttr_t panelMovTaskAttributes = {
    .name = "panelMov",
    .stack_size = 128 * 4};
osThreadId_t panelTaskHandle;
const osThreadAttr_t panelDetTaskAttributes = {
    .name = "panelDet",
    .stack_size = 128 * 4};
const osThreadAttr_t driveModeStatusTaskAttributes = {
    .name = "driveModeStatus",
    .stack_size = 128 * 4};
const osThreadAttr_t reverseSwitchStatusTaskAttributes = {
    .name = "reverseSwitchStatus",
    .stack_size = 128 * 4};
const osThreadAttr_t safetyModeAlertTaskAttributes = {
    .name = "safetyModeAlert",
    .stack_size = 128 * 4};
void panelMov_task(void *args)
{
    uint8_t panelMov_data[2];
    uint32_t corto=20;
    uint32_t largo=200;
    register_canlib_rx(0x51, 0x05, VANTTEC_CANLIB_BYTE, &panelMov_data, 1);
    for (;;)
    {
        switch (panelMov_data[0])
        {
        case 0x10:
            // Frenado
            HAL_GPIO_WritePin(L3D_GPIO_Port, L3D_Pin, SET);
            HAL_GPIO_WritePin(L2D_GPIO_Port, L2D_Pin, SET);
            HAL_GPIO_WritePin(L1D_GPIO_Port, L1D_Pin, SET);
            HAL_Delay(largo);
            HAL_GPIO_WritePin(L3D_GPIO_Port, L3D_Pin, RESET);
            HAL_GPIO_WritePin(L2D_GPIO_Port, L2D_Pin, RESET);
            HAL_GPIO_WritePin(L1D_GPIO_Port, L1D_Pin, RESET);
            HAL_Delay(largo);
            break;
        case 0x11:
            // Reversa
            HAL_GPIO_WritePin(L3D_Port, L3D_Pin, SET);
            HAL_GPIO_WritePin(L2D_Port, L2D_Pin, SET);
            HAL_GPIO_WritePin(PA5_GPIO_Port, PA5_Pin, SET);
            HAL_Delay(corto);
            HAL_GPIO_WritePin(L3D_Port, L3D_Pin, RESET);
            HAL_GPIO_WritePin(L2D_Port, L2D_Pin, RESET);
            HAL_GPIO_WritePin(PA5_GPIO_Port, PA5_Pin, RESET);
            HAL_Delay(largo);
            break;
        case 0x13:
            // Carro encendido          
            for (int i = 0; i < 3; i++)
            {
                HAL_GPIO_WritePin(PA5_GPIO_Port, PA5_Pin, SET);
                HAL_Delay(corto);
                HAL_GPIO_WritePin(PA5_GPIO_Port, PA5_Pin, RESET);
                HAL_Delay(corto);
            }
                HAL_GPIO_WritePin(L1D_GPIO_Port, L1D_Pin, SET);  
            break;
        case 0x14:
            // Carro apagado
            for (int i = 0; i < 2; i++)
            {
                HAL_GPIO_WritePin(PA5_GPIO_Port, PA5_Pin, SET);
                HAL_Delay(corto);
                HAL_GPIO_WritePin(PA5_GPIO_Port, PA5_Pin, RESET);
                HAL_Delay(largo);
            }
            // Apaga todo
            HAL_GPIO_WritePin(L1D_GPIO_Port, L1D_Pin, RESET);
            HAL_GPIO_WritePin(L5D_GPIO_Port, L5D_Pin, RESET);
            HAL_GPIO_WritePin(PA5_Pin_GPIO_Port, PA5_Pin, RESET);
            HAL_GPIO_WritePin(L3D_GPIO_Port, L3D_Pin, RESET);
            HAL_GPIO_WritePin(L2D_GPIO_Port, L2D_Pin, RESET);
            break;
        case 0x16:
            //Vehiculo estacionario pero encendido
            for (int i = 0; i < 3; i++)
            {
                HAL_GPIO_WritePin(L3D_GPIO_Port, L3D_Pin, SET);
                HAL_GPIO_WritePin(L2D_GPIO_Port, L2D_Pin, SET);
                HAL_Delay(corto);
                HAL_GPIO_WritePin(L3D_GPIO_Port, L3D_Pin, RESET);
                HAL_GPIO_WritePin(L2D_GPIO_Port, L2D_Pin, RESET);
                HAL_Delay(largo);
            break;
            }
            break;
        case 0x17:
            //Aceleracion
            for (int i = 0; i < 2; i++)
            {
                HAL_GPIO_WritePin(L5D_GPIO_Port, L5D_Pin, SET);
                HAL_Delay(corto);
                HAL_GPIO_WritePin(L5D_GPIO_Port, L5D_Pin, RESET);
                HAL_Delay(largo);
            }
            break;
        case 0x18:
            //Tiempo sin cambios
            for (int i = 0; i < 2; i++)
            {
                HAL_GPIO_WritePin(L5D_GPIO_Port, L5D_Pin, SET);
                HAL_GPIO_WritePin(L3D_Port, L3D_Pin, SET);
                HAL_GPIO_WritePin(L2D_Port, L2D_Pin, SET);
                HAL_Delay(corto);
                HAL_GPIO_WritePin(L5D_GPIO_Port, L5D_Pin, RESET);
                HAL_GPIO_WritePin(L3D_Port, L3D_Pin, RESET);
                HAL_GPIO_WritePin(L2D_Port, L2D_Pin, RESET);
                HAL_Delay(largo);
            }
            // Apaga todo
            HAL_GPIO_WritePin(L1D_GPIO_Port, L1D_Pin, RESET);
            HAL_GPIO_WritePin(L5D_GPIO_Port, L5D_Pin, RESET);
            HAL_GPIO_WritePin(PA5_Pin_GPIO_Port, PA5_Pin, RESET);
            HAL_GPIO_WritePin(L3D_GPIO_Port, L3D_Pin, RESET);
            HAL_GPIO_WritePin(L2D_GPIO_Port, L2D_Pin, RESET);
            break;
        default:
            continue;
        }
    }
}
void panelDet_task(void *args)
{
    uint8_t panelDet_data[2];
    uint32_t corto=20;
    uint32_t largo=200;
    register_canlib_rx(0x51, 0x06, VANTTEC_CANLIB_BYTE, &panelDet_data, 1);
    for (;;)
    {
        switch (panelDet_data[0])
            {
            case 0x10:
                // Giro Prominente
                if (panelDet_data[1] == 0x0)
                    {
                    //Derecha
                     for (int i = 0; i < 2; i++)
                    {
                        HAL_GPIO_WritePin(L3D_Port, L3D_Pin, SET);
                        HAL_Delay(corto);
                        HAL_GPIO_WritePin(L3D_Port, L2D_Pin, RESET);
                        HAL_Delay(largo);
                    }
                }
                else if (panelDet_data[1] == 0x1)
                {
                    for (int i = 0; i < 3; i++)
                    {
                        HAL_GPIO_WritePin(L3D_Port, L3D_Pin, SET);
                        HAL_Delay(corto);
                        HAL_GPIO_WritePin(L3D_Port, L2D_Pin, RESET);
                        HAL_Delay(largo);
                    }
                }
                break;
            case 0x11:
            // Gran tráfico humano
                for (int i = 0; i < 3; i++)
                {
                    HAL_GPIO_WritePin(PA5_GPIO_Port, PA5_Pin, SET);
                    HAL_Delay(largo);
                    HAL_GPIO_WritePin(PA5_GPIO_Port, PA5_Pin, RESET);
                    HAL_Delay(largo);
                    HAL_GPIO_WritePin(PA5_GPIO_Port, PA5_Pin, SET);
                    HAL_Delay(corto);
                    HAL_GPIO_WritePin(PA5_GPIO_Port, PA5_Pin, RESET);
                    HAL_Delay(largo);
                }
            break;
            case 0x12:
            // Giro repentino
                for (int i = 0; i < 2; i++)
                {
                    HAL_GPIO_WritePin(PA5_GPIO_Port, PA5_Pin, SET);
                    HAL_Delay(corto);
                    HAL_GPIO_WritePin(PA5_GPIO_Port, PA5_Pin, RESET);
                    HAL_Delay(largo);
                    HAL_GPIO_WritePin(PA5_GPIO_Port, PA5_Pin, SET);
                    HAL_Delay(corto);
                    HAL_GPIO_WritePin(PA5_GPIO_Port, PA5_Pin, RESET);
                    HAL_Delay(largo);
                    HAL_GPIO_WritePin(PA5_GPIO_Port, PA5_Pin, SET);
                    HAL_Delay(largo);
                    HAL_GPIO_WritePin(PA5_GPIO_Port, PA5_Pin, RESET);
                    HAL_Delay(largo);
                }
            break;
        default:
            continue;
        }
    }
}
void driveModeStatus_task(void *args)
{
    //Si está en modo autónomo(10) se prenderá el led indicador 1, si está en manual se apagará este led.
    uint8_t driveModeStatus_data=0;
    uint8_t driveModeStatusNew_data=0;
    uint32_t corto=20;
    uint32_t largo=200;
    register_canlib_rx(0x51, 0x07, VANTTEC_CANLIB_BYTE, &driveModeStatus_data, 1);
    for (;;)
    {
        if (driveModeStatus == 0x10)
        {
            HAL_GPIO_WritePin(Led1_GPIO_Port, Led1_Pin, SET);
        }
        else if (driveModeStatus == 0x11)
        {
            HAL_GPIO_WritePin(Led1_GPIO_Port, Led1_Pin, RESET);
        }
    }
}
void reverseSwitchStatus_task(void *args)
{
    //Si el carro va de reversa(10) se prendera el led indicador 2, si va de frente, se apagara
    uint8_t reverseSwitchStatus_data=0;
    uint8_t reverseSwitchStatusNew_data=0;
    uint32_t corto=20;
    uint32_t largo=200;
    register_canlib_rx(0x51, 0x07, VANTTEC_CANLIB_BYTE, &reverseSwitchStatus_data, 1);
    for (;;)
    {
        if (reverseSwitchStatus == 0x10)
        {
            HAL_GPIO_WritePin(Led2_GPIO_Port, Led2_Pin, SET);
        }
        else if (reverseSwitchStatus == 0x11)
        {
            HAL_GPIO_WritePin(Led2_GPIO_Port, Led2_Pin, RESET);
        }
    }
}
void safetyModeAlert_task(void *args)
{
    uint8_t safetyModeAlert_data=0;
    uint8_t safetyModeAlertNew_data=0;
    uint32_t corto=20;
    uint32_t largo=200;
    register_canlib_rx(0x51, 0x07, VANTTEC_CANLIB_BYTE, &safetyModeAlertNew_data, 1);
    for (;;)
    {

        if (safetyModeAlert == 0x10)
        {
            while(){
                //Mientras este en safety mode, parpadeara uno si, uno no
            HAL_GPIO_WritePin(Led1_GPIO_Port, Led1_Pin, SET);
            HAL_GPIO_WritePin(Led2_GPIO_Port, Led2_Pin, RESET);
            HAL_GPIO_WritePin(Led3_GPIO_Port, Led3_Pin, SET);
            HAL_GPIO_WritePin(Led4_GPIO_Port, Led4_Pin, RESET);
            HAL_GPIO_WritePin(Led5_GPIO_Port, Led5_Pin, SET);
            HAL_delay(largo);
            HAL_GPIO_WritePin(Led1_GPIO_Port, Led1_Pin, RESET);
            HAL_GPIO_WritePin(Led2_GPIO_Port, Led2_Pin, SET);
            HAL_GPIO_WritePin(Led3_GPIO_Port, Led3_Pin, RESET);
            HAL_GPIO_WritePin(Led4_GPIO_Port, Led4_Pin, SET);
            HAL_GPIO_WritePin(Led5_GPIO_Port, Led5_Pin, RESET);
            HAL_delay(largo);
            }
        }
        else if (safetyModeAlert == 0x11)
        {
            HAL_GPIO_WritePin(Led1_GPIO_Port, Led1_Pin, RESET);
            HAL_GPIO_WritePin(Led2_GPIO_Port, Led2_Pin, RESET);
            HAL_GPIO_WritePin(Led3_GPIO_Port, Led3_Pin, RESET);
            HAL_GPIO_WritePin(Led4_GPIO_Port, Led4_Pin, RESET);
            HAL_GPIO_WritePin(Led5_GPIO_Port, Led5_Pin, RESET);
        }
    }
}
void init_panel_task()
{
    panelMovTaskHandle = osThreadNew(panelMov_task, NULL, &panelMovTaskAttributes);
//    panelDetTaskHandle = osThreadNew(panelDet_task, NULL, &panelDetTaskAttributes);
//    driveModeStatusTaskHandle = osThreadNew(driveModeStatus_task, NULL, &driveModeStatusTaskAttributes);
//    reverseSwitchStatusTaskHandle = osThreadNew(reverseSwitchStatus_task, NULL, &reverseSwitchStatusTaskAttributes);
//    safetyModeAlertTaskHandle = osThreadNew(safetyModeAlert_task, NULL, &safetyModeAlertTaskAttributes);
}
