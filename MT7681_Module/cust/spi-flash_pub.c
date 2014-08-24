#include <config.h>
#include <types.h>
#include <stdio.h>
#include "flash_map.h"
//#include "pdma.h"
//#include "misc.h"
#include "iot_api.h"
#include "eeprom.h"


/* used for crc32 */
#define REFLECT_DATA(X)         ((uint8) reflect((X), 8))
#define INITIAL_REMAINDER       0xFFFFFFFF
#define WIDTH                   (8 * sizeof(uint32))
#define TOPBIT                  (1 << (WIDTH - 1))
#define POLYNOMIAL              0x04C11DB7
#define REFLECT_REMAINDER(X)    ((uint32) reflect((X), WIDTH))
#define FINAL_XOR_VALUE         0xFFFFFFFF


extern IOT_ADAPTER	 IoTpAd;

uint8 dump_spi_flash(UINT32 start, UINT32 end);

uint32 reflect(uint32 data, uint8 nBits)
{
    UINT32 reflection = 0x00000000;
    UINT8  bit;

    for (bit = 0; bit < nBits; ++bit)
    {
        if (data & 0x01)
        {
            reflection |= (1 << ((nBits - 1) - bit));
        }
        data = (data >> 1);
    }
    return (reflection);
}

uint32 crc32(uint8 *msg, int32 len)
{
    UINT32 remainder = INITIAL_REMAINDER;
    INT32 byte;
    UINT8 bit;

    for (byte = 0; byte < len; byte++)
    {
        remainder ^= (REFLECT_DATA(msg[byte]) << (WIDTH - 8));
        for (bit = 8; bit > 0; bit--)
        {
            if (remainder & TOPBIT)
            {
                remainder = (remainder << 1) ^ POLYNOMIAL;
            }
            else
            {
                remainder = (remainder << 1);
            }
        }
    }
    return (REFLECT_REMAINDER(remainder) ^ FINAL_XOR_VALUE);
}


uint8 spi_flash_update_fw_done(uint8 type)
{
	Printf_High("\nUpdate Region[%d] Successful, please Reboot !!\n", type);
	return 0;
}

uint8 spi_flash_update_fw(uint8 type, uint32 offset, uint8 *pdata, uint16 len)
{
	uint16 maxSector = 0;       /*stored the max sector number  for the Update FW region in Flash*/
	uint16 sid = 0;
	uint32 crc1, crc2;
	uint32 RegionOffset = 0;    /**/
	//uint32 i;

	if ((len > sizeof(IoTpAd.flash_rw_buf)) || (!pdata))
		return 2;


	if(type == UART_FlASH_UPG_ID_LOADER)      /*upgrade to loader Block*/
	{
		RegionOffset = FLASH_OFFSET_LOADER_START;
		maxSector    = FLASH_LOADER_SIZE/FLASH_SECTOR_SIZE;
	}
	else if(type == UART_FlASH_UPG_ID_RECOVERY_FW) /*upgrade to Recovery Mode Block*/
	{
		RegionOffset = FLASH_OFFSET_UPG_FW_START;
		maxSector    = FLASH_UPG_FW_SIZE/FLASH_SECTOR_SIZE;
	}
	else if(type == UART_FlASH_UPG_ID_STA_FW) /*upgrade to STA Firmware Block*/
	{
		RegionOffset = FLASH_OFFSET_STA_FW_START;
		maxSector    = FLASH_STA_FW_SIZE/FLASH_SECTOR_SIZE;
	}
	else if(type == UART_FlASH_UPG_ID_STA_XIP_FW) /*upgrade to STA-XIP Firmware Block*/
	{
		RegionOffset = FLASH_OFFSET_STA_XIP_FW_START;
		maxSector    = FLASH_STA_XIP_FW_SIZE/FLASH_SECTOR_SIZE;
	}
	else if(type == UART_FlASH_UPG_ID_AP_FW) /*upgrade to AP Firmware Block*/
	{
		RegionOffset = FLASH_OFFSET_AP_FW_START;
		maxSector    = FLASH_AP_FW_SIZE/FLASH_SECTOR_SIZE;
	}
	else if(type == UART_FlASH_UPG_ID_AP_XIP_FW) /*upgrade to AP-XIP Firmware Block*/
	{
		RegionOffset = FLASH_OFFSET_AP_XIP_FW_START;
		maxSector    = FLASH_AP_XIP_FW_SIZE/FLASH_SECTOR_SIZE;
	}
	else
	{   /*Invalid Type*/
		return 1;
	}
	
	crc1 = crc32(pdata, len);
	//for(i=0; i<len; i++)
	//  printf("0x%x ",pdata[i]);
	printf("\nCRC1=0x%08x\n",crc1);

	if (offset == 0)
	{
		/*If  FLASH_OFFSET_UPG_FW_START  is not begining position of  a block
		   there is a high risk to erase a whole block, if call spi_flash_erase_BE() here!
		   thus,  use spi_flash_erase_SE() to instead */
		for(sid=0; sid < maxSector; sid++)
			spi_flash_erase_SE(RegionOffset + sid*FLASH_SECTOR_SIZE);
	}
	spi_flash_write_func(RegionOffset+offset, pdata, len);

	usecDelay(300);    //jinchuan.bao , if not delay 300us, spi_flash_read()  will fail !
	
	spi_flash_read(RegionOffset+offset, IoTpAd.flash_rw_buf, len);
	crc2 = crc32(IoTpAd.flash_rw_buf, len);

	//for(i=0; i<len; i++)
	//  printf("0x%x ",IoTpAd.flash_rw_buf[i]);
	printf("CRC2=0x%08x\n",crc2);

	if (crc1 != crc2)
		return 1;
	
	printf("CRC is correct\n");
	return 0;
}

