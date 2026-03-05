	/*
 * panel_tasks.c
 *
 *  Created on: March 3, 2026
 *      Author: books
 */
#include "main.h"
#include "vanttec_canlib_tx_task.h"
#include "vanttec_canlib_rx_task.h"
#include "ltc4151cms.h"
#include "vanttec_sdv_ids.h"

osThreadId_t panelMovTaskHandle;
const osThreadAttr_t panelMovTaskAttributes = {
 	.name = "panelMov",
 	.stack_size = 128 * 4};

void panelMov_task(void *args)
{
	uint8_t L1D_encendidas=0;
	uint8_t L1D_apagadas=1; //tipo y nombre y la igualacion (0-255) 
	//1 para que se mantengan apagadas al inciar
	
	//L1D altas
	register_canlib_rx(VANTTEC_CAN_ID_PANEL_RX, 0x05, VANTTEC_CANLIB_BYTE, &L1D_apagadas, 1); 
	register_canlib_rx(VANTTEC_CAN_ID_PANEL_RX, 0x06, VANTTEC_CANLIB_BYTE, &L1D_encendidas, 1);
	//L2D LEFT blinker
	register_canlib_rx(VANTTEC_CAN_ID_PANEL_RX, 0x07, VANTTEC_CANLIB_BYTE, &L2D_apagadas, 1);
	register_canlib_rx(VANTTEC_CAN_ID_PANEL_RX, 0x08, VANTTEC_CANLIB_BYTE, &L2D_encendidas, 1);
	//L3D RIGHT blinker
	register_canlib_rx(VANTTEC_CAN_ID_PANEL_RX, 0x09, VANTTEC_CANLIB_BYTE, &L3D_apagadas, 1);
	register_canlib_rx(VANTTEC_CAN_ID_PANEL_RX, 0x10, VANTTEC_CANLIB_BYTE, &L3D_encendidas, 1);
	
	for (;;)
	{
		//L1D ALTAS
		//si recibo 1 prende luces y si no pues no
		if (L1D_encedidas == 1){
			HAL_GPIO_PinWrite(L1D_GPIO_Port, L1D_Pin, SET);
		}
		else if (L1D_apagadas == 1){
					HAL_GPIO_PinWrite(L1D_GPIO_Port, L1D_Pin, RESET);
				}
		//L2D LEFT BLINKER
		//si recibo 1 prende luces y si no pues no
		if (L2D_encedidas == 1){
			HAL_GPIO_PinWrite(L2D_GPIO_Port, L2D_Pin, SET);
		}
		else if (L2D_apagadas == 1){
			HAL_GPIO_PinWrite(L2D_GPIO_Port, L2D_Pin, RESET);
				}
		//L3D RIGHT BLINKER
		//si recibo 1 prende luces y si no pues no
		if (L3D_encedidas == 1){
			HAL_GPIO_PinWrite(L3D_GPIO_Port, L3D_Pin, SET);
		}
		else if (L3D_apagadas == 1){
			HAL_GPIO_PinWrite(L3D_GPIO_Port, L3D_Pin, RESET);
				}
	}
}

void init_panel_task()
{
	panelMovTaskHandle = osThreadNew(panelMov_task, NULL, &panelMovTaskAttributes);
	debugTaskHandle = osThreadNew(debug_task, NULL, &debugTaskAttributes);
	panelDetTaskHandle = osThreadNew(panelDet_task, NULL, &panelDetTaskAttributes);
	driveModeStatusFlagTaskHandle = osThreadNew(driveModeStatusFlag_task, NULL, &driveModeStatusFlagTaskAttributes);
//	reverseSwitchStatusFlagTaskHandle = osThreadNew(reverseSwitchStatusFlag_task, NULL, &reverseSwitchStatusFlagTaskAttributes);
	driverPresentFlagTaskHandle = osThreadNew(driverPresentFlag_task, NULL, &driverPresentFlagTaskAttributes);
	detectLaneFlagTaskHandle = osThreadNew(detectLaneFlag_task, NULL, &detectLaneFlagTaskAttributes);
	safetyModeAlertFlagTaskHandle = osThreadNew(safetyModeAlertFlag_task, NULL, &safetyModeAlertFlagTaskAttributes);
	recognizeTrafficSignFlagTaskHandle = osThreadNew(recognizeTrafficSignFlag_task, NULL, &recognizeTrafficSignFlagTaskAttributes);
	objectNotificationFlagTaskHandle = osThreadNew(objectNotificationFlag_task, NULL, &objectNotificationFlagTaskAttributes);
	showTaskHandle = osThreadNew(show_task, NULL, &showTaskAttributes);
//	multimeterTaskHandle = osThreadNew(multimeter_task, NULL, &multimeterAttributes);
	sendtxTaskHandle = osThreadNew(send_tx_task, NULL, &sendtxAttributes);
}
