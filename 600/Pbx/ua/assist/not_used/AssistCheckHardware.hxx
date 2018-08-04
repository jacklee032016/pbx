#ifndef __ASSIST_CHECK_HARDWARE_H__
#define __ASSIST_CHECK_HARDWARE_H__

/* 
 * $Id: AssistCheckHardware.hxx,v 1.2 2007/03/21 18:12:39 lizhijie Exp $
 */

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <cassert>

#include "UaConfiguration.hxx"
#include "ResGwDevice.hxx"

class AssistCheckHardware
{
    public:
        ///
        AssistCheckHardware();
        ///
        ~AssistCheckHardware();
        ///
        const char* getType();
        ///
        const char* name() const;
};

#endif
