/*
* $Id: ReadWriteLockTEST.cxx,v 1.2 2007/03/02 07:29:49 lizhijie Exp $
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
#include "VRWLock.hxx"
Data testdata = "RED WINGS RULES!!!";

VRWLock myLock;
void readerThread(void*)
{
    //    cpDebug(LOG_DEBUG, "Reader thread (%d)", pthread_self());
    int i = 5;
    while (i)
    {
        myLock.ReadLock();
        cout << "reader thread" << pthread_self() << testdata.getData() << endl;
        myLock.Unlock();
    }
}


void writerThread(void *)
{
    int i = 5;
    while (i)
    {
        myLock.WriteLock();
        testdata = "";
        testdata += "RED WINGS RULES!!!";
        cout << "writer thread " << pthread_self() << testdata.getData() << endl;
        myLock.Unlock();

    }
}

int
main(int argc, const char** argv)
{
    try
    {
        VThreadPool pool(6);
        VFunctor functor(readerThread, NULL);
        pool.addFunctor(functor);

        VFunctor functor1(readerThread, NULL);
        pool.addFunctor(functor1);

        VFunctor functor2(readerThread, NULL);
        pool.addFunctor(functor2);

        VFunctor functor3(readerThread, NULL);
        pool.addFunctor(functor3);

        VFunctor functor4(writerThread, NULL);
        pool.addFunctor(functor4);

        VFunctor functor5(writerThread, NULL);
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
