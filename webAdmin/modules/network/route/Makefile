#
# $Id$
#

include $(ROOT_DIR)/Makefile.pre

CODE_SUBDIRS = .

# name of all files to be compiled (don't include header files)
SRC = \
	net_routes.c \
	net_routes_update.c


LDFLAGS+= -lcgi

CGIS = route.cgi

LOCAL_ALL:=$(CGIS) 

include $(ROOT_DIR)/Makefile.post

route.cgi: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)
	$(STRIP) $@
	$(MOVE) $@ $(CGI_BIN_DIR)/network   
