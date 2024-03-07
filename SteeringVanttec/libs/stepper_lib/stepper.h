/*
 *      Author: Abiel
 */

#ifndef INC_STEPPER_H
#define INC_STEPPER_H

#include "stm32l4xx_hal.h"
#include <stdbool.h>
#include <stdint.h>

/*
A timer channel is configured in PWM mode, where a pulse of arbitrary length is
defined. Once pulse has been finsihed, HAL_TIM_OC_DelayElapsedCallback interrupt
is called. Here, it is determined if we should pulse the stepper again.

NOTE: This does not configure timer. Timer should be configured as
TIM_COUNTERMODE_DOWN.
*/

typedef struct {
  bool enable_encoder_correction;
  bool enable_soft_limit;

  // Once limit has been reached, motor will stop moving.
  // Setpoints that exceed step count, will be clamped.
  int32_t max_steps;
  int32_t min_steps;

  // If error is within tolerance, stepper will not move.
  int32_t step_deadband;

  TIM_HandleTypeDef *step_timer;
  uint32_t step_timer_channel;

  GPIO_TypeDef *enable_port;
  uint16_t enable_pin;

  GPIO_TypeDef *fault_port;
  uint16_t fault_pin;

  GPIO_TypeDef *direction_port;
  uint16_t direction_pin;
  bool invert;

  // For now, we only support a fixed pulse length.
  uint32_t pulse_length;

  float degs_per_step;
  float gear_reduction;
} StepperConfiguration;

typedef struct {
  bool enabled;
  bool has_fault;

  float mechanisim_angle;
  int32_t position;
  int32_t setpoint;

  // If true, motor pulses will increase step count.
  bool direction;

  StepperConfiguration config;
} Stepper;

void create_default_stepper_config(StepperConfiguration *config);

HAL_StatusTypeDef stepper_initialize(Stepper *stepper,
                                     int32_t initial_position);

void stepper_enable(Stepper *stepper);
void stepper_disable(Stepper *stepper);

// Must be called in a task, this handles timers, updating from encoder values
// (if enabled). Setpoint must be given on update.
void stepper_update(Stepper *stepper, float encoder_value, uint32_t encoder_tick_time);

bool stepper_at_setpoint(Stepper *stepper);

// This must be called from HAL_TIM_PWM_PulseFinishedCallback such that we can
// determine if stepper should be pulsed.
void stepper_irq_callback(Stepper *stepper, TIM_HandleTypeDef *htim);

// Angle is defined in radians. 
// Degs per step is defined in degrees, as this is the commonly used unit in stepper datasheets.
// Gear reduction should be writen in motor turns/mechanisim turns.
// Eg: in case of a reduction, value should be larger than 1.
int32_t mechanisim_angle_to_steps(float gear_reduction, float degs_per_step, float rads);

// Angle is returned in radians.
float steps_to_mechanisim_angle(float gear_reduction, float degs_per_step, int32_t stepper_steps);

#endif /* INC_STEPPER_H */
