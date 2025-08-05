#ifndef __SELF_DRIVER_BOOTLOAD_H__
#define __SELF_DRIVER_BOOTLOAD_H__
#include <stdint.h>
#define APPLICATION_ADDRESS	0x8008000

enum iap_status_t
{
	IAP_READ_FLASH_DATA,
	IAP_WRITE_FLASH_DATA,
	IAP_COMPLETE
};

typedef struct 
{
	uint16_t package_index;
	uint16_t getFlashDataComplete;
	enum iap_status_t iap_status;
} bootload_info_t;

/**
 * @brief   BootLoadUpgrateTaskEntry
 * @details bootload任务函数 
 * @param   none
 * @param   none
 * @return  return description
 */
void BootLoadUpgrateTaskEntry(void);

/**
 * @brief   JumpToAPP
 * @details 跳转APP 
 * @param   none
 * @param   none
 * @return  none
 */
void JumpToAPP(void);

#endif /* __SELF_DRIVER_BOOTLOAD_H__ */