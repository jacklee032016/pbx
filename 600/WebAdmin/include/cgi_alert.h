/*
* $Id: cgi_alert.h,v 1.3 2007/03/27 20:22:57 lizhijie Exp $
*/
#ifndef	__CGI_ALERT_H__
#define	__CGI_ALERT_H__

#define	ALERT_OP_STOP_PBX					"AlertStopPbx"
#define	ALERT_OP_RECORD					"AlertRecord"
#define	ALERT_OP_PLAY						"AlertPlay"
#define	ALERT_OP_DELETE					"AlertDelete"
#define	ALERT_OP_ACTIVATE					"AlertActivate"
#define	ALERT_OP_PARAMETER				"AlertParameter"

#define	ALERT_ACTION_FILE					"file"

#define	RECORD_DEVICE_TYPE				"Device_Type"
#define	RECORD_DEVICE_NUMBER				"Device_Fd"
#define	RECORD_FILE_PATH					"File_Path"

/* added for multiple voice files record */
#define	RECORD_FILE_HOME					"FILE_HOME"
#define	RECORD_FILE_NAME					"FILE_NAME"

#define	RECORD_TIME_LENGTH				"Time_Length"
#define	RECORD_START_TIPTONE				"Start_tipTone"
#define	RECORD_END_TIPTONE				"End_tipTone"
#define	RECORD_ACTION_TYPE				"Action_Type"
#define	RECORD_WAIT_TIME					"Wait_Time"

#define	RECORD_ACTION_TYPE_RECORD		"play"
#define	RECORD_ACTION_TYPE_PLAY			"recorder"


#define	UARTD_UARDT_LOGLEVEL				"UARTD_LOG_LEVEL"
#define	UARTD_CGIPBX_LOGLEVEL			"CGIPBX_LOG_LEVEL"
#define	UARTD_SIPUA_STARTUP				"PBX_ONBOOT"


#define	PXB_RECORD_CONF					"/etc/pbx/record.conf"
#define	PXB_UARTD_CONF					"/etc/pbx/uartd.conf"


typedef	struct
{
	int		isRecord;		/* record or play */
	llist		cgiVariables;
	
	llist		cfgsRecord;
	llist		cfgsUart;
}ALERT_INFO;

#endif

