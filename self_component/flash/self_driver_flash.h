#ifndef __SELF_DRIVER_FLASH_H__
#define __SELF_DRIVER_FLASH_H__

#include "stm32g0xx_hal_flash.h"
// HAL_StatusTypeDef flash_write(uint32_t address, uint64_t data);
// void flash_page_erase(uint32_t page);

#include "stdint.h"
#include "stdbool.h"
/**
*******************************************************************
* @function 指定地址开始写入指定个数的数据
* @param    writeAddr,写入地址
* @param    pBuffer,数组首地址
* @param    numToWrite,要写入的数据个数
* @return                                                         
*******************************************************************
*/
HAL_StatusTypeDef FlashWrite(uint32_t writeAddr, uint64_t *pBuffer, uint32_t numToWrite);

/**
*******************************************************************
* @function 指定地址开始读出指定个数的数据
* @param    readAddr,读取地址
* @param    pBuffer,数组首地址
* @param    numToRead,要读出的数据个数
* @return   
*******************************************************************
*/
bool FlashRead(uint32_t readAddr, uint64_t *pBuffer, uint32_t numToRead);

/**
*******************************************************************
* @function 擦除从eraseAddr开始到eraseAddr + numToErase的页
* @param    eraseAddr,地址
* @param    numToErase,对应写入数据时的个数 对应uint64
* @return                                                         
*******************************************************************
*/
HAL_StatusTypeDef FlashErase(uint32_t eraseAddr, uint32_t numToErase);

#endif /* __SELF_DRIVER_FLASH_H__ */