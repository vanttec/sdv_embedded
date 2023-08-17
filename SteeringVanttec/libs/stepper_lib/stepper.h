/*
 * stepper.h
 *
 *  Created on: Mar 7, 2023
 *      Author: saveasmtz
 */

#ifndef INC_STEPPER_H
#define INC_STEPPER_H

#include "main.h"
#include "cmsis_os.h"
#include "stdlib.h"
#include "encoder.h"

extern TIM_HandleTypeDef htim2;
extern encoder ifm_encoder;
extern encoder briter_encoder;

typedef enum {
	STEERING,
	BRAKING
} stepper_type;

typedef enum {
	CONTROLLER,
	AUTONOMOUS
} stepper_mode;

typedef enum {
	CW,		// Clock-Wise
	CCW,	// Counter Clock-Wise
	IDLE	// No direction
} stepper_direction;

typedef struct {
	uint8_t is_active;			//0: inactive, 1: active
	uint8_t is_exec_started;
	stepper_mode mode;
	stepper_direction direction;

	float MAX_ANGLE;			// Degrees
	float MIN_ANGLE;			// Degrees
	float current_angle; 		// Degrees
	float desired_angle;		// Degrees

	float STEPPER_OFFSET;				// Offset to stop spinning

} stepper;

void configure_steering();
void configure_braking();
void start(const stepper_type stepper);
void pause(const stepper_type stepper);
void stop(const stepper_type stepper);
void steer(uint8_t direction);
void set_setpoint(const stepper_type stepper, float setpoint);

void steer_by_setpoint(uint8_t direction, float error);
void brake_by_setpoint(uint8_t direction, float error);
void update_stepper_pos(const stepper_type stepper);

//void stepping_by_pwm(stepper *stpr, stepper_type id);
//void stepping_by_steps(stepper *stpr, stepper_type id);

#endif /* INC_STEPPER_H */
