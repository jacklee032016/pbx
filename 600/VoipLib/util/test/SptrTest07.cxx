/*
* $Id: SptrTest07.cxx,v 1.3 2007/03/03 18:38:55 lizhijie Exp $
*/

#include "Sptr.hxx"
#include "Verify.hxx"
#include "VMutex.h"
#include "ThreadIf.hxx"
#include <unistd.h>
#include <list>

class Test
{
    public:
	Test()
	{
            cout << "constructor" << endl;
	}

	~Test()
	{
            cout << "destructor" << endl;
	}
};

Sptr<Test> a;


class Start : public ThreadIf
{
    public:
        void thread()
        {
            while(!isShutdown())
            {
                {
                    list <Sptr <Test> > x;
                    for(int i = 0 ; i < 10000; i++)
                    {
                        x.push_back(a);
                    }
                    assert(!a.getCount()->compare(1));
                }
                if(a.getCount()->compare(1))
                {
                    cout << "ding!" << endl;
                }
            }
        }
    
};


// test 2 -- this is construction of dual "parallel" Sptrs that would happen
// using the constructor


int main()
{
    /// test items

    a = new Test;

    Start w;
    Start x;
    Start y;

    w.run();
    x.run();
    y.run();

    sleep(60);
    w.shutdown();
    x.shutdown();
    y.shutdown();

    w.join();
    x.join();
    y.join();

    test_verify(a.getCount()->compare(1));

    return test_return_code(1);
}


