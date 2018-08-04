/*
* $Id: cgi_video.h,v 1.7 2007/02/07 19:04:16 lizhijie Exp $
*/
/* pages for Video */

#ifndef	__CGI_VIDEO_H__
#define	__CGI_VIDEO_H__

//#include "ascam.h"
#include "g_list.h"

#define	CGI_VIDEO_OP_SETUP_AUDIO					"opAudio"		/*  */
#define	CGI_VIDEO_OP_SETUP_AUDIOSR				"opAudiosr"
#define	CGI_VIDEO_OP_SETUP_AUDIOCH				"opAudioch"
#define	CGI_VIDEO_OP_STREAM_DESC				"opstreamdesc"
#define	CGI_VIDEO_OP_STREAM_CFG					"opstreamcfg"
#define	CGI_VIDEO_OP_ENAUTH						"openauth"
#define	CGI_VIDEO_OP_WATCHDOG					"opwatchdog"
#define	CGI_VIDEO_OP_MEDIASERVER_LOG			"opmediaserverlog"
#define	CGI_VIDEO_OP_ACL							"opACL"
/* BUTTON control in HTML form */
#define	CGI_VIDEO_OP_SETUP_SHARPNESS			"opSharp"		/*  */
#define	CGI_VIDEO_OP_SETUP_HUE					"opHue"			/*  */
#define	CGI_VIDEO_OP_SETUP_SATURATION			"opSaturation"		/*  */
#define	CGI_VIDEO_OP_SETUP_CONTRAST				"opContrast"		/*  */
#define	CGI_VIDEO_OP_SETUP_BRIGHTNESS			"opBright"		/*  */
/* SELECT control in HTML form */
#define	CGI_VIDEO_OP_SETUP_FRAMERATE			"opFrameRate"		/*  */
#define	CGI_VIDEO_OP_SETUP_BITRATE				"opBitRate"		/*  */
#define	CGI_VIDEO_OP_SETUP_RESOLUTION			"opResolution"		/*  */
#define	CGI_VIDEO_OP_SETUP_FLICKER				"opFlicker"		/*  */
#define	CGI_VIDEO_OP_SETUP_QUALITY				"opQuality"		/*  */

#define	CGI_VIDEO_OP_SETUP_MIRROR				"opMirror"		/*  */
#define	CGI_VIDEO_OP_SETUP_VFLIP					"opVFlip"			/*  */

#define	CGI_VIDEO_OP_VIEW							"VideoView"
#define	CGI_VIDEO_OP_RECORD						"VideoRecord"
#define	CGI_VIDEO_OP_CAPTURE						"VideoCapture"

#define	CGI_VIDEO_OP_STATUS						"VideoStatus"		/* record status*/
#define	CGI_VIDEO_OP_MOTION_DETECT				"VideoMDetect"		/* record status*/

#define	CGI_VIDEO_MSG_OP_TYPE					"VideoMsgType"
#define	VIDEO_KEYWORD_AUDIO_ENABLE				"AudioCodec"
#define	VIDEO_KEYWORD_AUDIO_SAMLPERATE			"AudioSampleRate"
#define	VIDEO_KEYWORD_AUDIO_CHANNELS			"AudioChannels"
#define	VIDEO_KEYWORD_AUDIO_ENAUTH				"EnableAuth"
#define	VIDEO_KEYWORD_AUDIO_WATCHDOG			"EnableWd"
#define	VIDEO_KEYWORD_MEDIASERVER_LOG			"DebugLevel"
#define	VIDEO_KEYWORD_ACL						"ACL"

#define	VIDEO_KEYWORD_ONBOOT					"VIDEO_ONBOOT"
#define	VIDEO_KEYWORD_CAMERA_NUM				"CAMERA_NUM"
#define	VIDEO_KEYWORD_CAMERA_RESOLUTION		"CAMERA_RESOLUTION"
#define	VIDEO_KEYWORD_CAMERA_QUALITY			"CAMERA_QUALITY" 
#define	VIDEO_KEYWORD_CAMERA_FLICKER			"CAMERA_FLICKER"  
#define	VIDEO_KEYWORD_CAMERA_BITRATE			"VideoBitRate"//"CAMERA_BITRATE"
#define	VIDEO_KEYWORD_CAMERA_FRAMERATE		"VideoFrameRate"//"CAMERA_FRAMERATE"
#define	VIDEO_KEYWORD_CAMERA_NETWORK_PORT	"CAMERA_NETWORK_PORT"
#define	VIDEO_KEYWORD_CAMERA_ONBOOT			"CAMERA_ONBOOT"

#define	VIDEO_KEYWORD_CAMERA_RECORD			"CAMERA_RECORD"
#define	VIDEO_KEYWORD_CAMERA_FILE_DURATION	"CAMERA_FILE_DURATION"
#define	VIDEO_KEYWORD_CAMERA_VIDEO_DATA_DIR	"VIDEO_DATA_DIR"


#define	VIDEO_KEYWORD_MOTION_ENABLE			"MotionDetectEnabled"
#define	VIDEO_KEYWORD_SENSITIVITY				"Sensity"
#define	VIDEO_KEYWORD_THRESHOLD					"Threshold"


#define	VIDEO_KEYWORD_MAIL_ENABLE				"SMTPEnabled"
#define	VIDEO_KEYWORD_MAIL_RECIPIENTS			"MailRecipients"
#define	VIDEO_KEYWORD_MAIL_SERVER				"MailServer"
#define	VIDEO_KEYWORD_MAIL_USER					"MailUser"
#define	VIDEO_KEYWORD_MAIL_PASSWD				"MailPassword"
#define	VIDEO_KEYWORD_MAIL_SUBJECT				"MailSubject"

#define	VIDEO_KEYWORD_FTP_ENABLE				"FTPEnabled"
#define	VIDEO_KEYWORD_FTP_SERVER				"FTPServer"
#define	VIDEO_KEYWORD_FTP_USER					"FTPUser"
#define	VIDEO_KEYWORD_FTP_PASSWD				"FTPPassword"
#define	VIDEO_KEYWORD_FTP_REMOTE_PATH			"FTPDirectory"

#define VIDEO_ACTIVEX_VERSION                   1,0,0,4



#define	VIDEO_KEYWORD_CAMERA_DATA_HOME		"camera"		/* reference to admin_video.conf */

#define	VIDEO_VALUE_ON							CGI_STR_YES_CASE
#define	VIDEO_VALUE_OFF							CGI_STR_NO_CASE

#define	VIDEO_DEV_KEY_RESOLUTION					"Resolution"

#define	VIDEO_RPOCESS_SERVER						"camserv"
#define	VIDEO_RPOCESS_RECORDER					"camrecord"

#if 0
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
#endif

typedef	struct
{
	char					name[64];
	
	int					isPlugged;
	int					isDriverLoad;
	
	llist 					cgiVariables;
	
	llist					globalCfgs;
	llist				        global2Cfgs;
	llist				        recordCfgs;
	llist					motionCfgs;
	glist					*aviFiles;
	
//	cgi_camera_engine_t	mgr;
}VIDEO_INFO;

int	videoView(VIDEO_INFO *info);
int	videoRecordStatus(VIDEO_INFO *info);

int cgi_camera_save_config(VIDEO_INFO *info);

#endif

