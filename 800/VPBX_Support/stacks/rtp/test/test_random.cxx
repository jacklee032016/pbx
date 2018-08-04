/*
* $Log: test_random.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: test_random.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/
#include <iostream.h>
#include <stdlib.h>
#ifdef __vxworks
#include "netinet/ppp/random.h"
#include <taskLib.h>
#endif

int main ()
{
    /*
    #ifdef __vxworks
        srand (taskIdSelf());
    #else
        srandom (getpid());
    #endif
    */

    cout << random();
    cout << endl;
    return 0;
}
