/*
 * can_bus.h
 *
 *  Created on: Jan 7, 2022
 *      Author: abiel
 */

#ifndef SRC_CAN_CAN_BUS_H_
#define SRC_CAN_CAN_BUS_H_

#include <stdint.h>

#define HEARTBEAT_ID 0x01
//16 SBUS channels
/*
#define SBUS_ID_START 0x02
#define SBUS_ID_END 0x12
#define SBUS_FAILSAFE 0x13
*/
#define DEBUG_LOG_ID 0x14

// Motor IDs
#define STEPPERS_STATE 0x15
#define CONTROLLER_STEERING 0x16
#define AUTONOMOUS_STEERING 0x17

#define CONTROLLER_BRAKING 0x15
#define AUTONOMOUS_BRAKING 0x17

//2 encoders
#define ENCODER_ID_IFM 0x18
#define ENCODER_ID_BRITTER 0x19

//Jetson heartbeat
#define JETSON_HEARTBEAT_ID 0x1E
#define PING_ID 0x1F
//#define BATTERY_VOLTAGE_ID 0x20

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	uint16_t motor_1_steps;	// Steps
	uint16_t motor_2_steps;	// Steps
	int8_t motor_1_direction;
	int8_t motor_2_direction;
	float encoderAngle[2];		// Degrees
	uint32_t jetsonHBTick;
} can_rx;

typedef struct {
	uint32_t heartbeat;
	uint16_t sbusData[16];
	uint8_t sbusIsFailsafe;
} can_tx;

extern volatile can_rx can_rx_data;
extern volatile can_tx can_tx_data;

typedef struct {
	uint8_t buf[8];
	uint8_t msg_size;
} CAN_TX_QUEUE_OBJ;

#ifdef __cplusplus
}
#endif

#endif /* SRC_CAN_CAN_BUS_H_ */
