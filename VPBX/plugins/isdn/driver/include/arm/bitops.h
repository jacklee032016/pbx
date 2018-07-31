/*
* $Id: bitops.h,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#ifndef  __BITOPS_H__
#define __BITOPS_H__

#ifdef  __cplusplus
	extern "C"
	{
#endif

//__inline__ 
int test_bit(int nr, const void * addr);
//__inline__ 
int test_and_set_bit(int nr, void * addr);
//__inline__ 
int test_and_clear_bit(int nr, void * addr);

#ifdef __cplusplus
	};
#endif

#endif

