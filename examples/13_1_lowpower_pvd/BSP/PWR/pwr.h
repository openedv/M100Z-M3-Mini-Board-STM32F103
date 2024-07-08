/**
 ****************************************************************************************************
 * @file        pwr.h
 * @author      ALIENTEK
 * @brief       pwr code
 * @license     Copyright (C) 2012-2024, ALIENTEK
 ****************************************************************************************************
 * @attention
 *
 * platform     : ALIENTEK M100-STM32F103 board
 * website      : www.alientek.com
 * forum        : www.openedv.com/forum.php
 *
 * change logs  ：
 * version      data         notes
 * V1.0         20240409     the first version
 *
 ****************************************************************************************************
 */

#ifndef PWR_PWR_H_
#define PWR_PWR_H_

#include "stdint.h"
#include "main.h"

void pwr_pvd_init(uint32_t pls);     /* PVD voltage detection initialization function */

#endif /* PWR_PWR_H_ */
