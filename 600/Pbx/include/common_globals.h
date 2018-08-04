#ifndef  __COMMON_GLOBALS_H__
#define __COMMON_GLOBALS_H__
/*
 *$Id: common_globals.h,v 1.2 2007/03/02 04:19:27 lizhijie Exp $
 * common globals used in pbx and assist subsystems both
 */

#define A_LAW_CODE					0
#define U_LAW_CODE					1  /* default code method of our driver */

#define  AS_DEVICE_NOT_DEFINED		-1

/* codec law support in our system, both used in SLIC/PCM and DSP software */
typedef enum
{
	as_g711_a_law = 0,
	as_g711_u_law =1 	
}as_law_type_t;

#define DTMF_DETECT_STEP_SIZE		102

#define AS_DEVICE_NAME				"/dev/astel"
#define AS_DEVICE_MAX				16

#endif

