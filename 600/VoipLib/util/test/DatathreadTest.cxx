/*
* $Id: DatathreadTest.cxx,v 1.2 2007/03/02 07:29:49 lizhijie Exp $
*/
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include "cpLog.h"
//#include "PepAgent.hxx"
#include "VThreadPool.hxx"
//#include "CommandLine.hxx"
#include "Data.hxx"
Data testdata = "RED WINGS RULES!!!";

void loopingWriter(void*)
{
    while(1)
    {
	Data dupdata = testdata;
	LocalScopeAllocator lo;
        cout << "writer thread" << pthread_self() << dupdata.getData(lo) << endl;
        dupdata = "";
        dupdata += "RED WINGS RULES!!!";
        cout << "writer thread" << pthread_self() << dupdata.getData(lo) << endl;
    }
}


void loopingBadWriter(void *)
{
    while (1)
    {
        testdata = "";
        testdata += "RED WINGS RULES!!!";
	LocalScopeAllocator lo;
        cout << "writer thread " << pthread_self() << testdata.getData(lo) << endl;
    }
}


void readerThread(void*)
{
    //    cpDebug(LOG_DEBUG, "Reader thread (%d)", pthread_self());
    int i = 5;
    while (i)
    {
	LocalScopeAllocator lo;
        cout << "reader thread" << pthread_self() << testdata.getData(lo) << endl;
    }
}


void writerThread(void *)
{
    int i = 5;
    while (i)
    {
        testdata = "";
        testdata += "RED WINGS RULES!!!";
	LocalScopeAllocator lo;
        cout << "writer thread " << pthread_self() << testdata.getData(lo) << endl;
    }
}

int
main(int argc, const char** argv)
{
    try
    {
        VThreadPool pool(5);


	VFUNC function = loopingWriter;

	
        VFunctor functor1(function, NULL);
        pool.addFunctor(functor1);

        VFunctor functor2(function, NULL);
        pool.addFunctor(functor2);

        VFunctor functor3(function, NULL);
        pool.addFunctor(functor3);

        VFunctor functor4(function, NULL);
        pool.addFunctor(functor4);

        VFunctor functor5(function, NULL);
        pool.addFunctor(functor5);


        while (true)
        {
            sleep(1);
        }
    }
    catch (VException& e)
    {
        cerr << "Caught exception:" << e.getDescription() << endl;
        exit( -1);
    }
}
