/* USER CODE BEGIN Header */
/**
 ******************************************************************************
  * @file    user_diskio.c
  * @brief   This file includes a diskio driver skeleton to be completed by the user.
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

#ifdef USE_OBSOLETE_USER_CODE_SECTION_0
/*
 * Warning: the user section 0 is no more in use (starting from CubeMx version 4.16.0)
 * To be suppressed in the future.
 * Kept to ensure backward compatibility with previous CubeMx versions when
 * migrating projects.
 * User code previously added there should be copied in the new user sections before
 * the section contents can be deleted.
 */
/* USER CODE BEGIN 0 */
/* USER CODE END 0 */
#endif

/* USER CODE BEGIN DECL */

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "ff_gen_drv.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define USER_SECTOR_SIZE         _MIN_SS
#define USER_FLASH_SIZE          W25Qxx_FlashSize
#define USER_ERASE_BLOCK_SIZE    W25Qxx_EraseSectorSize
#define USER_SECTOR_COUNT        (USER_FLASH_SIZE / USER_SECTOR_SIZE)
#define USER_BLOCK_SECTOR_COUNT  (USER_ERASE_BLOCK_SIZE / USER_SECTOR_SIZE)

#if (_MIN_SS > _MAX_SS)
#error "FatFs sector size configuration is invalid"
#endif

#if ((W25Qxx_FlashSize % _MIN_SS) != 0)
#error "W25Q64 size must be aligned to the logical FatFs sector size"
#endif

#if ((USER_ERASE_BLOCK_SIZE % _MIN_SS) != 0)
#error "W25Q64 erase size must be an integer multiple of the logical FatFs sector size"
#endif

/* Private variables ---------------------------------------------------------*/
/* Disk status */
static volatile DSTATUS Stat = STA_NOINIT;
static uint8_t erase_block_cache[USER_ERASE_BLOCK_SIZE];

/* USER CODE END DECL */

/* Private function prototypes -----------------------------------------------*/
DSTATUS USER_initialize (BYTE pdrv);
DSTATUS USER_status (BYTE pdrv);
DRESULT USER_read (BYTE pdrv, BYTE *buff, DWORD sector, UINT count);
#if _USE_WRITE == 1
  DRESULT USER_write (BYTE pdrv, const BYTE *buff, DWORD sector, UINT count);
#endif /* _USE_WRITE == 1 */
#if _USE_IOCTL == 1
  DRESULT USER_ioctl (BYTE pdrv, BYTE cmd, void *buff);
#endif /* _USE_IOCTL == 1 */

Diskio_drvTypeDef  USER_Driver =
{
  USER_initialize,
  USER_status,
  USER_read,
#if  _USE_WRITE
  USER_write,
#endif  /* _USE_WRITE == 1 */
#if  _USE_IOCTL == 1
  USER_ioctl,
#endif /* _USE_IOCTL == 1 */
};

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initializes a Drive
  * @param  pdrv: Physical drive number (0..)
  * @retval DSTATUS: Operation status
  */
DSTATUS USER_initialize (
	BYTE pdrv           /* Physical drive nmuber to identify the drive */
)
{
  /* USER CODE BEGIN INIT */
    if (pdrv != 0U)
    {
      return STA_NOINIT;
    }

    if (OSPI_W25Qxx_Init() == OSPI_W25Qxx_OK)
    {
      Stat &= (DSTATUS)~STA_NOINIT;
    }
    else
    {
      Stat |= STA_NOINIT;
    }

    return Stat;
  /* USER CODE END INIT */
}

/**
  * @brief  Gets Disk Status
  * @param  pdrv: Physical drive number (0..)
  * @retval DSTATUS: Operation status
  */
DSTATUS USER_status (
	BYTE pdrv       /* Physical drive number to identify the drive */
)
{
  /* USER CODE BEGIN STATUS */
    if (pdrv != 0U)
    {
      return STA_NOINIT;
    }

    return Stat;
  /* USER CODE END STATUS */
}

/**
  * @brief  Reads Sector(s)
  * @param  pdrv: Physical drive number (0..)
  * @param  *buff: Data buffer to store read data
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to read (1..128)
  * @retval DRESULT: Operation result
  */
DRESULT USER_read (
	BYTE pdrv,      /* Physical drive nmuber to identify the drive */
	BYTE *buff,     /* Data buffer to store read data */
	DWORD sector,   /* Sector address in LBA */
	UINT count      /* Number of sectors to read */
)
{
  /* USER CODE BEGIN READ */
    uint32_t read_addr;
    uint32_t read_size;

    if ((pdrv != 0U) || (count == 0U) || (buff == NULL))
    {
      return RES_PARERR;
    }

    if ((sector >= USER_SECTOR_COUNT) || ((sector + count) > USER_SECTOR_COUNT))
    {
      return RES_PARERR;
    }

    if ((Stat & STA_NOINIT) != 0U)
    {
      return RES_NOTRDY;
    }

    read_addr = (uint32_t)sector * USER_SECTOR_SIZE;
    read_size = (uint32_t)count * USER_SECTOR_SIZE;

    if (OSPI_W25Qxx_ReadBuffer(buff, read_addr, read_size) != OSPI_W25Qxx_OK)
    {
      return RES_ERROR;
    }

    return RES_OK;
  /* USER CODE END READ */
}

