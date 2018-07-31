/*
* $Id: cgi_navigate.h,v 1.11 2007/09/15 19:45:14 lizhijie Exp $
*/

#ifndef	__CGI_NAVIGATE_H__
#define	__CGI_NAVIGATE_H__

#define	NAVIGATE_NAME_LENGTH			128

#define	FOLDER_TREE_NAME					"foldersTree"

//#define	_(text)								(text)

typedef enum
{
	NAVIFATE_TARGET_NEW_WINDOW	= 0,
	NAVIGATE_TARGET_MAIN_FRAME		= 2
}NAVIGATE_TARGET_TYPE;


typedef enum
{
	NAVI_FOLDER_ROOT			=	1,
	NAVI_FOLDER_NETWORK,
	NAVI_FOLDER_SYSTEM,
	NAVI_FOLDER_SOFT_HARDWARE,
	NAVI_FOLDER_UPDTAE,
	NAVI_FOLDER_BACKUP,
	NAVI_FOLDER_RESTORE,

	NAVI_FOLDER_SERVICES,
	NAVI_FOLDER_SERVICES_CONFIG,
	NAVI_FOLDER_SERVICES_DDNS,

	NAVI_FOLDER_RUNTIME,			/* runtime info for our critical tasks */

	NAVI_FOLDER_WIRELESS,

	NAVI_FOLDER_SERVERS,
	NAVI_FOLDER_SERVER_WWW,
	
	NAVI_FOLDER_VIDEO,
	NAVI_FOLDER_VIDEO_CONFIG,
	NAVI_FOLDER_AUDIO_CONFIG,   //wyb
	NAVI_FOLDER_VIDEO_DYNAMIC,
	NAVI_FOLDER_VIDEO_ACTION,

	
	
	NAVI_FOLDER_NOT_EXIST
}FOLDER_ID;

struct _menu_folder
{
	FOLDER_ID					id;
	char							name[NAVIGATE_NAME_LENGTH];

	char							menuName[NAVIGATE_NAME_LENGTH];
	char							iconName[NAVIGATE_NAME_LENGTH];
	char							hReference[NAVIGATE_NAME_LENGTH];

	FOLDER_ID					parentId;

	int							where;
};

struct _menu_item
{
	NAVIGATE_TARGET_TYPE		target;
	
	char							name[NAVIGATE_NAME_LENGTH];
	char							alertMsg[NAVIGATE_NAME_LENGTH];
	char							iconName[NAVIGATE_NAME_LENGTH];

	char							hReference[NAVIGATE_NAME_LENGTH];

	FOLDER_ID					folderId;

	int							where;
};

typedef	struct _menu_folder		NAVIGATE_FOLDER;
typedef	struct _menu_item		NAVIGATE_MENU_ITEM;

#define	MENU_WHERE_BASE			0x00000001
#define	MENU_WHERE_SERVICES		0x00000002
#define	MENU_WHERE_SERVERS		0x00000004
#define	MENU_WHERE_VIDEO			0x00000008
#define	MENU_WHERE_ANYWHERE	0xFFFFFFFF

typedef		enum
{
	WEB_TYPE_BASE = 0,
	WEB_TYPE_SERVICES,
	WEB_TYPE_SERVERS,
	WEB_TYPE_VIDEO,
	WEB_TYPE_INVALIDATE
}WEB_TYPE_T;

/* for navigate.cgi and content.cgi */
typedef	struct	
{
	llist				cgiVariables;
	WEB_TYPE_T		type;

	int				where;
	
}BASE_INFO;

#define	CGI_NAVIGATE_KEYWORD_BASE			"base"
#define	CGI_NAVIGATE_KEYWORD_SERVICES		"services"
#define	CGI_NAVIGATE_KEYWORD_SERVERS		"servers"
#define	CGI_NAVIGATE_KEYWORD_VIDEO			"video"

#endif

