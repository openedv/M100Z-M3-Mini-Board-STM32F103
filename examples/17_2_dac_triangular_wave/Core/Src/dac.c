/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    dac.c
  * @brief   This file provides code for the configuration
  *          of the DAC instances.
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
#include "dac.h"
#include "delay.h"
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

DAC_HandleTypeDef hdac;

/* DAC init function */
void MX_DAC_Init(void)
{

  /* USER CODE BEGIN DAC_Init 0 */

  /* USER CODE END DAC_Init 0 */

  DAC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN DAC_Init 1 */

  /* USER CODE END DAC_Init 1 */

  /** DAC Initialization
  */
  hdac.Instance = DAC;
  if (HAL_DAC_Init(&hdac) != HAL_OK)
  {
    Error_Handler();
  }

  /** DAC channel OUT1 config
  */
  sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;
  if (HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN DAC_Init 2 */
  HAL_DAC_Start(&hdac, DAC_CHANNEL_1);  /* Open DAC channel 1 */
  /* USER CODE END DAC_Init 2 */

}

void HAL_DAC_MspInit(DAC_HandleTypeDef* dacHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(dacHandle->Instance==DAC)
  {
  /* USER CODE BEGIN DAC_MspInit 0 */

  /* USER CODE END DAC_MspInit 0 */
    /* DAC clock enable */
    __HAL_RCC_DAC_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**DAC GPIO Configuration
    PA4     ------> DAC_OUT1
    */
    GPIO_InitStruct.Pin = GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN DAC_MspInit 1 */

  /* USER CODE END DAC_MspInit 1 */
  }
}

void HAL_DAC_MspDeInit(DAC_HandleTypeDef* dacHandle)
{

  if(dacHandle->Instance==DAC)
  {
  /* USER CODE BEGIN DAC_MspDeInit 0 */

  /* USER CODE END DAC_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_DAC_CLK_DISABLE();

    /**DAC GPIO Configuration
    PA4     ------> DAC_OUT1
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_4);

  /* USER CODE BEGIN DAC_MspDeInit 1 */

  /* USER CODE END DAC_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
/**
 * @brief  Sets DAC_OUT1 to output triangular waves
 * @param  maxval  : Maximum value (0 < maxval < 4096), (maxval + 1) must be greater than or equal to samples/2
 * @param  dt      : the latency (in us) of each sample point
 * @param  samples : The number of sample points, samples must be less than or equal to (maxval + 1) * 2, and maxval cannot be equal to 0
 * @param  n       : Number of output waveforms,0 to 65535
 * @retval None
 */
void dac_triangular_wave(uint16_t maxval, uint16_t dt, uint16_t samples, uint16_t n)
{
    uint16_t i, j;
    float incval;                           /* incremental change */
    float curval;                           /* current value */

    if ((maxval + 1) <= samples)
    {
        return ;                            /* Data is not valid */
    }

    incval = (maxval + 1) / (samples / 2);  /* Calculate the increment */

    for (j = 0; j < n; j++)
    {
        curval = 0;
        HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, curval);      /* I'm going to print 0 first. */
        for (i = 0; i < (samples / 2); i++)                                   /* Output rising edge */
        {
            curval +=  incval;                                                /* New output value */
            HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, curval);
            delay_us(dt);                                                     /* delay 5ms */
        }
        for (i = 0; i < (samples / 2); i++)                                   /* Output falling edge */
        {
            curval -=  incval;                                                /* New output value */
            HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, curval);
            delay_us(dt);                                                     /* delay 5ms */
        }
    }
}
/* USER CODE END 1 */
