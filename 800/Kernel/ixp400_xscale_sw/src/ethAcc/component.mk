# 
# @par
# IXP400 SW Release Crypto version 2.0
# 
# -- Intel Copyright Notice --
# 
# @par
# Copyright 2002-2005 Intel Corporation All Rights Reserved.
# 
# @par
# The source code contained or described herein and all documents
# related to the source code ("Material") are owned by Intel Corporation
# or its suppliers or licensors.  Title to the Material remains with
# Intel Corporation or its suppliers and licensors.
# 
# @par
# The Material is protected by worldwide copyright and trade secret laws
# and treaty provisions. No part of the Material may be used, copied,
# reproduced, modified, published, uploaded, posted, transmitted,
# distributed, or disclosed in any way except in accordance with the
# applicable license agreement .
# 
# @par
# No license under any patent, copyright, trade secret or other
# intellectual property right is granted to or conferred upon you by
# disclosure or delivery of the Materials, either expressly, by
# implication, inducement, estoppel, except in accordance with the
# applicable license agreement.
# 
# @par
# Unless otherwise agreed by Intel in writing, you may not remove or
# alter this notice or any other notice embedded in Materials by Intel
# or Intel's suppliers or licensors in any way.
# 
# @par
# For further details, please see the file README.TXT distributed with
# this software.
# 
# @par
# -- End Intel Copyright Notice --

ethAcc_OBJ :=  IxEthAcc.o IxEthAccCommon.o IxEthAccControlInterface.o IxEthAccDataPlane.o IxEthAccMac.o IxEthAccMii.o
ethAcc_CFLAGS := -I./src/ethAcc/include


ifeq ($(IX_TARGET_OS),linux)
ethAcc_OBJ += IxEthAccSymbols.o
ethAcc_EXPORT_OBJ := IxEthAccSymbols.o
endif
