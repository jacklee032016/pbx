/*
 * $Id: capifs.h,v 1.1.1.1 2006/11/30 16:27:58 lizhijie Exp $
 */

void capifs_new_ncci(char type, unsigned int num, kdev_t device);
void capifs_free_ncci(char type, unsigned int num);
