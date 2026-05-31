/*-----------------------------------------------------------------------*/
/* Low level disk I/O module for FatFs                                   */
/*-----------------------------------------------------------------------*/

#include "diskio.h"
#include "sdcard.h"
#include <string.h>

#define SD_DRIVE        0U
#define SD_SECTOR_SIZE  512U

static DSTATUS g_sd_status = STA_NOINIT;
static uint32_t g_sector_buffer[SD_SECTOR_SIZE / 4U];

DSTATUS disk_initialize(BYTE drv)
{
    if (drv != SD_DRIVE) {
        return STA_NOINIT;
    }

    if (sd_init() == SD_OK) {
        g_sd_status = 0;
    } else {
        g_sd_status = STA_NOINIT | STA_NODISK;
    }

    return g_sd_status;
}

DSTATUS disk_status(BYTE drv)
{
    if (drv != SD_DRIVE) {
        return STA_NOINIT;
    }

    return g_sd_status;
}

DRESULT disk_read(BYTE drv, BYTE *buff, DWORD sector, BYTE count)
{
    BYTE i;
    sd_error_enum status;

    if ((drv != SD_DRIVE) || (buff == 0) || (count == 0U)) {
        return RES_PARERR;
    }

    if (g_sd_status & STA_NOINIT) {
        return RES_NOTRDY;
    }

    for (i = 0U; i < count; i++) {
        status = sd_block_read(g_sector_buffer,
                               (uint32_t)(sector + i) * SD_SECTOR_SIZE,
                               SD_SECTOR_SIZE);
        if (status != SD_OK) {
            return RES_ERROR;
        }
        memcpy(buff + ((uint32_t)i * SD_SECTOR_SIZE), g_sector_buffer, SD_SECTOR_SIZE);
    }

    return RES_OK;
}

#if _READONLY == 0
DRESULT disk_write(BYTE drv, const BYTE *buff, DWORD sector, BYTE count)
{
    BYTE i;
    sd_error_enum status;

    if ((drv != SD_DRIVE) || (buff == 0) || (count == 0U)) {
        return RES_PARERR;
    }

    if (g_sd_status & STA_NOINIT) {
        return RES_NOTRDY;
    }

    for (i = 0U; i < count; i++) {
        memcpy(g_sector_buffer, buff + ((uint32_t)i * SD_SECTOR_SIZE), SD_SECTOR_SIZE);
        status = sd_block_write(g_sector_buffer,
                                (uint32_t)(sector + i) * SD_SECTOR_SIZE,
                                SD_SECTOR_SIZE);
        if (status != SD_OK) {
            return RES_ERROR;
        }
    }

    return RES_OK;
}
#endif

DRESULT disk_ioctl(BYTE drv, BYTE ctrl, void *buff)
{
    if (drv != SD_DRIVE) {
        return RES_PARERR;
    }

    if (g_sd_status & STA_NOINIT) {
        return RES_NOTRDY;
    }

    switch (ctrl) {
    case CTRL_SYNC:
        return RES_OK;

    case GET_SECTOR_SIZE:
        if (buff == 0) {
            return RES_PARERR;
        }
        *(WORD *)buff = SD_SECTOR_SIZE;
        return RES_OK;

    case GET_SECTOR_COUNT:
        if (buff == 0) {
            return RES_PARERR;
        }
        *(DWORD *)buff = (DWORD)((sd_card_capacity_get() * 1024UL) / SD_SECTOR_SIZE);
        return RES_OK;

    case GET_BLOCK_SIZE:
        if (buff == 0) {
            return RES_PARERR;
        }
        *(DWORD *)buff = 1UL;
        return RES_OK;

    default:
        return RES_PARERR;
    }
}
