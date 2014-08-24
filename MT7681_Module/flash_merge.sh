#!/bin/bash

#Terrence,2014/04/01,update based on latest flash partition table

#flash_merge.sh option    #merge flash bin file

# option =
# 1 : MT7681_sta_ram.bin + MT7681_sta_xip.bin + MT7681_sta_overlay.bin                              -->  MT7681_sta.bin
# 2 : MT7681_ap_ram.bin + MT7681_ap_xip.bin + MT7681_ap_overlay.bin                                 -->  MT7681_ap.bin
# 3 : MT7681_loader.bin + MT7681_recovery.bin + MT7681_eep.bin + MT7681_sta.bin + MT7681_ap.bin     -->  MT7681_all.bin

###################################
#MT7681_sta_ram.bin
#start:0x00
#size:16*4K

#MT7681_sta_xip.bin
#start:17*4K 
#size:15*4K

#MT7681_sta_overlay.bin 
#start:32*4K
#size:15*4K       
###################################
#MT7681_ap_ram.bin
#start:0x00
#size:16*4k

#MT7681_ap_xip.bin
#start:17*4k 
#size:15*4k

#MT7681_ap_overlay.bin 
#start:32*4k 
#size:15*4k       
###################################
#MT7681_loader.bin
#start:0x00
#size:5*4k

#MT7681_recovery.bin 
#start:6*4K 
#size:16*4K

#MT7681_eep.bin
#start:23*4K
#size:1*4K

#MT7681_sta.bin
#start:31*4K
#size:47*4K

#MT7681_ap.bin
#start:79*4K
#size:47*4K

#####################################	

sta_bin=out/sta/MT7681_sta.bin	
sta_ram_bin=out/sta/MT7681_sta_ram.bin
sta_xip_bin=out/sta/MT7681_sta_xip.bin
sta_overlay_bin=out/sta/MT7681_sta_overlay.bin

ap_bin=out/ap/MT7681_ap.bin
ap_ram_bin=out/ap/MT7681_ap_ram.bin
ap_xip_bin=out/ap/MT7681_ap_xip.bin
ap_overlay_bin=out/ap/MT7681_ap_overlay.bin

all_bin=MT7681_all.bin	
loader_bin=refbin/loader_0322_94973.bin
eep_bin=refbin/MT7681E2_EEPROM_layout_20140330.bin
recovery_bin=out/recovery/MT7681_recovery_old.bin
sta_bin=out/sta/MT7681_sta.bin		
ap_bin=out/ap/MT7681_ap.bin
	
option=$1
echo "option is ${option}"

######################################################################################################################
# option =
# 1 : MT7681_sta_ram.bin + MT7681_sta_xip.bin + MT7681_sta_overlay.bin                              -->  MT7681_sta.bin
if  [ $option -eq 1 ]; then
new_file=${sta_bin}
#<1>.copy MT7681_sta_ram.bin to MT7681_sta.bin
input_file=${sta_ram_bin}
let "flash_size=0x30000 - 0x1F000"    #sta_xip_start - sta_ram_start
if  [ ! -f "${input_file}" ]; then
	echo -e -n "${input_file} not exist,exit!\n"
	exit 0
fi

file_size=$(stat -c %s $input_file)

if  [ $flash_size -lt $file_size ]; then
	echo -e -n "MT7681_sta_ram.bin size error,exit!\n"
	exit 0
fi

BS=4096
COUNT=16
SKIP=0
SEEK=0
CONV=notrunc

dd if=${input_file} of=${new_file} bs=${BS} skip=${SKIP} count=${COUNT} seek=${SEEK} conv=${CONV}


#<2>.copy MT7681_sta_xip.bin to MT7681_sta.bin	
input_file=${sta_xip_bin}
let "flash_size=0x3F000 - 0x30000"   #sta_overlay_start - sta_xip_start

if  [ ! -f $input_file ]; then
	echo -e -n "${input_file} not exist,set it as null\n"
	cat /dev/null > $input_file
fi

file_size=$(stat -c %s $input_file)

