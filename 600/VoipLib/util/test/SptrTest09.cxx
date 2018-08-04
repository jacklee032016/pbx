/*
* $Id: SptrTest09.cxx,v 1.2 2007/03/02 07:29:49 lizhijie Exp $
*/

#include "Sptr.hxx"
#include <iostream>
#include "Verify.hxx"
#include "ThreadIf.hxx"
#include "Fifo.h"

using namespace std;

using namespace Vocal;

struct X
{
	X()
	{
	    x = new int(10);
	    y.push_back(x);
	}
	Sptr <int> x;
	list < Sptr <int> > y;
};


Fifo < Sptr<X> > fifo;
Fifo < Sptr<X> > fifo2;

class Producer : public ThreadIf
{
    public:
        void thread()
        {
	    for(int i = 0 ; i < 100000; ++i)
	    {
		Sptr<X> p = new X;
		fifo.add(p);
		fifo2.add(p);
                if (i % 10000 == 0)
                    cout << "Producer: " << i << "\n";
	    }
	}

};

class Consumer : public ThreadIf
{
    public:
        void thread()
        {
	    for(int i = 0 ; i < 100000; ++i)
	    {
		Sptr<X> p = fifo.getNext();
                if (i % 1000 == 0)
                    cout << "Consumer: " << i << "\n";
	    }
	}
};

class GC : public ThreadIf
{
    public:
	void thread()
	{
	    for(int i = 0 ; i < 100000; ++i)
	    {
		Sptr< X > p = fifo2.getNext();
		Sptr< int > q = p->x;
		p = 0;
                if (i % 1000 == 0)
                 cout << "GC: " << i << "\n";
	    }
	}
};

int main()
{
    {
	Consumer t1;
	Producer t2;
	GC t3;
	t1.run();    
	t2.run();
	t3.run();
	t1.join();    
	t2.join();    
	t3.join();
    }
    return 0;
}