/**
  * @brief  Writes Sector(s)
  * @param  pdrv: Physical drive number (0..)
  * @param  *buff: Data to be written
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to write (1..128)
  * @retval DRESULT: Operation result
  */
#if _USE_WRITE == 1
DRESULT USER_write (
	BYTE pdrv,          /* Physical drive nmuber to identify the drive */
	const BYTE *buff,   /* Data to be written */
	DWORD sector,       /* Sector address in LBA */
	UINT count          /* Number of sectors to write */
)
{
  /* USER CODE BEGIN WRITE */
    uint32_t processed_sectors;

    if ((pdrv != 0U) || (count == 0U) || (buff == NULL))
    {
      return RES_PARERR;
    }

    if ((sector >= USER_SECTOR_COUNT) || ((sector + count) > USER_SECTOR_COUNT))
    {
      return RES_PARERR;
    }

    if ((Stat & STA_NOINIT) != 0U)
    {
      return RES_NOTRDY;
    }

    processed_sectors = 0U;
    while (processed_sectors < count)
    {
      uint32_t current_lba;
      uint32_t block_first_lba;
      uint32_t block_end_lba;
      uint32_t sectors_in_block;
      uint32_t copy_index;
      uint32_t block_base_addr;
      uint32_t offset_in_block;

      current_lba = (uint32_t)sector + processed_sectors;
      block_first_lba = (current_lba / USER_BLOCK_SECTOR_COUNT) * USER_BLOCK_SECTOR_COUNT;
      block_end_lba = block_first_lba + USER_BLOCK_SECTOR_COUNT;
      sectors_in_block = (uint32_t)count - processed_sectors;
      if ((current_lba + sectors_in_block) > block_end_lba)
      {
        sectors_in_block = block_end_lba - current_lba;
      }

      block_base_addr = block_first_lba * USER_SECTOR_SIZE;
      offset_in_block = (current_lba - block_first_lba) * USER_SECTOR_SIZE;

      if (OSPI_W25Qxx_ReadBuffer(erase_block_cache, block_base_addr, USER_ERASE_BLOCK_SIZE) != OSPI_W25Qxx_OK)
      {
        return RES_ERROR;
      }

      for (copy_index = 0U; copy_index < sectors_in_block; copy_index++)
      {
        memcpy(&erase_block_cache[offset_in_block + (copy_index * USER_SECTOR_SIZE)],
               &buff[(processed_sectors + copy_index) * USER_SECTOR_SIZE],
               USER_SECTOR_SIZE);
      }

      if (OSPI_W25Qxx_SectorErase(block_base_addr) != OSPI_W25Qxx_OK)
      {
        return RES_ERROR;
      }

      if (OSPI_W25Qxx_WriteBuffer(erase_block_cache, block_base_addr, USER_ERASE_BLOCK_SIZE) != OSPI_W25Qxx_OK)
      {
        return RES_ERROR;
      }

      processed_sectors += sectors_in_block;
    }

    return RES_OK;
  /* USER CODE END WRITE */
}
#endif /* _USE_WRITE == 1 */

/**
  * @brief  I/O control operation
  * @param  pdrv: Physical drive number (0..)
  * @param  cmd: Control code
  * @param  *buff: Buffer to send/receive control data
  * @retval DRESULT: Operation result
  */
#if _USE_IOCTL == 1
DRESULT USER_ioctl (
	BYTE pdrv,      /* Physical drive nmuber (0..) */
	BYTE cmd,       /* Control code */
	void *buff      /* Buffer to send/receive control data */
)
{
  /* USER CODE BEGIN IOCTL */
    DRESULT res;

    if (pdrv != 0U)
    {
      return RES_PARERR;
    }

    if ((Stat & STA_NOINIT) != 0U)
    {
      return RES_NOTRDY;
    }

    res = RES_OK;
    switch (cmd)
    {
      case CTRL_SYNC:
        break;

      case GET_SECTOR_COUNT:
        if (buff == NULL)
        {
          return RES_PARERR;
        }
        *(DWORD *)buff = (DWORD)USER_SECTOR_COUNT;
        break;

      case GET_SECTOR_SIZE:
        if (buff == NULL)
        {
          return RES_PARERR;
        }
        *(WORD *)buff = (WORD)USER_SECTOR_SIZE;
        break;

      case GET_BLOCK_SIZE:
        if (buff == NULL)
        {
          return RES_PARERR;
        }
        *(DWORD *)buff = (DWORD)USER_BLOCK_SECTOR_COUNT;
        break;

      default:
        res = RES_PARERR;
        break;
    }

    return res;
  /* USER CODE END IOCTL */
}
#endif /* _USE_IOCTL == 1 */

