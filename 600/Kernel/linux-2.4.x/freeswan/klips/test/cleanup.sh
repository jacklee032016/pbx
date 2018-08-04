#
# use this script to undo effects of sourcing a "testparams.sh" into
# your shell, when testing.
#
# $Id: cleanup.sh,v 1.1.1.1 2006/11/29 08:55:38 lizhijie Exp $
#
unset SCRIPT
unset REFCONSOLEOUTPUT 
unset PRIVINPUT
unset PUBINPUT
unset REFPRIVOUTPUT
unset REFPUBOUTPUT
unset TCPDUMPARGS

#
# $Log: cleanup.sh,v $
# Revision 1.1.1.1  2006/11/29 08:55:38  lizhijie
# AS600 Kernel
#
# Revision 1.1.1.1  2005/07/08 09:46:23  lizhijie
# Linux-2.4.24 integrated Intel Access Library 2.0
#
# Revision 1.1.1.1  2004/08/17 02:50:02  lizhijie
# linux-2.4.24 for IXP422
#
# Revision 1.3  2002/02/20 07:26:24  rgb
# Corrected de-pluralized variable names.
#
# Revision 1.2  2001/11/23 01:08:12  mcr
# 	pullup of test bench from klips2 branch.
#
# Revision 1.1.2.1  2001/10/23 04:43:18  mcr
# 	shell/testing cleaning script.
#
# 
