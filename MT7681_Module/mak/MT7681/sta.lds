/*******************************************************************************
 * MODULE NAME:     MT7681.lds
 * PROJECT CODE:    MT7681
 * DESCRIPTION:     Link Script for _____SIMULATION_____
 *                  No bootloader
 * DESIGNER:        Charles Su
 * DATE:            Aug 2011
 *
 * SOURCE CONTROL:
 *
 * LICENSE:
 *     This source code is copyright (c) 2011 Ralink Tech. Inc.
 *     All rights reserved.
 *
 * REVISION     HISTORY:
 *   V1.0.0     Aug 2011    - Initial Version V1.0
 *
 *
 * SOURCE:
 * ISSUES:
 *    First Implementation.
 * NOTES TO USERS:
 *
 *******************************************************************************/
INCLUDE mak/MT7681/rom_ref.sym
OUTPUT_FORMAT("elf32-nds32", "elf32-nds32", "elf32-nds32")
OUTPUT_ARCH(nds32)
ENTRY(_xcpt_vctr)

MEMORY
{
	/* ***************************************************** */
	/*       Ralink MT7681 Memory Mapping (Physical)        */
	/* ***************************************************** */
	/* SRAM  : 0x0000_0000 ~ 0x0000_FFFF (64KB)             */
	/* ROM   : 0x0010_2000 ~ 0x0010_3FFF (8KB)               */
	/* FLASH : 0x0050_0000 ~ 0x005F_FFFF (1MB = 8Mbits)               */
	/* ***************************************************** */

	SRAM(awx)	:	ORIGIN = 0x00000000,	LENGTH = 0x00010000
	ROM(arx)	:	ORIGIN = 0x00102000,	LENGTH = 0x00002000	
	FLASH(arx)	:	ORIGIN = 0x00500000,	LENGTH = 0x00100000
	FLASH_XIP_STA(arx) :	ORIGIN = 0x00530000,	LENGTH = 0x0000F000   /* 60KB*/
	FLASH_OVERLAY_STA(arx) :	ORIGIN = 0x0053F000,	LENGTH = 0x0000F000   /* 60KB*/
}

