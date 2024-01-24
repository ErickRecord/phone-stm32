/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include "../Inc/lcd.h"
#include "../Inc/keypad_4x4.h"

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
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

// LCD
LCD_t Lcd;

// Keyboard
uint8_t keyboard;
char number[11];
uint8_t posArray = 0;

// Comming call
uint8_t incomingCallRX[20];

// I check the conection
uint8_t checkConectionRX[15];
uint8_t hasConnection = 1;

// Call
uint8_t isCallRX[20];
uint8_t cont = 0;

// Main menu
uint8_t flag = 1;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */
void initialScreen();
void connectionSIM808();
int isFinishedCall();
void endCall();
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
  MX_USART2_UART_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */

  Lcd.RS_port = LCD_RS_GPIO_Port;
  Lcd.RS_pin = LCD_RS_Pin;
  Lcd.EN_port = LCD_EN_GPIO_Port;
  Lcd.EN_pin = LCD_EN_Pin;
  Lcd.D4_port = D4_GPIO_Port;
  Lcd.D4_pin = D4_Pin;
  Lcd.D5_port = D5_GPIO_Port;
  Lcd.D5_pin = D5_Pin;
  Lcd.D6_port = D6_GPIO_Port;
  Lcd.D6_pin = D6_Pin;
  Lcd.D7_port = D7_GPIO_Port;
  Lcd.D7_pin = D7_Pin;

  lcd_begin(&Lcd, 16, 2, LCD_5x8DOTS);

  	memset(incomingCallRX, 0, sizeof(incomingCallRX));


  	// Calling Line Identification Presentation
	char at_command[] = "AT+CLIP=1\r\n";
	HAL_UART_Transmit(&huart2, (uint8_t*)at_command, strlen(at_command), HAL_MAX_DELAY);

	// I check the connection
  	connectionSIM808();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1)
    {
    	 keyboard = Keypad_Get_Char();
    	    if(hasConnection == 1){
    	    	if(flag == 1){
    	    		initialScreen();
    	    		flag = 0;
    	    	}

    	    	if(HAL_UART_Receive_IT(&huart2, incomingCallRX, sizeof(incomingCallRX)) == HAL_OK){
    	    		if(strstr((char*)incomingCallRX, "+CLIP:")){
    	    			HAL_TIM_Base_Start_IT(&htim2);
    	    			HAL_TIM_Base_Start_IT(&htim3);
    	    			lcd_clear(&Lcd);
    	    			lcd_home(&Lcd);
    	    			lcd_print(&Lcd, "Llamada!!!");
    	    			lcd_setCursor(&Lcd, 0, 1);
    	    			while(1){
    	    				keyboard = Keypad_Get_Char();

    	    				if(cont == 15){
    	    					HAL_TIM_Base_Stop_IT(&htim2);
    	    					HAL_TIM_Base_Stop_IT(&htim3);
    	    					HAL_GPIO_WritePin(buzzer_GPIO_Port, buzzer_Pin, 0);
    	    					cont= 0;
    	    					endCall();
    	    					break;
    	    				}

    	    				if(keyboard == 'A'){
    	    					char at[6] = "ATH0\r\n";
    	    					HAL_UART_Transmit(&huart2, (uint8_t*)at, strlen(at), HAL_MAX_DELAY);
    	    					HAL_TIM_Base_Stop_IT(&htim2);
    	    					HAL_GPIO_WritePin(buzzer_GPIO_Port, buzzer_Pin, 0);
    	    					flag = 1;
    	    					break;
    	    				}
    	    				if(keyboard == 'D'){

    	    					HAL_TIM_Base_Stop_IT(&htim2);
    	    					HAL_GPIO_WritePin(buzzer_GPIO_Port, buzzer_Pin, 0);
    	    					lcd_clear(&Lcd);
    	    					lcd_home(&Lcd);

    	    					if(isFinishedCall() == 0){
    	    						lcd_print(&Lcd, "LLamada en");
    	    						lcd_setCursor(&Lcd, 0, 1);
    	    						lcd_print(&Lcd, "curso");

    	    					} else{
    	    						lcd_print(&Lcd, "LLamada");
    	    						lcd_setCursor(&Lcd, 0, 1);
    	    						lcd_print(&Lcd, "finalizada");
    	    						HAL_Delay(2000);
    	    						flag = 1;
    	    					}
    	    					while(1){
    	    						if(flag == 1){
    	    							break;
    	    						}
    	    						keyboard = Keypad_Get_Char();
    	    						if(keyboard == 'A'){
    	    							char at[5] = "ATH\r\n";
    	    							HAL_UART_Transmit(&huart2, (uint8_t*)at, strlen(at), HAL_MAX_DELAY);
    	    							flag = 1;
    	    							break;
    	    						}
    	    					}
    	    				}
    	    			}
    	    		}
    	    	}


    	    if((keyboard == '0' || keyboard == '1' || keyboard == '2' ||
    	    	keyboard == '3' || keyboard == '4' || keyboard == '5' ||
				keyboard == '6' || keyboard == '7' || keyboard == '8' ||
				keyboard == '9') && posArray < 10 ){
    	    		number[posArray] = keyboard;
    	    		number[posArray + 1] = '\0';
    	    		posArray++;

    	    	lcd_write(&Lcd, keyboard);
    	    }

    	    if(keyboard == 'D'){
    	    	connectionSIM808();
    	    	if(posArray == 10){
    	    		char at_command[15] = "ATD";
    	    		strcat(at_command, number);
    	    		at_command[13] = ';';
    	    		at_command[14] = '\0';
    	    		at_command[14] = 0x0d;
    	    		HAL_UART_Transmit(&huart2, (uint8_t*)at_command, strlen(at_command), HAL_MAX_DELAY);
    	    		HAL_TIM_Base_Start_IT(&htim3);
    	    		lcd_clear(&Lcd);
    	    		lcd_home(&Lcd);
    	    		lcd_print(&Lcd, "Marcando...");
    	    		lcd_setCursor(&Lcd, 0, 2);
    	    		lcd_print(&Lcd, number);
    	    		while(1){

    	    			keyboard = Keypad_Get_Char();
    	    			if(cont == 20){
    	    				HAL_TIM_Base_Stop_IT(&htim3);
    	    				cont= 0;
    	    				endCall();
    	    				break;
    	    			}
    	    			if(keyboard == 'A'){
    	    				endCall();
    	    				break;
    	    			}
    	    		}

    	    	}





    	    }
    	    if(keyboard == 'A'){
    	    	if (posArray > 0) {
    	    		posArray--;
    	    		number[posArray] = '\0';
    	    		lcd_clearRow1(&Lcd);
    	    		lcd_print(&Lcd,number);
    	    	}
    	    }

    	    	} else{
    	    		if(keyboard == '*'){
    	    			connectionSIM808();
    	    	}
    	    }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    }
  /* USER CODE END 3 */
}

