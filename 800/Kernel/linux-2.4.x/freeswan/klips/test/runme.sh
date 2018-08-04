#!/bin/sh

#
# $Id: runme.sh,v 1.1.1.1 2006/11/30 16:28:46 lizhijie Exp $
#
# use this script to run a single test from within that test directory.
# note that this assumes a "klipstest" type test.
#

. ../../../umlsetup.sh
. ../setup.sh
. ../functions.sh
. testparams.sh

( netjigtest )

stat=$?
testdir=$TESTNAME
testtype=klipstest

(cd .. && recordresults $testdir $testtype $stat)

 
# $Log: runme.sh,v $
# Revision 1.1.1.1  2006/11/30 16:28:46  lizhijie
# AS800 Kernel
#
# Revision 1.1.1.1  2006/01/10 06:01:25  lizhijie
# kernel and driver for DUX project
#
# Revision 1.1.1.1  2005/07/08 09:46:23  lizhijie
# Linux-2.4.24 integrated Intel Access Library 2.0
#
# Revision 1.1.1.1  2004/08/17 02:50:02  lizhijie
# linux-2.4.24 for IXP422
#
# Revision 1.3  2002/04/02 02:48:21  mcr
# *** empty log message ***
#
# Revision 1.2  2001/11/23 01:08:12  mcr
# 	pullup of test bench from klips2 branch.
#
# Revision 1.1.2.1  2001/10/23 02:27:14  mcr
# 	more setup and utility scripts.
#
# 

