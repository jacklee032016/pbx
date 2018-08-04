/*
* $Id: SptrTest04.cxx,v 1.2 2007/03/02 07:29:49 lizhijie Exp $
*/
#include "Sptr.hxx"
#include <iostream>

using namespace std;

int main()
{
    /// test const behavior

    Sptr < int > x = new int;
    Sptr < const int > y = new int(7);
    const Sptr < const int > z = new int(24);

    *x = 1;
    cout << *y << endl;
    cout << *z << endl;

    y = new int(88);
    cout << *y << endl;

#if 0
    // this is illegal because it attempts to modify a const Sptr
    z = new int(64);
    cout << *z << endl;
#endif 
    return 0;
}
