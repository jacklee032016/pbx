/*
* $Id: as_utils.h,v 1.8 2007/03/25 08:32:47 lizhijie Exp $
*/

#ifndef  __AS_UTILS_H__
#define  __AS_UTILS_H__

#include <stdio.h>

#define AS_LOG_0      (16<<3) /* facility not used */
#define AS_LOG_1      (17<<3) /* facility not used */
#define AS_LOG_2      (18<<3) /* facility not used*/
#define AS_LOG_3      (19<<3) /* facility not used */
#define AS_LOG_4      (20<<3) /* facility for cdr */
#define AS_LOG_5      (21<<3) /* facility for sipserver */
#define AS_LOG_6      (22<<3) /* facility for cgi */
#define AS_LOG_7      (23<<3) /* facility for pbx */

#define AS_LOG_UNKNOW   0   /* facility for unknow */

typedef unsigned int log_facility_t;

typedef enum
{
	USE_SYSLOG = 0,		/* log messages to syslog */
	USE_CONSOLE ,		/* log messages to console */
	USE_FILE			/* log messages into a normal disk file */

}log_style_t;

//#ifdef LOG_EMERG
#if 1
	typedef enum
	{
		AS_LOG_EMERG = 0,		/* system is unusable */
		AS_LOG_ALERT,			/* action must be taken immediately */
		AS_LOG_CRIT ,			/* critical conditions */
		AS_LOG_ERR , 			/* error conditions */
		AS_LOG_WARNING ,		/* warning conditions */
		AS_LOG_NOTICE ,		/* normal but significant condition */
		AS_LOG_INFO,			/* informational */
		AS_LOG_DEBUG			/* 7 : debug-level messages */
	}log_level_t;
#else
	typedef enum
	{
		LOG_EMERG = 0,       /* system is unusable */
		LOG_ALERT,       	     /* action must be taken immediately */
		LOG_CRIT ,              /* critical conditions */
		LOG_ERR ,       /* error conditions */
		LOG_WARNING , /* warning conditions */
		LOG_NOTICE ,     /* normal but significant condition */
		LOG_INFO ,   /* informational */
		LOG_DEBUG      /* debug-level messages */
		
	}log_level_t;

#endif

#define	AS_PROGRAM_NAME_LENGTH			64
struct log_object
{
	char			name[AS_PROGRAM_NAME_LENGTH];
	log_style_t 	lstyle;
	log_level_t 	llevel;
	log_facility_t 	lfacility;
	int			isDaemonized;		/* 1 : daemonized; 0 : front-end */
	char			*logFileName;

	FILE			*fp;
};

typedef struct log_object log_stru_t;

#ifndef   __AS_RELEASE__
void log_information(int pri, const char* file, int line, const char* frmt,...);
#else
void log_information(int pri, const char* frmt,...);
#endif

log_level_t get_current_level();

#endif

