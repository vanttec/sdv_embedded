#include "encoder.h"

encoder ifm_encoder = {.steps = 4096, .revolutions = 4096, .resolution = 16777216};
encoder briter_encoder  = {.steps = 0, .revolutions = 0, .resolution = 0};

void parse_ifm_encoder(uint32_t pos){
	int32_t position = pos;
	int32_t step = pos%ifm_encoder.steps;

	if(position > ifm_encoder.resolution/2)
	{
		position -= ifm_encoder.resolution;
		step -= ifm_encoder.steps;
	}

	ifm_encoder.turn = -(int16_t) position/ifm_encoder.steps;	// To account for gear counter rotation
	ifm_encoder.absolute_angle = - (float) 360*position/ifm_encoder.revolutions;  	// To account for gear counter rotation
	ifm_encoder.angle = -(float) 360*step/ifm_encoder.steps;  	// To account for gear counter rotation
}

void parse_briter_encoder(uint32_t pos){

}