void initialScreen(){
	lcd_clear(&Lcd);
	lcd_home(&Lcd);
	lcd_print(&Lcd, "Numero:");
	lcd_cursor(&Lcd);
	lcd_setCursor(&Lcd, 0, 1);
	lcd_blink(&Lcd);
}


void connectionSIM808(){

	HAL_UART_Receive(&huart2, checkConectionRX, sizeof(checkConectionRX), 20);
	char at_command[] = "AT\r\n";
	  	HAL_UART_Transmit(&huart2, (uint8_t*)at_command, strlen(at_command), HAL_MAX_DELAY);
		HAL_UART_Receive(&huart2, checkConectionRX, 20,1000);
	if(strstr((char*)checkConectionRX, "OK") != NULL){
		hasConnection = 1;
	} else{
		lcd_clear(&Lcd);
				lcd_home(&Lcd);
				lcd_print(&Lcd, "Conectando...");
				lcd_setCursor(&Lcd, 0, 1);
				lcd_print(&Lcd, "Presiona *");
				hasConnection = 0;

	}

}

int isFinishedCall(){

	HAL_UART_Receive(&huart2, isCallRX, 20, 20);
	char at_command[] = "ATA\r\n";
	  	HAL_UART_Transmit(&huart2, (uint8_t*)at_command, strlen(at_command), HAL_MAX_DELAY);
		HAL_UART_Receive(&huart2, isCallRX, 20,1500);
	if(strstr((char*)isCallRX, "ERROR") != NULL){
		return 1;
	} else{
		return 0;
	}

}

void endCall(){
	char at_command[] = "ATH0\r\n";
	HAL_UART_Transmit(&huart2, (uint8_t*)at_command, strlen(at_command), HAL_MAX_DELAY);
	for (int i = 0; i < 10; i++) {
		number[i] = '\0';
	}
	posArray = 0;
	flag = 1;
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

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

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 35999;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 50;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 8999;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 999;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LCD_EN_Pin|LCD_RS_Pin|D7_Pin|D6_Pin
                          |D5_Pin|D4_Pin|r4_Pin|r3_Pin
                          |r2_Pin|r1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(buzzer_GPIO_Port, buzzer_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : LCD_EN_Pin LCD_RS_Pin D7_Pin D6_Pin
                           D5_Pin D4_Pin r4_Pin r3_Pin
                           r2_Pin r1_Pin */
  GPIO_InitStruct.Pin = LCD_EN_Pin|LCD_RS_Pin|D7_Pin|D6_Pin
                          |D5_Pin|D4_Pin|r4_Pin|r3_Pin
                          |r2_Pin|r1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : buzzer_Pin */
  GPIO_InitStruct.Pin = buzzer_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(buzzer_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : c4_Pin */
  GPIO_InitStruct.Pin = c4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(c4_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : c3_Pin c2_Pin c1_Pin */
  GPIO_InitStruct.Pin = c3_Pin|c2_Pin|c1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* Prevent unused argument(s) compilation warning */
  if(htim->Instance == TIM2){
	  HAL_GPIO_TogglePin(buzzer_GPIO_Port, buzzer_Pin);
  }

  if(htim->Instance == TIM3){
	  cont++;
  }

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
