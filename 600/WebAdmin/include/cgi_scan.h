/*
* $Id: cgi_scan.h,v 1.6 2007/03/05 08:51:11 lizhijie Exp $
*/
/* pages for Scanner */

#ifndef	__CGI_SCAN_H__
#define	__CGI_SCAN_H__

#include "ascam.h"
#include "g_list.h"

#define	CGI_SCAN_OP_SETUP						"ScanSetup"		/* set attributes of scan */
#define	CGI_SCAN_OP_SCAN						"ScanScan"		/* begin to scanning and save on disk */
#define	CGI_SCAN_OP_PREVIEW					"ScanPreview"		/* load image to broswer after scanned */
#define	CGI_SCAN_OP_CONVERT					"ScanFormat"		/* change raw image from pnm to jpeg */
#define	CGI_SCAN_OP_IMAGE					"ScanImage"		/* load image to broswer after scanned */
#define	CGI_SCAN_OP_SAVE						"ScanSave"		/* save file into hd disk */


#define	SCAN_KEYWORD_FORMAT					"SCAN_FORMAT"
#define	SCAN_KEYWORD_DATA_DIR				"SCAN_DATA_DIR"
#define	SCAN_KEYWORD_FILENAME				"FILE_NAME"
#define	SCAN_KEYWORD_SVAE_CONFIG			"SVAE_CONFIG"

#define	SCAN_KEYWORD_MODE					"mode"
#define	SCAN_KEYWORD_RESOLUTION				"resolution"
#define	SCAN_KEYWORD_DEPTH					"depth"

#define	SCAN_KEYWORD_LEFT					"tl_x"
#define	SCAN_KEYWORD_TOP						"tl_y"
#define	SCAN_KEYWORD_RIGHT					"br_x"
#define	SCAN_KEYWORD_BOTTOM					"br_y"

#define	SCAN_KEYWORD_MODE_COLOR			"Color"
#define	SCAN_KEYWORD_MODE_GRAY				"Gray"
#define	SCAN_KEYWORD_MODE_BINARY			"Binary"

#define	SCAN_KEYWORD_FORMAT_TIFF			"tiff"
#define	SCAN_KEYWORD_FORMAT_PNG			"png"
#define	SCAN_KEYWORD_FORMAT_GIF				"gif"
#define	SCAN_KEYWORD_FORMAT_JPEG			"jpeg"

#define	SCAN_KEYWORD_FORMAT_PNM			"pnm"

#define	SCAN_VALUE_ON							CGI_STR_YES_CASE
#define	SCAN_VALUE_OFF						CGI_STR_NO_CASE

#define	SCAN_CONFIG_FILE						"/etc/sys/scanner.conf"

#define	SCAN_DEFAULT_DATA_DIR				"/var/data/share/scan"

#define	SANE_SCAN_CMD							"/bin/scan"

#endif

