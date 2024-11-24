/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 * @authors        : CHAIR / AMEUR
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "shell.h"
#include "pwm.h"
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
extern TIM_HandleTypeDef htim1;
extern ADC_HandleTypeDef hadc1;
uint8_t newline[] = "\r\n";

uint32_t uartRxReceived;
uint8_t uartRxBuffer[UART_RX_BUFFER_SIZE];
uint8_t uartTxBuffer[UART_TX_BUFFER_SIZE];
uint8_t alpha_now = 60;
uint8_t alpha_dest = 60;
float vitesse_encod = 0;
float current_v = 0.0;
float offset_current = 0.0;  // Initialiser à 0
uint32_t adc_value = 0;      // Définition de adc_value

#define ADC_BUFFER_SIZE 100         // Taille du buffer pour les mesures continues
#define ADC_OFFSET_SAMPLES 100      // Nombre d'échantillons pour calculer l'offset

uint16_t adc_buffer[ADC_BUFFER_SIZE];
uint16_t adc_offset_buffer[ADC_OFFSET_SAMPLES];

volatile uint8_t offset_calculated = 0;  // Indicateur pour savoir si l'offset est calculé



//float offset_current=2.5268;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{

	/* USER CODE BEGIN 1 */
	char	 	cmdBuffer[CMD_BUFFER_SIZE];
	int 		idx_cmd;
	int 		newCmdReady = 0;
	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_DMA_Init();
	MX_TIM1_Init();
	MX_USART2_UART_Init();
	MX_TIM7_Init();
	MX_TIM6_Init();
	MX_ADC1_Init();
	MX_TIM2_Init();
	MX_TIM8_Init();
	/* USER CODE BEGIN 2 */
	memset(cmdBuffer,NULL,CMD_BUFFER_SIZE*sizeof(char));
	memset(uartRxBuffer,NULL,UART_RX_BUFFER_SIZE*sizeof(char));
	memset(uartTxBuffer,NULL,UART_TX_BUFFER_SIZE*sizeof(char));
	//start_command(alpha);
	/*HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
	int cpt=0;
	for (cpt=0;cpt<10;cpt++)
	{
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
		adc_value+=HAL_ADC_GetValue(&hadc1);
	}
	HAL_ADC_Stop(&hadc1);
	adc_value=adc_value/10;
	offset_current=(((adc_value)/4096.0)*3.3);
	adc_value=0; */

    /* Calibration de l'ADC */
    HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);

    /* Démarrer l'ADC en mode DMA pour l'offset */
    if (HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_offset_buffer, ADC_OFFSET_SAMPLES) != HAL_OK)
    {
        /* Gestion des erreurs */
        Error_Handler();
    }

    /* Attendre que le DMA ait rempli le buffer pour l'offset */
    while (!offset_calculated)
    {
        // Vous pouvez ajouter un timeout si nécessaire
    }

    /* Arrêter l'ADC DMA pour l'offset */
    HAL_ADC_Stop_DMA(&hadc1);

    /* Afficher l'offset calculé (optionnel) */
    char offset_msg[50];
    sprintf(offset_msg, "Offset = %f V\r\n", offset_current);
    HAL_UART_Transmit(&huart2, (uint8_t*)offset_msg, strlen(offset_msg), HAL_MAX_DELAY);

    /* Maintenant, démarrer l'ADC en DMA pour les mesures continues */
    if (HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buffer, ADC_BUFFER_SIZE) != HAL_OK)
    {
        /* Gestion des erreurs */
        Error_Handler();
    }

    HAL_UART_Receive_IT(&huart2, uartRxBuffer, UART_RX_BUFFER_SIZE);
    HAL_Delay(10);
    start_display();
    prompt_display();

    /* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		// uartRxReceived is set to 1 when a new character is received on uart 1
		if(uartRxReceived){
			switch(uartRxBuffer[0]){
			// Nouvelle ligne, instruction à traiter
			case ASCII_CR:
				HAL_UART_Transmit(&huart2, newline, sizeof(newline), HAL_MAX_DELAY);
				cmdBuffer[idx_cmd] = '\0';

				idx_cmd = 0;
				newCmdReady = 1;
				break;
				// Suppression du dernier caractère
			case ASCII_DEL:
				cmdBuffer[idx_cmd--] = 0;
				//idx_cmd--;
				HAL_UART_Transmit(&huart2, uartRxBuffer, UART_RX_BUFFER_SIZE, HAL_MAX_DELAY);
				break;
				// Nouveau caractère
			default:
				cmdBuffer[idx_cmd++] = uartRxBuffer[0];
				HAL_UART_Transmit(&huart2, uartRxBuffer, UART_RX_BUFFER_SIZE, HAL_MAX_DELAY);
			}
			uartRxReceived = 0;
		}

		shell(&newCmdReady,cmdBuffer);

		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/** Configure the main internal regulator output voltage
	 */
	HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV4;
	RCC_OscInitStruct.PLL.PLLN = 85;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
	RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
	{
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{
    uartRxReceived = 1;
    HAL_UART_Receive_IT(&huart2, uartRxBuffer, UART_RX_BUFFER_SIZE);
}

/* Callback appelé lorsque le buffer entier est rempli */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    if (hadc->Instance == ADC1)
    {
        if (!offset_calculated)
        {
            // Calculer l'offset du courant
            Process_ADC_Offset_Data(adc_offset_buffer, ADC_OFFSET_SAMPLES);
            offset_calculated = 1;  // Indiquer que l'offset est calculé
        }
        else
        {
            // Traiter la seconde moitié du buffer ADC
            Process_ADC_Data(&adc_buffer[ADC_BUFFER_SIZE / 2], ADC_BUFFER_SIZE / 2);
        }
    }
}

/* Callback appelé lorsque la moitié du buffer est remplie */
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc)
{
    if (hadc->Instance == ADC1 && offset_calculated)
    {
        // Traiter la première moitié du buffer ADC
        Process_ADC_Data(adc_buffer, ADC_BUFFER_SIZE / 2);
    }
}

/* Fonction pour calculer l'offset du courant */
void Process_ADC_Offset_Data(uint16_t* data, uint16_t length)
{
    uint32_t sum = 0;
    for (uint16_t i = 0; i < length; i++)
    {
        sum += data[i];
    }
    float average = sum / (float)length;
    offset_current = (average / 4096.0) * 3.3;
}

/* Fonction pour traiter les données ADC */
void Process_ADC_Data(uint16_t* data, uint16_t length)
{
    uint32_t sum = 0;
    for (uint16_t i = 0; i < length; i++)
    {
        sum += data[i];
    }
    float average = sum / (float)length;

    /* Convertir en tension */
    float voltage = (average / 4096.0) * 3.3;

    /* Calculer le courant en soustrayant l'offset */
    current_v = voltage - offset_current;

    /* Vous pouvez ajouter du code ici pour utiliser current_v */
}
/* USER CODE END 4 */
/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