SECTIONS
{
	ROM_BASEADDR	= ORIGIN(ROM);
	ROM_SIZE		= LENGTH(ROM);
	PROVIDE(_ROM_BASEADDR = ROM_BASEADDR);
	PROVIDE(_ROM_SIZE = ROM_SIZE);

	RAM_BASEADDR	= ORIGIN(SRAM);
	RAM_SIZE		= LENGTH(SRAM);
	PROVIDE(_RAM_BASEADDR = RAM_BASEADDR);
	PROVIDE(_RAM_SIZE = RAM_SIZE);


	HALF_SRAM_SIZE	= LENGTH(SRAM)>>1;

	ILMBA	= ORIGIN(SRAM);
	DLMBA	= ORIGIN(SRAM) + HALF_SRAM_SIZE;
	ROMBA	= ORIGIN(ROM);
	FLASHBA	= ORIGIN(FLASH);
	PROVIDE(_ILMBA = ILMBA);
	PROVIDE(_DLMBA = DLMBA);

	STACK_ORIGIN = ORIGIN(SRAM) + LENGTH(SRAM) - 256;
	GLOBAL_ORIGIN = ILMBA;
	PROVIDE(_STACK_ORIGIN = STACK_ORIGIN);
	PROVIDE(_GLOBAL_ORIGIN = GLOBAL_ORIGIN);

	
        __ilm_start = .; 
	/* ***************************************************** */
	/*                   Exception Vector                    */
	/* ***************************************************** */
	/* Totally spend 64 bytes. This section is reserved for  */
	/* placing the exception vectors                         */
	/* ***************************************************** */
	VECT_SECT_VMA = RAM_BASEADDR;
	.VECT VECT_SECT_VMA :
	{
		KEEP(*(.vector))
	} > SRAM = 0


	/* ***************************************************** */
	/*                        .TEXT                          */
	/* ***************************************************** */
	/* .text :                                               */
	/*   Compiler- or assembler-generated instructions       */
	/* .text.* :                                             */
	/*   Functions when compiled with -ffunction-sections    */
	/*   are output to uniquely named sections of this form  */
	/* ***************************************************** */
	. = ALIGN(16);
	TEXT_SECT_VMA = .;
	.TEXT TEXT_SECT_VMA :
	{
		KEEP(*(.Xcpt))
		KEEP(*(.ctxsw))
		KEEP(*(.startup))
		*(.text)
		*(.text.*)
	} > SRAM = 0

	
	
	/* ***************************************************** */
	/*                        .STACK                         */
	/* ***************************************************** */
	/*                                                       */
	/* ***************************************************** */
	. = ALIGN(16);
	STACK_SECT_VMA = .;
	.STACK STACK_SECT_VMA :
	{
		PROVIDE(_STACK_START = .);
		KEEP (*(.stack))
		PROVIDE(_STACK_END = .);
	} > SRAM = 0



	/* ***************************************************** */
	/*                        .DATA                          */
	/* ***************************************************** */
	/* .rodata :                                             */
	/*   Strings and C data declared const                   */
	/* .rodata.* :                                           */
	/*   Constant data when compiled with -fdata-sections    */
	/*   are output to uniquely named sections of this form  */
	/* .rodata.strS.A :                                      */
	/*   Mergeable strings of size S and alignment A         */
	/* .rodata.cstA :                                        */
	/*   Mergeable constant data of alignment A              */
	/* ***************************************************** */
	. = ALIGN(16);
	DATA_SECT_VMA = .;
	.DATA DATA_SECT_VMA :
	{ 
		*(.rodata)
		*(.rodata.*)
		. = ALIGN(16);

		*(.data)
		*(.data.*)
	} > SRAM = 0

	. = ALIGN(4);
	FW_VERSION_TAG  :
	{
		*(FW_VERSION_TAG)
	} > SRAM = 0

	__tag_end = .;
	
	/********************************************************************************/
    /************************************ OVERLAY sections**************************/
    /********************************************************************************/
	
  	/* overlay ram0 */
    OVERLAY : NOCROSSREFS
    {
	  .ovlysec0
	  {             
		  /*KEEP(*(.ovlysec0))*/
	  }         

	  .ovlysec1     
	  {                    
		  /*KEEP(*(.ovlysec1))*/  
	  }
	  
	  .ovlysec2 
	  {             
		  /*KEEP(*(.ovlysec2))*/ 
	  }         

	  .ovlysec3     
	  {                    
		  /*KEEP(*(.ovlysec3))*/  
	  }
	  
	  .ovlysec4     
	  {                    
		  /*KEEP(*(.ovlysec4))*/  
	  }
	  
	  .ovlysec5     
	  {                    
		  /*KEEP(*(.ovlysec5))*/  
	  }
	  
	  .ovlysec6 
	  {             
		  /*KEEP(*(.ovlysec6))*/ 
	  }         

	  .ovlysec7     
	  {                    
		  /*KEEP(*(.ovlysec7))*/  
	  }
	  
	  
    } > SRAM AT> FLASH_OVERLAY_STA
	
	/* overlay ram1 */
    OVERLAY : NOCROSSREFS
    {
	  .ovlysec8 
	  {             
		  /*KEEP(*(.ovlysec8))*/ 
	  }         

	  .ovlysec9     
	  {                    
		  /*KEEP(*(.ovlysec9))*/  
	  }
	  
      .ovlysec10     
	  {                    
		  /*KEEP(*(.ovlysec10))*/  
	  }  
	  
	  .ovlysec11     
	  {                    
		  /*KEEP(*(.ovlysec11))*/  
	  }    
	  
    } > SRAM AT> FLASH_OVERLAY_STA
    
    /*********1.add more overlay ram/section begin ********/
		
		
    /*********1.add more overlay ram/section end ********/
	
	/**********************************************************/
	/**********************************************************/
	. = ALIGN(4);
    .nds32.ovly.tbl :
	{
		_ovly_table = .;
		
		/*****************ram0***************/
		LONG(ABSOLUTE(ADDR(.ovlysec0)));
		LONG(SIZEOF(.ovlysec0));
		LONG(ABSOLUTE(LOADADDR(.ovlysec0)));
		LONG(0);
		
		LONG(ABSOLUTE(ADDR(.ovlysec1)));
		LONG(SIZEOF(.ovlysec1));
		LONG(ABSOLUTE(LOADADDR(.ovlysec1)));
		LONG(0);
		
		
		LONG(ABSOLUTE(ADDR(.ovlysec2)));
		LONG(SIZEOF(.ovlysec2));
		LONG(ABSOLUTE(LOADADDR(.ovlysec2)));
		LONG(0);
		
		LONG(ABSOLUTE(ADDR(.ovlysec3)));
		LONG(SIZEOF(.ovlysec3));
		LONG(ABSOLUTE(LOADADDR(.ovlysec3)));
		LONG(0);
		
		LONG(ABSOLUTE(ADDR(.ovlysec4)));
		LONG(SIZEOF(.ovlysec4));
		LONG(ABSOLUTE(LOADADDR(.ovlysec4)));
		LONG(0);
		
		LONG(ABSOLUTE(ADDR(.ovlysec5)));
		LONG(SIZEOF(.ovlysec5));
		LONG(ABSOLUTE(LOADADDR(.ovlysec5)));
		LONG(0);	
		
		LONG(ABSOLUTE(ADDR(.ovlysec6)));
		LONG(SIZEOF(.ovlysec6));
		LONG(ABSOLUTE(LOADADDR(.ovlysec6)));
		LONG(0);
		
		LONG(ABSOLUTE(ADDR(.ovlysec7)));
		LONG(SIZEOF(.ovlysec7));
		LONG(ABSOLUTE(LOADADDR(.ovlysec7)));
		LONG(0);
		
		/*****************ram1***************/
		LONG(ABSOLUTE(ADDR(.ovlysec8)));
		LONG(SIZEOF(.ovlysec8));
		LONG(ABSOLUTE(LOADADDR(.ovlysec8)));
		LONG(1);
		
		LONG(ABSOLUTE(ADDR(.ovlysec9)));
		LONG(SIZEOF(.ovlysec9));
		LONG(ABSOLUTE(LOADADDR(.ovlysec9)));
		LONG(1);
		
		LONG(ABSOLUTE(ADDR(.ovlysec10)));
		LONG(SIZEOF(.ovlysec10));
		LONG(ABSOLUTE(LOADADDR(.ovlysec10)));
		LONG(1);
		
		LONG(ABSOLUTE(ADDR(.ovlysec11)));
		LONG(SIZEOF(.ovlysec11));
		LONG(ABSOLUTE(LOADADDR(.ovlysec11)));
		LONG(1);	

		
		/*********2.insert more overlay section to table begin ********/
		
		
		
		
		/*********2.insert more overlay section to table end  ********/	
		_novlys = .;
		LONG((_novlys - _ovly_table) / 16);
		. = ALIGN(8);
		/**********************************************************/
		/**********************************************************/
		_ovly_ram_table = .;
		LONG(0);
		LONG(0xFFFFFFFF);
		LONG(0);
		
		LONG(1);
		LONG(0xFFFFFFFF);
		LONG(0);
	    /*********3.insert more overlay ram to table begin  ********/
		
		
		
		/*********3.insert more overlay ram to table end  ********/
		_novlyr = .;
		LONG((_novlyr - _ovly_ram_table) / 12);
	}  > SRAM 
	
	/* ***************************************************** */
	/*                         .BSS                          */
	/* ***************************************************** */
	/* .sbss :                                               */
	/*   Uninitialised variables <=n bytes (compiled -Gn)    */
	/* .sbss.* :                                             */
	/*   Small uninitialised variables compiled with         */
	/*   -fdata-sections                                     */
	/* .bss :                                                */
	/*   Uninitialised larger variables                      */
	/* .bss.* :                                              */
	/*   Uninitialised variables compiled with               */
	/*   -fdata-sections                                     */
	/*                                            Charles Su */
	/* ***************************************************** */
	. = ALIGN(16);
	BSS_SECT_VMA = .;
	.BSS BSS_SECT_VMA : 
	{
		PROVIDE(_BSS_START = .);
		*(.sbss)
		*(.sbss.*)
		*(.scommon)
		*(.scommon.*)
		*(.sbss_w)
		*(.sbss_w.*)
		*(.sbss_h)
		*(.sbss_h.*)
		*(.sbss_b)
		*(.sbss_b.*)

		. = ALIGN(16);
		*(.bss)
		*(.bss.*)
		*(COMMON)
		. = ALIGN(4);
		PROVIDE(_BSS_END = .);
	} > SRAM = 0

	__ilm_end = .;

	/* top of free memory */
	. = ALIGN(32);
	_end = .;
	PROVIDE(_MEMORY_TOP = .);

 /* . = DATA_SEGMENT_END (.); */
  /* Stabs debugging sections.  */
  .stab          0 : { *(.stab) }
  .stabstr       0 : { *(.stabstr) }
  .stab.excl     0 : { *(.stab.excl) }
  .stab.exclstr  0 : { *(.stab.exclstr) }
  .stab.index    0 : { *(.stab.index) }
  .stab.indexstr 0 : { *(.stab.indexstr) }
  .comment       0 : { *(.comment) }
  /* DWARF debug sections.
     Symbols in the DWARF debugging sections are relative to the beginning
     of the section so we begin them at 0.  */
  /* DWARF 1 */
  .debug          0 : { *(.debug) }
  .line           0 : { *(.line) }
  /* GNU DWARF 1 extensions */
  .debug_srcinfo  0 : { *(.debug_srcinfo) }
  .debug_sfnames  0 : { *(.debug_sfnames) }
  /* DWARF 1.1 and DWARF 2 */
  .debug_aranges  0 : { *(.debug_aranges) }
  .debug_pubnames 0 : { *(.debug_pubnames) }
  /* DWARF 2 */
  .debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
  .debug_abbrev   0 : { *(.debug_abbrev) }
  .debug_line     0 : { *(.debug_line) }
  .debug_frame    0 : { *(.debug_frame) }
  .debug_str      0 : { *(.debug_str) }
  .debug_loc      0 : { *(.debug_loc) }
  .debug_macinfo  0 : { *(.debug_macinfo) }
  /* SGI/MIPS DWARF 2 extensions */
  .debug_weaknames 0 : { *(.debug_weaknames) }
  .debug_funcnames 0 : { *(.debug_funcnames) }
  .debug_typenames 0 : { *(.debug_typenames) }
  .debug_varnames  0 : { *(.debug_varnames) }
  /* PROVIDE (_stack = 0x13000000); */
  /*PROVIDE (_stack = (_DLM_STACK)); */
  /*PROVIDE (_stack = 0x02000000); */
  /DISCARD/ : { *(.note.GNU-stack) *(.preboot) *(.boot) } 
  
 
	
    /********************************************************************************/
    /***************************** XIP section ,book 3 sections**********************/
    /********************************************************************************/
	.xipsec0 : 
	{             
		/*KEEP(*(.xipsec0))*/ 
	} > FLASH_XIP_STA         

	.xipsec1 :       
	{                    
		/*KEEP(*(.xipsec1))*/  
	} > FLASH_XIP_STA     
	
	.xipsec2 : 
	{             
		/*KEEP(*(.xipsec2))*/ 
	} > FLASH_XIP_STA         
   
    /********************************************************************************/
    /********************************************************************************/	
	

	
	
}
