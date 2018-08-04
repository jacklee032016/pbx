#ifndef VOCALCONFIG_H_
#define VOCALCONFIG_H_

#ifdef WIN32
/* this section is for WIN32 ONLY, and only because we cannot use make */

/* The equivalent of /usr/local/vocal/ in Linux */
#define VOCAL_INSTALL_PATH_DEFAULT ""

#define VOCAL_INSTALL_PATH ""

#else
#include "vocalconfig-config.h"
#endif

#endif
