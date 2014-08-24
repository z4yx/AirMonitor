#*******************************************************************************
#* MODULE NAME:     makefile
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
#*   V1.0.1     MAR 2012    - Add compile dependence system
#*                          - Add description of project folder structure
#*                          - Add error log in build.log
#*
#*
#* PROJECT FOLDER STRUCTURE:
#* -- project 
#*            -- dat
#*            -- mak : make file rules and utilities
#*            -- obj : compiled obj and depends
#*            -- out : linked elf and binary(image)
#*            -- src : source codes
#*            -- patch : patch codes
#* 
#* SOURCE:
#*
#* ISSUES:
#*    First Implementation.
#* NOTES TO USERS:
#*
#*
#*******************************************************************************



#################################################################
# Include Build Spec
#      command argument priority :
#      cmd > file > user define
#################################################################
include buildspec.mak



#################################################################
# Quick Build Option
#    v - Production Version
#    r - Release Version
#       0: Recovery Firmware
#       1: STA Firmware
#       2: AP Firmware
#       example: "make b=2"  -->build AP Firmware   
#    o - radiO option
#    e - Chip ECO version
#################################################################



#*******************************************************************************
# DEFINE PROJECT
#*******************************************************************************
PRODUCT_DEF				=	MT$(v)
CHIPECO_DEF				=	MT$(v)_E$(e)

PRODUCT_VERSION			=	$(v)
ECO_VERSION				=	$(e)
RELEASE_VERSION			=	$(r)
BT_RADIO_VERSION		=	$(o)
WL_RADIO_VERSION		=	$(o)
BUILD_TYPE				=	$(b)
PACKAGE_BUILD			=	$(PKG)

#*******************************************************************************
# FILE INPORT
#*******************************************************************************
include	mak/$(PRODUCT_DEF)/compiler.mk
include	mak/$(PRODUCT_DEF)/tools.mk

#*******************************************************************************
# DEFINE SOURCE, OUTPUT AND INCLUDE PATH
#*******************************************************************************
SRC_PATH			=	src
API_PATH			=	cust
DAT_PATH			=	dat
INCLUDE_PATH		=	src

ifeq ($(BUILD_TYPE),0)
OPTION = recovery
include	mak/$(PRODUCT_DEF)/flags_recovery.mk
LDS_FILE			=	mak/$(PRODUCT_DEF)/recovery.lds
BIN_HEADER_TYPE         = 2
BIN_HEADER_XIP_TYPE     = 2
LIB_CUSTOM              = andesrecovery
else
ifeq ($(BUILD_TYPE),1)
OPTION = sta
include	mak/$(PRODUCT_DEF)/flags_sta.mk
LDS_FILE			=	mak/$(PRODUCT_DEF)/sta.lds
BIN_HEADER_TYPE         = 4
BIN_HEADER_XIP_TYPE     = 5
LIB_CUSTOM              = andessta
else
ifeq ($(BUILD_TYPE),2)
OPTION = ap
include	mak/$(PRODUCT_DEF)/flags_ap.mk
LDS_FILE			=	mak/$(PRODUCT_DEF)/ap.lds
BIN_HEADER_TYPE         = 6
BIN_HEADER_XIP_TYPE     = 7
LIB_CUSTOM              = andesap
else
$(error Undefined BUILD_TYPE = $(b). Please check buildspec.mak)
endif
endif
endif

OUT_PATH			=	out/$(OPTION)
OBJ_PATH			=	obj/$(OPTION)
IMAGE				=	$(OUT_PATH)/$(PRODUCT_DEF)_$(OPTION).elf
BINARY      	    =	$(OUT_PATH)/$(PRODUCT_DEF)_$(OPTION).bin
BINARY_OLD      	=	$(OUT_PATH)/$(PRODUCT_DEF)_$(OPTION)_old.bin
HEADER_BIN_OLD      =   $(OUT_PATH)/$(PRODUCT_DEF)_$(OPTION)_header_old.bin
BINARY_RAM		    =	$(OUT_PATH)/$(PRODUCT_DEF)_$(OPTION)_ram.bin
BINARY_XIP		    =	$(OUT_PATH)/$(PRODUCT_DEF)_$(OPTION)_xip.bin
BINARY_OVERLAY      = 	$(OUT_PATH)/$(PRODUCT_DEF)_$(OPTION)_overlay.bin

HEADER_BIN          =   $(OUT_PATH)/$(PRODUCT_DEF)_$(OPTION)_header.bin
HEADER_BINARY_RAM	=	$(OUT_PATH)/$(PRODUCT_DEF)_$(OPTION)_ram_header.bin
HEADER_BINARY_XIP   =	$(OUT_PATH)/$(PRODUCT_DEF)_$(OPTION)_xip_header.bin

