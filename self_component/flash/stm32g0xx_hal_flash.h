#ifndef __STM32G0XX_HAL_FLASH_H__
#define __STM32G0XX_HAL_FLASH_H__

#include "stm32g0b1xx.h"


typedef enum
{
  HAL_OK       = 0x00U,
  HAL_ERROR    = 0x01U,
  HAL_BUSY     = 0x02U,
  HAL_TIMEOUT  = 0x03U
} HAL_StatusTypeDef;

/**
  * @brief  HAL Lock structures definition
  */
typedef enum
{
  HAL_UNLOCKED = 0x00U,
  HAL_LOCKED   = 0x01U
} HAL_LockTypeDef;

/**
  * @brief  FLASH handle Structure definition
  */
typedef struct
{
  HAL_LockTypeDef   Lock;              /* FLASH locking object */
  uint32_t          ErrorCode;         /* FLASH error code */
  uint32_t          ProcedureOnGoing;  /* Internal variable to indicate which procedure is ongoing or not in IT context */
  uint32_t          Address;           /* Internal variable to save address selected for program in IT context */
  uint32_t          Banks;             /* Internal variable to save current bank selected during erase in IT context */
  uint32_t          Page;              /* Internal variable to define the current page which is erasing in IT context */
  uint32_t          NbPagesToErase;    /* Internal variable to save the remaining pages to erase in IT context */
} FLASH_ProcessTypeDef;

#define FLASH_KEY1                      0x45670123U
#define FLASH_KEY2                      0xCDEF89ABU

#define __HAL_LOCK(__HANDLE__)                                           \
                                do{                                        \
                                    if((__HANDLE__)->Lock == HAL_LOCKED)   \
                                    {                                      \
                                       return HAL_BUSY;                    \
                                    }                                      \
                                    else                                   \
                                    {                                      \
                                       (__HANDLE__)->Lock = HAL_LOCKED;    \
                                    }                                      \
                                  }while (0U)

#define __HAL_UNLOCK(__HANDLE__)                                          \
                                  do{                                       \
                                      (__HANDLE__)->Lock = HAL_UNLOCKED;    \
                                    }while (0U)


/** @defgroup FLASH_Error FLASH Error
  * @{
  */
#define HAL_FLASH_ERROR_NONE            0x00000000U
#define HAL_FLASH_ERROR_OP              FLASH_SR_OPERR
#define HAL_FLASH_ERROR_PROG            FLASH_SR_PROGERR
#define HAL_FLASH_ERROR_WRP             FLASH_SR_WRPERR
#define HAL_FLASH_ERROR_PGA             FLASH_SR_PGAERR
#define HAL_FLASH_ERROR_SIZ             FLASH_SR_SIZERR
#define HAL_FLASH_ERROR_PGS             FLASH_SR_PGSERR
#define HAL_FLASH_ERROR_MIS             FLASH_SR_MISERR
#define HAL_FLASH_ERROR_FAST            FLASH_SR_FASTERR
#if defined(FLASH_PCROP_SUPPORT)
#define HAL_FLASH_ERROR_RD              FLASH_SR_RDERR
#endif /* FLASH_PCROP_SUPPORT */
#define HAL_FLASH_ERROR_OPTV            FLASH_SR_OPTVERR
#define HAL_FLASH_ERROR_ECCD            FLASH_ECCR_ECCD

#if defined(FLASH_DBANK_SUPPORT)
#define OB_DUAL_BANK_BASE               (FLASH_R_BASE + 0x20U)               /*!< Not use cmsis FLASH alias to avoid iar warning about volatile reading sequence */
#define FLASH_SALES_TYPE_Pos            (24U)
#define FLASH_SALES_TYPE                (0x3UL << FLASH_SALES_TYPE_Pos)     /*!< 0x000001E0 */
#define FLASH_SALES_TYPE_0              (0x1UL << FLASH_SALES_TYPE_Pos)     /*!< 0x01000000 */
#define FLASH_SALES_TYPE_1              (0x2UL << FLASH_SALES_TYPE_Pos)     /*!< 0x02000000 */
#define FLASH_SALES_VALUE               ((*((uint32_t *)PACKAGE_BASE)) & (FLASH_SALES_TYPE))
#define OB_DUAL_BANK_VALUE              ((*((uint32_t *)OB_DUAL_BANK_BASE)) & (FLASH_OPTR_DUAL_BANK))
#define FLASH_BANK_NB                   (((FLASH_SALES_VALUE == 0U)\
                                          || ((FLASH_SALES_VALUE == FLASH_SALES_TYPE_0) && (OB_DUAL_BANK_VALUE == 0U)))?1U:2U)
