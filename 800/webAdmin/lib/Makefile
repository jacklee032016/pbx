#
# $Id$
#

include $(ROOT_DIR)/Makefile.pre

CODE_SUBDIRS = process lan wireless old

# name of all files to be compiled (don't include header files)
SRC += \
	cgi_string_lib.c \
	cgi_llist.c \
	cgi_lib.c \
	cgi_gettext.c \
	cgi_nic.c \
	cgi_html_lib.c \
	cgi_pages_lib.c \
	cgi_file_func.c \
	process/cgi_signal.c \
	process/cgi_upload_file.c \
	process/cgi_syscfg.c \
	process/cgi_services.c \
	wireless/cgi_wireless_utils.c \
	g_list.c 

#	lan/cgi_lan_dhcp.c \
#	lan/cgi_lan_setup.c \
#	wireless/cgi_wireless_mode.c \
#	ddnsUtils.c 
	 
SRC += \
	old/conn.c \
	old/net_unix.c \
	old/set_dns.c \
	old/set_network.c \
	old/cgi.c \
	old/param.c 

C_FLAGS += -I$(ROOT_DIR)/include/cgi -fPIC 

TARGET_LIB:=libcgi
TARGET_LIB_SHARED:=$(TARGET_LIB).so
		 
LOCAL_ALL:=

include $(ROOT_DIR)/Makefile.post

LIBS += -L$(ROOT_DIR)/common_Wifi/Binary/lib 

