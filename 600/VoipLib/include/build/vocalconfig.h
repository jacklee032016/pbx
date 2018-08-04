#ifndef VOCALCONFIG_H_
#define VOCALCONFIG_H_

/*
 * $Id: vocalconfig.h,v 1.2 2007/03/01 20:04:27 lizhijie Exp $
 */


#ifdef WIN32
/* this section is for WIN32 ONLY, and only because we cannot use make */

/* The equivalent of /usr/local/vocal/ in Linux */
#define VOCAL_INSTALL_PATH_DEFAULT ""

#define VOCAL_INSTALL_PATH ""

#else
#include "vocalconfig-config.h"
#endif

#endif
