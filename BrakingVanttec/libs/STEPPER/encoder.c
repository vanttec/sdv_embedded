#include "encoder.h"

encoder ifm_encoder = {.steps = 4096, .revolutions = 4096, .bit_resolution = 16777216};
encoder briter_encoder  = {.steps = 4096/*steps p rev*/, .revolutions = 24, .bit_resolution = 4096*24};

void parse_ifm_encoder(uint32_t pos){
	// Absolute position with positive and negative angle values
	int32_t position = pos;
	int32_t step = pos%ifm_encoder.steps;

	if(position > ifm_encoder.bit_resolution/2)
	{
		position -= ifm_encoder.bit_resolution;
		step -= ifm_encoder.steps;
	}

	ifm_encoder.turn = -(int16_t) position/ifm_encoder.steps;	// To account for gear counter rotation
	ifm_encoder.absolute_angle = -(float) 360*position/ifm_encoder.revolutions;  	// To account for gear counter rotation
	ifm_encoder.angle = -(float) 360*step/ifm_encoder.steps;  	// To account for gear counter rotation
}

void parse_briter_encoder(uint32_t pos){
	uint32_t step = pos%briter_encoder.steps;

	briter_encoder.turn = (int16_t) pos/briter_encoder.steps;
	briter_encoder.absolute_angle =  (float) 360*step/briter_encoder.revolutions;
	briter_encoder.angle = (float) 360*step/briter_encoder.steps;
}
