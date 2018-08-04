/*
 * $Author: lizhijie $
 * $Id: as_global.h,v 1.2 2007/03/16 19:29:34 lizhijie Exp $
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


