#include "encoder_task.h"
#include <stdint.h>
#include <cmsis_os.h>
#include <string.h>

#define IFM_NODE_ID 0x20
#define BRITER_CAN_ID 0x13
#define BRITER_RETURN_TIME_MICROSECONDS 10000
#define BRITER_RETURN_TIME_LOWER_BYTE (BRITER_RETURN_TIME_MICROSECONDS & 0xff)
#define BRITER_RETURN_TIME_UPPER_BYTE ((BRITER_RETURN_TIME_MICROSECONDS & 0xff00) >> 16)

// Temporary global variable to simplify debugging
uint32_t ifm_encoder_raw = 0;
uint32_t briter_encoder_raw = 0;

// When we got last encoder value.
uint32_t g_ifm_encoder_tick_last_update = 0;
uint32_t g_briter_encoder_tick_last_update = 0;

// Encoder position in turns
float g_ifm_encoder_position = 0;
float g_briter_encoder_position = 0;

void encoder_error_handler(){
	for(;;){
		;
	}
}

HAL_StatusTypeDef encoder_setup_can(CAN_HandleTypeDef *hcan) {
    CAN_FilterTypeDef filter;
	filter.FilterBank = 0;
	filter.FilterMode = CAN_FILTERMODE_IDLIST;
	filter.FilterScale = CAN_FILTERSCALE_32BIT;
	filter.FilterFIFOAssignment = CAN_RX_FIFO1;
	filter.FilterActivation = CAN_FILTER_ENABLE;
    // First ID in list.
	filter.FilterIdHigh = 0;
	filter.FilterIdLow = 0x01A0; // IFM

    // Mask is now second ID.
	filter.FilterMaskIdHigh = 0;
	filter.FilterMaskIdLow = 0x0013; // BRITER
	
	HAL_StatusTypeDef ret = HAL_CAN_ConfigFilter(hcan, &filter);
	return ret;
}

// Send op mode for ifm encoder.
HAL_StatusTypeDef encoder_initialize_op_mode(CAN_HandleTypeDef *hcan){
	// TODO Should we send configuration data on boot?
	static const uint8_t initialization_data[] = {0x01, 0x00};

	CAN_TxHeaderTypeDef header;
	header.IDE = CAN_ID_STD;
	header.StdId = BRITER_CAN_ID;
	header.RTR = CAN_RTR_DATA;
	header.DLC = 2;

	uint32_t mailbox;
	HAL_StatusTypeDef ret = HAL_CAN_AddTxMessage(hcan, &header, initialization_data, &mailbox);


	return ret;
}

// Initialize briter encoder, send post back mode.
HAL_StatusTypeDef encoder_initialize_briter(CAN_HandleTypeDef *hcan){
	static const uint8_t auto_post_back_mode[] = {0x04, BRITER_CAN_ID, 0x04, 0xAA};

	CAN_TxHeaderTypeDef header;
	header.IDE = CAN_ID_STD;
	header.StdId = BRITER_CAN_ID;
	header.RTR = CAN_RTR_DATA;
	header.DLC = 4;

	uint32_t mailbox;
	HAL_StatusTypeDef ret = HAL_CAN_AddTxMessage(hcan, &header, auto_post_back_mode, &mailbox);
	if(ret != HAL_OK){
		return ret;
	}

	if(BRITER_RETURN_TIME_MICROSECONDS < 50 || BRITER_RETURN_TIME_MICROSECONDS > 65535){
		return HAL_ERROR;
	}

	// Set return time.
	static uint8_t return_time[] = {0x05, BRITER_CAN_ID, 0x05, BRITER_RETURN_TIME_LOWER_BYTE, BRITER_RETURN_TIME_UPPER_BYTE};
	header.DLC = 4;
	ret = HAL_CAN_AddTxMessage(hcan, &header, return_time, &mailbox);

	// TODO We should probably check for encoder response
	return ret;
}

void encoder_task(void *attrs_hcan){
	CAN_HandleTypeDef *hcan = attrs_hcan;
	CAN_RxHeaderTypeDef header;
	uint8_t buf[8];

	HAL_StatusTypeDef ret = encoder_initialize_op_mode(hcan);
	if(ret != HAL_OK){
		encoder_error_handler();
	}

	ret = encoder_initialize_briter(hcan);
	if(ret != HAL_OK){
		encoder_error_handler();
	}
	osDelay(500);

	for(;;){
		if(HAL_CAN_GetRxFifoFillLevel(hcan, CAN_RX_FIFO1) && HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO1, &header, buf) == HAL_OK){
			uint8_t can_open_node_id = header.StdId & 0x7F; // LSB 7 bits of id.
			uint8_t can_open_func_code = (header.StdId & 0x780) >> 7; // MSB 4 bits of id.
			// FuncCode 0b0011 TPDO1.
			if(can_open_node_id == IFM_NODE_ID && can_open_func_code == 0b0011){
				// TPDO 1 from encoder.
				if(header.DLC != 4){
					encoder_error_handler();
				}

				memcpy(&ifm_encoder_raw, buf, 4);
				g_ifm_encoder_position = (float) ifm_encoder_raw / 4096.0f;
				g_ifm_encoder_tick_last_update = HAL_GetTick();
			} else if(header.StdId == BRITER_CAN_ID){
				// Briter encoder message.
				// We have to parse an annoying data frame where -> [size, device id, data id, data(1..4)]
				// uint32_t are returned low bytes first.

				// For position value, size = 0x07, id = 0x01.
				if(buf[0] == 0x07 && buf[1] == BRITER_CAN_ID && buf[2] == 0x01){
					uint32_t encoder_position;
					memcpy(&encoder_position, buf + 3, 4);
					briter_encoder_raw = __builtin_bswap32(encoder_position);
					g_briter_encoder_tick_last_update = HAL_GetTick();

					// TODO Parse raw pulses into turns.
				}
			}
		}

		osDelay(10);
	}
}