/*
* $Id: SptrTest02.cxx,v 1.2 2007/03/02 07:29:49 lizhijie Exp $
*/
#include "Sptr.hxx"

// test 2 -- this is construction of dual "parallel" Sptrs that would happen
// using the constructor

int main()
{
    /// test items

    int* a;
    a = new int;

    Sptr < int > x(a);
    Sptr < int > y(a);

    return 0;
}
