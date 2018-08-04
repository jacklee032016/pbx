/***************************************************************************
 $Author: lizhijie $
 $Log: g_list.h,v $
 Revision 1.1.1.1  2006/11/29 17:06:59  lizhijie
 AS600 WebAdmin

 Revision 1.2  2006/10/22 17:14:39  lizhijie
 dd

 Revision 1.1.1.1  2006/09/14 07:57:26  lizhijie
 remove locale engine based on multi-directories

 Revision 1.1.1.1  2006/08/09 17:47:35  lizhijie
 rebuild

 Revision 1.1.1.1  2006/07/13 13:27:12  lizhijie
 rename it

 Revision 1.1.1.1  2006/07/03 06:32:09  lizhijie
 New WebAdmin

 Revision 1.2  2005/06/16 04:08:09  gongyahui
 enable the reset button

 Revision 1.1  2004/11/01 02:25:36  lizhijie
 Add these files into CVS repository

 Revision 1.1  2004/10/30 01:35:36  lizhijie
 add DHTCP cgi into CVS repository

 $Revision: 1.1.1.1 $
 Gerneric List 
 ***************************************************************************/
#ifndef  __G_LIST_H__
#define __G_LIST_H__

#include <assert.h>
#include <stdlib.h>


#ifndef  OK
#define OK		1
#endif

#ifndef  FAIL
#define FAIL		-1
#endif


typedef struct _glist 
{
	struct _glist *next;

	int tag;
#if 0	
	int size;		/*length of obj*/
#endif	
	void *obj;
}glist;

#if GLIST_DEBUG

#else
int glist_get_length( glist* list);
glist *glist_add(glist  *list, int tag, void *object );
glist *glist_delete(glist *list , int isfree, int tag);
int glist_update(glist *list, int tag, void *new_object );
void *glist_get_data(glist *list, int tag );
void glist_destroy(glist *list, int isfree);
int glist_iterate(glist *list, int (*iterate_func)( void *, void *), void *args);
#endif

#endif

