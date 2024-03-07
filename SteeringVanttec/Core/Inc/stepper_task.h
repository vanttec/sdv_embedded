#pragma once
#include "stepper.h"

typedef struct {
    uint8_t stepper_id;
    Stepper *stepper;
    float *encoder_value;
    uint32_t *encoder_tick_value;
} stepper_task_attrs;

void stepper_task(void *task_attrs);