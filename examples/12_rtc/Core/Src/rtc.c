/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    rtc.c
  * @brief   This file provides code for the configuration
  *          of the RTC instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "rtc.h"

/* USER CODE BEGIN 0 */
#include "usart.h"
#include "led.h"

static uint8_t rtc_is_leap_year(uint16_t year);     /* Determines whether the current year is a leap year */
static long rtc_date2sec(uint16_t syear, uint8_t smon, uint8_t sday, uint8_t hour, uint8_t min, uint8_t sec);   /* Converts the year, month, day, hour, minute, and second to a number of seconds */
/* USER CODE END 0 */

RTC_HandleTypeDef hrtc;

/* RTC init function */
void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */
  uint16_t flag = 0;
  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef DateToUpdate = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
  hrtc.Init.OutPut = RTC_OUTPUTSOURCE_NONE;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */
  flag = rtc_read_bkr(1);  	/* Read the value of BKP0 */
  if ((flag != 0x5051) && (flag != 0x5050))   	/* Uninitialized before, reconfigured */
  {
  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0x9;
  sTime.Minutes = 0x0;
  sTime.Seconds = 0x0;

  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  DateToUpdate.WeekDay = RTC_WEEKDAY_WEDNESDAY;
  DateToUpdate.Month = RTC_MONTH_MAY;
  DateToUpdate.Date = 0x1;
  DateToUpdate.Year = 0x24;

  if (HAL_RTC_SetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */
    rtc_set_time(2024, 5, 1, 12, 0, 0);       	   /* set time */
    rtc_write_bkr(0, 0X5050);					   /* 0x5050 is written so that the time to initialize the RTC is not repeated */
  }
    __HAL_RTC_ALARM_ENABLE_IT(&hrtc, RTC_IT_SEC);  /* Allow second interrupts */
  /* Updating time information */
   rtc_get_time();
  /* USER CODE END RTC_Init 2 */

}

void HAL_RTC_MspInit(RTC_HandleTypeDef* rtcHandle)
{

  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspInit 0 */

  /* USER CODE END RTC_MspInit 0 */
    HAL_PWR_EnableBkUpAccess();
    /* Enable BKP CLK enable for backup registers */
    __HAL_RCC_BKP_CLK_ENABLE();
    /* RTC clock enable */
    __HAL_RCC_RTC_ENABLE();

    /* RTC interrupt Init */
    HAL_NVIC_SetPriority(RTC_IRQn, 2, 2);
    HAL_NVIC_EnableIRQ(RTC_IRQn);
  /* USER CODE BEGIN RTC_MspInit 1 */

  /* USER CODE END RTC_MspInit 1 */
  }
}

