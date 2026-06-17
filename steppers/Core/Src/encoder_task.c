#include "encoder_task.h"
#include <stdint.h>
#include <cmsis_os.h>
#include <string.h>
#include <math.h>
#include "main.h"

#define IFM_NODE_ID 0x20
#define BRITER_CAN_ID 0x13
#define BRITER_RETURN_TIME_MICROSECONDS 0xfffe
#define BRITER_RETURN_TIME_LOWER_BYTE (BRITER_RETURN_TIME_MICROSECONDS & 0xff)
#define BRITER_RETURN_TIME_UPPER_BYTE ((BRITER_RETURN_TIME_MICROSECONDS & 0xff00) >> 16)

// Temporary global variable to simplify debugging
int32_t ifm_encoder_raw = 0;
float briter_encoder_raw = 0.0;

// When we got last encoder value.
uint32_t g_ifm_encoder_tick_last_update = 0;
uint32_t g_briter_encoder_tick_last_update = 0;

// Encoder position in turns
float g_ifm_encoder_position   = 0;
float g_ifm_mechanism_position = 0;  // = g_ifm_encoder_position / -16.0 (mechanism angle, same space as setpoints)
float g_briter_encoder_position = 0;

void encoder_error_handler(){
	; // non-fatal: log and continue
}

HAL_StatusTypeDef encoder_setup_can(CAN_HandleTypeDef *hcan) {
    CAN_FilterTypeDef filter;
	filter.FilterBank = 2;
	filter.FilterMode = CAN_FILTERMODE_IDLIST;
	filter.FilterScale = CAN_FILTERSCALE_16BIT;
	filter.FilterFIFOAssignment = CAN_RX_FIFO1;
	filter.FilterActivation = CAN_FILTER_ENABLE;
	filter.SlaveStartFilterBank = 14; // was uninitialized (garbage); match canlib

	filter.FilterIdHigh = 0x013 << 5u; // Encoder briter
	filter.FilterIdLow = 0x01A0 << 5u; // Encoder IFM

	filter.FilterMaskIdLow = 0x0000;
	filter.FilterMaskIdHigh = 0x0000;

	HAL_StatusTypeDef ret = HAL_CAN_ConfigFilter(hcan, &filter);
	return ret;
}

// Send op mode for ifm encoder.
HAL_StatusTypeDef encoder_initialize_op_mode(CAN_HandleTypeDef *hcan){
	// TODO Should we send configuration data on boot?
	static const uint8_t initialization_data[] = {0x01, 0x00};
	HAL_StatusTypeDef ret;
	uint32_t mailbox;

	CAN_TxHeaderTypeDef header;
	header.IDE = CAN_ID_STD;
	header.RTR = CAN_RTR_DATA;

	header.StdId = 0x620;
	header.DLC = 8;
	static const uint8_t set_midpoint[] = {0x23, 0x03, 0x60, 0x00, 0x00, 0x00, 0x00, 0x80};
	
	// ret = HAL_CAN_AddTxMessage(hcan, &header, set_midpoint, &mailbox);

	header.StdId = 0x00;
	header.DLC = 2;
	ret = HAL_CAN_AddTxMessage(hcan, &header, initialization_data, &mailbox);

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

	osDelay(50);

	// Set return time.
	static uint8_t return_time_msg[] = {0x05, BRITER_CAN_ID, 0x05, 0x40, 0x9C};
	header.DLC = 5;
	ret = HAL_CAN_AddTxMessage(hcan, &header, return_time_msg, &mailbox);
	if(ret != HAL_OK){
		return ret;
	}

	osDelay(50);

	// set initial position as 0
//	static uint8_t init_pos_msg[] = {0x04, BRITER_CAN_ID, 0x06, 0x00};
//	header.DLC = 4;
//	ret = HAL_CAN_AddTxMessage(hcan, &header, init_pos_msg, &mailbox);
//	if(ret != HAL_OK){
//		return ret;
//	}

	// TODO We should probably check for encoder response
	return ret;
}

void encoder_task(void *attrs_hcan){
	CAN_HandleTypeDef *hcan = attrs_hcan;
	CAN_RxHeaderTypeDef header;
	uint8_t buf[8];

	osDelay(100);

	encoder_initialize_op_mode(hcan);
	encoder_initialize_briter(hcan);
	osDelay(500);

	for(;;){
		if(HAL_CAN_GetRxFifoFillLevel(hcan, CAN_RX_FIFO1) && HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO1, &header, buf) == HAL_OK){
			uint8_t can_open_node_id = header.StdId & 0x7F; // LSB 7 bits of id.
			uint8_t can_open_func_code = (header.StdId & 0x780) >> 7; // MSB 4 bits of id.

			// FuncCode 0b0011 TPDO1.
			if(can_open_node_id == IFM_NODE_ID && can_open_func_code == 0b0011){
				// TPDO 1 from encoder.
				if(header.DLC == 4){
					memcpy(&ifm_encoder_raw, buf, 4);

					ifm_encoder_raw -= 0x800000;

					g_ifm_encoder_position   = ((float) ifm_encoder_raw / 4096.0f) * 2.0 * M_PI * -1;
					g_ifm_mechanism_position = -g_ifm_encoder_position / 16.0f;
					g_ifm_encoder_tick_last_update = HAL_GetTick();
				}
			} else if(header.StdId == BRITER_CAN_ID){
				// Briter encoder message.
				// We have to parse an annoying data frame where -> [size, device id, data id, data(1..4)]
				// uint32_t are returned low bytes first.

				// For position value, size = 0x07, id = 0x01.
				if(buf[0] == 0x07 && buf[1] == BRITER_CAN_ID && buf[2] == 0x01){
					uint32_t encoder_position;
					memcpy(&encoder_position, buf + 3, 4);
					briter_encoder_raw = (float) (encoder_position - 49152.0); //To reset encoder, send manually with cansend [0x04][0x13][0x0C][0x01] to set the midpoint of the encoder
					// 49152 is the midpoint of the encoder value, we subtract that, so the value becomes 0 at reset

					// TODO Parse raw pulses into turns.
					g_briter_encoder_position = ((float) briter_encoder_raw / 1024.0f) * 2.0 * M_PI;


					// encoder goes from 0.0 to 1.0
					// but if for some reason, the encoder goes backwards because of mechanical
					// deformities, etc. this will reset the value to 0.0
//					if (g_briter_encoder_position > 2.0){ //TODO fix this
//						uint32_t mailbox;
//							CAN_TxHeaderTypeDef s_header;
//							s_header.IDE = CAN_ID_STD;
//							s_header.StdId = BRITER_CAN_ID;
//							s_header.RTR = CAN_RTR_DATA;
//							s_header.DLC = 4;
//						static uint8_t init_pos_msg[] = {0x04, BRITER_CAN_ID, 0x06, 0x00};
//						HAL_CAN_AddTxMessage(hcan, &s_header, init_pos_msg, &mailbox); //TODO fix this, it resets the encoder unnecessarily
//					}

					g_briter_encoder_tick_last_update = HAL_GetTick();
				}
			}
		}
		osDelay(10);
	}
}
