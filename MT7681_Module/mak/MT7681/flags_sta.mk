#*******************************************************************************
#* MODULE NAME:     flags.mak
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
# BUILD FLAGS
#*******************************************************************************
BFLAGS	 =
#			Uncomment this flag if try to build ROM code
BFLAGS	+=	-DROM_EXISTED=1
#			Support single antenna auto-adaption.
BFLAGS	+=	-DSINGLE_ANTENNA=1
#			Including $P0 and $P1 in CPU context
BFLAGS	+=	-DCTX_SUP_P0P1=0
#			Including $FP in CPU context
BFLAGS	+=	-DCTX_SUP_FP=1
#			Including $GP in CPU context
BFLAGS	+=	-DCTX_SUP_GP=1
#			Including $d0 and $d1 in CPU context
BFLAGS	+=	-DCTX_SUP_D0D1=0
#			Reset Bluetooth clock during booting
BFLAGS	+=	-DRST_CLK=1
#			Enable ILM
BFLAGS	+=	-DILM_ENA=0
#			Enable DLM
BFLAGS	+=	-DDLM_ENA=0
BFLAGS	+=	-DBUILD_LE
#			Reduced Register Setting
BFLAGS	+=	-DREDUCED_REG=1

#*******************************************************************************
# COMPILER FLAGS
#*******************************************************************************
CFLAGS	 =
#			Enable most warning messages
CFLAGS  +=	-g
CFLAGS	+=	-Wall
CFLAGS	+=	-Wextra
CFLAGS	+=	-Wcast-align
CFLAGS	+=	-Wcast-qual
CFLAGS	+=	-Wconversion
CFLAGS	+=	-fno-builtin
CFLAGS	+=	-fno-strict-aliasing

#*******************************************************************************
# TARGET FLAGS
#*******************************************************************************
TFLAGS	 =
#	Generate 16/32-bit mixed instructions
TFLAGS	+=	-m16bit
#	Not generate 16/32-bit mixed instructions
#TFLAGS	+=	-mno-16bit
# Specify which ABI type to generate code for: 1, 2, and 2fp. The default is 2.
TFLAGS	+=	-mabi=2
# Specify which baseline type to generate code for: V1, V2. The default is V2.
TFLAGS	+=	-mbaseline=V2                 
# Specify which CPU to generate code for: n1213_43u1h, n1213_30t2g, n12, n10, n9. The default is n10.
TFLAGS	+=	-mcpu=n9
#	Generate integer div instructions using register $d0/$d1
#TFLAGS	+=	-mdiv
#	Not generate integer div instructions using register $d0/$d1
TFLAGS	+=	-mno-div
#	Use little-endian byte order
TFLAGS	+=	-mel
# Specify a max count of moving multiple words from 4 to 8. The default is 4.
TFLAGS	+=	-mmw-count=8
#	Not use 16 registers to generate code
#TFLAGS	+=	-mno-reduce-regs
#	Use 16 registers to generate code
#TFLAGS +=	-mreduce-regs
# Not generate multiply with accumulation instructions using register $d0/$d1	
TFLAGS +=	-mno-ext-mac
#	Not generate instructions relative to dx registers
TFLAGS +=	-mno-dx-regs



#*******************************************************************************
# ASM FLAGS
#*******************************************************************************
ASFLAGS	 =



#*******************************************************************************
# OPTIMIZATION FLAGS
#*******************************************************************************
OFLAGS	 =
#			Optimize for space rather than speed
#           -Os enable all -O2 optimizations
OFLAGS	+=	-Os
#OFLAGS	+=	-O3
OFLAGS	+=  -finline-limit=9
OFLAGS	+=	-ffunction-sections
OFLAGS	+=	-fdata-sections

#*******************************************************************************
# MAKE LIB FLAGS
#*******************************************************************************
ARFLAGS	 =
#ARFLAGS += r



#*******************************************************************************
# LD FLAGS
#*******************************************************************************
LFLAGS	 =
LFLAGS	+=	-EL
LFLAGS	+=	-g
LFLAGS	+=	-Bstatic
LFLAGS	+=	--check-sections
LFLAGS	+=	--gc-sections
LFLAGS	+= -nostartfiles
#LFLAGS	+= -relax 


#*******************************************************************************
# objcopy FLAGS 
#*******************************************************************************
COPY_SRAM = 
COPY_XIP = 
COPY_OVERLAY = 
#############################################
# objcopy FLAGS for firmware run in SRAM
COPY_SRAM += -R .xipsec0
COPY_SRAM += -R .xipsec1
COPY_SRAM += -R .xipsec2

# objcopy FLAGS for firmware run in XIP flash
COPY_XIP += -j .xipsec0
COPY_XIP += -j .xipsec1
COPY_XIP += -j .xipsec2

#############################################
# objcopy FLAGS for Overlay 
COPY_SRAM += -R .ovlysec0
COPY_SRAM += -R .ovlysec1
COPY_SRAM += -R .ovlysec2
COPY_SRAM += -R .ovlysec3
COPY_SRAM += -R .ovlysec4
COPY_SRAM += -R .ovlysec5
COPY_SRAM += -R .ovlysec6
COPY_SRAM += -R .ovlysec7
COPY_SRAM += -R .ovlysec8
COPY_SRAM += -R .ovlysec9
COPY_SRAM += -R .ovlysec10
COPY_SRAM += -R .ovlysec11





COPY_OVERLAY += -j .ovlysec0
COPY_OVERLAY += -j .ovlysec1
COPY_OVERLAY += -j .ovlysec2
COPY_OVERLAY += -j .ovlysec3
COPY_OVERLAY += -j .ovlysec4
COPY_OVERLAY += -j .ovlysec5
COPY_OVERLAY += -j .ovlysec6
COPY_OVERLAY += -j .ovlysec7
COPY_OVERLAY += -j .ovlysec8
COPY_OVERLAY += -j .ovlysec9
COPY_OVERLAY += -j .ovlysec10
COPY_OVERLAY += -j .ovlysec11







