/**
 * @file    self_driver_systick.c
 * @brief   Brief description
 * @details Detailed description
 * @author  baoxn
 * @date    2025-06-03
 * @version 1.0
 */
/**************************************头文件****************************************/
#include "main.h"
/*************************************结构体定义*************************************/

/*************************************全局变量**************************************/
volatile uint64_t g_sysRunTime = 0;
/*************************************函数声明**************************************/
// extern void task_timer_update(void);
/*************************************代码段***************************************/
/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
    g_sysRunTime++;
    // task_timer_update();
}

/**
 * @brief   getSysRunTime
 * @details 获取系统运行时间 
 * @param   none
 * @param   none
 * @return  系统运行时间
 */
uint64_t getSysRunTime(void)
{
    return g_sysRunTime;
}