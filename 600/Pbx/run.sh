#/bin/sh
# run ippbx program

TOP_DIR:=$(shell pwd)/
PBX_WORK_DIR=/tmp/pbx
PBX_LOG_FILE=pbx.log
echo "make log file '$PBX_LOG_FILE'"
rm -rf $PBX_WORK_DIR
mkdir -p $PBX_WORK_DIR
touch $PBX_WORK_DIR/$PBX_LOG_FILE

export LD_LIBRARY_PATH=$(TOP_DIR)releases/lib
./pbx
