/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "dma.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "string.h"

#include <stdint.h>
#define RX_BUF_SIZE 128
uint8_t rx_data;

uint8_t rx_buf[RX_BUF_SIZE];
uint8_t send_buf[RX_BUF_SIZE];
uint8_t newline[] = "\r\n";
uint8_t entkey = '\r';
int check = 0;

#ifdef __GNUC__
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif

PUTCHAR_PROTOTYPE
{
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);
  HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF);
  
  return ch;
}

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
// #define RCC_AHB1ENR (*(volatile unsigned int*)(0x40023800 + 0x30))

// #define GPIOA_MODER (*(volatile unsigned int*)(0x40020000 + 0x00))
// #define GPIOA_OTYPER (*(volatile unsigned int*)(0x40020000 + 0x04))
// #define GPIOA_ODR (*(volatile unsigned int*)(0x40020014))

// #define GPIOB_MODER (*(volatile unsigned int*)(0x40020400 + 0x00))
// #define GPIOB_OTYPER (*(volatile unsigned int*)(0x40020400 + 0x04))
// #define GPIOB_ODR (*(volatile unsigned int*)(0x40020414))

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
// #define DEBOUNCE_DELAY_MS 50
// typedef struct _Button__t{
//   GPIO_TypeDef* port;
//   uint16_t pin;
//   uint32_t last_debounce_time;
//   GPIO_PinState last_raw_state;
//   GPIO_PinState stable_state;
// }Button_t;

// void Button_Init(Button_t* btn, GPIO_TypeDef* port, uint16_t pin){
//   btn->port = port;
//   btn->pin = pin;
//   btn->last_debounce_time = 0;
//   btn->last_debounce_time=HAL_GPIO_ReadPin(port, pin);
//   btn->stable_statfe=btn->last_raw_state;
// }

// bool Button_IsPressed(Button_t* btn){
//   GPIO_PinState current_raw = HAL_GPIO_ReadPin(btn->port, btn->pin);
//   uint32_t current_time = HAL_GetTick();
//   if(current_raw != btn->last_raw_state){
//     btn->last_debounce_time=current_time;
//     btn->last_raw_state=current_raw;
//   }
//   if((current_time - btn->last_debounce_time)>=DEBOUNCE_DELAY_MS){
//     if(btn->last_raw_state== GPIO_PIN_RESET)
//       return true;
//     else return false;
//   }

//   // HAL_GPIO_EXTI_IRQHandler(13);
//   // HAL_GPIO_EXTI_IRQHandler(1);

//   return false;
// }

// void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
// {
//   if(GPIO_Pin==GPIO_PIN_13){
//     HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
//   }
//   if(GPIO_Pin==GPIO_PIN_0){
//     HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_0);
//   }

// }


// void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
uint8_t rx_buf_1[RX_BUF_SIZE];
uint8_t rx_buf_2[RX_BUF_SIZE];

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart->Instance == USART2)
    {
        for (uint16_t i = 0; i < Size; i++)
        {
            uint8_t c = rx_buf_2[i];

            if (c == entkey)
            {
                if (check > 0)
                {
                    HAL_UART_Transmit(&huart1, send_buf, check, 100);
                }

                HAL_UART_Transmit(&huart1, newline, 2, 100);
                HAL_UART_Transmit(&huart2, newline, 2, 100);

                memset(send_buf, 0, RX_BUF_SIZE);
                check = 0;
            }
            else if (c == 0x08 || c == 0x7F)
            {
                if (check > 0)
                {
                    check--;

                    send_buf[check] = 0;

                    uint8_t backspace[] = "\b \b";
                    HAL_UART_Transmit(&huart2, backspace, 3, 100);
                }
            }
            else
            {
                if (check < RX_BUF_SIZE - 1)
                {
                    send_buf[check] = c;
                    check++;

                    HAL_UART_Transmit(&huart2, &c, 1, 10);
                }
            }
        }

        memset(rx_buf_2, 0, RX_BUF_SIZE);

        HAL_UARTEx_ReceiveToIdle_DMA(
            &huart2,
            rx_buf_2,
            RX_BUF_SIZE
        );
    }

    else if (huart->Instance == USART1)
    {
        HAL_UART_Transmit(&huart2, rx_buf_1, Size, 100);

        memset(rx_buf_1, 0, RX_BUF_SIZE);

        HAL_UARTEx_ReceiveToIdle_DMA(
            &huart1,
            rx_buf_1,
            RX_BUF_SIZE
        );
    }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2)
    {
        HAL_UARTEx_ReceiveToIdle_DMA(
            &huart2,
            rx_buf_2,
            RX_BUF_SIZE
        );
    }
    else if (huart->Instance == USART1)
    {
        HAL_UARTEx_ReceiveToIdle_DMA(
            &huart1,
            rx_buf_1,
            RX_BUF_SIZE
        );
    }
}



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
  MX_DMA_Init();
  MX_USART2_UART_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  // *(volatile unsigned int*)0x40020000 ^=(2u<<10);
  // HAL_UART_Receive_DMA(&huart2, rx_buf, RX_BUF_SIZE);
  HAL_UARTEx_ReceiveToIdle_DMA(&huart2, rx_buf_2, RX_BUF_SIZE);
  HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx_buf_1, RX_BUF_SIZE);
  // GPIOA 클럭활성화
  // *(volatile unsigned int*)(0x40023800 + 0x30) |= (1u<<0);
  // RCC_AHB1ENR |= (1u<<0);

  // GPIOA_MODER

  // GPIOA_MODER &= ~(3u<<10);
  // GPIOA_MODER |= (1u<<10);
  // push pull
  // GPIOA_OTYPER &= ~(1u<<5);
  

    // GPIOb 클럭활성화
  // RCC_AHB1ENR |= (1u<<1);
  // GPIOb_MODER

  // GPIOB_MODER &= ~(3u<<0);
  // GPIOB_MODER |= (1u<<0);
  
  // push pull
  // GPIOB_OTYPER &= ~(1u<<0);
 



    // uint8_t rx_data;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* Polling 방식으로 1초마다 콘솔에 문자열 출력 */
    // if(HAL_UART_Receive(&huart2, &rx_data,1,10)==HAL_OK){
    //   if(rx_data=='a') {
    //     printf("Hello STM32 Cortex-M4 USART Polling!\r\n");
    //   }
    //   else 
    //   {
    //     HAL_UART_Transmit(&huart2,&rx_data,1,10);
  
    //   }
    // }
    

    
    
    
    
    
    // GPIO_PinState button= HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_13);
    // if(button==GPIO_PIN_RESET){
		// HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
    // }
    // GPIO_PinState button1= HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_0);
    // if(button1==GPIO_PIN_RESET){
    //   HAL_Delay(50);
    //   button1= HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_0);
    //   if(button1==GPIO_PIN_RESET){
		//     HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);

    //   }
    // Button_t my_button;
    // Button_Init(&my_button, GPIOC, GPIO_PIN_0);

    //   if(Button_IsPressed(&my_button)){
    //     HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_0);
    //   }
    
  // GPIOA_ODR ^= (1u<<5);
  // GPIOB_ODR ^= (1u<<0);

  //   volatile int delay_count = 1000000;
  //   while(delay_count--){
  //   }

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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
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
#ifdef USE_FULL_ASSERT
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
