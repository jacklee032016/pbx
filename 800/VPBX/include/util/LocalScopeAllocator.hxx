#ifndef LOCALSCOPEALLOCATOR_H
#define LOCALSCOPEALLOCATOR_H
/*
* $Id: LocalScopeAllocator.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include <cassert>

/** LocalScopeAllocator allocates memory on the heap and frees it when
 * scope ends.  It is used to provide thread-local buffers to Data.
 *
 */

class LocalScopeAllocator
{

    public:
        ///
        LocalScopeAllocator(): _memArea(0) { };
        ///
        LocalScopeAllocator(int len)
        {
            _memArea = new char[len];
            memset(_memArea, 0, len);
        } 
        ///
        LocalScopeAllocator( char**buf, int len ) 
        {
            *buf = _memArea = new char[len];
            memset(_memArea, 0, len);
        }
        ///
        virtual ~LocalScopeAllocator( )
        {
            delete [] _memArea;
        }
 
        void allocate(int len)
        {
            delete [] _memArea;
            _memArea = new char[len];
            memset(_memArea, 0, len);
        }

        operator const char*() const
        {
            return _memArea;
        }

        operator void*() const
        {
            return _memArea;
        }
    private:
        ///
        char* _memArea;
};

//LOCAL_SCOPE_ALLOCATOR_H
#endif
