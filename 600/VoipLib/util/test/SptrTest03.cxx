/*
* $Id: SptrTest03.cxx,v 1.2 2007/03/02 07:29:49 lizhijie Exp $
*/
#include "Sptr.hxx"
#include <iostream>
using namespace std;

// test 3 -- this code munches the Sptr with an invalid value, like
// what would happen on a stack corruption.

int main()
{
    /// test items

    int* a = new int;

    Sptr < int > x(a);


    // hack x
    int* y = reinterpret_cast < int* > (&x);

    cout << a << endl;
    cout << (int*) y[0] << endl;
    cout << (int*) y[1] << endl;
    cout << (int*) y[2] << endl;
    y[0] = reinterpret_cast < int > (new int);

    return 0;
}
