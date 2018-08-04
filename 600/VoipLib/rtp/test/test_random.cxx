/*
* $Id: test_random.cxx,v 1.2 2007/03/02 07:29:48 lizhijie Exp $
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
