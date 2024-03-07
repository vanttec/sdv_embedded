#pragma once

#include "stm32l4xx_hal.h"

HAL_StatusTypeDef encoder_setup_can(CAN_HandleTypeDef *hcan);

HAL_StatusTypeDef encoder_initialize_op_mode(CAN_HandleTypeDef *hcan);

HAL_StatusTypeDef encoder_initialize_briter(CAN_HandleTypeDef *hcan);

void encoder_task(void *attrs_hcan);