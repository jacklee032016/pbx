/*
* $Id$
*/

#ifndef	___VIDEO_H__
#define	___VIDEO_H__

#include "cgiLib.h"
#include "cgi_syscfg.h"
#include "cgi_video.h"
#include "libCmn.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define	MOTION_CONFIG_FILE			"/etc/8120.cfg"
#define	FORM_NAME		"videoForm"
#define	KEY_STREAM		"<Stream"
#define	KEY_FORMAT	"Format"	
#define	KEY_ACL		"ACL"
#define	CRONTASKFILE	"/var/spool/cron/crontabs/root"
#define	OTHEROPTIONSFILE	"/etc/web/otheroptions.conf"

typedef	enum
{
	VIDEO_SELECT_TYPE_QUALITY	=	0,
	VIDEO_SELECT_TYPE_FLICKER,
	VIDEO_SELECT_TYPE_RESOLUTION,
	VIDEO_SELECT_TYPE_BITRATE,
	VIDEO_SELECT_TYPE_FRAMERATE,
	VIDEO_SELECT_TYPE_AUDIOSR,
	VIDEO_SELECT_TYPE_AUDIOCH,
	VIDEO_SELECT_TYPE_ENAUTH,
	VIDEO_SELECT_TYPE_WATCHDOG,
	VIDEO_SELECT_TYPE_MEDIASERVER_LOG,
	VIDEO_SELECT_TYPE_ACL,
	VIDEO_SELECT_TYPE_STREAMSELECT,
	VIDEO_SELECT_TYPE_AUDIO
}VIDEO_SELECT_TYPE_T;

#define	VIDEO_SELECT_TYPE_MAX			(VIDEO_SELECT_TYPE_AUDIO+1)


typedef	enum
{
	VIDEO_BUTTON_TYPE_SHARPNESS	=	0,
	VIDEO_BUTTON_TYPE_HUE,
	VIDEO_BUTTON_TYPE_SATURATION,
	VIDEO_BUTTON_TYPE_CONTRAST,
	VIDEO_BUTTON_TYPE_BRIGHTNESS,
}VIDEO_BUTTON_TYPE_T;

#define	VIDEO_BUTTON_TYPE_MAX			(VIDEO_BUTTON_TYPE_BRIGHTNESS+1)

typedef	enum
{
	VIDEO_S_BUTTON_TYPE_MIRROR	=	0,
	VIDEO_S_BUTTON_TYPE_VFLIP,
}VIDEO_SIMPLE_BUTTON_TYPE_T;


#define	VIDEO_SIMPLE_BUTTON_TYPE_MAX			(VIDEO_S_BUTTON_TYPE_VFLIP+1)


int	videoSimpleButtonsForm(VIDEO_INFO *info, VIDEO_SIMPLE_BUTTON_TYPE_T simpleButtonType);
int	videoButtonsForm(VIDEO_INFO *info, VIDEO_BUTTON_TYPE_T buttonType);
int	videoSelectForm(VIDEO_INFO *info, VIDEO_SELECT_TYPE_T selectType);
int	videoMotionForm(VIDEO_INFO *info );

int	videoActionButton(VIDEO_INFO *info, int msgOpType);
char *video_streamdesc_buttons(VIDEO_INFO *info);
char *video_streamcfg_buttons(VIDEO_INFO *info);
char *video_mediaserverlog_buttons(VIDEO_INFO *info,VIDEO_SELECT_TYPE_T selectType);
char *video_ACL_buttons(VIDEO_INFO *info,VIDEO_SELECT_TYPE_T selectType,VIDEO_SELECT_TYPE_T selectType2);	
int video_streamcfg(VIDEO_INFO *info);
int video_streamdesc(VIDEO_INFO *info);
int video_mediaserverlog(VIDEO_INFO *info,VIDEO_SELECT_TYPE_T selectType);
int video_ACL(VIDEO_INFO *info,VIDEO_SELECT_TYPE_T selectType,VIDEO_SELECT_TYPE_T selectType2);
int  write_list_by_key(llist *list, char *key,char *value,int x);
char *getvalue_list_by_key(llist *list, char *key,int x);
int add_ACL_node(llist *list, char *key,char* aclvalue,int x);
node *__list_newnode(char *key, char *value,char *type, char *rule);
int	get_ACLvalue(VIDEO_INFO *info);
char* ACL_list_info(VIDEO_INFO *info);
node *list_find_node_by_index(llist *list, char *key,int x);
short list_delete_by_index(llist *l, char  *name,int index);
int	media_status(VIDEO_INFO *info);
int evaluate_ACL(VIDEO_INFO *info,char ACLvalue[20][128]);
char* get_IPAddress();
int  effective_record_cfgfile(VIDEO_INFO *info);
int  effective_crontables(VIDEO_INFO *info);
char *str_replace(char string[] ,char old[] ,char new[] ,int lenth);
int remove_task_crontables(int taskindex);
char* autorecord_plan_history_page(VIDEO_INFO *info);
 int  check_cfgdata(VIDEO_INFO *info);
int status_page_adding_deleting(char *target,int delay,char *msg);
int check_inputIP(VIDEO_INFO *info);
  int effective_activeCtrl(char *key,char *value );
  char *Get_init_sensorvalue(char *key);
  int effect_modedata(char *B_value,char *C_value,char *H_value,char *Sa_value,char *Sh_value,char *G_value);
  int Cam_mode_Set(char *value);
  char* get_version();
#endif

