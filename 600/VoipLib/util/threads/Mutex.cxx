/*
 * $Id: Mutex.cxx,v 1.2 2007/03/01 20:04:33 lizhijie Exp $
 */

#include "global.h"
#include "Mutex.hxx"
#include <cassert>
#include <cerrno>


using Vocal::Threads::Mutex;
using Vocal::ReturnCode;
using Vocal::SUCCESS;

static vmutex_t local;

static
void safe()
{
   vmutex_init(&local);
}

Mutex::Mutex()
{
   static pthread_once_t initialized = PTHREAD_ONCE_INIT;
   pthread_once(&initialized, safe);
   
   vmutex_lock(&local);   
   ReturnCode  rc = vmutex_init(&myId);
   assert( rc != EINVAL);
   assert( rc != EBUSY);
   assert( rc != ENOMEM);
   assert( rc != EAGAIN);
   
   assert( rc == SUCCESS );
   vmutex_unlock(&local);   
}

Mutex::~Mutex ()
{
    ReturnCode  rc = vmutex_destroy(&myId);
    assert( rc != EBUSY );  // currently locked 
    assert( rc == SUCCESS );
}


void
Mutex::lock()
{
    ReturnCode  rc = vmutex_lock(&myId);
    assert( rc != EINVAL );
    assert( rc != EDEADLK );
    assert( rc == SUCCESS );
}


void
Mutex::unlock()
{
    ReturnCode  rc = vmutex_unlock(&myId);
    assert( rc != EINVAL );
    assert( rc != EPERM );
    assert( rc == SUCCESS );
}


vmutex_t *
Mutex::getId() const
{
    return ( &myId );
}


