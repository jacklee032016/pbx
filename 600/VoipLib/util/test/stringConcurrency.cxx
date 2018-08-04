/*
* $Id: stringConcurrency.cxx,v 1.2 2007/03/02 07:29:49 lizhijie Exp $
*/
#include <string>
#include "VThread.hxx"
#include "VMutex.h"
#include "LockHelper.hxx"
#include <stdio.h>
//#include "jprof.h"

class foo
{
    public:
        static foo* instance();
        static VMutex mutex;
        static const char* print();

    protected:
        foo();
        static foo* instance_;
        int x;
        string logLabel;
};

foo* foo::instance_ = 0;
VMutex foo::mutex;

foo::foo()
{
    x = 0;
}

foo*
foo::instance()
{
    if (!instance_)
    {
        instance_ = new foo;
    }
    return instance_;
}

const char*
foo::print()
{
    LockHelper lock(mutex);

    return foo::instance()->logLabel.c_str();
}



void* thread1(void*)
{
    const char* orig = 0;
    for (int i = 0; i < 1000000; i++)
    {
        const char* x = foo::print();
        if (orig != x)
        {
            orig = x;
            printf("%p\n", orig);
        }
        if (*x != '\0')
        {
            printf("not null!\n");
        }
        if (strlen(x) != 0)
        {
            printf("bad!\n");
        }
    }
    return 0;
}

void* thread2(void*)
{
    const char* orig = 0;
    for (int i = 0; i < 1000000; i++)
    {
        const char* x = foo::print();
        if (orig != x)
        {
            orig = x;
            printf("%p\n", orig);
        }
        if (*x != '\0')
        {
            printf("not null!\n");
        }
        if (strlen(x) != 0)
        {
            printf("bad!\n");
        }
    }
    return 0;
}


int main()
{
    //    setupProfilingStuff();

    string junk;

    VThread my_thread1;
    VThread my_thread2;

    my_thread1.spawn(thread1, 0);
    my_thread2.spawn(thread2, 0);

    for (int i = 0; i < 1000000000; i++);

    my_thread1.join();
    my_thread2.join();

    //    DumpAddressMap();
    //    finish();
    return 0;
}
