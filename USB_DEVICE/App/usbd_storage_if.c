/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : usbd_storage_if.c
  * @version        : v1.0_Cube
  * @brief          : Memory management layer.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "usbd_storage_if.h"

/* USER CODE BEGIN INCLUDE */

/* USER CODE END INCLUDE */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @brief Usb device.
  * @{
  */

/** @defgroup USBD_STORAGE
  * @brief Usb mass storage device module
  * @{
  */

/** @defgroup USBD_STORAGE_Private_TypesDefinitions
  * @brief Private types.
  * @{
  */

/* USER CODE BEGIN PRIVATE_TYPES */

/* USER CODE END PRIVATE_TYPES */

/**
  * @}
  */

/** @defgroup USBD_STORAGE_Private_Defines
  * @brief Private defines.
  * @{
  */

#define STORAGE_LUN_NBR                  1
#define STORAGE_BLK_NBR                  0x10000
#define STORAGE_BLK_SIZ                  0x200

/* USER CODE BEGIN PRIVATE_DEFINES */
#undef STORAGE_BLK_NBR
#define STORAGE_BLK_NBR                  (W25Qxx_FlashSize / STORAGE_BLK_SIZ)

#define STORAGE_ERASE_BLK_SIZ            W25Qxx_EraseSectorSize
#define STORAGE_BLK_PER_ERASE            (STORAGE_ERASE_BLK_SIZ / STORAGE_BLK_SIZ)
/* USER CODE END PRIVATE_DEFINES */

/**
  * @}
  */

/** @defgroup USBD_STORAGE_Private_Macros
  * @brief Private macros.
  * @{
  */

/* USER CODE BEGIN PRIVATE_MACRO */

/* USER CODE END PRIVATE_MACRO */

/**
  * @}
  */

/** @defgroup USBD_STORAGE_Private_Variables
  * @brief Private variables.
  * @{
  */

/* USER CODE BEGIN INQUIRY_DATA_HS */
/** USB Mass storage Standard Inquiry Data. */
const int8_t STORAGE_Inquirydata_HS[] = {/* 36 */

  /* LUN 0 */
  0x00,
  0x80,
  0x02,
  0x02,
  (STANDARD_INQUIRY_DATA_LEN - 5),
  0x00,
  0x00,
  0x00,
  'S', 'T', 'M', ' ', ' ', ' ', ' ', ' ', /* Manufacturer : 8 bytes */
  'P', 'r', 'o', 'd', 'u', 'c', 't', ' ', /* Product      : 16 Bytes */
  ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
  '0', '.', '0' ,'1'                      /* Version      : 4 Bytes */
};
/* USER CODE END INQUIRY_DATA_HS */

/* USER CODE BEGIN PRIVATE_VARIABLES */
static uint8_t storage_ready = 0U;
static uint8_t storage_erase_cache[STORAGE_ERASE_BLK_SIZ];

/* USER CODE END PRIVATE_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_STORAGE_Exported_Variables
  * @brief Public variables.
  * @{
  */

extern USBD_HandleTypeDef hUsbDeviceHS;

/* USER CODE BEGIN EXPORTED_VARIABLES */

/* USER CODE END EXPORTED_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_STORAGE_Private_FunctionPrototypes
  * @brief Private functions declaration.
  * @{
  */

static int8_t STORAGE_Init_HS(uint8_t lun);
static int8_t STORAGE_GetCapacity_HS(uint8_t lun, uint32_t *block_num, uint16_t *block_size);
static int8_t STORAGE_IsReady_HS(uint8_t lun);
static int8_t STORAGE_IsWriteProtected_HS(uint8_t lun);
static int8_t STORAGE_Read_HS(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len);
static int8_t STORAGE_Write_HS(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len);
static int8_t STORAGE_GetMaxLun_HS(void);

/* USER CODE BEGIN PRIVATE_FUNCTIONS_DECLARATION */

/* USER CODE END PRIVATE_FUNCTIONS_DECLARATION */

/**
  * @}
  */

USBD_StorageTypeDef USBD_Storage_Interface_fops_HS =
{
  STORAGE_Init_HS,
  STORAGE_GetCapacity_HS,
  STORAGE_IsReady_HS,
  STORAGE_IsWriteProtected_HS,
  STORAGE_Read_HS,
  STORAGE_Write_HS,
  STORAGE_GetMaxLun_HS,
  (int8_t *)STORAGE_Inquirydata_HS
};

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initializes the storage unit (medium).
  * @param  lun: Logical unit number.
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
int8_t STORAGE_Init_HS(uint8_t lun)
{
  /* USER CODE BEGIN 9 */
  if (lun >= STORAGE_LUN_NBR)
  {
    return (USBD_FAIL);
  }

  if (OSPI_W25Qxx_Init() == OSPI_W25Qxx_OK)
  {
    storage_ready = 1U;
    return (USBD_OK);
  }

  storage_ready = 0U;
  return (USBD_FAIL);
  /* USER CODE END 9 */
}

/**
  * @brief  Returns the medium capacity.
  * @param  lun: Logical unit number.
  * @param  block_num: Number of total block number.
  * @param  block_size: Block size.
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
int8_t STORAGE_GetCapacity_HS(uint8_t lun, uint32_t *block_num, uint16_t *block_size)
{
  /* USER CODE BEGIN 10 */
  if ((lun >= STORAGE_LUN_NBR) || (block_num == NULL) || (block_size == NULL))
  {
    return (USBD_FAIL);
  }

  *block_num  = STORAGE_BLK_NBR;
  *block_size = (uint16_t)STORAGE_BLK_SIZ;
  return (USBD_OK);
  /* USER CODE END 10 */
}

