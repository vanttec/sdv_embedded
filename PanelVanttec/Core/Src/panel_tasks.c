	/*
 * panel_tasks.c
 *
 *  Created on: May 9, 2023
 *      Author: books
 */
#include "main.h"
#include "vanttec_canlib_tx_task.h"
#include "vanttec_canlib_rx_task.h"
#include "ltc4151cms.h"
#include "vanttec_canlib_generic_ids.h"

osThreadId_t debugTaskHandle;
const osThreadAttr_t debugTaskAttributes = {
	.name = "debug",
	.stack_size = 128 * 4};
osThreadId_t panelMovTaskHandle;
const osThreadAttr_t panelMovTaskAttributes = {
 	.name = "panelMov",
 	.stack_size = 128 * 4};
osThreadId_t panelDetTaskHandle;
const osThreadAttr_t panelDetTaskAttributes = {
	.name = "panelDet",
	.stack_size = 128 * 4};
osThreadId_t driveModeStatusFlagTaskHandle;
const osThreadAttr_t driveModeStatusFlagTaskAttributes = {
	.name = "driveModeStatusFlag",
	.stack_size = 128 * 4};
osThreadId_t reverseSwitchStatusFlagTaskHandle;
const osThreadAttr_t reverseSwitchStatusFlagTaskAttributes = {
	.name = "reverseSwitchStatusFlag",
	.stack_size = 128 * 4};
osThreadId_t safetyModeAlertFlagTaskHandle;
const osThreadAttr_t safetyModeAlertFlagTaskAttributes = {
	.name = "safetyModeAlertFlag",
	.stack_size = 128 * 4};
osThreadId_t objectNotificationFlagTaskHandle;
const osThreadAttr_t objectNotificationFlagTaskAttributes = {
	.name = "objectNotificationFlag",
	.stack_size = 128 * 4};
osThreadId_t recognizeTrafficSignFlagTaskHandle;
const osThreadAttr_t recognizeTrafficSignFlagTaskAttributes = {
	.name = "recognizeTrafficSignFlag",
	.stack_size = 128 * 4};
osThreadId_t detectLaneFlagTaskHandle;
const osThreadAttr_t detectLaneFlagTaskAttributes = {
	.name = "detectLaneFlag",
	.stack_size = 128 * 4};
osThreadId_t multimeterTaskHandle;
const osThreadAttr_t multimeterAttributes = {
	.name = "multimeter",
	.stack_size = 128 * 4};
osThreadId_t sendtxTaskHandle;
const osThreadAttr_t sendtxAttributes = {
	.name = "send_tx",
	.stack_size = 128 * 4};
