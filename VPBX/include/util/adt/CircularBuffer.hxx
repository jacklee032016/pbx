#ifndef CIRCULARBUFFER_HXX_
#define CIRCULARBUFFER_HXX_
/*
* $Id: CircularBuffer.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include <cassert>
#include "VMutex.h"
#include "Lock.hxx"

/** Infrastructure common to ASSIST.
 */
namespace Assist 
{

/** Abstract data types
 */

namespace ADT
{

/** Circular buffer class.  used to keep a queue of bytes for the user
 */
template < class T >
class CircularBuffer
{
    public:
        /** construct a buffer with maximum size
            @param  size   maximum size
        */
        CircularBuffer(int size);
        ~CircularBuffer();

        /// report number of bytes free
        int bytesAvailable() const;
        /// report number of bytes used (available for data)
        int bytesUsed() const;

        /** get bytes from the buffer
            @param maxSize    maximum number of bytes to get
            @param T          place to store bytes
            @return  the number of bytes
        */
        int get(T* data, int maxSize);
        int put(const T* data, int size);

    private:

        int used() const;
        int available() const;

        T* myBuffer;
        int myStartUsed;
        int myEndUsed;
        int mySize;
        mutable VMutex myMutex;
};


template < class T >
CircularBuffer<T>::CircularBuffer(int size)
    :myBuffer(),
     myStartUsed(0),
     myEndUsed(0),
     mySize(size)
{
    myBuffer = new T[size];
}


template < class T >
CircularBuffer<T>::~CircularBuffer()
{
    delete[] myBuffer;
}


template < class T >
int
CircularBuffer<T>::used() const
{
    int used = myEndUsed - myStartUsed;
    if(used < 0)
    {
	used = mySize + used;
    }
    return used;
}


template < class T >
int
CircularBuffer<T>::available() const
{
    return mySize - used();
}


template < class T >
int 
CircularBuffer<T>::bytesAvailable() const
{
    Assist::Threads::Lock x_ (myMutex);
    return available();
}


template < class T >
int
CircularBuffer<T>::bytesUsed() const
{
    Assist::Threads::Lock x_(myMutex);
    return used();
}


template < class T >
int
CircularBuffer<T>::get(T* data, int maxSize)
{
    Assist::Threads::Lock x_(myMutex);
    // true for this input
    assert(maxSize <= mySize);

    // always true
    assert(myStartUsed >= 0);
    assert(myEndUsed >= 0);
    assert(myStartUsed < mySize);
    assert(myEndUsed < mySize);
    int copySize = 0;

    while((myStartUsed  != myEndUsed) && maxSize )
    {
	*data = myBuffer[myStartUsed];
	++data;
	myStartUsed = (myStartUsed + 1) % mySize;
	--maxSize;
	++copySize;
    }

    // always true
    assert(myStartUsed >= 0);
    assert(myEndUsed >= 0);
    assert(myStartUsed < mySize);
    assert(myEndUsed < mySize);

    return copySize;
}


template < class T >
int
CircularBuffer<T>::put(const T* data, int size)
{
    Assist::Threads::Lock x_(myMutex);

    // always true
    assert(myStartUsed >= 0);
    assert(myEndUsed >= 0);
    assert(myStartUsed < mySize);
    assert(myEndUsed < mySize);

    int next;
    while( ( (next = ((myEndUsed + 1) % mySize)) != myStartUsed) &&
	   size )
    {
	myBuffer[myEndUsed] = *data;
	++data;
	--size;
	myEndUsed = next;
    }

    // always true
    assert(myStartUsed >= 0);
    assert(myEndUsed >= 0);
    assert(myStartUsed < mySize);
    assert(myEndUsed < mySize);

    return size;
}


} // namespace ADT
} // namespace Assist

#endif
