#*******************************************************************************
#* MODULE NAME:     tools.mak
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
# SHELL COMMAND
#*******************************************************************************
MKDIR			:=	mkdir -p
RM				:=	rm -rf
CP				:=	cp -f
MV 				:=	mv
SED				:=	sed



#*******************************************************************************
# USER COMMAND
#*******************************************************************************
# $(call SRCFIND, SRC_PATH, SUFFIX)
SRCFIND = $(patsubst ./%,%,$(sort $(shell find $1 -name '*.$2')))



#END-OF-FILE#
