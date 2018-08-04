/*
* $Id: cgi_video.h,v 1.7 2007/02/07 19:04:16 lizhijie Exp $
*/
/* pages for Video */

#ifndef	__CGI_VIDEO_H__
#define	__CGI_VIDEO_H__

#include "ascam.h"
#include "g_list.h"

#define	CGI_VIDEO_OP_SETUP						"VideoSetup"		/* select MAC mode: AP/AdHoc/Station */
#define	CGI_VIDEO_OP_VIEW							"VideoView"
#define	CGI_VIDEO_OP_STATUS						"VideoStatus"		/* record status*/

#define	VIDEO_KEYWORD_ONBOOT					"VIDEO_ONBOOT"
#define	VIDEO_KEYWORD_CAMERA_NUM				"CAMERA_NUM"
#define	VIDEO_KEYWORD_CAMERA_RESOLUTION		"CAMERA_RESOLUTION"
#define	VIDEO_KEYWORD_CAMERA_NETWORK_PORT	"CAMERA_NETWORK_PORT"
#define	VIDEO_KEYWORD_CAMERA_ONBOOT			"CAMERA_ONBOOT"

#define	VIDEO_KEYWORD_CAMERA_RECORD			"CAMERA_RECORD"
#define	VIDEO_KEYWORD_CAMERA_FILE_DURATION	"CAMERA_FILE_DURATION"
#define	VIDEO_KEYWORD_CAMERA_VIDEO_DATA_DIR	"VIDEO_DATA_DIR"

#define	VIDEO_KEYWORD_CAMERA_DATA_HOME		"camera"		/* reference to admin_video.conf */

#define	VIDEO_VALUE_ON							CGI_STR_YES_CASE
#define	VIDEO_VALUE_OFF							CGI_STR_NO_CASE

#define	VIDEO_DEV_KEY_RESOLUTION					"Resolution"

#define	VIDEO_RPOCESS_SERVER						"camserv"
#define	VIDEO_RPOCESS_RECORDER					"camrecord"

#define	CGI_VIDEO_PROC_HOME				ASCAM_PROC_HOME"/"ASCAM_PROC_DIRNAME
#define	CGI_VIDEO_PROC_ENGINE_NAME		CGI_VIDEO_PROC_HOME"/"ASCAM_PROC_ENGINE
#define	CGI_VIDEO_PROC_DEVICE_NAME		CGI_VIDEO_PROC_HOME"/"ASCAM_PROC_VIDEO

typedef	struct
{
	char					name[ASCAM_NAME_LENGTH];
	char					sensor[ASCAM_NAME_LENGTH];

	char					format[ASCAM_NAME_LENGTH];
	char					status[ASCAM_NAME_LENGTH];

	int					resolutionCount;
	llist					resolutions;
}cgi_camera_t;

typedef	struct
{
	char					name[ASCAM_NAME_LENGTH];
	char					version[128];

	int 					count;
	glist					*cameras;
	
}cgi_camera_engine_t;

typedef	struct
{
	char					name[64];
	
	int					isPlugged;
	int					isDriverLoad;
	
	llist 					cgiVariables;
	
	llist					globalCfgs;

	glist					*aviFiles;
	
	cgi_camera_engine_t	mgr;
}VIDEO_INFO;

int	videoView(VIDEO_INFO *info);
int	videoRecordStatus(VIDEO_INFO *info);

#endif

