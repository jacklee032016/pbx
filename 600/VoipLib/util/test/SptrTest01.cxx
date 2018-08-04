/*
* $Id: SptrTest01.cxx,v 1.2 2007/03/02 07:29:49 lizhijie Exp $
*/
#include "Sptr.hxx"

// test 2 -- this is construction of dual "parallel" Sptrs that would
// happen using the assignment operator

int main()
{
    /// test items

    int* a;
    Sptr < int > x;
    Sptr < int > y;

    a = new int;

    x = a;
    y = a;

    return 0;
}
