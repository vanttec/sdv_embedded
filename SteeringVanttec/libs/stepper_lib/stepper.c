#include "stepper.h"
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include "cmsis_os.h"

// If we have not received a new encoder value within ms, disable stepper and trigger fault
#define STEPPER_ENCODER_TIME_TOLERANCE 1000

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
  config->step_deadband = 0;
  config->pulse_length = 1000;
}

HAL_StatusTypeDef stepper_initialize(Stepper *stepper,
                                     int32_t initial_position) {
  if (stepper == NULL) {
    return HAL_ERROR;
  }

  stepper->enabled = false;

  stepper->position = initial_position;

  stepper_disable(stepper);

  // Set step variables to default.
  stepper->setpoint = stepper->position;
  stepper->direction = false;

  stepper->has_fault = false;

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
  if(stepper->has_fault){
    // Do not enable stepper if we have a fault.
    return;
  }
  HAL_GPIO_WritePin(stepper->config.enable_port, stepper->config.enable_pin,
                    GPIO_PIN_SET);
}

void stepper_disable(Stepper *stepper) {
  HAL_GPIO_WritePin(stepper->config.enable_port, stepper->config.enable_pin,
                    GPIO_PIN_RESET);
}

void stepper_update(Stepper *stepper, float encoder_value, uint32_t encoder_tick_time) {
  //int32_t state = osKernelLock();
  if (stepper->config.enable_encoder_correction) {
    if(HAL_GetTick() - encoder_tick_time > STEPPER_ENCODER_TIME_TOLERANCE){
      // We have not received encoder value, disable stepper.
      stepper_disable(stepper);
      stepper->has_fault = true;
      return;
    }

    stepper->position = mechanisim_angle_to_steps(stepper->config.gear_reduction, stepper->config.degs_per_step, encoder_value);
  }

  GPIO_PinState fault_status = HAL_GPIO_ReadPin(stepper->config.fault_port, stepper->config.fault_pin);
  stepper->has_fault = fault_status == GPIO_PIN_SET;

  // TODO: should we disable stepper if we detected fault from motor controller?

  // Calculate mechanisim positions.
  stepper->mechanisim_angle = 
        steps_to_mechanisim_angle(stepper->config.gear_reduction, stepper->config.degs_per_step, stepper->position);

  if (stepper_at_setpoint(stepper)) {
    // Stepper is already at setpoint, nothing to do here.
    return;
  }

  // We need to move stepper, set and determine direction pin.
  if (stepper->position - stepper->setpoint < 0) {
    HAL_GPIO_WritePin(stepper->config.direction_port,
                      stepper->config.direction_pin,
                      stepper->config.invert ? GPIO_PIN_SET : GPIO_PIN_RESET);
    stepper->direction = true;
  } else {
    HAL_GPIO_WritePin(stepper->config.direction_port,
                      stepper->config.direction_pin,
                      stepper->config.invert ? GPIO_PIN_RESET : GPIO_PIN_SET);
    stepper->direction = false;
  }

  if(stepper->config.enable_soft_limit){
    // Prevent movement only in one direction.
    if(stepper->mechanisim_angle > stepper->config.max_angle && !stepper->direction){
      HAL_TIM_PWM_Stop_IT(stepper->config.step_timer,
                        stepper->config.step_timer_channel);
      return;
    }

    if(stepper->mechanisim_angle < stepper->config.min_angle && stepper->direction){
      HAL_TIM_PWM_Stop_IT(stepper->config.step_timer,
                        stepper->config.step_timer_channel);
      return;
    }
  }

  HAL_TIM_PWM_Start_IT(stepper->config.step_timer,
                    stepper->config.step_timer_channel);
  //osKernelRestoreLock(state);
}

#define MECHANISIM_DEGS_TO_RAD (M_PI / 180.0f)
int32_t mechanisim_angle_to_steps(float gear_reduction, float degs_per_step, float rads){
  const float rads_per_step = degs_per_step * MECHANISIM_DEGS_TO_RAD;

  return (rads * gear_reduction) / rads_per_step;
}

float steps_to_mechanisim_angle(float gear_reduction, float degs_per_step, int32_t stepper_steps){
  const float rads_per_step = degs_per_step * MECHANISIM_DEGS_TO_RAD;

  return ((float) stepper_steps * rads_per_step) / gear_reduction;
}