#include "encoder.h"

volatile encoder ifm_encoder = {.steps = 4096, .revolutions = 4096, .resolution = 16777216};
volatile encoder britter_encoder  = {.steps = 0, .revolutions = 0, .resolution = 0};

void parse_ifm_encoder(uint32_t pos){
	uint32_t position = pos;
	uint32_t step = pos%ifm_encoder.steps;

	if(position > ifm_encoder.resolution/2)
	{
		position -= ifm_encoder.resolution;
		step -= ifm_encoder.steps;
	}

	ifm_encoder.turn = (int16_t) position/ifm_encoder.steps;
	ifm_encoder.absolute_angle = (float) 360*position/ifm_encoder.revolutions;
	ifm_encoder.angle = (float) 360*step/ifm_encoder.steps;
}

void parse_brittle_encoder(uint32_t pos){

}
