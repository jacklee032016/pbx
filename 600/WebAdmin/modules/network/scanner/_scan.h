/*
* $Id: _scan.h,v 1.4 2007/02/07 19:04:16 lizhijie Exp $
*/
#ifndef	___SCAN_H__
#define	___SCAN_H__

#include "cgi_scan.h"
#include "aslib_scan.h"

typedef	struct
{
	char					name[ASCAM_NAME_LENGTH];
	char					bus[ASCAM_NAME_LENGTH];
	char					device[ASCAM_NAME_LENGTH];
	
	char					format[ASCAM_NAME_LENGTH];
	char					status[ASCAM_NAME_LENGTH];

	int					resolutionCount;
	llist					resolutions;
}cgi_scan_t;

typedef	struct
{
	int			x;
	int			y;
	int			width;
	int			height;

	int			dpi;
}cgi_scan_view_t;

typedef	struct
{
	char		*title;
	int		width;
	int		height;
}scan_page_t;

typedef	struct
{
	char					name[64];

	char					filename[256];
	
	int					isPlugged;
	
	llist 					cgiVariables;
	
	llist					globalCfgs;

	cgi_scan_view_t		view;
	cgi_scan_t			scanner;

	AS_SCAN_T			scan;
}SCAN_INFO;

int	scanScan(SCAN_INFO *info);
int	scanPreview(SCAN_INFO *info);
int	scanLoad(SCAN_INFO *info);
int	scanSetup(SCAN_INFO *info);
int	scanConvert(SCAN_INFO *info);
int cgi_scan_save_config(SCAN_INFO *info);

#endif