int32 spi_flash_write(uint32 addr, uint8 *data, uint16 len)
{
	UINT32  sec_start, sec_end;				/*indicate sector range*/
	UINT32  rid_addr_start, rid_addr_end;	/*indicate the flash address will be writed from current Ram RW buf*/
 	UINT32	sid=0, rid=0;                   /*indicate sector index and ram buf index */
	UINT16	cplen=0 ;
	UINT8   *pVaule = data;
	
	UINT8   pRw_buf[RAM_RW_BUF_SIZE]={0xff};

	/* because flash write buffer size's limitation,  the len of writing data should be limited */
	if ((len == 0) || (len > FLASH_RW_BUF_SIZE))
		return -1;
	
	if (pVaule == NULL) 
		return -1;

	/*  find the sector range for the writing data */
	sec_start = addr / FLASH_SECTOR_SIZE;
	sec_end   = (addr+len-1) / FLASH_SECTOR_SIZE;

	for(sid=sec_start; sid<=sec_end; sid++)
	{
		/* erase flash write buffer , default size is one sector [4KB] */
		spi_flash_erase_SE(FLASH_OFFSET_WRITE_BUF_START);
		
		/*  Move flash target sector  --> Ram RW buf --> flash write buffer  */
		/*  will move many times, because Ram RW buf's limitation */
		for (rid=0; rid < (FLASH_SECTOR_SIZE / RAM_RW_BUF_SIZE); rid++)
		{
			spi_flash_read((sid*FLASH_SECTOR_SIZE + RAM_RW_BUF_SIZE*rid), 
				            pRw_buf, 
				            RAM_RW_BUF_SIZE);

			usecDelay(300);    //jinchuan.bao , if not delay 300us, spi_flash_read()  will fail !

			spi_flash_write_func((FLASH_OFFSET_WRITE_BUF_START + RAM_RW_BUF_SIZE*rid),  
				            pRw_buf,  
				            RAM_RW_BUF_SIZE);
		}
		
		/* erase flash target sector , default [4KB] */
		spi_flash_erase_SE(sid*FLASH_SECTOR_SIZE);
		

		/*  Move flash write buffer --> Ram RW buf -->  flash target sector */
		for (rid=0; rid < (FLASH_SECTOR_SIZE / RAM_RW_BUF_SIZE); rid++)
		{
			rid_addr_start= ((sid*FLASH_SECTOR_SIZE) + (rid*RAM_RW_BUF_SIZE));
			rid_addr_end  = rid_addr_start + RAM_RW_BUF_SIZE;
			
			spi_flash_read((FLASH_OFFSET_WRITE_BUF_START + (RAM_RW_BUF_SIZE*rid)), 
							pRw_buf, 
							RAM_RW_BUF_SIZE);
			
		
			/* judge if merge the writing data to RAM RW buffer,  and write to flash target sector*/
			if ((addr >= rid_addr_start) && 
				(addr <  rid_addr_end) &&
				(len > 0))
			{
				/* Case1: if the remain length is more than RAM RW Buf size*/
				/* Case2: if the remain length is equal RAM RW Buf size*/
				/* Case3: if the remain length is less than RAM RW Buf size*/
				cplen = (len>RAM_RW_BUF_SIZE)?(RAM_RW_BUF_SIZE):len;
				memcpy(pRw_buf+ (addr - rid_addr_start), pVaule , cplen);

				pVaule += cplen;
				addr += cplen;
				len  -= cplen;
			}
			
			usecDelay(300);    //jinchuan.bao , if not delay 300us, spi_flash_read()  will fail !

			spi_flash_write_func(rid_addr_start, pRw_buf,  RAM_RW_BUF_SIZE); 
		}
	}
	
	return 0;

}