if  [ $flash_size -lt $file_size ]; then
	echo -e -n "${input_file} size error!exit\n"
	exit 0
fi

BS=4096
COUNT=15
SKIP=0
SEEK=17
CONV=notrunc

dd if=${input_file} of=${new_file} bs=${BS} skip=${SKIP} count=${COUNT} seek=${SEEK} conv=${CONV}

	
#<3>.copy MT7681_sta_overlay.bin to MT7681_sta.bin
input_file=${sta_overlay_bin}
let "flash_size=0x4E000 - 0x3F000"   #sta_overlay_start - sta_xip_start

if  [ ! -f $input_file ]; then
	echo -e -n "${input_file} not exist,set it as null\n"
	cat /dev/null > $input_file
fi

file_size=$(stat -c %s $input_file)

if  [ $flash_size -lt $file_size ]; then
	echo -e -n "${input_file} size error!exit\n"
	exit 0
fi

BS=4096
COUNT=15
SKIP=0
SEEK=32
CONV=notrunc

dd if=${input_file} of=${new_file} bs=${BS} skip=${SKIP} count=${COUNT} seek=${SEEK} conv=${CONV}


file_size=$(stat -c %s ${new_file})
echo -e -n "merge flash bin file:${new_file} ok,size is ${file_size} bytes \n"

fi




######################################################################################################################
# option =
# 2 : MT7681_ap_ram.bin + MT7681_ap_xip.bin + MT7681_ap_overlay.bin                                 -->  MT7681_ap.bin
if [ $option -eq 2 ]; then

new_file=${ap_bin}

#<1>.copy MT7681_ap_ram.bin to MT7681_ap.bin
input_file=${ap_ram_bin}
let "flash_size=0x60000 - 0x4F000"      #ap_xip_start - ap_ram_start

if  [ ! -f $input_file ]; then
	echo -e -n "${input_file} not exist,eixt\n"
	exit 0
fi

file_size=$(stat -c %s $input_file)

if  [ $flash_size -lt $file_size ]; then
	echo -e -n "${input_file} size error!exit\n"
	exit 0
fi

BS=4096
COUNT=16
SKIP=0
SEEK=0
CONV=notrunc

dd if=${input_file} of=${new_file} bs=${BS} skip=${SKIP} count=${COUNT} seek=${SEEK} conv=${CONV}


#<2>.copy MT7681_ap_xip.bin to MT7681_ap.bin	
input_file=${ap_xip_bin}
let "flash_size=0x6F000 - 0x60000"   #ap_overlay_start - ap_xip_start

if  [ ! -f $input_file ]; then
	echo -e -n "${input_file} not exist,set it as null\n"
	cat /dev/null > $input_file
fi

file_size=$(stat -c %s $input_file)

if  [ $flash_size -lt $file_size ]; then
	echo -e -n "${input_file} size error!exit\n"
	exit 0
fi

BS=4096
COUNT=15
SKIP=0
SEEK=17
CONV=notrunc

dd if=${input_file} of=${new_file} bs=${BS} skip=${SKIP} count=${COUNT} seek=${SEEK} conv=${CONV}

	
#<3>.copy MT7681_ap_overlay.bin to MT7681_ap.bin
input_file=${ap_overlay_bin}
let "flash_size=0x7E000 - 0x6F000"   #ap_overlay_start - ap_xip_start

if  [ ! -f $input_file ]; then
	echo -e -n "${input_file} not exist,set it as null\n"
	cat /dev/null > $input_file
fi

file_size=$(stat -c %s $input_file)

if  [ $flash_size -lt $file_size ]; then
	echo -e -n "${input_file} size error!exit\n"
	exit 0
fi

BS=4096
COUNT=15
SKIP=0
SEEK=32
CONV=notrunc

dd if=${input_file} of=${new_file} bs=${BS} skip=${SKIP} count=${COUNT} seek=${SEEK} conv=${CONV}


file_size=$(stat -c %s ${new_file})
echo -e -n "merge flash bin file:${new_file} ok,size is ${file_size} bytes\n"

fi