#define FLASH_BANK_SIZE                 ((FLASH_BANK_NB==1U)?(FLASH_SIZE):(FLASH_SIZE >> 1U)) /*!< FLASH Bank Size. Divided by 2 if 2 Banks */
#else /* FLASH_DBANK_SUPPORT */
#define FLASH_BANK_SIZE                 (FLASH_SIZE)   /*!< FLASH Bank Size */
#endif /* FLASH_DBANK_SUPPORT */

/** @defgroup FLASH_Type_Program FLASH Program Type
  * @{
  */
#define FLASH_TYPEPROGRAM_DOUBLEWORD    FLASH_CR_PG     /*!< Program a double-word (64-bit) at a specified address */
#define FLASH_TYPEPROGRAM_FAST          FLASH_CR_FSTPG  /*!< Fast program a 32 row double-word (64-bit) at a specified address */

/**
  * @brief  Clear the FLASH pending flags.
  * @param  __FLAG__ specifies the FLASH flags to clear.
  *         This parameter can be one of the following values :
  *     @arg @ref FLASH_FLAG_EOP FLASH End of Operation flag
  *     @arg @ref FLASH_FLAG_OPERR FLASH Operation error flag
  *     @arg @ref FLASH_FLAG_PROGERR FLASH Programming error flag
  *     @arg @ref FLASH_FLAG_WRPERR FLASH Write protection error flag
  *     @arg @ref FLASH_FLAG_PGAERR FLASH Programming alignment error flag
  *     @arg @ref FLASH_FLAG_SIZERR FLASH Size error flag
  *     @arg @ref FLASH_FLAG_PGSERR FLASH Programming sequence error flag
  *     @arg @ref FLASH_FLAG_MISERR FLASH Fast programming data miss error flag
  *     @arg @ref FLASH_FLAG_FASTERR FLASH Fast programming error flag
  *     @arg @ref FLASH_FLAG_RDERR FLASH PCROP read  error flag
  *     @arg @ref FLASH_FLAG_OPTVERR FLASH Option validity error flag
  *     @arg @ref FLASH_FLAG_ECCC1 FLASH one ECC error has been detected and corrected
  *     @arg @ref FLASH_FLAG_ECCD1 FLASH two ECC errors have been detected on bank 1
  *     @arg @ref FLASH_FLAG_ECCC2 FLASH one ECC error has been detected and corrected on bank 2(*)
  *     @arg @ref FLASH_FLAG_ECCD2 FLASH two ECC errors have been detected on bank 2(*)
  *     @arg @ref FLASH_FLAG_ECCC FLASH one ECC error has been detected and corrected - legacy name for single bank
  *     @arg @ref FLASH_FLAG_ECCD FLASH two ECC errors have been detected - legacy name for single bank
  * @note (*) availability depends on devices
  * @retval None
  */
#if defined(FLASH_DBANK_SUPPORT)
#define __HAL_FLASH_CLEAR_FLAG(__FLAG__)        do { if(((__FLAG__) & (FLASH_FLAG_SR_ID << FLASH_FLAG_REG_POS)) != 0U)         { FLASH->SR = (1uL << ((__FLAG__) & 0x1Fu)); }    \
                                                     else if(((__FLAG__) & (FLASH_FLAG_ECCR1_ID << FLASH_FLAG_REG_POS)) != 0U) { FLASH->ECCR = (1uL << ((__FLAG__) & 0x1Fu)); }  \
                                                     else if(((__FLAG__) & (FLASH_FLAG_ECCR2_ID << FLASH_FLAG_REG_POS)) != 0U) { FLASH->ECC2R = (1uL << ((__FLAG__) & 0x1Fu)); }  \
                                                   } while(0U)
#else
#define __HAL_FLASH_CLEAR_FLAG(__FLAG__)        do { if(((__FLAG__) & (FLASH_FLAG_SR_ID << FLASH_FLAG_REG_POS)) != 0U)         { FLASH->SR = (1uL << ((__FLAG__) & 0x1Fu)); }    \
                                                     else if(((__FLAG__) & (FLASH_FLAG_ECCR1_ID << FLASH_FLAG_REG_POS)) != 0U) { FLASH->ECCR = (1uL << ((__FLAG__) & 0x1Fu)); }  \
                                                   } while(0U)
