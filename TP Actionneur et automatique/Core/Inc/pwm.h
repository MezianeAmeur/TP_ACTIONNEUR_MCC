/*
 * pwm.h
 *
 *  Created on: Oct 24, 2022
 *      Author:
 */

#ifndef INC_PWM_H_
#define INC_PWM_H_

#include <stdio.h>
#include <stdint.h>

void start_command(uint8_t alpha);
void stop_command();
void change_speed(uint8_t alpha);
void change_speed_progressively(uint8_t alpha);
void read_current();
void start_up();
#endif /* INC_PWM_H_ */
