#include <config.h>
#include <types.h>
#include <stdio.h>
#include "flash_map.h"
//#include "pdma.h"
//#include "misc.h"
#include "iot_api.h"
#include "eeprom.h"

/************************************************/
/* Macros */
/************************************************/
#define ENABLE_SPI_PUB_ERASE 1
#define ENABLE_SPI_PUB__RW	 1


/* SPI Command Mode Index: */
#define PUB_SPICMD_WR_BYTE			(0x00)
#define PUB_SPICMD_WR_LASTBYTE		(0x04)
#define PUB_SPICMD_WORD_BE			(0x08)
#define PUB_SPICMD_LASTWORD_BE		(0x0c)
#define PUB_SPICMD_WORD_LE			(0x10)
#define PUB_SPICMD_LASTWORD_LE		(0x14)
#define PUB_SPICMD_RD_BYTE			(0x18)
#define PUB_SPICMD_RD_LASTBYTE		(0x1c)


//Serial flash command mode:Instruction opcode(1byte) + Address(A23~A0,3bytes) + Data...
//Instruction opcode of Serial flash spec
#define WRSR		0x01       //write status register
#define PP			0x02       //page program
#define READ		0x03       //read data
#define RDSR		0x05       //read status register
#define WREN		0x06       //write enable
#define BE			0x52       //block erase
#define CE			0x60       //chip erase
#define SE			0x20       //sector erase


/* used for crc32 */
#define REFLECT_DATA(X)         ((uint8) reflect((X), 8))
#define INITIAL_REMAINDER       0xFFFFFFFF
#define WIDTH                   (8 * sizeof(uint32))
#define TOPBIT                  (1 << (WIDTH - 1))
#define POLYNOMIAL              0x04C11DB7
#define REFLECT_REMAINDER(X)    ((uint32) reflect((X), WIDTH))
#define FINAL_XOR_VALUE         0xFFFFFFFF


extern IOT_ADAPTER	 IoTpAd;

/************************************************/
/* Extern Function */
/************************************************/
/*=====================================
	Routine Description:  Read/Write SPI CR
	Arguments:Offset: SPI CR index  [0/4/8/..../0x1C]
		          Val:     Write value to SPI CR
	Return Value:  NONE
======================================*/
extern inline VOID SPI_REG8(UINT8 Offset, UINT8 Val);

/*=====================================
	Routine Description: SPI command mode read data
	Arguments:
	Return Value:  if Ope=0,  Return 0
======================================*/
extern inline UINT32 SPI_READOUT(VOID);

/*=====================================
	Routine Description: Wait the flash controller free
	Arguments:
	Return Value: 
======================================*/
extern inline VOID WAIT_OP_DONE_PUB(VOID);

extern uint8 dump_spi_flash(UINT32 start, UINT32 end);




/************************************************/
/* Function */
/************************************************/
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


#if ENABLE_SPI_PUB_ERASE

/*************************************************
*spi_flash_erase_CE
*************************************************/
void spi_flash_erase_CE(void)
{
    //printf("spi_flash_erase_CE start\n");
    UINT8 SR;
	
	SPI_REG8(PUB_SPICMD_WR_LASTBYTE, WREN);
	WAIT_OP_DONE_PUB();
	SPI_REG8(PUB_SPICMD_WR_LASTBYTE, CE);

    /* Check if operation done */
    do
    {
		SPI_REG8(PUB_SPICMD_WR_BYTE, RDSR);        //read status register
		SPI_REG8(PUB_SPICMD_RD_LASTBYTE, 0x00);    //read the last byte
		SR = (uint8)(SPI_READOUT() >> 24);            //status value
    } while(SR&0x01);                                 //write in progress bit(whether in write status/program/erase cycle) 
	//printf("spi_flash_erase_CE end\n");
}

/*************************************************
*spi_flash_erase_SE
*************************************************/
void spi_flash_erase_SE(uint32 address)
{
    UINT8   SR;
    //printf("spi_flash_erase_SE start\n");
    
    //send spi command:write enable
	SPI_REG8(PUB_SPICMD_WR_LASTBYTE, WREN);
	WAIT_OP_DONE_PUB();
	//printf("send spi cmd\n");
	
	//send spi command:erase a sector
	SPI_REG8(PUB_SPICMD_WR_BYTE, SE);
	SPI_REG8(PUB_SPICMD_WR_BYTE, (uint8)((address>>16)&0xff));
	SPI_REG8(PUB_SPICMD_WR_BYTE, (uint8)((address>>8)&0xff));
	SPI_REG8(PUB_SPICMD_WR_LASTBYTE, (uint8)(address&0xff));
	
    /* Check if operation done */
    do
    {
		SPI_REG8(PUB_SPICMD_WR_BYTE, RDSR);
		SPI_REG8(PUB_SPICMD_RD_LASTBYTE, 0x00);
		SR = (uint8)(SPI_READOUT() >> 24);
    }
    while (SR & 0x01);       //write in progress bit(whether in write status/program/erase cycle) 

    //printf("spi_flash_erase_SE done\n");
}

