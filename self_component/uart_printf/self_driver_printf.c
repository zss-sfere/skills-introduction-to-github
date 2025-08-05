/**
 * @file    self_driver_printf.c
 * @brief   Brief description
 * @details Detailed description
 * @author  baoxn
 * @date    2025-06-04
 * @version 1.0
 */
/**************************************头文件****************************************/
#include "stdio.h"
#include "main.h"
/*************************************结构体定义*************************************/

/*************************************全局变量**************************************/
#define DEBUG_USART     USART3
/*************************************函数声明**************************************/

/*************************************代码段***************************************/
/**
 * @brief   Brief description
 * @details Detailed 
 * @param   param_1
 * @param   param_2
 * @return  return description
 */
int __io_putchar(int ch)
{
    DEBUG_USART->TDR = (uint8_t)ch;
    while (LL_USART_IsActiveFlag_TXE(DEBUG_USART) == RESET);
    return ch;
}