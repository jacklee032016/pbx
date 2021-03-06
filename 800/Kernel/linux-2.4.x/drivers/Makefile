#
# Makefile for the Linux kernel device drivers.
#
# 15 Sep 2000, Christoph Hellwig <hch@infradead.org>
# Rewritten to use lists instead of if-statements.
#

# added assist_isdn item, lizhijie, 2005.08.04
mod-subdirs :=	dio hil mtd sbus video macintosh usb input telephony ide \
		message/i2o message/fusion scsi md ieee1394 pnp isdn assist atm \
		fc4 net/hamradio i2c acpi bluetooth usb/gadget

subdir-y :=	parport char block net sound misc media cdrom hotplug
subdir-m :=	$(subdir-y)


subdir-$(CONFIG_DIO)		+= dio
subdir-$(CONFIG_PCI)		+= pci
subdir-$(CONFIG_GSC)		+= gsc
subdir-$(CONFIG_HIL)		+= hil
subdir-$(CONFIG_PCMCIA)		+= pcmcia
subdir-$(CONFIG_MTD)		+= mtd
subdir-$(CONFIG_SBUS)		+= sbus
subdir-$(CONFIG_ZORRO)		+= zorro
subdir-$(CONFIG_NUBUS)		+= nubus
subdir-$(CONFIG_TC)		+= tc
subdir-$(CONFIG_VT)		+= video
subdir-$(CONFIG_MAC)		+= macintosh
subdir-$(CONFIG_PPC32)		+= macintosh
subdir-$(CONFIG_USB)		+= usb
subdir-$(CONFIG_USB_GADGET)	+= usb/gadget
subdir-$(CONFIG_INPUT)		+= input
subdir-$(CONFIG_PHONE)		+= telephony
subdir-$(CONFIG_IDE)		+= ide
subdir-$(CONFIG_SCSI)		+= scsi
subdir-$(CONFIG_I2O)		+= message/i2o
subdir-$(CONFIG_FUSION)		+= message/fusion
subdir-$(CONFIG_MD)		+= md
subdir-$(CONFIG_IEEE1394)	+= ieee1394
subdir-$(CONFIG_PNP)		+= pnp
subdir-$(CONFIG_ISDN_BOOL)	+= isdn
# added this line, 2005.08.04 lizhijie
# subdir-$(CONFIG_ASSIST_ISDN_BOOL)	+= assist_isdn
# modify as this line, 2005.12.22 lizhijie
# subdir-$(CONFIG_ASSIST_ISDN_BOOL)	+= assist
# modify as this line, 2006.01.14 lizhijie
subdir-$(CONFIG_ASSIST_BOARD)	+= assist
subdir-$(CONFIG_ATM)		+= atm
subdir-$(CONFIG_FC4)		+= fc4

# CONFIG_HAMRADIO can be set without CONFIG_NETDEVICE being set  -- ch
subdir-$(CONFIG_HAMRADIO)	+= net/hamradio
subdir-$(CONFIG_I2C)		+= i2c
subdir-$(CONFIG_ACPI_BOOT)	+= acpi
subdir-$(CONFIG_PCF8594C2)	+= i2c/pcf8594c-2

subdir-$(CONFIG_BLUEZ)		+= bluetooth

ifdef CONFIG_ARCH_IXP425
subdir-$(CONFIG_IXP425_CSR)	+= ixp400
endif

include $(TOPDIR)/Rules.make
