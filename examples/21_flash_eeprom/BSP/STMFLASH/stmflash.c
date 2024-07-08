/**
 ****************************************************************************************************
 * @file        stmflash.c
 * @author      ALIENTEK
 * @brief       stmflash code
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

#include "stmflash.h"

/**
 * @brief   reads a single word of data from the specified address
 * @param   addr: Specifies the address from which to read the data
 * @retval  the single word read
 */
uint32_t stmflash_read_word(uint32_t addr)
{
    return (*(volatile uint32_t *)addr);
}

/**
 * @brief   reads the specified word from the specified address
 * @param   addr   : Specifies the address from which to read the data
 * @param   buf    : Store the starting address for reading data
 * @param   length : Specifies the length, in words, of the data to be read
 * @retval  None
 */
void stmflash_read(uint32_t addr, uint32_t *buf, uint32_t length)
{
    uint32_t index;
    
    for (index=0; index<length; index++)
    {
        buf[index] = stmflash_read_word(addr);
        addr += sizeof(uint32_t);
    }
}

/**
 * @brief   writes the specified word of data to the specified address without checking
 * @param   addr: Specifies the address to write the data to
 * @param   buf: The start address of the store to write data to
 * @param   length: This specifies the length of the data to be written in words
 * @retval  None
 */
void stmflash_write_nocheck(uint32_t addr, uint32_t *buf, uint16_t length)
{
    uint16_t index;
    
    HAL_FLASH_Unlock();
    for (index=0; index<length; index++)
    {
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr + (index << 2), buf[index]);
    }
    HAL_FLASH_Lock();
}

/**
 * @brief   writes a specified word of data to a specified address
 * @param   addr   : Specifies the address to write the data to
 * @param   buf    : The start address of the store to write data to
 * @param   length : This specifies the length of the data to be written in words
 * @retval  None
 */
static uint32_t g_flash_buf[FLASH_PAGE_SIZE / sizeof(uint32_t)];

void stmflash_write(uint32_t addr, uint32_t *buf, uint16_t length)
{
    uint32_t offaddr;
    uint16_t pagepos;
    uint16_t pageoff;
    uint16_t pageremain;
    uint16_t data_index;
    FLASH_EraseInitTypeDef flash_erase_init_struct = {0};
    uint32_t pageerr;
    
    /* Check the validity of writing to the address range */
    if ((!IS_FLASH_PROGRAM_ADDRESS(addr)) || (!IS_FLASH_PROGRAM_ADDRESS(addr + (length * sizeof(uint32_t)) - 1)))
    {
        return;
    }
    
    offaddr = addr - FLASH_BASE;
    pagepos = offaddr / FLASH_PAGE_SIZE;
    pageoff = (offaddr % FLASH_PAGE_SIZE) / sizeof(uint32_t);
    pageremain = (FLASH_PAGE_SIZE / sizeof(uint32_t)) - pageoff;
    if (length <= pageremain)
    {
        pageremain = length;
    }
    
    while (1)
    {
        stmflash_read(FLASH_BASE + pagepos * FLASH_PAGE_SIZE, g_flash_buf, sizeof(g_flash_buf) / sizeof(uint32_t));
        for (data_index=0; data_index<pageremain; data_index++)
        {
            /* Determine if erasure is required */
            if (g_flash_buf[data_index + pageoff] != 0xFFFFFFFF)
            {
                break;
            }
        }
        if (data_index < pageremain)
        {
            /* Erasing pages */
            flash_erase_init_struct.TypeErase = FLASH_TYPEERASE_PAGES;
            flash_erase_init_struct.Banks = FLASH_BANK_1;
            flash_erase_init_struct.PageAddress = FLASH_BASE + pagepos * FLASH_PAGE_SIZE;
            flash_erase_init_struct.NbPages = 1;
            HAL_FLASH_Unlock();
            HAL_FLASHEx_Erase(&flash_erase_init_struct, &pageerr);
            HAL_FLASH_Lock();
            
            for (data_index=0; data_index<pageremain; data_index++)
            {
                g_flash_buf[pageoff + data_index] = buf[data_index];
            }
            stmflash_write_nocheck(FLASH_BASE + pagepos * FLASH_PAGE_SIZE, g_flash_buf, sizeof(g_flash_buf) / sizeof(uint32_t));
        }
        else
        {
            stmflash_write_nocheck(addr, buf, pageremain);
        }
        
        /* Determines whether the write is complete */
        if (pageremain == length)
        {
            break;
        }
        else
        {
            pagepos++;
            pageoff = 0;
            buf += pageremain;
            addr += (pageremain * sizeof(uint32_t));
            length -= pageremain;
            
            if (length > (FLASH_PAGE_SIZE / sizeof(uint32_t)))
            {
                pageremain = FLASH_PAGE_SIZE / sizeof(uint32_t);
            }
            else
            {
                pageremain = length;
            }
        }
    }
}
/* Test code */

/**
 * @brief    test write data (write 1 word)
 * @param    waddr: Start address
 * @param    wdata: The data to write
 * @retval   the data read
 */
void test_write(uint32_t waddr, uint16_t wdata)
{
    stmflash_write(waddr, &wdata, 1);   /* Write a word and a half */
}

