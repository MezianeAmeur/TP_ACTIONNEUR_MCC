/**
  ******************************************************************************
  * @file	pwm.c
  * @authors	Adam Cheikh Brahim, Karim Abdellaziz and Sami Asfary
  * @brief	Functions that control the motor
  ******************************************************************************
  **/


#include "pwm.h"
#include "main.h"

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim7;
extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim8;
extern ADC_HandleTypeDef hadc1;
extern uint8_t alpha_now;
extern uint8_t alpha_dest;

extern float current_v;
extern uint32_t adc_value;



/**
  * @brief This function startup every peripheral used for the command of the motor
  * @note Startup the chopper and the encoder feedback.
  * @param alpha	The duty cycle used for this start.
  * @retval None
  */
void start_command(uint8_t alpha)
{


	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
	htim1.Instance->CCR1=ARR*alpha/100;
	htim1.Instance->CCR2=ARR*(100-alpha)/100;
	HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);
	HAL_TIM_Base_Start_IT(&htim1);
	HAL_TIM_Base_Start_IT(&htim8);
}

/**
  * @brief This function stops the timer related to the chopper control.
  * @note Only stops the chopper not the encoder feedback.
  * @retval None
  */
void stop_command()
{

	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
	HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
	HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_2);
	HAL_TIM_Base_Stop_IT(&htim1);
}

/**
  * @brief Immediatly change the duty cycle of the PWM controlling the chopper.
  * @note Warning : The motor may not follow the change if it is to fast.
  * @param alpha	New duty cycle of the PWM.
  * @retval None
  */
void change_speed(uint8_t alpha)
{

	htim1.Instance->CCR1=ARR*alpha/100;
	htim1.Instance->CCR2=ARR*(100-alpha)/100;

}

/**
  * @brief Change the duty cycle of the PWM controlling the chopper with a timer interrupt imposing a delay.
  * @param alpha	New duty cycle of the PWM.
  * @retval None
  */
void change_speed_progressively(uint8_t alpha)
{
	HAL_TIM_Base_Start_IT(&htim6);
	alpha_dest=alpha;

}

/**
  * @brief Launch the startup sequence for the chopper.
  * @note The startup consist of setting HIGH a pin during 20us.
  * @retval None
  */
void start_up()
{
	HAL_GPIO_WritePin(ISO_RESET_GPIO_Port, ISO_RESET_Pin, GPIO_PIN_SET);
	HAL_TIM_Base_Start_IT(&htim7); // Wait for 2.5 us to turn it off

}

/**
  * @brief Launch a reading of the current feedback of the chopper.
  * @retval None
  */
void read_current()
{
	HAL_ADC_Start_IT(&hadc1);
}


