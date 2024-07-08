/* USER CODE BEGIN Header */
/**
 ****************************************************************************************************
 * @file        main.c
 * @author      ALIENTEK
 * @brief       USB Card Reader code
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
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"
#include "fsmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "../../ATK_Middlewares/MALLOC/malloc.h"
#include "../../BSP/LED/led.h"
#include "../../BSP/KEY/key.h"
#include "../../BSP/LCD/lcd.h"
#include "../../BSP/NORFLASH/norflash.h"
#include "usbd_desc.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
extern uint8_t g_sd_error;
extern uint8_t g_sd_sta;
extern volatile uint8_t g_usb_state_reg;    /* USB status */
extern volatile uint8_t g_device_state;     /* USB connection condition */

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
	printf("USB_CARD_READER Test\r\n");
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
  uint8_t offline_cnt = 0;
  uint8_t tct = 0;
  uint8_t usb_sta;
  uint8_t device_sta;
  uint16_t id;
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
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */
  lcd_init();                         /* Initialize LCD */
  norflash_init();                    /* Initialize NOR FLASH */
  my_mem_init(SRAMIN);                /* Initialize the internal SRAM memory pool */
  stm32f103ve_show_mesg();

  lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
  lcd_show_string(30, 70, 200, 16, 16, "USB Card Reader TEST", RED);
  lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);

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

  id = norflash_read_id();
  if ((id == 0) || (id == 0XFFFF))
  {
    lcd_show_string(30, 110, 200, 16, 16, "NorFlash Error!", RED);  /* Detecting NorFlash errors */
  }
  else   /* SPI FLASH normal */
  {
    lcd_show_string(30, 130, 200, 16, 16, "SPI FLASH Size:7.25MB", RED);
  }

  usbd_port_config(0);    /* The USB cable is disconnected */
  HAL_Delay(500);
  usbd_port_config(1);    /* USB connection again */
  HAL_Delay(500);

  lcd_show_string(30, 170, 200, 16, 16, "USB Connecting...", RED);    /* A connection is being established */
  HAL_Delay(1800);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  HAL_Delay(1);

	  if (usb_sta != g_usb_state_reg)   /* The state changes */
	  {
	    lcd_fill(30, 190, 240, 210 + 16, WHITE); /* clear display */

	    if (g_usb_state_reg & 0x01)     /* Writing */
	    {
	      LED1(0);
	      lcd_show_string(30, 190, 200, 16, 16, "USB Writing...", RED); /* USB is writing data */
      }

	    if (g_usb_state_reg & 0x02)      /* Reading */
	    {
	      LED1(0);
	      lcd_show_string(30, 190, 200, 16, 16, "USB Reading...", RED); /* USB is reading data */
      }

	    if (g_usb_state_reg & 0x04)
	    {
	      lcd_show_string(30, 210, 200, 16, 16, "USB Write Err ", RED); /* Prompt write error */
	    }
	    else
	    {
	      lcd_fill(30, 210, 240, 230 + 16, WHITE); /* clear display */
	    }

	    if (g_usb_state_reg & 0x08)
	    {
	      lcd_show_string(30, 230, 200, 16, 16, "USB Read  Err ", RED); /* Prompt readout error */
	    }
	    else
	    {
	      lcd_fill(30, 230, 240, 250 + 16, WHITE); /* clear display */
	    }

	    usb_sta = g_usb_state_reg; /* Record the final state */
    }

	  if (device_sta != g_device_state)
	  {
	    if (g_device_state == 1)
	    {
	      lcd_show_string(30, 170, 200, 16, 16, "USB Connected    ", RED);    /* A USB connection has been established */
	    }
	    else
	    {
	      lcd_show_string(30, 170, 200, 16, 16, "USB DisConnected ", RED);    /* USB is unplugged */
	    }

	    device_sta = g_device_state;
	  }

	  tct++;

	  if (tct == 200)
	  {
	    tct = 0;
	    LED1(1);        /* turn off LED1 */
	    LED0_TOGGLE();  /* LED0 blink */

	    if (g_usb_state_reg & 0x10)
	    {
	      offline_cnt = 0;    /* When USB is connected, clear the offline counter */
	      g_device_state = 1;
	    }
	    else    /* No polling */
	    {
	      offline_cnt++;

	      if (offline_cnt > 100)
	      {
	        g_device_state = 0;/* If no online marker is received within 20s, the USB has been unplugged */
	      }
	    }

	    g_usb_state_reg = 0;
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
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
