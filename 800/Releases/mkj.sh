#!/bin/sh
# ����JFFS24usb�ĸ��ļ�ϵͳ�Ĺ�������Ҫ����Ľű�

RELEASES_ROOT=flashgeneral
REPOSITORY="$RELEASES_ROOT.tar.gz"
OUTPUT_FILE=jffs2.big.image

# 2368K jffs2 root fs partition
IMAGE_SIZE=2424832

export RELEASES_ROOT REPOSITORY IMAGE_SIZE OUTPUT_FILE

tar zxvf $REPOSITORY
mkfs.jffs2 --root=$RELEASES_ROOT  --big-endian --output=$OUTPUT_FILE --pad=$IMAGE_SIZE


echo "Success"
	
