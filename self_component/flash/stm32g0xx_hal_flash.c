/**
 * @file    stm32g0xx_hal_flash.c
 * @brief   Brief description
 * @details Detailed description
 * @author  baoxn
 * @date    2025-06-11
 * @version 1.0
 */
/**************************************头文件****************************************/
#include "stm32g0xx_hal_flash.h"
#include "main.h"
#include "self_driver_systick.h"
#include "self_driver_log.h"
/*************************************结构体定义*************************************/

/*************************************全局变量**************************************/
FLASH_ProcessTypeDef pFlash  = {.Lock = HAL_UNLOCKED, \
                                .ErrorCode = HAL_FLASH_ERROR_NONE, \
                                .ProcedureOnGoing = FLASH_TYPENONE, \
                                .Address = 0U, \
                                .Banks = 0U, \
                                .Page = 0U, \
                                .NbPagesToErase = 0U
                               };
/*************************************函数声明**************************************/

/*************************************代码段***************************************/
/**
  * @brief  Unlock the FLASH control register access.
  * @retval HAL Status
  */
HAL_StatusTypeDef HAL_FLASH_Unlock(void)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (READ_BIT(FLASH->CR, FLASH_CR_LOCK) != 0x00U)
  {
    /* Authorize the FLASH Registers access */
    WRITE_REG(FLASH->KEYR, FLASH_KEY1);
    WRITE_REG(FLASH->KEYR, FLASH_KEY2);

    /* verify Flash is unlock */
    if (READ_BIT(FLASH->CR, FLASH_CR_LOCK) != 0x00U)
    {
      status = HAL_ERROR;
    }
  }

  return status;
}

/**
  * @brief  Lock the FLASH control register access.
  * @retval HAL Status
  */
HAL_StatusTypeDef HAL_FLASH_Lock(void)
{
  HAL_StatusTypeDef status = HAL_ERROR;

  /* Set the LOCK Bit to lock the FLASH Registers access */
  SET_BIT(FLASH->CR, FLASH_CR_LOCK);

  /* verify Flash is locked */
  if (READ_BIT(FLASH->CR, FLASH_CR_LOCK) != 0x00u)
  {
    status = HAL_OK;
  }

  return status;
}

/**
  * @brief  Wait for a FLASH operation to complete.
  * @param  Timeout maximum flash operation timeout
  * @retval HAL_StatusTypeDef HAL Status
  */
HAL_StatusTypeDef FLASH_WaitForLastOperation(uint32_t Timeout)
{
  uint32_t error;
  /* Wait for the FLASH operation to complete by polling on BUSY flag to be reset.
     Even if the FLASH operation fails, the BUSY flag will be reset and an error
     flag will be set */
  uint32_t timeout = getSysRunTime() + Timeout;

  /* Wait if any operation is ongoing */
#if defined(FLASH_DBANK_SUPPORT)
  error = (FLASH_SR_BSY1 | FLASH_SR_BSY2);
#else
  error = FLASH_SR_BSY1;
#endif /* FLASH_DBANK_SUPPORT */

  while ((FLASH->SR & error) != 0x00U)
  {
    if (getSysRunTime() >= timeout)
    {
      return HAL_TIMEOUT;
    }
  }

  /* check flash errors */
  error = (FLASH->SR & FLASH_SR_ERRORS);

  /* Clear SR register */
  FLASH->SR = FLASH_SR_CLEAR;

  if (error != 0x00U)
  {
    /*Save the error code*/
    pFlash.ErrorCode = error;
    return HAL_ERROR;
  }

  /* Wait for control register to be written */
  timeout = getSysRunTime() + Timeout;

  while ((FLASH->SR & FLASH_SR_CFGBSY) != 0x00U)
  {
    if (getSysRunTime() >= timeout)
    {
      return HAL_TIMEOUT;
    }
  }

  return HAL_OK;
}

/**
  * @brief  Program double-word (64-bit) at a specified address.
  * @param  Address Specifies the address to be programmed.
  * @param  Data Specifies the data to be programmed.
  * @retval None
  */
