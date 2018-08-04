/*
* $Id: big_bitops.c,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
__inline__ int test_bit(int nr, const void * addr)
{
//    return ((unsigned char *) addr)[(nr ^ 0x18) >> 3] & (1U << (nr & 7));

	return (*(int *)addr) & (1<<nr);
}	

__inline__ int test_and_set_bit(int nr, void * addr)
{
	int ret = (*(int *)addr) & (1<<nr);	/* test */
	*(int *)addr |= (1<<nr);				/* set */
	
	return ret;	
}

__inline__ int test_and_clear_bit(int nr, void * addr)
{
	int ret = (*(int *)addr) & (1<<nr);		/* test */
	*(int *)addr &= ( ~(1<<nr) );				/* clear */
	
	return ret;
}
	
