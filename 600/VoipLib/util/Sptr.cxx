/*
 * $Id: Sptr.cxx,v 1.2 2007/03/01 20:04:32 lizhijie Exp $
 */

#include "global.h"
#include "Sptr.hxx"
#ifdef __linux__
#include <execinfo.h>
#endif
#include <stdio.h>

#define SPTR_TRACEBACK_RUNNING 0
/// this C file is only used for debugging, so it is totally ifdef'ed

#define SPTR_DEBUG_ACTIVE 0

#if SPTR_DEBUG

// static item
SptrDebugger* SptrDebugger::impl_ = 0;

SptrDebugger::SptrDebugger()
        : ptrsInUse(),
        mutex()
{}


SptrDebugger* SptrDebugger::getInstance()
{
    if (!impl_)
    {
        impl_ = new SptrDebugger;
    }
    assert(impl_);
    return impl_;
}


void SptrDebugger::newPtr(void* ptr)
{
#if SPTR_DEBUG_ACTIVE
    if (ptr == 0)
    {
        // do nothing for the null ptr.
        return ;
    }

    PtrMap::iterator i;

    getInstance()->mutex.lock();
    i = getInstance()->ptrsInUse.find(ptr);
    if (i != getInstance()->ptrsInUse.end())
    {
        // check to see if it's set to inUse

        if (i->second)
        {
            cerr << "pointer " << ptr
            << " already in a different Sptr" << endl;
            assert(0);
        }
    }
    getInstance()->ptrsInUse[ptr] = true;
    getInstance()->mutex.unlock();
#endif
}


void SptrDebugger::deletePtr(void* ptr)
{
#if SPTR_DEBUG_ACTIVE
    PtrMap::iterator i;

    getInstance()->mutex.lock();
    i = getInstance()->ptrsInUse.find(ptr);
    if (i == getInstance()->ptrsInUse.end())
    {
        cerr << "pointer " << ptr
        << " is not in any Sptr" << endl;
        assert(0);
    }
    else
    {
        if (i->second == false)
        {
            cerr << "pointer " << ptr
            << " was already deleted by another Sptr" << endl;
            assert(0);
        }
        else
        {
            getInstance()->ptrsInUse[ptr] = false;
        }
    }
    getInstance()->mutex.unlock();
#endif
}

#endif


#if SPTR_TRACEBACK

#if SPTR_TRACEBACK_RUNNING


#ifdef __linux__

class SptrTraceback
{
    public:
        SptrTraceback();
        SptrTraceback(const SptrTraceback& x);
        SptrTraceback(void* sptr);

        bool operator==(const SptrTraceback& x) const;
        SptrTraceback& operator=(const SptrTraceback& x);

        void dump(FILE* f);

    private:

        void* sptr;
        void *my_backtrace[50];
        int backtrace_size;
};

SptrTraceback::SptrTraceback()
        : sptr(0),
        backtrace_size(0)
{
}



SptrTraceback::SptrTraceback(const SptrTraceback& x)
        : sptr(x.sptr),
        backtrace_size(x.backtrace_size)
{
    for (int i = 0; i < backtrace_size; i++)
    {
        my_backtrace[i] = x.my_backtrace[i];
    }
}


SptrTraceback::SptrTraceback(void* sptr)
        : sptr(sptr),
        backtrace_size(0)
{
    backtrace_size = backtrace(my_backtrace, 50);
}


bool SptrTraceback::operator==(const SptrTraceback& x) const
{
    return sptr == x.sptr;
}


SptrTraceback& SptrTraceback::operator=(const SptrTraceback& x)
{
    sptr = x.sptr;
    backtrace_size = x.backtrace_size;
    for (int i = 0; i < backtrace_size; i++)
    {
        my_backtrace[i] = x.my_backtrace[i];
    }
    return *this;
}


void SptrTraceback::dump(FILE* f)
{
    fprintf(f, "  %p:", sptr);
    for (int i = 0; i < backtrace_size; i++)
    {
        fprintf(f, " %p", my_backtrace[i]);
    }
    fprintf(f, "\n");
}


struct ptr_less : public binary_function < void*, void*, bool >
{
    bool operator()(void* const & x, void* const & y) const
    {
        return ((unsigned int) x) < ((unsigned int) y);
    }
};


//////////////////////////////////////////////////////////////////////

class SptrTracebackPack
{
    public:
        SptrTracebackPack();
        SptrTracebackPack(const SptrTracebackPack& x);
        SptrTracebackPack(void* myPtr, void* sptr);

        void insert(void* sptr);
        void clear(void* sptr);

        void dump(FILE* f);

    private:
        void* ptr;
        map < void*, SptrTraceback, ptr_less > tracebacks;
};


SptrTracebackPack::SptrTracebackPack(void* myPtr, void* sptr)
        : ptr(myPtr),
        tracebacks()
{
    insert(sptr);
}


SptrTracebackPack::SptrTracebackPack()
        : ptr(0),
        tracebacks()
{
}



SptrTracebackPack::SptrTracebackPack(const SptrTracebackPack& x)
        : ptr(x.ptr),
        tracebacks(x.tracebacks)
{
}



void SptrTracebackPack::insert(void* sptr)
{
    SptrTraceback t(sptr);
    tracebacks[sptr] = t;
}


void SptrTracebackPack::clear(void* sptr)
{
    tracebacks.erase(tracebacks.find(sptr));
}


void SptrTracebackPack::dump(FILE* f)
{
    map < void*, SptrTraceback, ptr_less > ::iterator i = tracebacks.begin();

    fprintf(f, "%p>\n", ptr);

    while (i != tracebacks.end())
    {
        i->second.dump(f);
        ++i;
    }
}


typedef map < void*, SptrTracebackPack > TraceMap;

static TraceMap tracemap;


void sptrDebugMarkTraceback(void* sptr, void* ptr)
{
    TraceMap::iterator i;
    if(ptr)
    {
	i = tracemap.find(ptr);
	if (i != tracemap.end())
	{
	    i->second.insert(sptr);
	}
	else
	{
	    tracemap[ptr] = SptrTracebackPack(ptr, sptr);
	}
    }
}


void sptrDebugClearTraceback(void* sptr, void* ptr)
{
    TraceMap::iterator i;
    if(ptr)
    {
	i = tracemap.find(ptr);
	if (i != tracemap.end())
	{
	    i->second.clear(sptr);
	}
	else
	{
	    assert(0);
	}
    }
}


void sptrDebugDumpTraceback(char* filename)
{
    // open file, and dump
    FILE* f;
    f = fopen(filename, "w");
    if (!f)
    {
        return ;
    }

    TraceMap::iterator i = tracemap.begin();
    while (i != tracemap.end())
    {
        // dump the record
        i->second.dump(f);
        ++i;
    }
    fclose(f);
}

#else

// no linux -- these functions do nothing

void sptrDebugMarkTraceback(void* sptr, void* ptr)
{
}



void sptrDebugClearTraceback(void* sptr, void* ptr)
{
}



void sptrDebugDumpTraceback(char* filename)
{
}

// #if __linux__

#endif
// #if __linux__

// #if SPTR_TRACEBACK_RUNNING
#else
// #if SPTR_TRACEBACK_RUNNING


void sptrDebugMarkTraceback(void* sptr, void* ptr)
{
}



void sptrDebugClearTraceback(void* sptr, void* ptr)
{
}



void sptrDebugDumpTraceback(char* filename)
{
}


// #if SPTR_TRACEBACK_RUNNING

#endif
// #if SPTR_TRACEBACK_RUNNING

// #if SPTR_TRACEBACK
#endif
// #if SPTR_TRACEBACK
