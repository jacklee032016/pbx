#ifndef VTHREAD_VXWORKS_HXX_
#define VTHREAD_VXWORKS_HXX_
/*
* $Id: VThread_vxworks.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#ifdef __vxworks

#include <taskLib.h>
#include <sysLib.h>

// VThread types for VxWorks
typedef int vthread_t;
typedef SEM_ID vmutex_t;
typedef SEM_ID vcondition_t;
typedef unsigned int vthread_attr_t;
//VThread constants for VxWorks
const unsigned long VTHREAD_PRIORITY_VXWORKS_DEFAULT = 150;

/**
 ** Suspend execution of the calling task for the specified number 
 ** of seconds
 */
int
sleep( int seconds );

/**
 ** Suspend execution of the calling task for the specified number 
 ** of micro seconds
 */
int
usleep( int useconds );

#endif

#endif
