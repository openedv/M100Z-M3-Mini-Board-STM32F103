/* USER CODE BEGIN Header */
/**
 ****************************************************************************************************
 * @file        main.c
 * @author      ALIENTEK
 * @brief       fatfs code
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
#include "fatfs.h"
#include "sdio.h"
#include "spi.h"
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
#include "../../FatFs/exfuns/exfuns.h"
#include "ff.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
extern uint8_t g_sd_error;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
uint8_t read_buf[50] = {0};
uint8_t write_buf[50] = "Hello test demo";
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
	printf("FATFS Test\r\n");
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
  FRESULT res;
  uint8_t t = 0, key = 0;
  uint32_t total;
  UINT bw = 0, br = 0;
  uint32_t free;
  FIL *f_fil;
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
  MX_SPI2_Init();
  MX_FATFS_Init();
  /* USER CODE BEGIN 2 */
  lcd_init();                         /* Initialize LCD */
  my_mem_init(SRAMIN);                /* Initialize the internal SRAM memory pool */
  stm32f103ve_show_mesg();
  lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
  lcd_show_string(30, 70, 200, 16, 16, "FATFS TEST", RED);
  lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
  lcd_show_string(30, 110, 200, 16, 16, "Use USMART for test", RED);

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

  exfuns_init();

  /* mount SD card */
  f_mount(fs[0], "0:", 1);

  /* mount NOR Flash, if NOR Flash does not have a file system, it needs to be formatted */
  res = f_mount(fs[1], "1:", 1);
  if (res == FR_NO_FILESYSTEM)
  {
    lcd_show_string(30, 130, 200, 16, 16, "Flash Disk Formatting...", RED);
    res = f_mkfs("1:", 0, _MAX_SS);
    if (res == FR_OK)
    {
      f_setlabel((const TCHAR *)"1:ALIENTEK");
      lcd_show_string(30, 130, 200, 16, 16, "Flash Disk Format Finish", RED);
    }
    else
    {
      lcd_show_string(30, 130, 200, 16, 16, "Flash Disk Format Error ", RED);
    }
    HAL_Delay(1000);
  }
  lcd_fill(30, 130, 239, 145, WHITE);

  /* Gets the SD card capacity */
  while (exfuns_get_free("0", &total, &free) != 0)
  {
    lcd_show_string(30, 130, 200, 16, 16, "SD Card FatFs Error!", RED);
    HAL_Delay(200);
    lcd_fill(30, 130, 240, 150 + 16, WHITE);
    HAL_Delay(200);
    LED1_TOGGLE();
  }

  lcd_show_string(30, 130, 200, 16, 16, "FATFS OK!", BLUE);
  lcd_show_string(30, 150, 200, 16, 16, "SD Total Size:     MB", BLUE);
  lcd_show_string(30, 170, 200, 16, 16, "SD Free Size:     MB", BLUE);
  lcd_show_num(142, 150, total >> 10, 5, 16, BLUE);
  lcd_show_num(134, 170, free >> 10, 5, 16, BLUE);

  f_fil = (FIL *)mymalloc(SRAMIN, sizeof(f_fil));        /* Request memory for file */
  if (f_fil == NULL)                                     /* Memory request failed */
  {
     myfree(SRAMIN, f_fil);
  }
  printf("The file system was mounted successfully\r\n");
  printf("************ A file read and write test is coming ************\r\n");
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
  /*------------------- Filesystem tests: read and write tests ---------------------*/
  key = key_scan(0);
  if(key == WKUP_PRES)
  {
    res = f_open(f_fil, "1:test.txt", FA_OPEN_ALWAYS | FA_WRITE | FA_READ);/* Open the file and create a new file if there is no file */
    if(res == FR_OK)
    {
      f_lseek (f_fil, 0);			/* Offset writes data to the initial location */
      res = f_write(f_fil, write_buf, sizeof(write_buf), &bw);
      if(res == FR_OK)
      {
        printf("Data written successfully!!\r\n");
        lcd_show_string(30, 190, 200, 16, 16, "write OK", BLUE);
      }
    }
    else
    {
      lcd_show_string(30, 190, 200, 16, 16, "read and write Error", BLUE);
    }
  }
  else if (key == KEY0_PRES)
  {
    f_lseek (f_fil, 0);				/* Offset to the initial position to read the data */
    res = f_read(f_fil, read_buf, sizeof(read_buf), &br);
    lcd_show_string(30, 210, 200, 16, 16, (char *)read_buf, BLUE);
    printf("The data bits read: %s\r\n", read_buf);
    printf("Reading byte data: %d\r\n", br);
    f_close(f_fil);
  }

  t++;
  if (t % 20 == 0)
  {
    LED0_TOGGLE();  /* flashing LED0 indicates that the system is running */
  }

  HAL_Delay(10);    /* delay 10ms */
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