/*TYPE=1  dump stationFW region*/
/*TYPE=2  dump UpgradeFW region*/
uint8 dump_spi_flash_fw(UINT8 TYPE)
{
	uint32 rid, i;
	uint32 offset = FLASH_OFFSET_STA_FW_START;
	uint16 maxRamBlock = 0; 
	UINT8  pRw_buf[RAM_RW_BUF_SIZE]={0xff};

	/*  move data from UpgradeFW region  to STAFW region*/
	/*  will move many times, because Ram RW buf's limitation */
	
	if (TYPE == 1)
	{
		offset = FLASH_OFFSET_STA_FW_START;
		maxRamBlock = FLASH_STA_FW_SIZE/sizeof(pRw_buf);
	}
	else if (TYPE == 2)
	{	
		offset = FLASH_OFFSET_UPG_FW_START;
		maxRamBlock = FLASH_UPG_FW_SIZE/sizeof(pRw_buf);
	}
	else 
		return 1;
	
	for (rid=0; rid < maxRamBlock; rid++)
	{
		spi_flash_read(offset, pRw_buf, sizeof(pRw_buf));

		for(i=0; i<sizeof(pRw_buf); i++)
		{   
			if(i%16 ==0)
				Printf_High("\n Offset[0x%08x] : ",offset+i);
			
			Printf_High("0x%02x ", pRw_buf[i]);
		}
		Printf_High("\n");

		offset += sizeof(pRw_buf);
	}
	
	return 0;
}



uint8 dump_spi_flash(UINT32 start, UINT32 end)
{
	uint32 rid, i, rest;
	uint32 offset = start;
	uint16 maxRamBlock = 0; 
	UINT8  pRw_buf[RAM_RW_BUF_SIZE]={0xff};

	if(end < start)
		return;
	
	/*  move data from UpgradeFW region  to STAFW region*/
	/*  will move many times, because Ram RW buf's limitation */

	maxRamBlock = (end - start)/sizeof(pRw_buf);
	rest = ((end - start)%sizeof(pRw_buf));
	
	for (rid=0; rid < maxRamBlock; rid++)
	{
		spi_flash_read(offset, pRw_buf, sizeof(pRw_buf));

		for(i=0; i<sizeof(pRw_buf); i++)
		{   
			if(i%16 ==0)
				Printf_High("\n Offset[0x%08x] : ",offset+i);
			
			Printf_High("0x%02x ", pRw_buf[i]);
		}
		Printf_High("\n");

		offset += sizeof(pRw_buf);
	}

	
	if (rest > 0);
	{
		spi_flash_read(offset, pRw_buf, rest);
	
		for(i=0; i<rest; i++)
		{	
			if(i%16 ==0)
				Printf_High("\n Offset[0x%08x] : ",offset+i);
			
			Printf_High("0x%02x ", pRw_buf[i]);
		}
		Printf_High("\n");
	}
	
	return 0;
}

