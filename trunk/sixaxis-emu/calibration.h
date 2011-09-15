/*
 * calibration.h
 *
 *  Created on: 1 juin 2011
 *      Author: Matlo
 */

#ifndef CALIBRATION_H_
#define CALIBRATION_H_

void cal_button(int, int);
void cal_key(int, int, int);
s_mouse_cal* cal_get_mouse(int, int);
int cal_skip_event(SDL_Event*);
void cal_init();

#endif /* CALIBRATION_H_ */