#endif /* FLASH_DBANK_SUPPORT */

/** @defgroup FLASH_Banks FLASH Banks
  * @{
  */
#define FLASH_BANK_1                    FLASH_CR_MER1   /*!< Bank 1 */
#if defined(FLASH_DBANK_SUPPORT)
#define FLASH_BANK_2                    FLASH_CR_MER2   /*!< Bank 2 */
#endif /* FLASH_DBANK_SUPPORT */
#define FLASH_BANK_2_START               0x8040000


/* Internal defines for HAL macro usage */
#define FLASH_FLAG_REG_POS              16u
#define FLASH_FLAG_SR_ID                1u
#define FLASH_FLAG_CR_ID                2u
#define FLASH_FLAG_ECCR1_ID             4u
#define FLASH_FLAG_ECCR2_ID             8u

/** @defgroup FLASH_Flags FLASH Flags Definition
  * @{
  */
#define FLASH_FLAG_EOP                  ((FLASH_FLAG_SR_ID << FLASH_FLAG_REG_POS) | FLASH_SR_EOP_Pos)         /*!< FLASH End of operation flag */
#define FLASH_FLAG_OPERR                ((FLASH_FLAG_SR_ID << FLASH_FLAG_REG_POS) | FLASH_SR_OPERR_Pos)       /*!< FLASH Operation error flag */
#define FLASH_FLAG_PROGERR              ((FLASH_FLAG_SR_ID << FLASH_FLAG_REG_POS) | FLASH_SR_PROGERR_Pos)     /*!< FLASH Programming error flag */
#define FLASH_FLAG_WRPERR               ((FLASH_FLAG_SR_ID << FLASH_FLAG_REG_POS) | FLASH_SR_WRPERR_Pos)      /*!< FLASH Write protection error flag */
#define FLASH_FLAG_PGAERR               ((FLASH_FLAG_SR_ID << FLASH_FLAG_REG_POS) | FLASH_SR_PGAERR_Pos)      /*!< FLASH Programming alignment error flag */
#define FLASH_FLAG_SIZERR               ((FLASH_FLAG_SR_ID << FLASH_FLAG_REG_POS) | FLASH_SR_SIZERR_Pos)      /*!< FLASH Size error flag  */
#define FLASH_FLAG_PGSERR               ((FLASH_FLAG_SR_ID << FLASH_FLAG_REG_POS) | FLASH_SR_PGSERR_Pos)      /*!< FLASH Programming sequence error flag */
#define FLASH_FLAG_MISERR               ((FLASH_FLAG_SR_ID << FLASH_FLAG_REG_POS) | FLASH_SR_MISERR_Pos)      /*!< FLASH Fast programming data miss error flag */
#define FLASH_FLAG_FASTERR              ((FLASH_FLAG_SR_ID << FLASH_FLAG_REG_POS) | FLASH_SR_FASTERR_Pos)     /*!< FLASH Fast programming error flag */
#if defined(FLASH_PCROP_SUPPORT)
#define FLASH_FLAG_RDERR                ((FLASH_FLAG_SR_ID << FLASH_FLAG_REG_POS) | FLASH_SR_RDERR_Pos)       /*!< FLASH PCROP read error flag */
#endif /* FLASH_PCROP_SUPPORT */
#define FLASH_FLAG_OPTVERR              ((FLASH_FLAG_SR_ID << FLASH_FLAG_REG_POS) | FLASH_SR_OPTVERR_Pos)     /*!< FLASH Option validity error flag */
#define FLASH_FLAG_BSY1                 ((FLASH_FLAG_SR_ID << FLASH_FLAG_REG_POS) | FLASH_SR_BSY1_Pos)        /*!< FLASH Operation Busy flag for Bank 1 */
#if defined(FLASH_DBANK_SUPPORT)
#define FLASH_FLAG_BSY2                  ((FLASH_FLAG_SR_ID << FLASH_FLAG_REG_POS) | FLASH_SR_BSY2_Pos)       /*!< FLASH Operation Busy flag for Bank 2 */
#endif /* FLASH_DBANK_SUPPORT */
#define FLASH_FLAG_BSY                  FLASH_FLAG_BSY1                                                       /*!< FLASH Operation Busy flag - legacy name for single bank */
#define FLASH_FLAG_CFGBSY               ((FLASH_FLAG_SR_ID << FLASH_FLAG_REG_POS) | FLASH_SR_CFGBSY_Pos)      /*!< FLASH Configuration Busy flag */
#if defined(FLASH_DBANK_SUPPORT)
#define FLASH_FLAG_PESD                 ((FLASH_FLAG_SR_ID << FLASH_FLAG_REG_POS) | FLASH_SR_PESD_Pos)        /*!< FLASH Programming/erase operation suspended */
#endif /* FLASH_DBANK_SUPPORT */
#define FLASH_FLAG_ECCC1                ((FLASH_FLAG_ECCR1_ID << FLASH_FLAG_REG_POS) | FLASH_ECCR_ECCC_Pos)   /*!< FLASH ECC correction on bank 1 */
#define FLASH_FLAG_ECCD1                ((FLASH_FLAG_ECCR1_ID << FLASH_FLAG_REG_POS) | FLASH_ECCR_ECCD_Pos)   /*!< FLASH ECC detection on bank 1 */
#if defined(FLASH_DBANK_SUPPORT)
#define FLASH_FLAG_ECCC2                ((FLASH_FLAG_ECCR2_ID << FLASH_FLAG_REG_POS) | FLASH_ECC2R_ECCC_Pos)  /*!< FLASH ECC correction on bank 2 */
#define FLASH_FLAG_ECCD2                ((FLASH_FLAG_ECCR2_ID << FLASH_FLAG_REG_POS) | FLASH_ECC2R_ECCD_Pos)  /*!< FLASH ECC detection on bank 2 */
#endif /* FLASH_DBANK_SUPPORT */
#define FLASH_FLAG_ECCC                 FLASH_FLAG_ECCC1   /*!< FLASH ECC correction - legacy name for single bank */
#define FLASH_FLAG_ECCD                 FLASH_FLAG_ECCD1   /*!< FLASH ECC detection - legacy name for single bank */

