# $Id: build.sh,v 1.1.1.1 2006/11/29 18:55:32 lizhijie Exp $
# /bin/sh
# scripts which make a primary file system on device of CF card or Flash Disk
# Li Zhijie, 2006.11.04

: ${1?"Usage: $0 [device_name]"}

#if [ $1 = "" ]; then
#	echo "Prompt : build.sh [device_name]"
#	exit 1
#fi

ROOT_DIR=`pwd`
export ROOT_DIR

echo ""
echo "Current directory is '$ROOT_DIR'"

echo ""
echo "make ext2 FileSystem on device $1....."
mke2fs $1

echo ""
echo "adding journy log and tuning mount count on device $1....."
tune2fs -j -c 0 $1

echo ""
echo "mount $1 on the mount point of /mnt....."
mount $1 /mnt
cd /mnt

echo ""
echo "Please uncompressing the FileSystem Release on mount point of /mnt"
tar xvzf $ROOT_DIR/AS600_FileSystem_*

echo ""
echo "modify permission for MAIL Server(SMTP and POP/IMAP) /mnt"
chmod -R 777 /mnt/var/spool/mail
chmod -R 777 /mnt/var/spool/smtp

echo ""
echo "umount device of $1 from /mnt"
cd $ROOT_DIR
umount /mnt

echo "Build FileSystem on $1 successfully!"