#*******************************************************************************
# DEBUG FLAGS
#*******************************************************************************
DBGFLAGS	 =
DBGFLAGS	+=	-DDBG_ENA=1
DBGFLAGS    +=  -DDBG_TRAP=0
DBGFLAGS	+=	-DDBG_ASSERT=0
DBGFLAGS	+=	-DSIM_RTL=0
DBGFLAGS	+=	-DSIM_ADS=0
DBGFLAGS	+=	-DDBG_FPGA=1
DBGFLAGS	+=	-DDBG_MICTOR=0


#*******************************************************************************
# FUNCTION FLAGS
#*******************************************************************************

# BSP SETTING
FUNCFLAGS	=	-D__MT7681
FUNCFLAGS	+=	-DUART_SUPPORT=1
FUNCFLAGS	+=	-DUART_INTERRUPT=1          #0,UART polling ;1,UART interrupt,we prefer to use UART interrupt 
FUNCFLAGS	+=	-DMT7681_POWER_SAVING=1     #STA power saving mode as Specification said

# WIFI/BT FUNCTIONS 
FUNCFLAGS	+=	-DBT_SUPPORT=0
FUNCFLAGS	+=	-DWIFI_SUPPORT=1
FUNCFLAGS	+=	-DUSB_SUPPORT=0
FUNCFLAGS	+=	-DSCRIPT_EXE_SUPPORT=0
# WIN FEATURES FLAGS
FUNCFLAGS	+=	-DRADAR_DETEC=0
FUNCFLAGS	+=	-DRESP_WOWPKT=0
FUNCFLAGS	+=	-DWOW_SUPPORT=0
FUNCFLAGS	+=	-DNLO_SUPPORT=0
FUNCFLAGS	+=	-DBITMAP_PATT=0
FUNCFLAGS	+=	-DARP_OFFLOAD=0
FUNCFLAGS	+=	-DCOL_SUPPORT=0
FUNCFLAGS	+=	-DWINDOWS8=0
FUNCFLAGS	+=	-DADV_PWR_SAVING=0
FUNCFLAGS	+=	-DDOT11W_PMF_SUPPORT=0

# DEBUG MESSAGE FLAGS
FUNCFLAGS	+=	-DDBG_MSG=0
FUNCFLAGS	+=	-DDBG_MSG_HIGH=1
FUNCFLAGS	+=	-DNO_USED_CODE_REMOVE=1
FUNCFLAGS	+=	-DCFG_RANDOM_MAC=0

# AT CMD FLAGS
FUNCFLAGS	+=	-DATCMD_SUPPORT=1
FUNCFLAGS	+=	-DATCMD_TCPIP_SUPPORT=0
FUNCFLAGS	+=	-DATCMD_UART_SUPPORT=0
FUNCFLAGS	+=	-DATCMD_UART_FW_SUPPORT=0     #Only available while ATCMD_RECOVERY_SUPPORT=1
FUNCFLAGS	+=	-DATCMD_CH_SWITCH_SUPPORT=0
FUNCFLAGS	+=	-DATCMD_NET_MODE_SUPPORT=0
FUNCFLAGS	+=	-DATCMD_REBOOT_SUPPORT=1
FUNCFLAGS	+=	-DATCMD_GET_VER_SUPPORT=0
FUNCFLAGS	+=	-DATCMD_SET_SMNT_SUPPORT=1
FUNCFLAGS	+=	-DATCMD_PS_SUPPORT=0

FUNCFLAGS	+=	-DATCMD_RECOVERY_SUPPORT=0    #Only available in flags_recovery.mk
FUNCFLAGS	+=	-DATCMD_ATE_SUPPORT=0         #Only available in flags_recovery.mk
FUNCFLAGS	+=	-DATCMD_EFUSE_SUPPORT=0
FUNCFLAGS	+=	-DATCMD_FLASH_SUPPORT=1
FUNCFLAGS	+=	-DATCMD_JTAGMODE_SUPPORT=0    #not support


# DATA PARSER FLAGS
FUNCFLAGS	+=	-DIOT_PWM_SUPPORT=1
FUNCFLAGS	+=	-DAES_DATAPARSING_SUPPORT=0
FUNCFLAGS	+=	-DENABLE_DATAPARSING_SEQUENCE_MGMT=0
#----UARTRX_TO_AIR_LEVEL
#----1,handle uart rx data normaly,and send uart rx data which left in ring to air by peer query;
#----2,do not handle uart rx data,only send uart rx data to air automatically
FUNCFLAGS	+=	-DUARTRX_TO_AIR_LEVEL=1

# FEATURE FLAGS
FUNCFLAGS	+=	-DXIP_ENABLE=1              #xip_ovly.h
FUNCFLAGS	+=	-DOVERLAY_ENABLE=1          #xip_ovly.h
FUNCFLAGS	+=	-DEP_CFM=1
FUNCFLAGS	+=	-DEP_LOAD_SUPPORT=1
FUNCFLAGS	+=	-DRX_CLASS123=0
FUNCFLAGS	+=	-DCRYPTWEP_SUPPORT=1
FUNCFLAGS	+=	-DCFG_SUPPORT_4WAY_HS=1
FUNCFLAGS	+=	-DCFG_SUPPORT_TCPIP=1
FUNCFLAGS	+=	-DCFG_SUPPORT_DNS=0
FUNCFLAGS	+=	-DCFG_SUPPORT_MTK_SMNT=1
FUNCFLAGS	+=	-DHW_TIMER1_SUPPORT=1
#END-OF-FILE#
