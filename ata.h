#ifndef INCLUDE_ATA_H
#define INCLUDE_ATA_H

#include "io.h"
#define ATA_PIO_DATA_PORT 0x1F0
#define ATA_PIO_ERR_PORT 0x1F1
#define ATA_PIO_FEATURES_PORT 0x1F1
#define ATA_PIO_SCTR_COUNT_PORT 0x1F2
#define ATA_PIO_LBA_LOW_PORT 0x1F3
#define ATA_PIO_LBA_MID_PORT 0x1F4
#define ATA_PIO_LBA_HI_PORT 0x1F5
#define ATA_PIO_DRIVE_NUM_PORT 0x1F6
#define ATA_PIO_STATAUS_PORT 0x1F7
#define ATA_PIO_COMMAND_PORT 0x1F7
#define ATA_PIO_DEV_CONTROL_PORT 0x3F6
#define ATA_PIO_DEV_ADDR_PORT 0x3F7
#define ATA_PIO_AMNF inb(ATA_PIO_ERR_PORT) & 1
#define ATA_PIO_TKZNF inb(ATA_PIO_ERR_PORT) & (1 << 1)
#define ATA_PIO_ABRT inb(ATA_PIO_ERR_PORT) & (1 << 2)
#define ATA_PIO_MCR inb(ATA_PIO_ERR_PORT) & (1 << 3)
#define ATA_PIO_IDNF inb(ATA_PIO_ERR_PORT) & (1 << 4)
#define ATA_PIO_MC inb(ATA_PIO_ERR_PORT) & (1 << 5)
#define ATA_PIO_UNC inb(ATA_PIO_ERR_PORT) & (1 << 6)
#define ATA_PIO_BBK inb(ATA_PIO_ERR_PORT) & (1 << 7)
#define ATA_PIO_READ_SCTR_CMD 0X20
#define ATA_PIO_WRITE_SCTR_CMD 0X30

void ata_identify();

unsigned short *ata_read_sector(unsigned long long sector_num);

void ata_write_sector(unsigned long long sector_num, unsigned short *sector_contect);

#endif