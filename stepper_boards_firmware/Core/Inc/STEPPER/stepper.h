/*
 * stepper_tasks.h
 *
 *  Created on: Mar 7, 2023
 *      Author: saveasmtz
 */

#ifndef INC_STEPPER_STEPPER_H_
#define INC_STEPPER_STEPPER_H_

#include "main.h"
#include "cmsis_os.h"
#include "stdlib.h"
#include <math.h>

extern TIM_HandleTypeDef htim2;

typedef enum {
	STEERING,
	BRAKING
} stepper_id;

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
	stepper_mode mode;
	uint8_t active;
	uint8_t exec_started;

	/*
	float MAX_ANGLE;			// Degrees
	float STEP_ANGLE;			// Degrees
	uint16_t STEPS_REV;			// Steps per revolution
	uint16_t MAX_STEPS;

	float desired_angle;		// Degrees
	float current_angle; 		// Degrees
	*/

	stepper_direction direction;

	uint16_t req_steps;			// Required steps to reach desired angle
	uint16_t current_step;
} stepper;

extern volatile stepper brake_stepper;
extern volatile stepper steering_stepper;

void configure_steppers();
void start();
void stop();
void set_direction(const stepper_id stepper, uint8_t direction);
void set_setpoint(const stepper_id stepper, uint16_t setpoint, int8_t direction);

/*
void steering_task();
void braking_task();
*/

void steer();
void brake();
//void stepping_by_pwm(stepper *stpr, stepper_id id);
//void stepping_by_steps(stepper *stpr, stepper_id id);

#endif /* INC_STEPPER_STEPPER_H_ */
