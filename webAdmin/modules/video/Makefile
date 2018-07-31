#
# $Id$
#

include $(ROOT_DIR)/Makefile.pre

CODE_SUBDIRS = .

C_FLAGS += 

LDFLAGS+= -lcgi -lShared -lpthread

# name of all files to be compiled (don't include header files)
SRC = \
	videoView.c \
	videoMotionDetect.c \
	videoSimpleButtons.c \
	videoButtons.c \
	videoSelect.c \
	videoRecord.c \
	videoAction.c \
	videoMain.c
	
#CGIS = video.cgi
CGIS = video.cgi videoview.cgi activeCtrl

LOCAL_ALL:=$(CGIS) 

include $(ROOT_DIR)/Makefile.post

video.cgi: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)
	$(STRIP) $@
	$(MOVE) $@ $(CGI_BIN_DIR)/video 
	  
videoview.cgi:page_videoview.c
	$(CC) -o $@ $(C_FLAGS) $(LDFLAGS)  $^
	$(STRIP) $@
	$(MOVE) $@ $(CGI_BIN_DIR)
	

activeCtrl:active_ctrl.c
	$(CC) -o $@ $(C_FLAGS) $(LDFLAGS)  $^
	$(STRIP) $@
	$(MOVE) $@ $(CGI_BIN_DIR)
