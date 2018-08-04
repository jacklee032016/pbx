/*
* $Author: lizhijie $
* $Revision: 1.1.1.1 $
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "g_list.h"

#ifdef DMALLOC
#include <dmalloc.h>
#endif

int glist_get_length( glist  *list)
{
	glist *tmp;
	int i=0;

	tmp=list;

	while( tmp )
	{
		i++;
#if TEST	
		trace
#endif
		tmp = tmp->next;
#if TEST	
		trace
#endif
	}
	return i;
}

glist *glist_add(glist  *list, int tag, void *object )
{
	glist  *new_item;
	glist  *ptmp=list;

	new_item = (glist *)malloc(sizeof(glist) );
	if(new_item)
	{
		memset(new_item, 0, sizeof(glist));
		new_item->obj = object;
		new_item->tag = tag;
		new_item->next=NULL;

		if(!ptmp)
		{
			return new_item;
		}
		while(ptmp->next)
		{
			ptmp=ptmp->next;
		}
		ptmp->next=new_item;
		return list;
#if 0
		if( list)
		{
#if TEST	
		trace
#endif
			new_item->next = list;
//			*list = new_item;
		}
			return new_item;
#endif
	}
	else
	{
		fprintf(stderr, "No mem in %s\r\n", __FUNCTION__);
		exit(1);
	}

	return NULL;
}


int glist_update(glist *list, int tag, void *new_object )
{
	int result = FAIL;
	glist  *tmp = list;

	while(tmp)
	{
		if( tmp->tag==tag)
		{
			tmp->obj = new_object;
			result = OK;
		}
		tmp = tmp->next;
	}

	return result;
}

void *glist_get_data(glist *list, int tag )
{
	glist  *tmp = list;

	while( tmp )
	{
		if(tmp->tag == tag)
		{
			return tmp->obj;
		}
		tmp = tmp->next;
	}
	
	return NULL;
}

/* isfree is not 0, then free the object in every node , return next one*/
glist *glist_delete(glist *list , int isfree, int tag)
{ 
	glist  *tmp = NULL, *last, *ptmp;

	tmp = list;
	last = list;
	while( tmp)
	{
		if( tmp->tag == tag )
		{
			if(list == tmp)
				list = tmp->next;
			else
				last->next = tmp->next;
			
//following section: when delete the current node, the tag of node after it should got decrement
			ptmp=tmp->next;
			while(ptmp)
			{
				ptmp->tag--;
				ptmp=ptmp->next;
			}
//			
			if(isfree)
			{
				free( tmp->obj);
				tmp->obj = NULL;
			}	
			free( tmp );
			tmp = NULL;
			
			return list ;
		}
		last = tmp;
		tmp = tmp->next;	
	}

	return list;
}

glist *glist_destroy_this(glist *list , int isfree  )
{ 
	glist *next = NULL;

	if(list)
	{
		next = list->next;
		if(list->obj && isfree )
		{
			free(list->obj);
			list->obj = NULL;
		}
		free(list);
		list = NULL;	
	}

	return next;
}


void glist_destroy(glist *list, int isfree)
{
	glist *next;
	
	if( !list)
		return;

	next = list;
	do
	{
#if TEST	
		trace
		printf("value=%s\r\n", (char *)next->obj);
#endif		
		next = glist_destroy_this(next, isfree);
	}while(next);
		
//	list = NULL;
}

/* The iteration is not continue when iterate_funct return is not 0 */
int glist_iterate(glist *list, int (*iterate_func)( void *, void *), void *args)
{
	glist *tmp;
	int result = 0;
	
	for(tmp = list; 0==result && tmp; tmp=tmp->next)
	{
		result = (*iterate_func)( tmp, args/* &(tmp->tag)*/);
	}

	return result;
}

#if TEST
int glist_debug(void *obj, void *arg)
{
	glist *node = (glist *)obj;
	printf("value=%s\r\n",  (char *)node->obj);
	return 0;
}

int main(int argc, int argv)
{
	char *a="12",*b="34", *c= "56";

	glist *list = NULL;

	list = glist_add( list, 1, a);
	printf("count =%d\r\n", glist_get_length(list));
	list = glist_add(list, 2, b);
	list = glist_add(list, 3, c);

	printf("count =%d\r\n", glist_get_length(list));

	glist_iterate(list, glist_debug, NULL);

	glist_update(list, 1, "abcd");
	
	glist_iterate(list, glist_debug, NULL);
	list = glist_delete(list , 0, 2);
	printf("after delete, count =%d\r\n", glist_get_length(list));
	glist_iterate(list, glist_debug, NULL);
	glist_destroy(list, 0);
	return 0;
}
#endif