/**********************************************
*spi_flash_erase_BE
**********************************************/
void spi_flash_erase_BE(uint32 address)
{
	UINT8	SR;
	//uint32	c1;
	//uint32	counter = 0;
	
	//printf("spi_flash_erase_BE start\n");
	SPI_REG8(PUB_SPICMD_WR_LASTBYTE, WREN);
	WAIT_OP_DONE_PUB();
	
	//printf("send spi cmd\n");
	SPI_REG8(PUB_SPICMD_WR_BYTE, BE);
	SPI_REG8(PUB_SPICMD_WR_BYTE, (uint8)((address>>16)&0xff));
	SPI_REG8(PUB_SPICMD_WR_BYTE, (uint8)((address>>8)&0xff));
	SPI_REG8(PUB_SPICMD_WR_LASTBYTE, (uint8)(address&0xff));

	/* Check if operation done */
	do
	{
		SPI_REG8(PUB_SPICMD_WR_BYTE, RDSR);
		SPI_REG8(PUB_SPICMD_RD_LASTBYTE, 0x00);
		SR = (uint8)(SPI_READOUT() >> 24);
	}
	while (SR & 0x01);

	//printf("spi_flash_erase_BE done\n");
}
#endif


#if ENABLE_SPI_PUB__RW
/**********************************************
* spi_flash_read_m2()  ,   
* read the flash data ,just same as spi_flash_read()
* this function can be extended to support other flash cmds
**********************************************/
int32 spi_flash_read_m2(uint32 addr, uint8 *data, uint16 len)
{
	UINT16 idx;

	WAIT_OP_DONE_PUB();
	
	//send read data command
	SPI_REG8(PUB_SPICMD_WR_BYTE, READ);
	SPI_REG8(PUB_SPICMD_WR_BYTE, ((addr>>16)&0xFF));
	SPI_REG8(PUB_SPICMD_WR_BYTE, ((addr>>8)&0xFF));
	SPI_REG8(PUB_SPICMD_WR_BYTE, ((addr>>0)&0xFF));
	
    for(idx=0; idx<len; idx++)
    {
		if ((idx+1) == len)
		{
			SPI_REG8(PUB_SPICMD_RD_LASTBYTE, 0x00);  //read the last byte
		}
		else
		{
			SPI_REG8(PUB_SPICMD_RD_BYTE, 0x00);  //read the last byte
		}
	 	data[idx] = (uint8)(SPI_READOUT() >> 24);
    }
	
    return 0;
}


/**********************************************
*spi_flash_page_write
**********************************************/
static void spi_flash_page_write(uint32 addr, uint8 *data, uint16 len)
{
    //send write enable command
	SPI_REG8(PUB_SPICMD_WR_LASTBYTE, WREN);
    WAIT_OP_DONE_PUB();
	
    //send page program command
	SPI_REG8(PUB_SPICMD_WR_BYTE, PP);
	SPI_REG8(PUB_SPICMD_WR_BYTE, ((addr>>16)&0xFF));
	SPI_REG8(PUB_SPICMD_WR_BYTE, ((addr>>8)&0xFF));
	SPI_REG8(PUB_SPICMD_WR_BYTE, ((addr>>0)&0xFF));
	
    //write at most 256 bytes(that is one page) one time based on serial flash spec
    len = (len>256)? 256: len;
    for( ; len>0; len--)
    {
		if (len == 1)
		{
			SPI_REG8(PUB_SPICMD_WR_LASTBYTE, *data++);  //write last byte
		}
		else
		{
			SPI_REG8(PUB_SPICMD_WR_BYTE, *data++);  //write 1 byte
		}
    }
}


/**********************************************
*spi_flash_write_func
**********************************************/
int32 spi_flash_write_func(uint32 addr, uint8 *data, uint16 len)
{
	UINT8 SR;
	printf("spi_flash_write_func_pub\n");

	for( ; len>=256; len-=256)
	{
		spi_flash_page_write(addr, data, 256);
		addr += 256;
		data += 256;

		/* Check if operation done */
		do
		{
			SPI_REG8(PUB_SPICMD_WR_BYTE, RDSR);
			SPI_REG8(PUB_SPICMD_RD_LASTBYTE, 0x00);
			SR = (uint8)(SPI_READOUT() >> 24);
		} while(SR&0x01);
	}
	
	if (len > 0)
	{
		spi_flash_page_write(addr, data, len);
	}
	return 0;
}
#endif

/**********************************************
*spi_flash_update_fw_done
**********************************************/
uint8 spi_flash_update_fw_done(uint8 type)
{
	Printf_High("\nUpdate Region[%d] Successful, please Reboot !!\n", type);
	return 0;
}

/**********************************************
*spi_flash_update_fw
**********************************************/
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

/**********************************************
*spi_flash_write
**********************************************/
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


/**********************************************
*dump_spi_flash_fw
**********************************************/
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



/**********************************************
*dump_spi_flash
**********************************************/
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

