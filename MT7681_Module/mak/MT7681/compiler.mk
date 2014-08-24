#*******************************************************************************
#* MODULE NAME:     compiler.mak
#* PROJECT CODE:
#* DESCRIPTION:
#* DESIGNER:        Charles Su
#* DATE:            Aug 2011
#*
#* SOURCE CONTROL:
#*
#* LICENSE:
#*     This source code is copyright (c) 2011 Ralink Tech. Inc.
#*     All rights reserved.
#*
#* REVISION     HISTORY:
#*   V1.0.0     Aug 2011    - Initial Version V1.0
#*
#*
#* SOURCE:
#* ISSUES:
#*    First Implementation.
#* NOTES TO USERS:
#*
#*******************************************************************************



#*******************************************************************************
# TOOLCHAINS DEFINE
#*******************************************************************************
#For Linux TS
TOOLCHAINS	=	/mtktools/Andestech/BSPv320/toolchains/nds32le-elf-newlib-v2j/bin

#For Windows TS
#TOOLCHAINS	=	/cygdrive/c/Andestech/BSPv310/toolchains/nds32le-elf-newlib-v2j/bin

CROSS		=	$(TOOLCHAINS)/nds32le-elf

#*******************************************************************************
# COMPILER DEFINE
#*******************************************************************************
#			C Compiler
CC			=	$(CROSS)-gcc -c -nostdinc
MAKE_DEPEND	=	$(CC) -M

#			Preprocessor
CPP			=	$(CROSS)-gcc -E -nostdinc

#			Assembler
AS			=	$(CROSS)-gcc -c -nostdinc

#			Archiver
AR 			=	$(CROSS)-ar

#			Linker
LD			=	$(CROSS)-ld

#			Strip
STRIP		=	$(CROSS)-strip -S


OBJDUMP		=	$(CROSS)-objdump
OBJCOPY 	=	$(CROSS)-objcopy
NM			=	$(CROSS)-nm
READELF		=	$(CROSS)-readelf
SIZE		=	$(CROSS)-size

#END-OF-FILE#
