#include "stepper.h"
#include <stdint.h>
#include <stdlib.h>

HAL_StatusTypeDef configure_stepper_timer_channel(Stepper *stepper,
                                                  TIM_HandleTypeDef *timer,
                                                  uint32_t timer_channel) {
  if (stepper->config.pulse_length <= 1) {
    return HAL_ERROR;
  }

  // This assumes that timer has already been configured and initialized!
  TIM_OC_InitTypeDef configOC = {0};
  configOC.OCMode = TIM_OCMODE_PWM1;
  configOC.Pulse = stepper->config.pulse_length - 1;
  configOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  configOC.OCFastMode = TIM_OCFAST_ENABLE;

  if (HAL_TIM_PWM_ConfigChannel(timer, &configOC, timer_channel) != HAL_OK) {
    return HAL_ERROR;
  };

  __HAL_TIM_SET_COUNTER(timer, stepper->config.pulse_length);

  return HAL_OK;
}

void create_default_stepper_config(StepperConfiguration *config) {
  // TODO Create sensible defaults.
  config->enable_soft_limit = false;
  config->step_deadband = 10;
}

HAL_StatusTypeDef stepper_initialize(Stepper *stepper,
                                     int32_t initial_position) {
  if (stepper == NULL) {
    return HAL_ERROR;
  }

  stepper->enabled = false;

  if (stepper->config.enable_soft_limit) {
    if (initial_position > stepper->config.max_steps ||
        initial_position < stepper->config.min_steps) {
      return HAL_ERROR;
    }
  }

  stepper->position = initial_position;

  stepper_disable(stepper);

  // Set step variables to default.
  stepper->setpoint = stepper->position;
  stepper->direction = false;

  if (configure_stepper_timer_channel(stepper, stepper->config.step_timer,
                                      stepper->config.step_timer_channel) ==
      HAL_ERROR) {
    return HAL_ERROR;
  }
}

HAL_TIM_ActiveChannel get_tim_active_channel(uint32_t channel) {
  switch (channel) {
  case TIM_CHANNEL_1:
    return HAL_TIM_ACTIVE_CHANNEL_1;
  case TIM_CHANNEL_2:
    return HAL_TIM_ACTIVE_CHANNEL_2;
  case TIM_CHANNEL_3:
    return HAL_TIM_ACTIVE_CHANNEL_3;
  case TIM_CHANNEL_4:
    return HAL_TIM_ACTIVE_CHANNEL_4;
  default:
    // TODO Determine a better default case.
    return HAL_TIM_ACTIVE_CHANNEL_5;
  }
}

void stepper_irq_callback(Stepper *stepper, TIM_HandleTypeDef *htim) {
  // We assume that this has been called from stepper->step_timer interrupt.
  // We only need to check if interrupt has been fired for the correct channel.

  if (stepper->config.step_timer != htim ||
      htim->Channel !=
          get_tim_active_channel(stepper->config.step_timer_channel)) {
    // This irq was not fired for this stepper.
    return;
  }

  // Update stepper position from pulse
  if (stepper->direction) {
    stepper->position++;
  } else {
    stepper->position--;
  }

  if (stepper_at_setpoint(stepper)) {
    // We have reached setpoint, stop stepper.
    HAL_TIM_PWM_Stop_IT(stepper->config.step_timer,
                        stepper->config.step_timer_channel);
  }
}

bool stepper_at_setpoint(Stepper *stepper) {
  return abs(stepper->position - stepper->setpoint) <=
         stepper->config.step_deadband;
}

void stepper_enable(Stepper *stepper) {
  HAL_GPIO_WritePin(stepper->config.enable_port, stepper->config.enable_pin,
                    GPIO_PIN_SET);
}

void stepper_disable(Stepper *stepper) {
  HAL_GPIO_WritePin(stepper->config.enable_port, stepper->config.enable_pin,
                    GPIO_PIN_RESET);
}

void stepper_update(Stepper *stepper, int32_t encoder_step_value) {
  if (stepper->config.enable_encoder_correction) {
    stepper->position = encoder_step_value;
  }

  if (stepper_at_setpoint(stepper)) {
    // Stepper is already at setpoint, nothing to do here.
    return;
  }

  // We need to move stepper, set and determine direction pin.
  if (stepper->position - stepper->setpoint < 0) {
    HAL_GPIO_WritePin(stepper->config.direction_port,
                      stepper->config.direction_pin,
                      stepper->config.invert ? GPIO_PIN_SET : GPIO_PIN_RESET);
  } else {
    HAL_GPIO_WritePin(stepper->config.direction_port,
                      stepper->config.direction_pin,
                      stepper->config.invert ? GPIO_PIN_RESET : GPIO_PIN_SET);
  }

  HAL_TIM_PWM_Start(stepper->config.step_timer,
                    stepper->config.step_timer_channel);
}