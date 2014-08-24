#!/bin/bash

#header.sh input_file  type  output_file
# the range of type is [1~7] 
# 1: upgrade to loader Block
# 2: upgrade to Recovery FW Block                 
# 3: upgrade to EEPROM Block	           
# 4: upgrade to STA Firmware Block	
# 5: upgrade to STA-XIP Firmware Block	
# 6: upgrade to AP Firmware Block	       
# 7: upgrade to AP-XIP Firmware Block	
	
bin_file=$1
type=$2
new_file=$3
echo $1 $2 $3


size=$(stat -c %s $bin_file)
filename=${bin_file%.*}


cat /dev/null > file_header

#1. MAGIC NUMBER (3 bytes)

echo -e -n "\x`printf %x 0x76`" >>file_header
echo -e -n "\x`printf %x 0x81`" >>file_header
echo -e -n "\x`printf %x 0xAA`" >>file_header


#2. BIN FILE SIZE (4 bytes)
cnt=$size
for ((i = 0;i <4;i++))
do
let cnt_tmp="cnt & 0xff"
let cnt="cnt >> 8"
echo -e -n "\x`printf %x $cnt_tmp`" >>file_header
done

#3. TYPE (1 byte)
echo -e -n "\x`printf %x $type`" >>file_header

#4. PADDING (120 byte)
for ((i = 0;i <120;i++))
do
echo -e -n "\x`printf %x 0x00`" >>file_header
done


#5. Create new header bin file
#cat file_header $bin_file > "$filename"_hdr.bin

if  [ $size -gt 0 ]; then
	cat file_header $bin_file > $new_file
fi



