/*
* $Id: SptrTest06.cxx,v 1.2 2007/03/02 07:29:49 lizhijie Exp $
*/
#include "Sptr.hxx"
#include <deque>
#include <stdlib.h>

// this is a simple test of the debugger stuff

using namespace std;

struct foo
{
    int a;
};


Sptr < foo > a()
{
    return new foo;
}


Sptr < foo > b()
{
    return new foo;
}


Sptr < foo > c()
{
    return new foo;
}


Sptr < foo > d()
{
    return new foo;
}


deque < Sptr < foo > > fooDeque;

void insert()
{
    int x = rand() % 4;

    switch (x)
    {
        case 0:
        fooDeque.push_back(a());
        break;
        case 1:
        fooDeque.push_back(b());
        break;
        case 2:
        fooDeque.push_back(c());
        break;
        case 3:
        fooDeque.push_back(d());
        break;
    }
}

int main()
{
    srand(5600);

    for (int i = 0; i < 500; i++)
    {
        insert();
    }

#if SPTR_TRACEBACK
    sptrDebugDumpTraceback("traceback.out");
#endif 
    return 0;
}