extern I2C_HandleTypeDef hi2c1;
extern struct LTC4151 multimeter;
extern bool enable_multimeter;
float car_voltage = 0;
int8_t voltage_flag = 0;
void debug_task(void *args)
{
	uint8_t debug_data = 0;
	uint32_t corto = 20;
	uint32_t largo = 200;
	// register_canlib_rx(VANTTEC_CAN_ID_PANEL_RX, 0x04, VANTTEC_CANLIB_BYTE, &debug_data, 1);
	for (;;)
	{
		HAL_GPIO_WritePin(DEBUG_1_GPIO_Port, DEBUG_1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(DEBUG_3_GPIO_Port, DEBUG_3_Pin, GPIO_PIN_SET);
		/*HAL_GPIO_WritePin(L3D_GPIO_Port, L3D_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(L2D_GPIO_Port, L2D_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(L1D_GPIO_Port, L1D_Pin, GPIO_PIN_SET); 
		HAL_GPIO_WritePin(L4D_GPIO_Port, L4D_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(L5D_GPIO_Port, L5D_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(STMTB1_GPIO_Port, STMTB1_Pin, SET);
		HAL_GPIO_WritePin(STMTB2_GPIO_Port, STMTB2_Pin, SET);
		HAL_GPIO_WritePin(STMTB3_GPIO_Port, STMTB3_Pin, SET);
		HAL_GPIO_WritePin(STMTB4_GPIO_Port, STMTB4_Pin, SET);
		HAL_GPIO_WritePin(STMTB5_GPIO_Port, STMTB5_Pin, SET);
		*/
		osDelay(1000);
	}
}
void panelMov_task(void *args)
{
	uint8_t panelMov_data=0;
	uint8_t last_panelMov_data = 0;
	uint32_t corto = 600;
	uint32_t largo = 1200;
	uint8_t buf[8];
	register_canlib_rx(VANTTEC_CAN_ID_PANEL_RX, 0x05, VANTTEC_CANLIB_BYTE, &panelMov_data, 1);
	for (;;)
	{
			// Frenado
			if (panelMov_data == 0x10)
			{
				HAL_GPIO_WritePin(L3D_GPIO_Port, L3D_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(L2D_GPIO_Port, L2D_Pin, GPIO_PIN_SET);
			 	HAL_GPIO_WritePin(L1D_GPIO_Port, L1D_Pin, GPIO_PIN_SET);
				buf[0] = 0x05;
            	buf[1] = 0x0;
            	update_table(VANTTEC_CAN_ID_PANEL_RX, 0x05, buf, 2); 
			}
			// Carro encendido
			else if (panelMov_data == 0x11)
			{
				for (int i = 0; i < 2; i++)
				{
					HAL_GPIO_WritePin(EXTRA_5_GPIO_Port, EXTRA_5_Pin, GPIO_PIN_SET);
					HAL_GPIO_WritePin(L2D_GPIO_Port, L2D_Pin, GPIO_PIN_SET);
					HAL_GPIO_WritePin(L3D_GPIO_Port, L3D_Pin, GPIO_PIN_SET);
					HAL_GPIO_WritePin(L4D_GPIO_Port, L4D_Pin, GPIO_PIN_SET);
					osDelay(corto);
					HAL_GPIO_WritePin(EXTRA_5_GPIO_Port, EXTRA_5_Pin, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(L2D_GPIO_Port, L2D_Pin, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(L3D_GPIO_Port, L3D_Pin, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(L4D_GPIO_Port, L4D_Pin, GPIO_PIN_RESET);
					osDelay(corto);
				}
				HAL_GPIO_WritePin(L1D_GPIO_Port, L1D_Pin, GPIO_PIN_SET);
				panelMov_data = 0x05;
			}
			//Carro apagado
			else if (panelMov_data == 0x12)
			{
				for (int i = 0; i < 3; i++)
				{
					HAL_GPIO_WritePin(EXTRA_5_GPIO_Port, EXTRA_5_Pin, GPIO_PIN_SET);
					HAL_GPIO_WritePin(L2D_GPIO_Port, L2D_Pin, GPIO_PIN_SET);
					HAL_GPIO_WritePin(L3D_GPIO_Port, L3D_Pin, GPIO_PIN_SET);
					HAL_GPIO_WritePin(L4D_GPIO_Port, L4D_Pin, GPIO_PIN_SET);
					osDelay(corto);
					HAL_GPIO_WritePin(EXTRA_5_GPIO_Port, EXTRA_5_Pin, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(L2D_GPIO_Port, L2D_Pin, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(L3D_GPIO_Port, L3D_Pin, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(L4D_GPIO_Port, L4D_Pin, GPIO_PIN_RESET);
					osDelay(corto);
				}
				HAL_GPIO_WritePin(L1D_GPIO_Port, L1D_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(L2D_GPIO_Port, L2D_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(L3D_GPIO_Port, L3D_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(L4D_GPIO_Port, L4D_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(L5D_GPIO_Port, L5D_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(STMTB1_GPIO_Port, STMTB1_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(STMTB2_GPIO_Port, STMTB2_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(STMTB3_GPIO_Port, STMTB3_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(STMTB4_GPIO_Port, STMTB4_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(STMTB5_GPIO_Port, STMTB5_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(EXTRA_5_GPIO_Port, EXTRA_5_Pin, GPIO_PIN_RESET);
				panelMov_data = 0x05;
			}
			//Carro estacionario
			else if(panelMov_data == 0x13){
				HAL_GPIO_WritePin(L3D_GPIO_Port, L3D_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(L2D_GPIO_Port, L2D_Pin, GPIO_PIN_SET);
				osDelay(corto);
				HAL_GPIO_WritePin(L3D_GPIO_Port, L3D_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(L2D_GPIO_Port, L2D_Pin, GPIO_PIN_RESET);
				osDelay(corto);
			}
			//Aceleracion
			else if(panelMov_data == 0x14){
				for (int i = 0; i < 2; i++)
				{
					HAL_GPIO_WritePin(L5D_GPIO_Port, L5D_Pin, GPIO_PIN_SET);
					osDelay(corto);
					HAL_GPIO_WritePin(L5D_GPIO_Port, L5D_Pin, GPIO_PIN_RESET);
					osDelay(corto);
				}
				panelMov_data = 0x05;
			}
			//Tiempo sin cambios
			else if (panelMov_data == 0x15)
			{
				for (int i = 0; i < 3; i++)
				{
					HAL_GPIO_WritePin(L2D_GPIO_Port, L2D_Pin, GPIO_PIN_SET);
					HAL_GPIO_WritePin(L3D_GPIO_Port, L3D_Pin, GPIO_PIN_SET);
					HAL_GPIO_WritePin(L4D_GPIO_Port, L4D_Pin, GPIO_PIN_SET);
					osDelay(corto);
					HAL_GPIO_WritePin(L2D_GPIO_Port, L2D_Pin, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(L3D_GPIO_Port, L3D_Pin, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(L4D_GPIO_Port, L4D_Pin, GPIO_PIN_RESET);
					osDelay(corto);
				}
				HAL_GPIO_WritePin(L1D_GPIO_Port, L1D_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(L2D_GPIO_Port, L2D_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(L3D_GPIO_Port, L3D_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(L4D_GPIO_Port, L4D_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(L5D_GPIO_Port, L5D_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(STMTB1_GPIO_Port, STMTB1_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(STMTB2_GPIO_Port, STMTB2_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(STMTB3_GPIO_Port, STMTB3_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(STMTB4_GPIO_Port, STMTB4_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(STMTB5_GPIO_Port, STMTB5_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(EXTRA_5_GPIO_Port, EXTRA_5_Pin, GPIO_PIN_RESET);
				panelMov_data = 0x05;
			}
			//Reset
			else if(panelMov_data == 0x0A)
			{
				HAL_GPIO_WritePin(L1D_GPIO_Port, L1D_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(L3D_GPIO_Port, L3D_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(L2D_GPIO_Port, L2D_Pin, GPIO_PIN_RESET);
			}
		osDelay(10);
	}
}
void panelDet_task(void *args)
{
	uint8_t panelDet_data = 0;
	uint8_t last_panelDet_data = 0;
	uint32_t corto = 600;
	uint32_t largo = 1200;
	register_canlib_rx(VANTTEC_CAN_ID_PANEL_RX, 0x06, VANTTEC_CANLIB_BYTE, &panelDet_data, 1);
	for (;;)
	{
		if (panelDet_data != last_panelDet_data)
		{
			// Giro Prominente Derecha
			if (panelDet_data == 0x10)
			{
				for (int i = 0; i < 4; i++)
				{
					HAL_GPIO_WritePin(L3D_GPIO_Port, L3D_Pin, SET);
					HAL_GPIO_WritePin(EXTRA_5_GPIO_Port, EXTRA_5_Pin, GPIO_PIN_SET);
					osDelay(corto);
					HAL_GPIO_WritePin(L3D_GPIO_Port, L3D_Pin, RESET);
					HAL_GPIO_WritePin(EXTRA_5_GPIO_Port, EXTRA_5_Pin, GPIO_PIN_RESET);
					osDelay(largo);
				}
				panelDet_data = 0x05;
			}
			// Giro Prominente Izquierda
			else if (panelDet_data == 0x11)
			{
				for (int i = 0; i < 4; i++)
				{
					HAL_GPIO_WritePin(L2D_GPIO_Port, L2D_Pin, SET);
					HAL_GPIO_WritePin(EXTRA_5_GPIO_Port, EXTRA_5_Pin, GPIO_PIN_SET);
					osDelay(corto);
					HAL_GPIO_WritePin(L2D_GPIO_Port, L2D_Pin, RESET);
					HAL_GPIO_WritePin(EXTRA_5_GPIO_Port, EXTRA_5_Pin, GPIO_PIN_RESET);
					osDelay(largo);
				}
				panelDet_data = 0x05;
			}

			else if (panelDet_data == 0x12)
			{
				// Gran tráfico humano
					HAL_GPIO_WritePin(L3D_GPIO_Port, L3D_Pin, SET);
					HAL_GPIO_WritePin(L2D_GPIO_Port, L2D_Pin, SET);
					HAL_GPIO_WritePin(EXTRA_5_GPIO_Port, EXTRA_5_Pin, SET);
					osDelay(largo);
					HAL_GPIO_WritePin(EXTRA_5_GPIO_Port, EXTRA_5_Pin, RESET);
					osDelay(largo);
					HAL_GPIO_WritePin(L3D_GPIO_Port, L3D_Pin, RESET);
					HAL_GPIO_WritePin(L2D_GPIO_Port, L2D_Pin, RESET);
					HAL_GPIO_WritePin(EXTRA_5_GPIO_Port, EXTRA_5_Pin, SET);
					osDelay(corto);
					HAL_GPIO_WritePin(EXTRA_5_GPIO_Port, EXTRA_5_Pin, RESET);
					osDelay(largo);
			}
			else if (panelDet_data == 0x13)
			{
				// Giro repentino derecha
				for (int i = 0; i < 2; i++)
				{
					HAL_GPIO_WritePin(EXTRA_5_GPIO_Port, EXTRA_5_Pin, SET);
					HAL_GPIO_WritePin(L3D_GPIO_Port, L3D_Pin, SET);
					osDelay(corto);
					HAL_GPIO_WritePin(EXTRA_5_GPIO_Port, EXTRA_5_Pin, RESET);
					HAL_GPIO_WritePin(L3D_GPIO_Port, L3D_Pin, RESET);
					osDelay(largo);
					HAL_GPIO_WritePin(EXTRA_5_GPIO_Port, EXTRA_5_Pin, SET);
					HAL_GPIO_WritePin(L3D_GPIO_Port, L3D_Pin, SET);
					osDelay(corto);
					HAL_GPIO_WritePin(EXTRA_5_GPIO_Port, EXTRA_5_Pin, RESET);
					HAL_GPIO_WritePin(L3D_GPIO_Port, L3D_Pin, RESET);
					osDelay(largo);
					HAL_GPIO_WritePin(EXTRA_5_GPIO_Port, EXTRA_5_Pin, SET);
					HAL_GPIO_WritePin(L3D_GPIO_Port, L3D_Pin, SET);
					osDelay(largo);
					HAL_GPIO_WritePin(EXTRA_5_GPIO_Port, EXTRA_5_Pin, RESET);
					HAL_GPIO_WritePin(L3D_GPIO_Port, L3D_Pin, RESET);
					osDelay(largo);
				}
				panelDet_data = 0x05;
			}
			else if (panelDet_data == 0x14)
			{
				// Giro repentino izquierda
				for (int i = 0; i < 2; i++)
				{
					HAL_GPIO_WritePin(EXTRA_5_GPIO_Port, EXTRA_5_Pin, SET);
					HAL_GPIO_WritePin(L2D_GPIO_Port, L2D_Pin, SET);
					osDelay(corto);
					HAL_GPIO_WritePin(EXTRA_5_GPIO_Port, EXTRA_5_Pin, RESET);
					HAL_GPIO_WritePin(L3D_GPIO_Port, L2D_Pin, RESET);
					osDelay(largo);
					HAL_GPIO_WritePin(EXTRA_5_GPIO_Port, EXTRA_5_Pin, SET);
					HAL_GPIO_WritePin(L2D_GPIO_Port, L2D_Pin, SET);
					osDelay(corto);
					HAL_GPIO_WritePin(EXTRA_5_GPIO_Port, EXTRA_5_Pin, RESET);
					HAL_GPIO_WritePin(L2D_GPIO_Port, L2D_Pin, RESET);
					osDelay(largo);
					HAL_GPIO_WritePin(EXTRA_5_GPIO_Port, EXTRA_5_Pin, SET);
					HAL_GPIO_WritePin(L2D_GPIO_Port, L2D_Pin, SET);
					osDelay(largo);
					HAL_GPIO_WritePin(EXTRA_5_GPIO_Port, EXTRA_5_Pin, RESET);
					HAL_GPIO_WritePin(L2D_GPIO_Port, L2D_Pin, RESET);
					osDelay(largo);
				}
				panelDet_data = 0x05;
			}
			//Reset
			else if (panelDet_data == 0x0A)
			{
				HAL_GPIO_WritePin(L3D_GPIO_Port, L3D_Pin, RESET);
				HAL_GPIO_WritePin(L2D_GPIO_Port, L2D_Pin, RESET);
			}
		}
		osDelay(10);
	}
}
void driveModeStatusFlag_task(void *args)
{
	// Si está en modo autónomo(10) se prenderá el led indicador 1, si está en manual se apagará este led.
	uint8_t driveModeStatusFlag_data = 0;
	uint8_t last_driveModeStatusFlag_data = 0;
	register_canlib_rx(VANTTEC_CAN_ID_PANEL_RX, 0x07, VANTTEC_CANLIB_BYTE, &driveModeStatusFlag_data, 1);
	for (;;)
	{
		if (driveModeStatusFlag_data != last_driveModeStatusFlag_data)
		{
			if (driveModeStatusFlag_data == 0x01)
			{
				// Autonomo
				HAL_GPIO_WritePin(STMTB1_GPIO_Port, STMTB1_Pin, SET);
			}
			else
			{
				// Manual
				HAL_GPIO_WritePin(STMTB1_GPIO_Port, STMTB1_Pin, RESET);
			}

			last_driveModeStatusFlag_data = driveModeStatusFlag_data;
		}
		osDelay(10);
	}
}
void reverseSwitchStatusFlag_task(void *args)
{
	// Si el carro va de reversa(10) se prendera el led indicador 2, si va de frente, se apagara
	uint8_t reverseSwitchStatusFlag_data = 0;
	uint8_t last_reverseSwitchStatusFlag_data = 0;
	uint32_t corto = 600;
	uint32_t largo = 1200;
	register_canlib_rx(VANTTEC_CAN_ID_PANEL_RX, 0x08, VANTTEC_CANLIB_BYTE, &reverseSwitchStatusFlag_data, 1);
	for (;;)
	{
		//		if (last_reverseSwitchStatusFlag_data != reverseSwitchStatusFlag_data)
		//		{
		if (reverseSwitchStatusFlag_data == 0x10)
		{
			// Reversa
			HAL_GPIO_WritePin(STMTB2_GPIO_Port, STMTB2_Pin, SET);
			HAL_GPIO_WritePin(L3D_GPIO_Port, L3D_Pin, SET);
			HAL_GPIO_WritePin(L2D_GPIO_Port, L2D_Pin, SET);
			HAL_GPIO_WritePin(EXTRA_5_GPIO_Port, EXTRA_5_Pin, SET);
			osDelay(corto);
			HAL_GPIO_WritePin(L3D_GPIO_Port, L3D_Pin, RESET);
			HAL_GPIO_WritePin(L2D_GPIO_Port, L2D_Pin, RESET);
			HAL_GPIO_WritePin(EXTRA_5_GPIO_Port, EXTRA_5_Pin, RESET);
			osDelay(largo);
		}
		// De frente
		else
		{
			HAL_GPIO_WritePin(STMTB2_GPIO_Port, STMTB2_Pin, RESET);
		}
		last_reverseSwitchStatusFlag_data = reverseSwitchStatusFlag_data;
		//}
		osDelay(10);
	}
}
void safetyModeAlertFlag_task(void *args)
{
	uint8_t safetyModeAlertFlag_data = 0;
	uint8_t last_safetyModeAlertFlag_data = 0;
	uint32_t corto = 600;
	uint32_t largo = 1200;
	register_canlib_rx(VANTTEC_CAN_ID_PANEL_RX, 0x09, VANTTEC_CANLIB_BYTE, &safetyModeAlertFlag_data, 1);
	for (;;)
	{
		if (last_safetyModeAlertFlag_data != safetyModeAlertFlag_data)
		{
			if (safetyModeAlertFlag_data == 0x10)
			{
				// Mientras este en safety mode, parpadeara TODO
				//On
				HAL_GPIO_WritePin(L1D_GPIO_Port, L1D_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(L2D_GPIO_Port, L2D_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(L3D_GPIO_Port, L3D_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(L4D_GPIO_Port, L4D_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(L5D_GPIO_Port, L5D_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(STMTB1_GPIO_Port, STMTB1_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(STMTB2_GPIO_Port, STMTB2_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(STMTB3_GPIO_Port, STMTB3_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(STMTB4_GPIO_Port, STMTB4_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(STMTB5_GPIO_Port, STMTB5_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(EXTRA_5_GPIO_Port, EXTRA_5_Pin, GPIO_PIN_SET);
				osDelay(largo);
				HAL_GPIO_WritePin(L1D_GPIO_Port, L1D_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(L2D_GPIO_Port, L2D_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(L3D_GPIO_Port, L3D_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(L4D_GPIO_Port, L4D_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(L5D_GPIO_Port, L5D_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(STMTB1_GPIO_Port, STMTB1_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(STMTB2_GPIO_Port, STMTB2_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(STMTB3_GPIO_Port, STMTB3_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(STMTB4_GPIO_Port, STMTB4_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(STMTB5_GPIO_Port, STMTB5_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(EXTRA_5_GPIO_Port, EXTRA_5_Pin, GPIO_PIN_RESET);
				osDelay(largo);
			}
			else if (safetyModeAlertFlag_data == 0x11)
			{
				//Off
				HAL_GPIO_WritePin(L1D_GPIO_Port, L1D_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(L2D_GPIO_Port, L2D_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(L3D_GPIO_Port, L3D_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(L4D_GPIO_Port, L4D_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(L5D_GPIO_Port, L5D_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(STMTB1_GPIO_Port, STMTB1_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(STMTB2_GPIO_Port, STMTB2_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(STMTB3_GPIO_Port, STMTB3_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(STMTB4_GPIO_Port, STMTB4_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(STMTB5_GPIO_Port, STMTB5_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(EXTRA_5_GPIO_Port, EXTRA_5_Pin, GPIO_PIN_RESET);
			}
		}
		osDelay(10);
	}
}
void objectNotificationFlag_task(void *args)
{
	uint8_t objectNotificationFlag_data = 0;
	uint8_t last_objectNotificationFlag_data = 0;
	uint32_t corto = 600;
	uint32_t largo = 1200;
	register_canlib_rx(VANTTEC_CAN_ID_PANEL_RX, 0x10, VANTTEC_CANLIB_BYTE, &objectNotificationFlag_data, 1);
	for (;;)
	{
		if (last_objectNotificationFlag_data != objectNotificationFlag_data)
		{
			if (objectNotificationFlag_data == 0x10)
			{
				// Si hay un objeto en frente cerca se prende por unos segundos
				HAL_GPIO_WritePin(STMTB3_GPIO_Port, STMTB3_Pin, SET);
				HAL_GPIO_WritePin(EXTRA_5_GPIO_Port, EXTRA_5_Pin, SET);
				osDelay(corto);
				HAL_GPIO_WritePin(EXTRA_5_GPIO_Port, EXTRA_5_Pin, RESET);
				osDelay(largo);
			}
			else if (objectNotificationFlag_data == 0x11)
			{
				// Si hay un objeto lejos parpadea
				HAL_GPIO_WritePin(STMTB3_GPIO_Port, STMTB3_Pin, SET);
				osDelay(corto);
				HAL_GPIO_WritePin(STMTB3_GPIO_Port, STMTB3_Pin, RESET);
				osDelay(largo);
			}
			else if(objectNotificationFlag_data == 0x12)
			{
				//Si no detecta nada, reset
				HAL_GPIO_WritePin(STMTB3_GPIO_Port, STMTB3_Pin, RESET);
				HAL_GPIO_WritePin(EXTRA_5_GPIO_Port, EXTRA_5_Pin, RESET);
			}

		}
		osDelay(10);
	}
}
void recognizeTrafficSignFlag_task(void *args)
{
	uint8_t recognizeTrafficSignFlag_data = 0;
	uint8_t last_recognizeTrafficSignFlag_data = 0;
	uint32_t corto = 600;
	uint32_t largo = 1200;
	register_canlib_rx(VANTTEC_CAN_ID_PANEL_RX, 0x11, VANTTEC_CANLIB_BYTE, &recognizeTrafficSignFlag_data, 1);
	for (;;)
	{
		if (last_recognizeTrafficSignFlag_data != recognizeTrafficSignFlag_data)
		{
			if (recognizeTrafficSignFlag_data == 0x10)
			{
				// Si es un STOP Sign se prende
				HAL_GPIO_WritePin(STMTB3_GPIO_Port, STMTB3_Pin, SET);
			}
			else if (recognizeTrafficSignFlag_data == 0x11)
			{
				// Si es un Pedestrian Crossing Sign, parpadea
				HAL_GPIO_WritePin(STMTB3_GPIO_Port, STMTB3_Pin, SET);
				osDelay(corto);
				HAL_GPIO_WritePin(STMTB3_GPIO_Port, STMTB3_Pin, RESET);
				osDelay(largo);
			}
			else if (recognizeTrafficSignFlag_data == 0x0A)
			{
				//Reset
				HAL_GPIO_WritePin(STMTB3_GPIO_Port, STMTB3_Pin, RESET);
			}
		}
		osDelay(10);
	}
}
void detectLaneFlag_task(void *args)
{
	uint8_t detectLaneFlag_data = 0;
	uint8_t last_detectLaneFlag_data = 0;
	register_canlib_rx(VANTTEC_CAN_ID_PANEL_RX, 0x12, VANTTEC_CANLIB_BYTE, &detectLaneFlag_data, 1);
	uint32_t corto = 600;
	uint32_t largo = 1200;
	for (;;)
	{
		if (last_detectLaneFlag_data != detectLaneFlag_data)
		{
			if (detectLaneFlag_data == 0x10)
			{
				// Mientras haya linea presente
				HAL_GPIO_WritePin(STMTB5_GPIO_Port, STMTB5_Pin, SET);
			}
			else if (detectLaneFlag_data == 0x11)
			{
				// Cuando este fuera de la linea
				HAL_GPIO_WritePin(STMTB5_GPIO_Port, STMTB5_Pin, SET);
				osDelay(largo);
				HAL_GPIO_WritePin(STMTB5_GPIO_Port, STMTB5_Pin, RESET);
				osDelay(largo);
			}
			// Cuando no haya linea
			else
			{
				HAL_GPIO_WritePin(STMTB5_GPIO_Port, STMTB5_Pin, RESET);
			}
			last_detectLaneFlag_data = detectLaneFlag_data;
		}
		osDelay(10);
	}
}
void multimeter_task(void *args)
{
	uint8_t multimeter_data = 0;
	uint8_t buf[8];
	register_canlib_rx(VANTTEC_CAN_ID_PANELRX, 0x13, VANTTEC_CANLIB_BYTE, &multimeter_data, 1);
	for (;;)
	{
		if (multimeter_data == 0x1)
		{
			voltage_flag = 1;
			buf[0] = 0x13;
			buf[1] = 3;
			update_table(VANTTEC_CAN_ID_PANELRX, 0x13, buf, 2);
		}
		osDelay(10);
	}
}
void send_tx_task(void *args)
{
	for (;;)
	{
		if (enable_multimeter)
		{
			if (voltage_flag == 1)
			{
				car_voltage = (float)getSnapshotInputVoltage(&hi2c1, &multimeter);
				canlib_send_float(0x5, car_voltage);
				voltage_flag = 0;
			}
		}

		osDelay(10);
	}
}
void init_panel_task()
{
	panelMovTaskHandle = osThreadNew(panelMov_task, NULL, &panelMovTaskAttributes);
	debugTaskHandle = osThreadNew(debug_task, NULL, &debugTaskAttributes);
	panelDetTaskHandle = osThreadNew(panelDet_task, NULL, &panelDetTaskAttributes);
	driveModeStatusFlagTaskHandle = osThreadNew(driveModeStatusFlag_task, NULL, &driveModeStatusFlagTaskAttributes);
	reverseSwitchStatusFlagTaskHandle = osThreadNew(reverseSwitchStatusFlag_task, NULL, &reverseSwitchStatusFlagTaskAttributes);
	safetyModeAlertFlagTaskHandle = osThreadNew(safetyModeAlertFlag_task, NULL, &safetyModeAlertFlagTaskAttributes);
	recognizeTrafficSignFlagTaskHandle = osThreadNew(recognizeTrafficSignFlag_task, NULL, &recognizeTrafficSignFlagTaskAttributes);
	objectNotificationFlagTaskHandle = osThreadNew(objectNotificationFlag_task, NULL, &objectNotificationFlagTaskAttributes);
	multimeterTaskHandle = osThreadNew(multimeter_task, NULL, &multimeterAttributes);
	sendtxTaskHandle = osThreadNew(send_tx_task, NULL, &sendtxAttributes);
}
