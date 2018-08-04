/*
* $Id: CountSemaphoreTest.cxx,v 1.2 2007/03/02 07:29:49 lizhijie Exp $
*/
#include "CountSemaphore.hxx"
#include <iostream>
#include "Verify.hxx"
#include "ThreadIf.hxx"

using namespace std;



void test()
{
    bool r;

    CountSemaphore s;
    assert( s.compare(0) );

    s.increment();
    assert( s.compare(1) );

    s.increment();
    assert( s.compare(2) );

    s.increment();
    assert( s.compare(3) );

    r = s.decrement();
    assert( s.compare(2) );
    assert( !r );

    r = s.decrement();
    assert( s.compare(1) );
    assert( !r );

    r = s.decrement();
    assert( s.compare(0) );
    assert( r );

    void* p1 = new char[50];
    void* p2 = new char[50];
    void* p3 = p1;
    void* p4 = p2;

    cout << p1 << endl;
    cout << p2 << endl;
    cout << p3 << endl;
    cout << p4 << endl;

    s.exchange(&p1, &p2);

    cout << p1 << endl;
    cout << p2 << endl;
    cout << p3 << endl;
    cout << p4 << endl;

    assert(p2 == p3);
    assert(p1 == p4);


}

CountSemaphore share;

void test2()
{
}

class T2 : public ThreadIf
{
    public:
        void thread()
        {
    for(int i = 0 ; i < 10000000; ++i)
    {
        share.increment();
        share.decrement();
    }
    }
};

int share2 = 0;

class T3 : public ThreadIf
{
    public:
        void thread()
        {
    for(int i = 0 ; i < 10000000; ++i)
    {
        share2++;
        share2--;
    }
    }
};


int main()
{
    test();
    assert(share.compare(0) == 1);
    cout << "pass" << endl;
    {
	T2 t1;
	T2 t2;
	T2 t3;
	T2 t4;
	t1.run();    
	t2.run();
	t3.run();
	t4.run();
	t1.join();    
	t2.join();    
	t3.join();    
	t4.join();    
    }

    assert(share2 == 0);
    {
	T3 t1;
	T3 t2;
	T3 t3;
	T3 t4;
	t1.run();    
	t2.run();
	t3.run();
	t4.run();
	t1.join();    
	t2.join();    
	t3.join();    
	t4.join();    
    }
    if(share2 != 0)
    {
	cout << "unprotected int corrupted: " << share2 << endl;
    }
    assert(share.compare(0) == 1);
    return 0;
}
