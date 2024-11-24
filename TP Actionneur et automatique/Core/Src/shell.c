/**
 ******************************************************************************
 * @file	shell.c
 * @authors
 * @brief	Commands management
 ******************************************************************************
 **/

#include "shell.h"
#include "pwm.h"
#include "main.h"



#define MAX_SPEED 99 /* Vitesse maximale */
#define MIN_SPEED 1 /* Vitesse minimale */

extern uint8_t alpha_now;
extern uint8_t alpha_dest;
extern UART_HandleTypeDef huart2;






uint8_t started[]=
		"\r\n*-----------------------------*"
		"\r\n| Welcome on Nucleo-STM32G431 |"
		"\r\n*-----------------------------*"
		"\r\n";
uint8_t prompt[]="user@Nucleo-STM32G431>>";



uint8_t help[]="Usage:\r\n"
		//"pinout (to show all the connected pins and their functions\r\n"
		"start (to turn on the motor)\r\n"
		"stop (to turn off the motor)\r\n";
/*uint8_t pinout[]="Pinout:\r\n"
		"PA2: USART2_TX: to transfer data via STLink to the laptop\r\n"
		"PA3: USART2_RX: to receive data via STLink from the laptop\r\n"
		"PA5: LED: To turn ON or OFF the LED\r\n"
		"PA8: TIM1_CH1: to control the motor: PWM signal\r\n"
		"PA9: TIM1_CH2: to control the motor: PWM shifted signal\r\n"
		"PA11: TIM1_CH1N: to control the motor: PWM complementary signal\r\n"
		"PA12: TIM1_CH1N: to control the motor: PWM complementary shifted signal\r\n"
		"PA13: T_SWDIO: to debug the stm32 (serial wire)\r\n"
		"PA14: T_SWCLK: to debug the stm32 (serial wire)\r\n"
		"PC13: BUTTON: to detect the pressing of the button\r\n";*/
uint8_t start[]="Power ON\r\n";
uint8_t stop[]="Power OFF\r\n";
uint8_t cmdNotFound[]="Command not found\r\n";
uint8_t speed[50];
uint8_t current[50];
uint8_t encoder[50];
uint8_t first_start=1;

uint16_t speed_value;
float current_value;
int rotary_counter=0;
extern float vitesse_encod;

extern float current_v;
extern uint32_t adc_value;


/**
 * @brief Print the prompt message: "user@Nucleo-STM32G431>>".
 */
void prompt_display(void)
{
	HAL_UART_Transmit(&huart2, prompt, sizeof(prompt), HAL_MAX_DELAY);
}

/**
 * @brief Print the starting message: "Welcome on Nucleo-STM32G431".
 */
void start_display(void)
{
	HAL_UART_Transmit(&huart2, started, sizeof(started), HAL_MAX_DELAY);
}


/**
 * @brief Respond to the command written in the serial terminal.
 * @note Personalized responses to each command.
 * @param *newCmdReady	Reception Flag (1 when a command is received).
 * @param cmdBuffer   Command.
 */
void shell(int * newCmdReady,char cmdBuffer[CMD_BUFFER_SIZE])
{
	if (*newCmdReady)
	{
		if(strncmp(cmdBuffer,"help",strlen("help"))==0)
		{
			HAL_UART_Transmit(&huart2, help, sizeof(help), HAL_MAX_DELAY);
		}
		/*else if (strncmp(cmdBuffer,"pinout",strlen("pinout"))==0)
		{
			HAL_UART_Transmit(&huart2, pinout, sizeof(pinout), HAL_MAX_DELAY);*/
		//}
		else if (strncmp(cmdBuffer,"start",strlen("start"))==0)
		{

			if (first_start==1)
			{
				start_up();
				first_start=0;
			}
			else
			{
				start_command(alpha_dest);
			}

			HAL_UART_Transmit(&huart2, start, sizeof(start), HAL_MAX_DELAY);
		}else if (strncmp(cmdBuffer,"restart",strlen("restart"))==0)
		{
			start_up();
			HAL_UART_Transmit(&huart2, start, sizeof(start), HAL_MAX_DELAY);
		}
		else if (strncmp(cmdBuffer,"stop",strlen("stop"))==0)
		{
			stop_command();
			HAL_UART_Transmit(&huart2, stop, sizeof(stop), HAL_MAX_DELAY);
		}
		else if (strncmp(cmdBuffer,"speed=",strlen("speed="))==0)
		{

			speed_value=atoi(cmdBuffer+strlen("speed="));
			if (speed_value>MAX_SPEED)
			{
				speed_value=MAX_SPEED;
			}
			else if (speed_value<MIN_SPEED)
			{
				speed_value=MIN_SPEED;
			}
			change_speed_progressively(speed_value);
			sprintf(speed,"Speed is changed to %d\r\n",speed_value);
			HAL_UART_Transmit(&huart2, speed, sizeof(speed), HAL_MAX_DELAY);
		}
		else if (strncmp(cmdBuffer,"current",strlen("current"))==0)
		{
			char current_msg[50];
			sprintf(current,"Current value is %f\r\n",current_v);
			HAL_UART_Transmit(&huart2, current, sizeof(current), HAL_MAX_DELAY);
		}
		else if (strncmp(cmdBuffer,"encoder",strlen("encoder"))==0)
		{

			sprintf(encoder,"Speed is %f t/s\r\n",vitesse_encod);
			HAL_UART_Transmit(&huart2, encoder, sizeof(encoder), HAL_MAX_DELAY);


		}
		else
		{
			HAL_UART_Transmit(&huart2, cmdNotFound, sizeof(cmdNotFound), HAL_MAX_DELAY);
		}

		prompt_display();
		memset(cmdBuffer,NULL,CMD_BUFFER_SIZE*sizeof(char)); // to clear the reception buffer cmdBuffer
		*newCmdReady = 0; // to wait for another reception
	}
}
/**
 * @}
 */
