/***************************************************************************
 $Author: lizhijie $
 $Revision: 1.1.1.1 $
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