static void FLASH_Program_DoubleWord(uint32_t Address, uint64_t Data)
{
  /* Set PG bit */
  SET_BIT(FLASH->CR, FLASH_CR_PG);

  /* Program first word */
  *(uint32_t *)Address = (uint32_t)Data;

  /* Barrier to ensure programming is performed in 2 steps, in right order
    (independently of compiler optimization behavior) */
  __ISB();

  /* Program second word */
  *(uint32_t *)(Address + 4U) = (uint32_t)(Data >> 32U);
}


/**
  * @brief  Program double word or fast program of a row at a specified address.
  * @param  TypeProgram Indicate the way to program at a specified address.
  *                      This parameter can be a value of @ref FLASH_Type_Program
  * @param  Address Specifies the address to be programmed.
  * @param  Data Specifies the data to be programmed
  *               This parameter is the data for the double word program and the address where
  *               are stored the data for the row fast program depending on the TypeProgram:
  *               TypeProgram = FLASH_TYPEPROGRAM_DOUBLEWORD (64-bit)
  *               TypeProgram = FLASH_TYPEPROGRAM_FAST (32-bit).
  *
  * @retval HAL_StatusTypeDef HAL Status
  */
HAL_StatusTypeDef HAL_FLASH_Program(uint32_t TypeProgram, uint32_t Address, uint64_t Data)
{
  HAL_StatusTypeDef status;


  /* Process Locked */
  __HAL_LOCK(&pFlash);

  /* Reset error code */
  pFlash.ErrorCode = HAL_FLASH_ERROR_NONE;

  /* Wait for last operation to be completed */
  status = FLASH_WaitForLastOperation(FLASH_TIMEOUT_VALUE);

  if (status == HAL_OK)
  {
    if (TypeProgram == FLASH_TYPEPROGRAM_DOUBLEWORD)
    {

      /* Program double-word (64-bit) at a specified address */
      FLASH_Program_DoubleWord(Address, Data);
    }

    /* Wait for last operation to be completed */
    status = FLASH_WaitForLastOperation(FLASH_TIMEOUT_VALUE);

    /* If the program operation is completed, disable the PG or FSTPG Bit */
    CLEAR_BIT(FLASH->CR, TypeProgram);
  }

  /* Process Unlocked */
  __HAL_UNLOCK(&pFlash);

  /* return status */
  return status;
}


/**
  * @brief  Erase the specified FLASH memory page.
  * @param  Banks: Banks to be erased
  *         This parameter can one of the following values:
  *            @arg FLASH_BANK_1: Bank1 to be erased
  *            @arg FLASH_BANK_2: Bank2 to be erased*
  * @param  Page FLASH page to erase
  *         This parameter must be a value between 0 and (max number of pages in Flash - 1)
  * @note (*) availability depends on devices
  * @retval None
  */
void FLASH_PageErase(uint32_t addr)
{
  uint32_t tmp;
  uint32_t banks = 0;
  uint32_t page = 0;

	if (addr < FLASH_BANK_2_START)
    {
        banks = FLASH_BANK_1;
    }
    else
    {
        banks = FLASH_BANK_2;
    }

    //计算地址所在的页
    page = (addr - FLASH_BASE) / FLASH_PAGE_SIZE;

    if (page > 127)
    {
        page += 128;
    }
    LOG_D("擦除的页是%lu.",page);
  /* Get configuration register, then clear page number */
  tmp = (FLASH->CR & ~FLASH_CR_PNB);

#if defined(FLASH_DBANK_SUPPORT)
  /* Check if page has to be erased in bank 1 or 2 */
  if (banks != FLASH_BANK_1)
  {
    tmp |= FLASH_CR_BKER;
  }
  else
  {
    tmp &= ~FLASH_CR_BKER;
  }
#endif /* FLASH_DBANK_SUPPORT */

  /* Set page number, Page Erase bit & Start bit */
  FLASH->CR = (tmp | (FLASH_CR_STRT | (page <<  FLASH_CR_PNB_Pos) | FLASH_CR_PER));
}


