# FreeS/WAN subdir makefile
# Copyright (C) 1998-2001  Henry Spencer.
# 
# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the
# Free Software Foundation; either version 2 of the License, or (at your
# option) any later version.  See <http://www.fsf.org/copyleft/gpl.txt>.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
# for more details.
#
# RCSID $Id: Makefile,v 1.1.1.1 2006/11/29 08:55:38 lizhijie Exp $

TOPDIR=..
include $(TOPDIR)/Makefile.inc

SUBDIRS=utils test

def:
	@echo "Please read doc/intro.html or INSTALL before running make"
	@false

# programs

cleanall distclean mostlyclean realclean install programs checkprograms check clean spotless:
	@for d in $(SUBDIRS); \
	do \
		(cd $$d && $(MAKE) TOPDIR=$(TOPDIR)/.. $@ ) || exit 1 ; \
	done

