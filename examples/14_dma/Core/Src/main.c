/* USER CODE BEGIN Header */
/**
 ****************************************************************************************************
 * @file        main.c
 * @author      ALIENTEK
 * @brief       dma code
 * @license     Copyright (C) 2012-2024, ALIENTEK
 ****************************************************************************************************
 * @attention
 *
 * platform     : ALIENTEK M100-STM32F103 board
 * website      : www.alientek.com
 * forum        : www.openedv.com/forum.php
 *
 * change logs  :
 * version      data         notes
 * V1.0         20240409     the first version
 *
 ****************************************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "usart.h"
#include "gpio.h"
#include "fsmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "../../BSP/LED/led.h"
#include "../../BSP/KEY/key.h"
#include "../../BSP/LCD/lcd.h"
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

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
const uint8_t TEXT_TO_SEND[] = {"ALIENTEK STM32 DMA code"};      /* The string to send in a loop */

#define SEND_BUF_SIZE       (sizeof(TEXT_TO_SEND) + 2) * 200     /* The length of the sent data, which is 200 times larger than sizeof(TEXT_TO_SEND) + 2. */

uint8_t g_sendbuf[SEND_BUF_SIZE];                                /* Send data buffer */

extern DMA_HandleTypeDef hdma_usart1_tx;
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/**
 * @brief   Displays information about the experiment
 * @param   None
 * @retval  None
 */
void stm32f103ve_show_mesg(void)
{
	/* The serial port outputs the experimental information */
	printf("\n");
	printf("********************************\r\n");
	printf("STM32F103\r\n");
	printf("DMA Test\r\n");
	printf("KEY0:Start\r\n");
	printf("ATOM@ALIENTEK\r\n");
	printf("********************************\r\n");
	printf("\r\n");
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  uint8_t  key = 0;
  uint16_t i, k;
  uint16_t len;
  uint8_t  mask = 0;
  float pro = 0;          /* rate of progress */
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
  MX_USART1_UART_Init();
  MX_FSMC_Init();
  /* USER CODE BEGIN 2 */

  lcd_init();                                         /* Initialize LCD */
  stm32f103ve_show_mesg();
  lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
  lcd_show_string(30, 70, 200, 16, 16, "DMA TEST", RED);
  lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
  lcd_show_string(30, 110, 200, 16, 16, "KEY0:Start", RED);
  len = sizeof(TEXT_TO_SEND);
  k = 0;

  for (i = 0; i < SEND_BUF_SIZE; i++) /* Fill in ASCII character set data */
  {
    if (k >= len)   /* Enter a newline character */
    {
      if (mask)
      {
        g_sendbuf[i] = 0x0a;
        k = 0;
      }
      else
      {
        g_sendbuf[i] = 0x0d;
        mask++;
      }
    }
    else     /* Copy the TEXT_TO_SEND statement */
    {
      mask = 0;
      g_sendbuf[i] = TEXT_TO_SEND[k];
      k++;
    }
  }

  i = 0;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    key = key_scan(0);

    if (key == KEY0_PRES)       /* press KEY0 */
    {
      printf("\r\nDMA DATA:\r\n");
      lcd_show_string(30, 130, 200, 16, 16, "Start Transimit....", BLUE);
      lcd_show_string(30, 150, 200, 16, 16, "   %", BLUE);    /* Display percent sign */

      HAL_UART_Transmit_DMA(&huart1, g_sendbuf, SEND_BUF_SIZE);

      while (1)
      {
        if ( __HAL_DMA_GET_FLAG(&hdma_usart1_tx, DMA_FLAG_TC4))
        {
          __HAL_DMA_CLEAR_FLAG(&hdma_usart1_tx, DMA_FLAG_TC4);
          HAL_UART_DMAStop(&huart1);  /* Close serial DMA after transmission */
          break;
        }

        pro = __HAL_DMA_GET_COUNTER(&hdma_usart1_tx);
        len = SEND_BUF_SIZE;        /* total length */
        pro = 1 - (pro / len);      /* get the percentage */
        pro *= 100;                 /* 100 times larger */
        lcd_show_num(30, 150, pro, 3, 16, BLUE);
      }
      lcd_show_num(30, 150, 100, 3, 16, BLUE);    /* Display 100% */
      lcd_show_string(30, 130, 200, 16, 16, "Transimit Finished!", BLUE); /* Prompt transmission completed */
    }

    i++;
    HAL_Delay(10);

    if (i == 20)
    {
      LED0_TOGGLE();  /* flashing LED0 indicates that the system is running */
      i = 0;
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
