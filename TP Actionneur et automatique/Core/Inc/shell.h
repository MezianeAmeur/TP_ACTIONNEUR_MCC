/*
 * shell.h
 *
 *  Created on: Oct 24, 2022
 *      Author: Adam CHEIKH BRAHIM and Karim Abdellaziz and Sami Asfary
 */

#ifndef INC_SHELL_H_
#define INC_SHELL_H_

#include "main.h"


void prompt_display(void);
void start_display(void);
/*
 *
 *
 */
void shell(int * newCmdReady,char cmdBuffer[CMD_BUFFER_SIZE]);


#endif /* INC_SHELL_H_ */
