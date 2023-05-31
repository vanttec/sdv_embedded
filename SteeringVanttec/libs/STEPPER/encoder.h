/*
 * encoder.h
 *
 *  Created on: May 18, 2023
 *      Author: vSebas
 */

#ifndef INC_ENCODER_H_
#define INC_ENCODER_H_

#include "stdint.h"

typedef struct {
	uint32_t steps;
	uint32_t revolutions;
	uint32_t bit_resolution;

	float 	absolute_angle;
	float 	angle;			//0 - 360 degrees
	int16_t turn;
} encoder;

void parse_ifm_encoder(uint32_t pos);
void parse_briter_encoder(uint32_t pos);

#endif /* INC_ENCODER_H_ */
