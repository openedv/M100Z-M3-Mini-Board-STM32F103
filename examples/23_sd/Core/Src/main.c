/* USER CODE BEGIN Header */
/**
 ****************************************************************************************************
 * @file        main.c
 * @author      ALIENTEK
 * @brief       sd code
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
#include "sdio.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "fsmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "../../ATK_Middlewares/MALLOC/malloc.h"
#include "../../BSP/LED/led.h"
#include "../../BSP/KEY/key.h"
#include "../../BSP/LCD/lcd.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
extern uint8_t g_sd_error;
extern HAL_SD_CardInfoTypeDef g_sd_card_info;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/**
 * @brief   displays SD card information
 * @param   None
 * @retval  None
 */
static void show_sd_info(void)
{
    HAL_SD_CardCIDTypeDef sd_card_cid = {0};

    HAL_SD_GetCardCID(&hsd, &sd_card_cid);

    printf("Card Type: %s\r\n", (g_sd_card_info.CardType == CARD_SDSC) ? ((g_sd_card_info.CardVersion == CARD_V1_X) ? ("SDSC V1") :
                                                                         ((g_sd_card_info.CardVersion == CARD_V1_X) ? ("SDSC V2") :
                                                                         (""))) :
                                ((g_sd_card_info.CardType == CARD_SDHC_SDXC) ? ("SDHC") :
                                ((g_sd_card_info.CardType == CARD_SECURED) ? ("SECURE") :
                                (""))));
    printf("Card ManufacturerID:%d\r\n", sd_card_cid.ManufacturerID);
    printf("Card RCA:%d\r\n", g_sd_card_info.RelCardAdd);
    printf("LogBlockNbr:%d \r\n", g_sd_card_info.LogBlockNbr);
    printf("LogBlockSize:%d \r\n", g_sd_card_info.LogBlockSize);
    printf("Card Capacity:%d MB\r\n", (uint32_t)(((uint64_t)g_sd_card_info.LogBlockNbr * g_sd_card_info.LogBlockSize) >> 20));
    printf("Card BlockSize:%d\r\n\r\n", g_sd_card_info.BlockSize);

    lcd_show_string(30, 146, 200, 16, 16, "SD Card Size:     MB", BLUE);
    lcd_show_num(30 + 13 * 8, 146, (uint32_t)(((uint64_t)g_sd_card_info.LogBlockNbr * g_sd_card_info.LogBlockSize) >> 20), 5, 16, BLUE);
}

static void sd_read_test(void)
{
    uint8_t *buf;
    uint16_t index;

    buf = (uint8_t *)mymalloc(SRAMIN, g_sd_card_info.BlockSize);
    if (buf == NULL)
    {
        return;
    }

    /* Read and print the data of block 0 of the SD card */
    if (sd_read_disk(buf, 0, 1) == 0)
    {
        lcd_show_string(30, 170, 200, 16, 16, "USART1 Sending Data...", BLUE);
        printf("Block 0 Data:\r\n");
        for (index=0; index<g_sd_card_info.BlockSize; index++)
        {
            printf("%02X ", buf[index]);
        }
        printf("\r\nData End\r\n");
        lcd_show_string(30, 170, 200, 16, 16, "USART1 Send Data Over!", BLUE);
    }
    else
    {
        printf("SD read Failure!\r\n");
        lcd_show_string(30, 170, 200, 16, 16, "SD read Failure!      ", BLUE);
    }

    myfree(SRAMIN, buf);
}
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
	printf("SD Test\r\n");
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
  uint8_t t = 0;
  uint8_t key;
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
  MX_USART1_UART_Init();
  MX_FSMC_Init();
  MX_TIM4_Init();
  MX_SDIO_SD_Init();
  /* USER CODE BEGIN 2 */
  lcd_init();                         /* Initialize LCD */
  my_mem_init(SRAMIN);                /* Initialize the internal SRAM memory pool */
  stm32f103ve_show_mesg();
  lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
  lcd_show_string(30, 70, 200, 16, 16, "SD TEST", RED);
  lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);

  lcd_show_string(30, 110, 200, 16, 16, "WKUP: Read Block 0", RED);

  while (g_sd_error == 1)
  {
    g_sd_error = 0;
    MX_SDIO_SD_Init();
    lcd_show_string(30, 130, 200, 16, 16, "SD Card Error!", RED);
    HAL_Delay(500);
    lcd_show_string(30, 130, 200, 16, 16, "Please Check! ", RED);
    HAL_Delay(500);
    LED1_TOGGLE();
  }

  lcd_show_string(30, 130, 200, 16, 16, "SD Card OK    ", BLUE);

  /* Display SD card information */
  show_sd_info();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    key = key_scan(0);
    if (key == WKUP_PRES)
    {
      /* SD card read test */
      sd_read_test();
    }

    if (++t == 20)
    {
      t = 0;
      LED0_TOGGLE();     /* flashing LED0 indicates that the system is running */
    }

    HAL_Delay(10);       /* delay 10ms */
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
