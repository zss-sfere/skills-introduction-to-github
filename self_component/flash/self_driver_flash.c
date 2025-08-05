/**
 * @file    self_driver_flash.c
 * @brief   Brief description
 * @details Detailed description
 * @author  baoxn
 * @date    2025-06-08
 * @version 1.0
 */
/**************************************头文件****************************************/
#include "main.h"
#include "self_driver_flash.h"
#include "self_driver_systick.h"
#include "stdio.h"
#include "stm32g0xx_hal_flash.h"
#include "self_driver_log.h"
/*************************************结构体定义*************************************/

/*************************************全局变量**************************************/

/*************************************函数声明**************************************/

/*************************************代码段***************************************/
// HAL_StatusTypeDef flash_write(uint32_t address, uint64_t data)
// {
// 	HAL_StatusTypeDef ret = HAL_OK;

// 	HAL_FLASH_Unlock();
// 	ret = HAL_FLASH_Program(TYPEPROGRAM_DOUBLEWORD, address, data);
// 	HAL_FLASH_Lock();

// 	return ret;
// }

// void flash_page_erase(uint32_t page)
// {
// 	HAL_StatusTypeDef test = HAL_FLASH_Unlock();

// 	test = FLASH_WaitForLastOperation(1000);
// 	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR);
// 	FLASH_PageErase(FLASH_BANK_2, page);
// 	test = FLASH_WaitForLastOperation(1000);
// 	CLEAR_BIT(FLASH->CR, FLASH_CR_PER);

// 	HAL_FLASH_Lock();
// }


/**
*******************************************************************
* @function 指定地址开始读出指定个数的数据
* @param    readAddr,读取地址
* @param    pBuffer,数组首地址
* @param    numToRead,要读出的数据个数 这个个数是说有多少个uint64
* @return   
*******************************************************************
*/
bool FlashRead(uint32_t readAddr, uint64_t *pBuffer, uint32_t numToRead)
{	
	if ((readAddr + numToRead * 8) > FLASH_END_ADDRESS)
	{
		return false;
	}
	
	uint32_t addr = readAddr;
	for (uint32_t i = 0; i < numToRead; i++) 
	{
		*pBuffer = *(uint64_t *)addr;
		addr = addr + 8;
		pBuffer++;
	}
	return true;
}

/**
*******************************************************************
* @function 指定地址开始写入指定个数的数据
* @param    writeAddr,写入地址
* @param    pBuffer,数组首地址
* @param    numToWrite,要写入的数据个数 这个个数是说有多少个uint64
* @return                                                         
*******************************************************************
*/
HAL_StatusTypeDef FlashWrite(uint32_t writeAddr, uint64_t *pBuffer, uint32_t numToWrite)
{	
	HAL_StatusTypeDef status = HAL_OK;
	if ((writeAddr + numToWrite * 8) > FLASH_END_ADDRESS)
	{
		status = HAL_ERROR;
		return status;
	}
	if (writeAddr % 2 == 1)   // 双字(2字节)写入，地址要对齐
	{
		status = HAL_ERROR;
		return status;
	}
	HAL_FLASH_Unlock();
	LOG_D("写flash的起始地址是%#lx.", writeAddr);
	for (uint32_t i = 0; i < numToWrite; i++) 
	{
		status = HAL_FLASH_Program(TYPEPROGRAM_DOUBLEWORD, writeAddr, *(uint64_t *)pBuffer);
		pBuffer += 1;
		writeAddr += 8;
	}
	LOG_D("写flash的结束地址是%#lx.", writeAddr);
	HAL_FLASH_Lock();
	
	return status;
}

/**
*******************************************************************
* @function 擦除从eraseAddr开始到eraseAddr + numToErase的页
* @param    eraseAddr,地址
* @param    numToErase,对应写入数据时的个数 对应uint64
* @return                                                         
*******************************************************************
*/
HAL_StatusTypeDef FlashErase(uint32_t eraseAddr, uint32_t numToErase)
{
	HAL_StatusTypeDef status = HAL_OK;
	uint32_t calNumToErase = numToErase * 8;
	if (calNumToErase == 0 || (eraseAddr + calNumToErase*8) > FLASH_END_ADDRESS)
	{
		status = HAL_ERROR;
		return status;
	} 
	
	uint8_t pageNum;
	uint8_t addrOffset = eraseAddr % FLASH_PAGE_SIZE; 	// mod运算求余在一页内的偏移，若eraseAddr是FLASH_PAGE_SIZE整数倍，运算结果为0

	HAL_FLASH_Unlock();
	status = FLASH_WaitForLastOperation(1000);
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR);

	if (calNumToErase > (FLASH_PAGE_SIZE - addrOffset))           // 跨页
	{
		/* Clear SR register */
    	FLASH->SR = FLASH_SR_CLEAR;
		FLASH_PageErase(eraseAddr);           // 擦本页
		
		
		eraseAddr += FLASH_PAGE_SIZE - addrOffset;   // 对齐到页地址
		calNumToErase -= FLASH_PAGE_SIZE - addrOffset;
		pageNum = calNumToErase / FLASH_PAGE_SIZE;

		while (pageNum--) 
		{
			/* Clear SR register */
    		FLASH->SR = FLASH_SR_CLEAR;
			FLASH_PageErase(eraseAddr);
			
			eraseAddr += FLASH_PAGE_SIZE;
		}
		if (calNumToErase % FLASH_PAGE_SIZE != 0)
		{
			FLASH->SR = FLASH_SR_CLEAR;
			FLASH_PageErase(eraseAddr);          
			
		}
	}
	else  // 没有跨页
	{
		FLASH->SR = FLASH_SR_CLEAR;
		FLASH_PageErase(eraseAddr);
		
	}
	status = FLASH_WaitForLastOperation(1000);
	CLEAR_BIT(FLASH->CR, FLASH_CR_PER);
	/* lock the main FMC after the erase operation */
    HAL_FLASH_Lock();
	return status;

}

