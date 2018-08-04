#/bin/bash

VAR_DIR=/tmp/msgfile

STATS_FILE=$VAR_DIR/statis.data
RCV_MSG=$VAR_DIR/smsg.data
SND_MSG=$VAR_DIR/rmsg.data
EVT_MSG=$VAR_DIR/emsg.data
LOG_FILE=$VAR_DIR/log.data
rm -rf $VAR_DIR
mkdir $VAR_DIR

touch $STATS_FILE
touch $RCV_MSG
touch $SND_MSG
touch $EVT_MSG
touch $LOG_FILE
chown root.root $STATS_FILE
chown root.root $RCV_MSG
chown root.root $SND_MSG
chown root.root $EVT_MSG
chown root.root $LOG_FILE
chmod 777 $STATS_FILE
chmod 777 $RCV_MSG
chmod 777 $SND_MSG
chmod 777 $EVT_MSG
chmod 777 $LOG_FILE