void HAL_RTC_MspDeInit(RTC_HandleTypeDef* rtcHandle)
{

  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspDeInit 0 */

  /* USER CODE END RTC_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_RTC_DISABLE();

    /* RTC interrupt Deinit */
    HAL_NVIC_DisableIRQ(RTC_IRQn);
  /* USER CODE BEGIN RTC_MspDeInit 1 */

  /* USER CODE END RTC_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
_calendar_obj g_calendar = {0};

/**
 * @brief   RTC writes to the fallback region SRAM
 * @param   bkrx : fallback register number, range: 0-41
 *                 Corresponding to RTC_BKP_DR1 to RTC_BKP_DR42
 * @param   data : 16 bits of data to be written
 * @retval  None
 */
void rtc_write_bkr(uint32_t bkrx, uint16_t data)
{
    HAL_PWR_EnableBkUpAccess(); /* Disable write protection for the backup area */
    HAL_RTCEx_BKUPWrite(&hrtc, bkrx + 1, data);
}

/**
 * @brief   RTC reads the fallback region SRAM
 * @param   bkrx: fallback register number, range: 0-31
 *                Corresponding to RTC_BKP_DR1 to RTC_BKP_DR42
 * @retval  the value read
 */
uint16_t rtc_read_bkr(uint32_t bkrx)
{
    uint32_t temp = 0;
    temp = HAL_RTCEx_BKUPRead(&hrtc, bkrx + 1);
    return (uint16_t)temp; /* Returns the value read */
}

/**
 * @brief  Determines if the year is a leap year
 * @note   month day table:
 *         Month 12 3 4 5 6 7 8 9 10 11 12
 *         Leap year 31 29 31 30 31 31 31 31 31 30 30 31 31
 *         Non-leap years 31 28 31 30 31 31 31 31 30 30 31 31
 * @param  year: The year
 * @retval 0, not a leap year; 1, is a leap year;
 */
static uint8_t rtc_is_leap_year(uint16_t year)
{
    /* Leap year rule: four years leap hundred years not leap, four hundred years and leap */
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
 * @brief  sets RTC timing information
 * @param  year  : The year
 * @param  month : month
 * @param  date  : Day
 * @param  hour  : when
 * @param  minute: minutes
 * @param  second: Seconds
 * @retval Sets the result
 * @arg     0: The setup was successful
 * @arg     1: Failed to set
 */
uint8_t rtc_set_time(uint16_t syear, uint8_t smon, uint8_t sday, uint8_t hour, uint8_t min, uint8_t sec)
{
  uint32_t seccount = 0;

  seccount = rtc_date2sec(syear, smon, sday, hour, min, sec); /* Converts the year, month, day, hour, minute, and second to a total number of seconds */

  __HAL_RCC_PWR_CLK_ENABLE(); /* Enable the power clock */
  __HAL_RCC_BKP_CLK_ENABLE(); /* Enable the backup domain clock */
  HAL_PWR_EnableBkUpAccess(); /* Disable backup domain write protection */

  RTC->CRL |= 1 << 4;         /* Enable configuration */

  RTC->CNTL = seccount & 0xffff;
  RTC->CNTH = seccount >> 16;

  RTC->CRL &= ~(1 << 4);      /* Configuration updates */

  while (!__HAL_RTC_ALARM_GET_FLAG(&hrtc, RTC_FLAG_RTOFF));   /* Wait for the RTC register operation to complete, that is, wait for RTOFF == 1 */

  return 0;
}

/**
 * @brief   Sets an alarm to the year, month, day, hour, and second
 * @note    takes January 1, 1970 as the base date and adds time backward
 * Legal years range from 1970 to 2105
 * @param   syear : The year
 * @param   smon  : Month
 * @param   sday  : The date
 * @param   hour  : The hour
 * @param   min   : Minutes
 * @param   sec   : seconds
 * @retval  0, success; 1. fail;
 */
uint8_t rtc_set_alarm(uint16_t syear, uint8_t smon, uint8_t sday, uint8_t hour, uint8_t min, uint8_t sec)
{
    uint32_t seccount = 0;

    seccount = rtc_date2sec(syear, smon, sday, hour, min, sec); /* Converts the year, month, day, hour, minute, and second to a total number of seconds */

    __HAL_RCC_PWR_CLK_ENABLE(); /* Enable the power clock */
    __HAL_RCC_BKP_CLK_ENABLE(); /* Enable the backup domain clock */
    HAL_PWR_EnableBkUpAccess(); /* Disable backup domain write protection */

    RTC->CRL |= 1 << 4;         /* Enable configuration */

    RTC->ALRL = seccount & 0xffff;
    RTC->ALRH = seccount >> 16;

    RTC->CRL &= ~(1 << 4);      /* Configuration updates */

    while (!__HAL_RTC_ALARM_GET_FLAG(&hrtc, RTC_FLAG_RTOFF));       /* Wait for the RTC register operation to complete, that is, wait for RTOFF == 1 */

    return 0;
}

/**
 * @brief   Gets the current time
 * @param   void
 * @retval  None
 */
void rtc_get_time(void)
{
    static uint16_t daycnt = 0;
    uint32_t seccount = 0;
    uint32_t temp = 0;
    uint16_t temp1 = 0;
    const uint8_t month_table[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}; /* Calendar of the month and date of the year */

    seccount = RTC->CNTH;       /* Get the value in the counter (seconds) */
    seccount <<= 16;
    seccount += RTC->CNTL;

    temp = seccount / 86400;    /* Get the day (as a number of seconds) */

    if (daycnt != temp)         /* It's been over a day. */
    {
        daycnt = temp;
        temp1 = 1970;           /* It started in 1970 */

        while (temp >= 365)
        {
            if (rtc_is_leap_year(temp1)) /* It's a leap year */
            {
                if (temp >= 366)
                {
                    temp -= 366; /* The number of seconds in a leap year */
                }
                else
                {
                    break;
                }
            }
            else
            {
                temp -= 365;    /* nonleap year */
            }

            temp1++;
        }

        g_calendar.year = temp1;/* Get the year */
        temp1 = 0;

        while (temp >= 28)      /* More than a month */
        {
            if (rtc_is_leap_year(g_calendar.year) && temp1 == 1) /* Is it a leap year/February */
            {
                if (temp >= 29)
                {
                    temp -= 29; /* The number of seconds in a leap year */
                }
                else
                {
                    break;
                }
            }
            else
            {
                if (temp >= month_table[temp1])
                {
                    temp -= month_table[temp1]; /* nonleap year */
                }
                else
                {
                    break;
                }
            }

            temp1++;
        }

        g_calendar.month = temp1 + 1; /* Get the month */
        g_calendar.date = temp + 1;   /* Get the date */
    }

    temp = seccount % 86400;                                                    /* Get the number of seconds */
    g_calendar.hour = temp / 3600;
    g_calendar.min = (temp % 3600) / 60;
    g_calendar.sec = (temp % 3600) % 60;
    g_calendar.week = rtc_get_week(g_calendar.year, g_calendar.month, g_calendar.date); /* get week */
}


/**
 * @brief   Year month day to week
 * @param   year  : The year
 * @param   month : month
 * @param   date  : Day
 * @retval  week
 */
uint8_t rtc_get_week(uint16_t year, uint8_t month, uint8_t date)
{
    uint8_t week = 0;

    if (month < 3)
    {
        month += 12;
        --year;
    }

    week = (date + 1 + 2 * month + 3 * (month + 1) / 5 + year + (year >> 2) - year / 100 + year / 400) % 7;

    return week;
}

/**
 * @brief converts the year, month, day, hour, minute, and second to a number of seconds
 * @param    syear : The year
 * @param    smon  : Month
 * @param    sday  : The date
 * @param    hour  : The hour
 * @param    min   : Minutes
 * @param    sec   : seconds
 * @retval   number of seconds after conversion
 */
static long rtc_date2sec(uint16_t syear, uint8_t smon, uint8_t sday, uint8_t hour, uint8_t min, uint8_t sec)
{
    uint32_t Y, M, D, X, T;
    signed char monx = smon;    /* Convert the month to a signed value for later calculations */

    if (0 >= (monx -= 2))       /* 1..12 -> 11,12,1..10 */
    {
        monx += 12;             /* Puts Feb last since it has leap day */
        syear -= 1;
    }

    Y = (syear - 1) * 365 + syear / 4 - syear / 100 + syear / 400; /* The number of leap years from A.D. 1 to the present */
    M = 367 * monx / 12 - 30 + 59;
    D = sday - 1;
    X = Y + M + D - 719162;                      /* Subtract the number of days from A.D. 1 to 1970 */
    T = ((X * 24 + hour) * 60 + min) * 60 + sec; /* Total seconds */
    return T;
}

/**
 * @brief   gets the current time
 * @note    This function does not return the time directly, the time data is stored in the calendar struct
 * @param   None
 * @retval  None
 */
void hal_rtc_get_tim(void)
{
  RTC_DateTypeDef sDate;
  RTC_TimeTypeDef sTime;

  /* Get the RTC current Time */
  HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
  /* Get the RTC current Date */
  HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
  g_calendar.hour = sTime.Hours;
  g_calendar.min = sTime.Minutes;
  g_calendar.sec = sTime.Seconds;
  g_calendar.year = 2000 + sDate.Year;
  g_calendar.month = sDate.Month;
  g_calendar.date = sDate.Date;
  g_calendar.week = rtc_get_week(g_calendar.year, g_calendar.month, g_calendar.date);
}

/* USER CODE END 1 */