MAP_FILE			=	$(OUT_PATH)/$(PRODUCT_DEF)_$(OPTION).map
DMP_FILE			=	$(OUT_PATH)/$(PRODUCT_DEF)_$(OPTION).dmp
SIZE_FILE			=	$(OUT_PATH)/$(PRODUCT_DEF)_$(OPTION).txt
BUILD_LOG			=	$(OUT_PATH)/build_$(OPTION).log

#*******************************************************************************
# USER-DEFINED MACRO
#*******************************************************************************
MFLAGS					 =

#*******************************************************************************
# Dependence INPORT
#*******************************************************************************
include	$(call SRCFIND,obj,d)


#*******************************************************************************
# FIND SOURCE AND DEFINE OBJECTS
#*******************************************************************************
# Template
#EXC_SRC_FILES		=	$(call SRCFIND,$(SRC_PATH)/wifi,S)

ASM_FILES			=	$(call SRCFIND,$(SRC_PATH),S)
SRC_FILES			=	$(call SRCFIND,$(SRC_PATH),c)
API_FILES			=	$(call SRCFIND,$(API_PATH),c)

HDR_FILES			=	$(call SRCFIND,$(INCLUDE_PATH),h)
INCLUDES2			=	$(sort $(dir $(HDR_FILES)))
INCLUDES			=	$(INCLUDES2:%/=%)

ASM_OBJS			=	$(API_PATH)/vectors.o $(API_PATH)/startup.o $(API_PATH)/ctxsw.o 
SRC_OBJS			=	$(subst $(SRC_PATH)/,$(OBJ_PATH)/,$(subst .c,.o,$(SRC_FILES)))
API_OBJS			=	$(subst $(API_PATH)/,$(OBJ_PATH)/,$(subst .c,.o,$(API_FILES)))
OLD_OBJS			=	$(call SRCFIND,$(OBJ_PATH),o)
OBJECTS				=	$(ASM_OBJS) $(SRC_OBJS) $(API_OBJS)

ASM_DEPENDS			=	$(ASM_OBJS:%.o=%.d)
SRC_DEPENDS			=	$(SRC_OBJS:%.o=%.d)
OLD_DEPENDS			=	$(call SRCFIND,$(OBJ_PATH),d)


#*******************************************************************************
# MAKE OUTPUT DIRECTORY
#*******************************************************************************
REQ_DIRS			=	$(OBJ_PATH) $(OUT_PATH) $(subst $(SRC_PATH)/,$(OBJ_PATH)/,$(sort $(dir $(ASM_FILES) $(SRC_FILES))))


#*******************************************************************************
# ALL AND CLEAN
#*******************************************************************************
# Default make
.PHONY : all
all : mkdir clear_log $(BINARY_OLD) $(BINARY_RAM) $(BINARY_XIP) $(BINARY_OVERLAY) depend

# make instruction and data binary
.PHONY : dat
dat : $(DAT)

