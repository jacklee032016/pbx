/*
 * $Author: lizhijie $
 * $Log: as_global.h,v $
 * Revision 1.1.1.1  2006/11/30 16:24:17  lizhijie
 * AS800 SharedLib
 *
 * Revision 1.1.1.1  2006/02/21 09:38:37  wangwei
 * no message
 *
 * Revision 1.1.1.1  2005/05/27 08:43:37  lizhijie
 * add shared library and programs depend on it
 *
 * $Revision: 1.1.1.1 $
 * $Id: as_global.h,v 1.1.1.1 2006/11/30 16:24:17 lizhijie Exp $
*/

#ifndef __AS_GLOBAL_H__
#define __AS_GLOBAL_H__

#define trace		printf("%s[%d]\r\n", __FUNCTION__, __LINE__);


#define AS_OK 0
#define AS_FAIL -1


#define FALSE   0
#define TRUE    (!FALSE)

#ifndef AS_ELAST
#define AS_ELAST 500
#endif


#define AS_MAX_NAME_LENGTH		64

#endif


