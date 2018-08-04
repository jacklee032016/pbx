/*
* $Id: test_struct.cxx,v 1.2 2007/03/02 07:29:48 lizhijie Exp $
*/
#include <iostream.h>
#include <stdlib.h>
#include "vtypes.h"

int main ()
{

    struct s1
    {
        unsigned int a ;
        char b;
    };

    struct s2
    {
        char a;
        char b;
    };

    struct s3
    {
    u_int32_t a :
        8;
    u_int32_t b :
        8;
    u_int32_t c :
        8;
    u_int32_t e :
        8;
        char d;
    };

    cout << "struct 1 size =" << sizeof(s1) << endl;
    cout << "struct 2 size =" << sizeof(s2) << endl;
    cout << "struct 3 size =" << sizeof(s3) << endl;
    return 0;
}
