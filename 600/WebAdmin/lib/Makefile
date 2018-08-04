#
# $Id: Makefile,v 1.8 2007/09/15 19:45:14 lizhijie Exp $
#

CUPS_DIR:=$(shell pwd)

include $(ROOT_DIR)/Makefile.pre

CODE_SUBDIRS = cfg process lan wireless

# name of all files to be compiled (don't include header files)
SRC += \
	cgi_string_lib.c \
	cgi_llist.c \
	cgi_lib.c \
	cgi_nic.c \
	cgi_html_lib.c \
	cgi_pages_lib.c \
	cgi_file_func.c \
	g_list.c \
	cfg/cgi_cfg_file.c \
	cfg/cgi_cfg_boa.c \
	cfg/cgi_cfg_mail_alias.c \
	process/cgi_services.c \
	process/cgi_syscfg.c \
	process/cgi_signal.c \
	process/cgi_upload_file.c \
	lan/cgi_lan_dhcp.c \
	lan/cgi_lan_setup.c \
	as_disk_info.c \
	wireless/cgi_wireless_utils.c \
	wireless/cgi_wireless_mode.c \
	ddnsUtils.c 
	 
ifeq ($(STC_NEW),yes)
	SRC += cgi_pbx.c 
else
	SRC += pbx_cfg_data.c 
endif

ifeq ($(MESH_SUPPORT),YES)
SRC += wireless/cgi_mesh_mode.c
endif


C_FLAGS += -I./cfg -I$(ROOT_DIR)/common_Wifi/tools/lib 

TARGET_LIB:=libassistcgi
TARGET_LIB_SHARED:=$(TARGET_LIB).so
		 
LOCAL_ALL:=

include $(ROOT_DIR)/Makefile.post

LIBS += -L$(ROOT_DIR)/common_Wifi/Binary/lib -lwifi