#define FLASH_PAGE_SIZE                 0x00000800U    /*!< FLASH Page Size, 2 KBytes */
#define FLASH_PAGE_NB                   (FLASH_BANK_SIZE/FLASH_PAGE_SIZE) /* Number of pages per bank */
#define FLASH_TIMEOUT_VALUE             1000U          /*!< FLASH Execution Timeout, 1 s */
#define FLASH_TYPENONE                  0x00000000U    /*!< No programming Procedure On Going */
#define FLASH_END_ADDRESS					      0x0807FFFF  // 512K

#define TYPEPROGRAM_DOUBLEWORD        FLASH_TYPEPROGRAM_DOUBLEWORD

#if defined(FLASH_PCROP_SUPPORT)
#define FLASH_SR_ERRORS                 (FLASH_SR_OPERR  | FLASH_SR_PROGERR | FLASH_SR_WRPERR | \
                                         FLASH_SR_PGAERR | FLASH_SR_SIZERR  | FLASH_SR_PGSERR |  \
                                         FLASH_SR_MISERR | FLASH_SR_FASTERR | FLASH_SR_RDERR |   \
                                         FLASH_SR_OPTVERR) /*!< All SR error flags */
#else
#define FLASH_SR_ERRORS                 (FLASH_SR_OPERR  | FLASH_SR_PROGERR | FLASH_SR_WRPERR | \
                                         FLASH_SR_PGAERR | FLASH_SR_SIZERR  | FLASH_SR_PGSERR |  \
                                         FLASH_SR_MISERR | FLASH_SR_FASTERR |                    \
                                         FLASH_SR_OPTVERR) /*!< All SR error flags */
#endif /* FLASH_PCROP_SUPPORT */

#if defined(FLASH_DBANK_SUPPORT)
#define FLASH_SR_CLEAR                  (FLASH_SR_ERRORS | FLASH_SR_EOP | FLASH_SR_PESD)
#else
#define FLASH_SR_CLEAR                  (FLASH_SR_ERRORS | FLASH_SR_EOP)
#endif /* FLASH_DBANK_SUPPORT */

void FLASH_PageErase(uint32_t addr);
HAL_StatusTypeDef HAL_FLASH_Program(uint32_t TypeProgram, uint32_t Address, uint64_t Data);
HAL_StatusTypeDef FLASH_WaitForLastOperation(uint32_t Timeout);
HAL_StatusTypeDef HAL_FLASH_Lock(void);
HAL_StatusTypeDef HAL_FLASH_Unlock(void);

#endif /* __STM32G0XX_HAL_FLASH_H__ */