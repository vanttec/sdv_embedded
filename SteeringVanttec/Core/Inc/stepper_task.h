#pragma once
#include "stepper.h"

void stepper_task(uint8_t stepper_id, Stepper *stepper_data, int32_t *encoder_value);