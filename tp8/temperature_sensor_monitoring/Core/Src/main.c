/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"
#include "string.h"
#include "inttypes.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define MCP9808_ADRESS 				0x18 //adress of MCP9808
#define MCP9808_AMBIANT_REG_ADDR 	0x05 //adress of ambiant temperature register
#define MCP9808_RESOL_REG_ADRR		0x08 //adress of resolution register
#define MCP9808_SENS_CONF_REG_ADDR 	0x01 //adress of sensor configuration register
#define MCP9808_UPPER_REG_ADRR		0x02 //adress of upper temperature limit register
#define MCP9808_LOWER_REG_ADRR		0x03 //adress of lower temperature limit register
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

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
	HAL_StatusTypeDef status;

	uint16_t MCP9808_adress = MCP9808_ADRESS << 1; // 0b011000 = 0x18 on décale de 1 car code sur 7 bits
	uint8_t receive_buffer[2] = {0};//buffer ou on recoit les donnees de I2C
	uint8_t transmit_data = MCP9808_AMBIANT_REG_ADDR;
	uint8_t	register_config[3];



	/*Config de la precision (resolution) à 0.25°C*/
	register_config[0] = MCP9808_RESOL_REG_ADRR;
	register_config[1] = 0x01; //bit for 0.25°C resolution
	HAL_I2C_Master_Transmit(&hi2c1, MCP9808_adress, &register_config, (uint16_t) 2, HAL_MAX_DELAY);

	/*Config de l'hysteresis à 1.5°C*/
	register_config[0] = MCP9808_SENS_CONF_REG_ADDR;
	register_config[1] = 0b010; //bit for 1.5°C hysteresis
	HAL_I2C_Master_Transmit(&hi2c1, MCP9808_adress, &register_config, (uint16_t) 3, HAL_MAX_DELAY);

	/*Config lower temperature limit to 24°C*/
	register_config[0] = MCP9808_LOWER_REG_ADRR;//24 = 2^4 + 2^3
	register_config[1] = 0b01; // bit 8 correspondant a 2^4 °C
	register_config[2] = 0b10000000; //bit 7 correspondnat a 2^3 °C, bit 5 correspondant a 2^1°c et bit 4 a 2^0°C
	HAL_I2C_Master_Transmit(&hi2c1, MCP9808_adress, &register_config, (uint16_t) 3, HAL_MAX_DELAY);


	/*Config upper temperature limit to 27°C*/
	register_config[0] = MCP9808_UPPER_REG_ADRR;//27 = 2^4 + 2^3 + 2^1 + 2^0
	register_config[1] = 0b01; // bit 8 correspondant a 2^4 °C
	register_config[2] = 0b10110000; //bit 7 correspondnat a 2^3 °C
	HAL_I2C_Master_Transmit(&hi2c1, MCP9808_adress, &register_config, (uint16_t) 3, HAL_MAX_DELAY);


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
  MX_I2C1_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  status = HAL_I2C_Master_Transmit(&hi2c1, MCP9808_adress, &transmit_data, (uint16_t) 1, HAL_MAX_DELAY);

	  if ( status != HAL_OK )
	  {
		  char* message = "Error start\r\n";
		  HAL_UART_Transmit(&huart2, (uint8_t*) message, (uint16_t) strlen(message) , HAL_MAX_DELAY);
	  }
	  else
	  {
		  status = HAL_I2C_Master_Receive(&hi2c1, MCP9808_adress, receive_buffer, (uint16_t) 2, HAL_MAX_DELAY);

		  if ( status != HAL_OK )
		  {
			  char* message = "Error master receive\r\n";
			  HAL_UART_Transmit(&huart2, (uint8_t*) message, (uint16_t) strlen(message), HAL_MAX_DELAY);
		  }
		  else
		  {
			  float temperature=0;
			  float f_temp[2]; //pour convertir les valeurs de receive_buffer en float

			  receive_buffer[0] &= 0x1F;
			  if((receive_buffer[0] & 0x10) == 0x10)
			  {
				  receive_buffer[0] &= 0x0F;
				  temperature -= 256;
			  }
			  f_temp[0] = receive_buffer[0];
			  f_temp[1] = receive_buffer[1];
			  temperature += (f_temp[0] * 16 + f_temp[1] / 16);

			  char message[256];
			  sprintf(message, "Temperature : %f\r\n", temperature);
			  HAL_UART_Transmit(&huart2, message,  strlen(message), HAL_MAX_DELAY);

		  }
		  HAL_Delay(10*1000);
		  HAL_PWR_EnterSTANDBYMode+(10*1000);
	  }

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
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