/**
  * @brief   Checks whether the medium is ready.
  * @param  lun:  Logical unit number.
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
int8_t STORAGE_IsReady_HS(uint8_t lun)
{
  /* USER CODE BEGIN 11 */
  if (lun >= STORAGE_LUN_NBR)
  {
    return (USBD_FAIL);
  }

  return (storage_ready != 0U) ? USBD_OK : USBD_FAIL;
  /* USER CODE END 11 */
}

/**
  * @brief  Checks whether the medium is write protected.
  * @param  lun: Logical unit number.
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
int8_t STORAGE_IsWriteProtected_HS(uint8_t lun)
{
  /* USER CODE BEGIN 12 */
  UNUSED(lun);
  return (USBD_OK);
  /* USER CODE END 12 */
}

/**
  * @brief  Reads data from the medium.
  * @param  lun: Logical unit number.
  * @param  buf: data buffer.
  * @param  blk_addr: Logical block address.
  * @param  blk_len: Blocks number.
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
int8_t STORAGE_Read_HS(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len)
{
  /* USER CODE BEGIN 13 */
  uint32_t start_addr;
  uint32_t read_size;

  if ((lun >= STORAGE_LUN_NBR) || (buf == NULL) || (blk_len == 0U))
  {
    return (USBD_FAIL);
  }

  if ((storage_ready == 0U) || (blk_addr >= STORAGE_BLK_NBR) || ((blk_addr + blk_len) > STORAGE_BLK_NBR))
  {
    return (USBD_FAIL);
  }

  start_addr = blk_addr * STORAGE_BLK_SIZ;
  read_size = ((uint32_t)blk_len) * STORAGE_BLK_SIZ;

  if (OSPI_W25Qxx_ReadBuffer(buf, start_addr, read_size) != OSPI_W25Qxx_OK)
  {
    return (USBD_FAIL);
  }

  return (USBD_OK);
  /* USER CODE END 13 */
}

/**
  * @brief  Writes data into the medium.
  * @param  lun: Logical unit number.
  * @param  buf: data buffer.
  * @param  blk_addr: Logical block address.
  * @param  blk_len: Blocks number.
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
int8_t STORAGE_Write_HS(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len)
{
  /* USER CODE BEGIN 14 */
  uint32_t processed_blocks;

  if ((lun >= STORAGE_LUN_NBR) || (buf == NULL) || (blk_len == 0U))
  {
    return (USBD_FAIL);
  }

  if ((storage_ready == 0U) || (blk_addr >= STORAGE_BLK_NBR) || ((blk_addr + blk_len) > STORAGE_BLK_NBR))
  {
    return (USBD_FAIL);
  }

  processed_blocks = 0U;
  while (processed_blocks < blk_len)
  {
    uint32_t current_lba;
    uint32_t erase_first_lba;
    uint32_t erase_end_lba;
    uint32_t blocks_in_erase;
    uint32_t copy_idx;
    uint32_t erase_base_addr;
    uint32_t offset_in_erase;

    current_lba = blk_addr + processed_blocks;
    erase_first_lba = (current_lba / STORAGE_BLK_PER_ERASE) * STORAGE_BLK_PER_ERASE;
    erase_end_lba = erase_first_lba + STORAGE_BLK_PER_ERASE;

    blocks_in_erase = (uint32_t)blk_len - processed_blocks;
    if ((current_lba + blocks_in_erase) > erase_end_lba)
    {
      blocks_in_erase = erase_end_lba - current_lba;
    }

    erase_base_addr = erase_first_lba * STORAGE_BLK_SIZ;
    offset_in_erase = (current_lba - erase_first_lba) * STORAGE_BLK_SIZ;

    if (OSPI_W25Qxx_ReadBuffer(storage_erase_cache, erase_base_addr, STORAGE_ERASE_BLK_SIZ) != OSPI_W25Qxx_OK)
    {
      return (USBD_FAIL);
    }

    for (copy_idx = 0U; copy_idx < blocks_in_erase; copy_idx++)
    {
      memcpy(&storage_erase_cache[offset_in_erase + copy_idx * STORAGE_BLK_SIZ],
             &buf[(processed_blocks + copy_idx) * STORAGE_BLK_SIZ],
             STORAGE_BLK_SIZ);
    }

    if (OSPI_W25Qxx_SectorErase(erase_base_addr) != OSPI_W25Qxx_OK)
    {
      return (USBD_FAIL);
    }

    if (OSPI_W25Qxx_WriteBuffer(storage_erase_cache, erase_base_addr, STORAGE_ERASE_BLK_SIZ) != OSPI_W25Qxx_OK)
    {
      return (USBD_FAIL);
    }

    processed_blocks += blocks_in_erase;
  }

  return (USBD_OK);
  /* USER CODE END 14 */
}

/**
  * @brief  Returns the Max Supported LUNs.
  * @param  None
  * @retval Lun(s) number.
  */
int8_t STORAGE_GetMaxLun_HS(void)
{
  /* USER CODE BEGIN 15 */
  return (STORAGE_LUN_NBR - 1);
  /* USER CODE END 15 */
}

/* USER CODE BEGIN PRIVATE_FUNCTIONS_IMPLEMENTATION */

/* USER CODE END PRIVATE_FUNCTIONS_IMPLEMENTATION */

/**
  * @}
  */

/**
  * @}
  */

