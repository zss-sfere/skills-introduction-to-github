/**
 * @file    self_driver_bootload.c
 * @brief   Brief description
 * @details Detailed description
 * @author  baoxn
 * @date    2025-06-12
 * @version 1.0
 */
/**************************************头文件****************************************/
#include "self_driver_bootload.h"
#include "main.h"
#include "self_driver_flash.h"
#include "self_driver_log.h"
/*************************************结构体定义*************************************/



/*************************************全局变量**************************************/
#define BOOTLOAD_DMA_ID			DMA1
#define BOOTLOAD_DMA_CHANNEL	LL_DMA_CHANNEL_1
#define BOOTLOAD_DMA_IRQHandler	DMA1_Channel1_IRQHandler
#define FLASH_SRC_ADDR    		0x8040000
#define FLASH_DST_ADDR			0x8008000
#define BOOTLOAD_PER_DATA_NUM	1024
static uint8_t g_iapBuf[BOOTLOAD_PER_DATA_NUM] = {0};
extern uint16_t g_packageSum;
bootload_info_t g_bootload_info = {.package_index = 1, .iap_status = IAP_READ_FLASH_DATA, .getFlashDataComplete = 0};
/*************************************函数声明**************************************/

/*************************************代码段***************************************/
/**
 * @brief   GetFlashDataToRam
 * @details Detailed 
 * @param   flashAddress	flash的地址
 * @param   none
 * @return  return description
 */
void GetFlashDataToRam(uint32_t flashAddress)
{
	LL_DMA_EnableIT_TC(BOOTLOAD_DMA_ID, BOOTLOAD_DMA_CHANNEL);
	LL_DMA_SetM2MSrcAddress(BOOTLOAD_DMA_ID, BOOTLOAD_DMA_CHANNEL, (uint32_t)flashAddress);
	LL_DMA_SetM2MDstAddress(BOOTLOAD_DMA_ID, BOOTLOAD_DMA_CHANNEL, (uint32_t)&g_iapBuf);
	LL_DMA_SetDataLength(BOOTLOAD_DMA_ID, BOOTLOAD_DMA_CHANNEL, BOOTLOAD_PER_DATA_NUM);
	LL_DMA_EnableChannel(BOOTLOAD_DMA_ID, BOOTLOAD_DMA_CHANNEL);
}

/**
 * @brief   BootLoadUpgrateTaskEntry
 * @details bootload任务函数 
 * @param   none
 * @param   none
 * @return  return description
 */
void BootLoadUpgrateTaskEntry(void)
{
	switch (g_bootload_info.iap_status)
	{
		case IAP_READ_FLASH_DATA:
			GetFlashDataToRam(FLASH_SRC_ADDR + (g_bootload_info.package_index - 1) * BOOTLOAD_PER_DATA_NUM);
			g_bootload_info.iap_status = IAP_WRITE_FLASH_DATA;
			LOG_D("IAP中读的地址是 %#x", (FLASH_SRC_ADDR + (g_bootload_info.package_index - 1) * BOOTLOAD_PER_DATA_NUM));
		break;
		case IAP_WRITE_FLASH_DATA:
			if (g_bootload_info.getFlashDataComplete)
			{
				g_bootload_info.getFlashDataComplete = 0;
				//奇数帧需要擦掉下一页
				if (g_bootload_info.package_index % 2 != 0)
				{
					if (FlashErase(FLASH_DST_ADDR + (g_bootload_info.package_index/2)*FLASH_PAGE_SIZE, 1) != HAL_OK)
					{
						FlashErase(FLASH_DST_ADDR + (g_bootload_info.package_index/2)*FLASH_PAGE_SIZE, 1);
					}
					else
					{
						if (FlashWrite(FLASH_DST_ADDR + (g_bootload_info.package_index/2)*FLASH_PAGE_SIZE, (uint64_t*)g_iapBuf, 1024/8) != HAL_OK)     //这里传进去的个数是指有多少个uint64 所以要除以8
						{
							FlashWrite(FLASH_DST_ADDR + (g_bootload_info.package_index/2)*FLASH_PAGE_SIZE, (uint64_t*)g_iapBuf, 1024/8);
						}
					
					}
					
					LOG_D("IAP package_index是%d, 擦除地址%#x.", g_bootload_info.package_index, (FLASH_DST_ADDR + (g_bootload_info.package_index/2)*FLASH_PAGE_SIZE));
				}
				else
				{
					FlashWrite(FLASH_DST_ADDR + (g_bootload_info.package_index - 1)*BOOTLOAD_PER_DATA_NUM, (uint64_t*)g_iapBuf, 1024/8);
					LOG_D("IAP package_index是%d, 保存地址%#x.", g_bootload_info.package_index, (FLASH_DST_ADDR + (g_bootload_info.package_index - 1)*BOOTLOAD_PER_DATA_NUM));
				}
				LOG_D("IAP 当前包的序号:%d.", g_bootload_info.package_index);
				if (++g_bootload_info.package_index > g_packageSum)
				{
					g_bootload_info.iap_status = IAP_COMPLETE;
					LOG_D("完成iap升级");
				}
				else
				{
					g_bootload_info.iap_status = IAP_READ_FLASH_DATA;
					
				}
			}
			
			
		break;
		case IAP_COMPLETE:
			g_bootload_info.package_index = 1;
			FlashErase(IAP_FLAG_ADDR, 1);
			JumpToAPP();
		break;
		default:
		break;
	}
}

/**
 * @brief   BOOTLOAD_DMA_IRQHandler
 * @details dma传输完成中断 
 * @param   none
 * @param   none
 * @return  return description
 */
void BOOTLOAD_DMA_IRQHandler(void)
{
	g_bootload_info.getFlashDataComplete = 1;
	LL_DMA_ClearFlag_TC1(BOOTLOAD_DMA_ID);
	LL_DMA_DisableIT_TC(BOOTLOAD_DMA_ID, BOOTLOAD_DMA_CHANNEL);
	LL_DMA_DisableChannel(BOOTLOAD_DMA_ID, BOOTLOAD_DMA_CHANNEL);
}

/**
 * @brief   JumpToAPP
 * @details 跳转APP 
 * @param   none
 * @param   none
 * @return  none
 */
void JumpToAPP(void)
{
	__disable_irq();
	void (*JumpToApplication)(void);
	__set_MSP(*(__IO uint32_t *)APPLICATION_ADDRESS);
	JumpToApplication = (void(*)(void))(*((uint32_t *)(APPLICATION_ADDRESS + 4)));
	__set_PRIMASK(1);
	JumpToApplication();
}
