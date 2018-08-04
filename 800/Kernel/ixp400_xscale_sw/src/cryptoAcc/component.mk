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

cryptoAcc_OBJ := IxCryptoAccUtilities.o		\
		 IxCryptoAcc.o			\
		 IxCryptoAccDescMgmt.o		\
		 IxCryptoAccCCDMgmt.o		\
		 IxCryptoAccQAccess.o		\
		 IxCryptoAccCryptoRegister.o	\
		 IxCryptoAccCryptoPerform.o

# Exclude assembly code from unit test
# Use the stub in unit test folder
ifneq ($(IX_TARGET),vxsim)
cryptoAcc_OBJ += IxCryptoAccXScaleWep.o
endif

cryptoAcc_CFLAGS := -I./src/cryptoAcc/include -I./$LINUX_SRC/include/asm-arm

# Force NDEBUG unless debug info has been specifically requested.
# This way the unit test will be built with NDEBUG.
ifndef IX_DEBUG
cryptoAcc_CFLAGS += -DNDEBUG
endif

ifeq ($(IX_TARGET_OS),linux)
cryptoAcc_OBJ += IxCryptoAccSymbols.o
cryptoAcc_EXPORT_OBJ := IxCryptoAccSymbols.o
endif