######################################################################################################################
# option =
# 3 : MT7681_loader.bin + MT7681_recovery.bin + MT7681_eep.bin + MT7681_sta.bin + MT7681_ap.bin     -->  MT7681_all.bin
if [ $option -eq 3 ]; then

new_file=${all_bin}

#<1>.copy MT7681_loader.bin to MT7681_all.bin
input_file=${loader_bin}
let "flash_size=0x6000 - 0x0000"            #recovery_start - loader_start

if  [ ! -f $input_file ]; then
	echo -e -n "${input_file} not exist,exit!\n"
	exit 0
fi

file_size=$(stat -c %s $input_file)

if  [ $flash_size -lt $file_size ]; then
	echo -e -n "${input_file} size error!exit\n"
	exit 0
fi

BS=4096
COUNT=5
SKIP=0
SEEK=0
CONV=notrunc

dd if=${input_file} of=${new_file} bs=${BS} skip=${SKIP} count=${COUNT} seek=${SEEK} conv=${CONV}


#<2>.copy MT7681_recovery.bin to MT7681_all.bin	
input_file=${recovery_bin}
let "flash_size=0x17000 - 0x6000"           #eep_start - recovery_start

if  [ ! -f $input_file ]; then
	echo -e -n "${input_file} not exist,exit!\n"
	exit 0
fi

file_size=$(stat -c %s $input_file)

if  [ $flash_size -lt $file_size ]; then
	echo -e -n "${input_file} size error!exit\n"
	exit 0
fi

BS=4096
COUNT=16
SKIP=0
SEEK=6
CONV=notrunc

dd if=${input_file} of=${new_file} bs=${BS} skip=${SKIP} count=${COUNT} seek=${SEEK} conv=${CONV}

	
#<3>.copy MT7681_eep.bin to MT7681_all.bin
input_file=${eep_bin}
let "flash_size=0x1F000 - 0x17000"   #sta_ram_start - eep_start

if  [ ! -f $input_file ]; then
	echo -e -n "${input_file} not exist,error!exit\n"
	exit 0
fi

file_size=$(stat -c %s $input_file)

if  [ $flash_size -lt $file_size ]; then
	echo -e -n "${input_file} size error!exit\n"
	exit 0
fi

BS=4096
COUNT=1
SKIP=0
SEEK=23
CONV=notrunc

dd if=${input_file} of=${new_file} bs=${BS} skip=${SKIP} count=${COUNT} seek=${SEEK} conv=${CONV}


#<4>.copy MT7681_sta.bin to MT7681_all.bin
input_file=${sta_bin}
let "flash_size=0x4F000 - 0x1F000"          #ap_ram_start - sta_ram_start

if  [ ! -f $input_file ]; then
	echo -e -n "${input_file} not exist,set it as null\n"
	cat /dev/null > $input_file
fi

file_size=$(stat -c %s $input_file)

if  [ $flash_size -lt $file_size ]; then
	echo -e -n "${input_file} size error!exit\n"
	exit 0
fi


BS=4096
COUNT=47
SKIP=0
SEEK=31
CONV=notrunc

dd if=${input_file} of=${new_file} bs=${BS} skip=${SKIP} count=${COUNT} seek=${SEEK} conv=${CONV}


#<5>.copy MT7681_ap.bin to MT7681_all.bin
input_file=${ap_bin}
let "flash_size=0x7E000 - 0x4F000"            #other_start - ap_ram_start

if  [ ! -f $input_file ]; then
	echo -e -n "${input_file} not exist,set it as null\n"
	cat /dev/null > $input_file
fi
file_size=$(stat -c %s $input_file)

if  [ $flash_size -lt $file_size ]; then
	echo -e -n "${input_file}size error!exit\n"
	exit 0
fi

BS=4096
COUNT=47
SKIP=0
SEEK=79
CONV=notrunc

dd if=${input_file} of=${new_file} bs=${BS} skip=${SKIP} count=${COUNT} seek=${SEEK} conv=${CONV}


file_size=$(stat -c %s ${new_file})
echo -e -n "merge flash bin file:${new_file} ok,size is ${file_size} bytes\n"


fi




