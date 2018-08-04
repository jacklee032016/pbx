#ifndef  __VPBX_GLOBAL_H__
#define __VPBX_GLOBAL_H__

/*
* $Id: vpbx_globals.h,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#ifdef  __cplusplus
	extern "C"
	{
#endif

#define WITH_DEBUG			1

#define WITH_Q931_DEBUG	1


#define ASSIST_INSTALL_PATH_DEFAULT "/usr/local/vocal"

#define ASSIST_INSTALL_PATH ASSIST_INSTALL_PATH_DEFAULT

/*
* 0 : continue handled by WorkerThread
* 1 : handled, so free it, for example L2 msg
* 2 : not handled, but must be free now
*/
#define AS_MSG_CONTINUE			0
#define AS_MSG_HANDLED			1
#define AS_MSG_NEED_FREE			2


#ifndef AS_OK
#define AS_OK				0
#define AS_OK   							0
#endif
#ifndef AS_FAIL
#define AS_FAIL				1
#endif

#if  __ARM_IXP__
	#define 	ISDNPBX_ROOT_DIR			"/usr/"
#else
	#define 	ISDNPBX_ROOT_DIR			"releases/usr/"
#endif

#define ISDNPBX_WORK_ROOT      			"/tmp/pbx"


#define ISDNPBX_CONFIG_FILE			ISDNPBX_ROOT_DIR"etc/vpbx/vpbx.cfg"		
#define ISDNPBX_PID_FILE				ISDNPBX_WORK_ROOT"/vpbx.pid"

#define DEBUG_ASSIST_DEVICE			1	/* debug info for /dev/asstel and the associated */			

//as_span_t  *as_check_hardware(const char *name, assist_app_type apptype, int law);

#define GAIN_NO_CHANGE			0.0		
#define GAIN_2_TIMES			6.0  		/* 6 DB is 'volume*2' */
#define GAIN_3_TIMES			9.542
#define GAIN_4_TIMES			12.0		/* 12 DB is 'volume*4' */
#define GAIN_8_TIMES			18.06
/* law is defined in  common_globals.h */
void  as_dsp_set_gain(int fd, int law, float rx_gain_db, float tx_gain_gb );

//#include "assist_dsp_dtmf.h"

#define TRACE	printf("%s:%s:%d\r\n", __FILE__, __FUNCTION__, __LINE__)
 
#define VPBX_NAME_LENGTH			64

#if WITH_DEBUG
	#define DEBUG_BUFFER_SIZE		1024
#endif

#define MAX_ACCESS_CODE_LENGTH			6

#define  AS_DEVICE_NOT_DEFINED				-1


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <errno.h>

#ifdef __cplusplus
	};
#endif


#endif