# make clean
.PHONY : clean
clean :
	@echo clean $(OLD_OBJS) $(IMAGE) $(OLD_DEPENDS) $(BUILD_LOG) $(HEADER_BIN_FILE)
	@$(RM) $(OLD_OBJS) $(HEADER_BIN_FILE)
	@$(RM) $(DAT)
	@$(RM) $(OLD_DEPENDS)
	@$(RM) $(BUILD_LOG)
	@$(RM) $(DAT_PATH)/NDSROM.dat $(DAT_PATH)/NDSROM.list
	@$(RM) $(OUT_PATH)/*

# make dir in obj/
.PHONY : mkdir
mkdir :
	@for d in $(REQ_DIRS);			\
	 do								\
#	 [[ -d $$d ]] || $(MKDIR) $$d;	\
	 $(MKDIR) $$d; \
	 done

.PHONY : clear_log
clear_log :
	@$(RM) $(BUILD_LOG)

# make dependency timestamp
depend : $(ASM_FILES) $(SRC_FILES) $(HDR_FILES)
	@touch depend



#*******************************************************************************
# PATTERN RULE
#*******************************************************************************
# Claim dependence of depend and obj
#$(ASM_OBJS) : $(OBJ_PATH)/%.o : $(OBJ_PATH)/%.d
#$(SRC_OBJS) : $(OBJ_PATH)/%.o : $(OBJ_PATH)/%.d


# Pattern for compilering
$(SRC_OBJS) : $(OBJ_PATH)/%.o : $(SRC_PATH)/%.c
	@echo Compiling $< ...
	@echo Compiling $< ... >> $(BUILD_LOG)
#	$(CC) -g $(BFLAGS) $(CFLAGS) $(TFLAGS) $(OFLAGS) $(BFLAGS) $(MFLAGS) $(DBGFLAGS) $(FUNCFLAGS) $(foreach i,$(INCLUDES),-I$i) -Wa,-ahl=$(@:.o=.lst) -o $@ $< 2>> $(BUILD_LOG)
	@$(CC) -g $(BFLAGS) $(CFLAGS) $(TFLAGS) $(OFLAGS) $(BFLAGS) $(MFLAGS) $(DBGFLAGS) $(FUNCFLAGS) $(foreach i,$(INCLUDES),-I $i) -o $@ $< 2>> $(BUILD_LOG)
ifeq ($(PACKAGE_BUILD),1)
	@$(STRIP) $@
endif

$(API_OBJS) : $(OBJ_PATH)/%.o : $(API_PATH)/%.c
	echo Compiling $< ...
	echo Compiling $< ... >> $(BUILD_LOG)
#	$(CC) -g $(BFLAGS) $(CFLAGS) $(TFLAGS) $(OFLAGS) $(BFLAGS) $(MFLAGS) $(DBGFLAGS) $(FUNCFLAGS) $(foreach i,$(INCLUDES),-I$i) -Wa,-ahl=$(@:.o=.lst) -o $@ $< 2>> $(BUILD_LOG)
	@$(CC) -g $(BFLAGS) $(CFLAGS) $(TFLAGS) $(OFLAGS) $(BFLAGS) $(MFLAGS) $(DBGFLAGS) $(FUNCFLAGS) $(foreach i,$(INCLUDES),-I $i) -o $@ $< 2>> $(BUILD_LOG)
	@$(STRIP) $@

# Pattern for c code dependence
$(SRC_DEPENDS) : $(OBJ_PATH)/%.d : $(SRC_PATH)/%.c
	@$(MAKE_DEPEND) $(foreach i,$(INCLUDES),-I $i) $< > $@.$$$$; \
	$(SED) 's,$(notdir $*)\.o,$(@:%.d=%.o),g' < $@.$$$$ > $@; \
	$(RM) $@.$$$$



# Pattern for ELF generation
$(IMAGE) : $(OBJECTS)
	@echo Linking $@ ...
	@echo Linking $@ ... >> $(BUILD_LOG)
	$(AR) r lib$(LIB_CUSTOM).a $(SRC_OBJS)
	$(LD) $(LFLAGS) -T $(LDS_FILE) -Map $(MAP_FILE) -o $@ --start-group $(API_OBJS) $(ASM_OBJS) -L. -l$(LIB_CUSTOM) --end-group >> $(BUILD_LOG)	

# Pattern for binary generation
#ifeq ($(CHIPECO_DEF),MT7681_E2)
$(BINARY_OLD) : $(IMAGE)
	@echo Making $(BINARY_OLD) ... >> $(BUILD_LOG)
	@$(OBJCOPY)  -O binary $< $@ 2>> $(BUILD_LOG)
$(BINARY_RAM) : $(IMAGE)
	@echo Making $(BINARY_RAM) ... >> $(BUILD_LOG)
	@$(OBJCOPY) $(COPY_SRAM) -O binary $< $@ 2>> $(BUILD_LOG)
$(BINARY_XIP) : $(IMAGE)
	@echo Making $(BINARY_XIP) ... >> $(BUILD_LOG)
	@$(OBJCOPY) $(COPY_XIP) -O binary $< $@ 2>> $(BUILD_LOG)
$(BINARY_OVERLAY) : $(IMAGE)
	@echo Making $(BINARY_OVERLAY) ... >> $(BUILD_LOG)
	@$(OBJCOPY) $(COPY_OVERLAY) -O binary $< $@ 2>> $(BUILD_LOG)
#endif
	
#generate flash bin 
ifeq ($(BUILD_TYPE),0)
	@./flash_merge.sh 3
endif	

ifeq ($(BUILD_TYPE),1)
	@./flash_merge.sh 1
	@./flash_merge.sh 3
endif	
	
ifeq ($(BUILD_TYPE),2)
	@./flash_merge.sh 2
	@./flash_merge.sh 3
endif	

#generate header bin
ifeq ($(PRODUCT_VERSION),7681)
	@if [ -s  $(BINARY_XIP) -o -s  $(BINARY_OVERLAY) ]; then \
	   $(RM)  $(BINARY_OLD) ;\
	   . header.sh $(BINARY) $(BIN_HEADER_TYPE) $(HEADER_BIN);\
	else \
	   $(RM)  $(BINARY_RAM) $(BINARY_XIP) $(BINARY_OVERLAY);\
	   . header.sh $(BINARY_OLD) $(BIN_HEADER_TYPE) $(HEADER_BIN);\
	fi;
	
endif

#END-OF-FILE#
