/*
* $Log: SipTransHashTable.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:10  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipTransHashTable.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "global.h"

#ifdef __linux__
#include <malloc.h>
#endif

#include "SipTransHashTable.hxx"
#include "SipTransDebugger.hxx"

using namespace Assist;

#ifndef TARGET_CPS
#define TARGET_CPS 50
#endif

#ifndef SOFTSTATE_SECS
#define SOFTSTATE_SECS 20
#endif

/// this is 2x 'coz of GC's implementation (of swapping)
#define OPTIMUM_SIZE 12289

static const SipTransHashTable::SizeType primeList[] =
{
  1,    1543,       3079,         6151,        12289,     24593,
  49157,      98317,        0
};

SipTransHashTable::SipTransHashTable()
    :buckets(0),
     size(0),
     count(0),
#ifdef REHASH_THREAD
     rehashFlag(false),
#endif
     rwLock()
{
#ifdef __linux__
    mallopt(M_MMAP_THRESHOLD, 1000*1000);
#endif
    size = nextSize(OPTIMUM_SIZE);
    buckets = new Bucket[size];
}

SipTransHashTable::~SipTransHashTable()
{
///// this is NOT clean, will need to GC all the level3 nodes...
///// (or may be transaction DB will do it)
    for(unsigned int i=0; i<size;i++)
    {
	Node *curr, *prev;
	curr=prev=buckets[i].first;
	while(curr)
	{
	    prev=curr->next;
	    delete curr;
	    curr=prev;
	}
    }
    delete[] buckets;
}

SipTransHashTable::SipTransHashTable(SizeType sizeHint)
    :buckets(0),
     size(0),
     count(0),
#ifdef REHASH_THREAD
     rehashFlag(false),
#endif
     rwLock()
{
    size = nextSize(sizeHint);
    buckets = new Bucket[size];
}

SipTransHashTable::SizeType
SipTransHashTable::nextSize(SizeType sizeHint)
{
    SizeType newSize = primeList[0];
     for(int i=1; primeList[i];i++)
	if((newSize = primeList[i]) > sizeHint)
	    break;
    return newSize;
}

SipTransHashTable::SizeType
SipTransHashTable::hash(const SipTransactionId::KeyTypeI& key, SizeType size)
{
    // write the hashing function...
    // using stl hash function, replace by CLR or something...

  SizeType h = 0; 
  /*******
  const char* s = static_cast<const char*>(key.getCStr());
  for ( ; *s; ++s)
      h = 5*h + *s;
  **********/
  for(int i=0; i<key.length();i++)
    h = 5*h + key.getChar(i);

  if(!h) h++; // to ensure size-1
  return (h%size);
}

SipTransHashTable::Node*
SipTransHashTable::find(const KeyType& key
#ifdef USE_NODE_LOCK
			,SipTransRWLockHelper* rwHelper /*defualt arg*/
#endif
    )
{
    if(!count) return 0;

#ifdef REHASH_THREAD
    while(rehashFlag);
#endif

    SizeType index;
    rwLock.ReadLock();
    index = hash(key, size);
    buckets[index].rwLock.ReadLock();
    rwLock.Unlock();

    Node* currNode = buckets[index].first;
    for(;currNode;currNode=currNode->next)
    {
	if(key == currNode->keyCopy)
	{
#ifdef USE_NODE_LOCK
	    if(rwHelper)
		rwHelper->readLock(&(currNode->rwLock));
	    else
		currNode->rwLock.ReadLock();
#endif
	    break;
	}
    }
    buckets[index].rwLock.Unlock();
    return currNode;
}

SipTransHashTable::Node*
SipTransHashTable::findOrInsert(const KeyType& key
#ifdef USE_NODE_LOCK
			,SipTransRWLockHelper* rwHelper /*defualt arg*/
#endif
    )
{
#ifdef REHASH_THREAD
    while(rehashFlag);
#endif

    SizeType index;
    rwLock.ReadLock();
    index = hash(key,size);
    buckets[index].rwLock.ReadLock();
    rwLock.Unlock();

    Node* currNode = buckets[index].first;
    int coCount = 0;
    for(;currNode;currNode=currNode->next)
    {
        coCount++;
	if(key == currNode->keyCopy)
	{
#ifdef USE_NODE_LOCK
	    if(rwHelper)
		rwHelper->readLock(&(currNode->rwLock));
	    else
		currNode->rwLock.ReadLock();
#endif
	    break;
	}
    }

    if(!currNode)
    {
	/// release all the read locks and then try again from hashtable level
	/// to acquire write locks
	buckets[index].rwLock.Unlock();
	rwLock.WriteLock();
#ifdef REHASH_THREAD
	if(++count >= (size-REHASH_THREASHOLD))
#else
	if(++count >= size)
#endif
	{
	    rehash(count);
	    index = hash(key, size);
	}
        coCount++;
	buckets[index].rwLock.WriteLock();
	rwLock.Unlock();
	currNode = new Node(key);
	currNode->next = buckets[index].first;
	currNode->myBucket = buckets+index;
	currNode->myTable = this;
	buckets[index].first = currNode;
#ifdef USE_NODE_LOCK
	if(rwHelper)
	    rwHelper->writeLock(&(currNode->rwLock));
	else
	    currNode->rwLock.WriteLock();
#endif
        if(coCount > 1)
        {
	   cpDebug(LOG_DEBUG,"%s[%s] at bucket #%d, contention list size:%d",
	      "contention for Level-I key",
	      currNode->keyCopy.logData(),index, coCount);
        }
    }
    buckets[index].rwLock.Unlock();
    return currNode;
}
 
SipTransHashTable::Node&
SipTransHashTable::operator[](const KeyType& key)
{
#ifdef USE_NODE_LOCK
    SipTransRWLockHelper helper;
    return *findOrInsert(key, &helper);
#else
    return *findOrInsert(key);
#endif
}


#ifdef USE_NODE_LOCK
void
SipTransHashTable::release(Node* node)
{
    node->rwLock.Unlock();
}
#endif


void
SipTransHashTable::unLock(Node* node)
{
    node->myBucket->rwLock.Unlock();
}

void
SipTransHashTable::lock(Node* node, SipTransRWLockHelper *helper /*default*/)
{
    rwLock.WriteLock();
    Bucket* bucketPtr = node->myBucket;
    if(helper)
	helper->writeLock(&bucketPtr->rwLock);
    else
	bucketPtr->rwLock.WriteLock();

#ifdef REHASH_THREAD
    /// we do another read of bucketPtr to refresh the latest value
    /// after getting the lock.
    /// (it might have been changed by rehash while we were waiting
    /// on the old value)
    if( bucketPtr != node->myBucket)
    {
//////////// this whole thing is wrong, works with dangling pointers!!!
////////////////////////////////////////////////////////////////////////
	if(helper)
	{
	    helper->rwLockPtr=0;
	}
	bucketPtr->rwLock.Unlock();
	
	bucketPtr = node->myBucket;

	if(helper)
	    helper->writeLock(&bucketPtr->rwLock);
	else
	    bucketPtr->rwLock.WriteLock();
    }

#endif
    rwLock.Unlock();
}


void
SipTransHashTable::erase(Node* node)
{
    if(!node || !count) return;

    rwLock.WriteLock();

/**************************************************************************\
 * this being done in two steps, so lock does the job...
    Bucket* bucketPtr = node->myBucket;
    bucketPtr->rwLock.WriteLock();

#ifdef REHASH_THREAD
    /// we do another read of bucketPtr to refresh the latest value
    /// after getting the lock.
    /// (it might have been changed by rehash while we were waiting
    /// on the old value)
    if( bucketPtr != node->myBucket)
    {
	bucketPtr->rwLock.Unlock();
	bucketPtr = node->myBucket;
	bucketPtr->rwLock.WriteLock();
    }
#endif
\**************************************************************************/

    /// we assume that most of the erase would be legal calls, hence we
    /// decrement the count in advance. note that it is safe to do this
    /// 'coz this is the only method that decreases count value (and in
    /// case of bad call it restores). this helps in avoiding an extra
    /// hash table lock
    count--;

    rwLock.Unlock();

    Node** prevNodePtr = &(node->myBucket->first);
    Node* currNode = *prevNodePtr;
    for(;currNode;)
    {
	/// yes this is a direct address comparision!!!
	if(currNode == node)
	{
	    *prevNodePtr = currNode->next;
	    cpLog(DEBUG_NEW_STACK,"erasing top node[%s]",
		  node->keyCopy.logData());
	    delete node;
	    break;
	}
	else
	{
	    prevNodePtr = &(currNode->next);
	    currNode = currNode->next;
	}
    }
/****************************************************************\
    bucketPtr->rwLock.Unlock();
\****************************************************************/

    /// if this was a BAD call
    if(!currNode)
    {
	cpLog(DEBUG_NEW_STACK,"*********** BAD CALL TO ERASE #$%!");
	rwLock.WriteLock();
	count++;
	rwLock.Unlock();
    }
}


void
SipTransHashTable::rehash(SizeType sizeHint)
{
#ifdef REHASH_THREAD
    if(rehashFlag != true)
    {
	rehashFlag = true;
	/// spawn the rehash thread...
    }
#else
    rehashFunc(sizeHint);
#endif
}

void
SipTransHashTable::rehashFunc(SizeType sizeHint)
{
/*************************************************************************\
 * for now this is fine, 'coz the whole table is locked during the whole
 * operation. but with threaded rehash *might* have to lock the node also
 * to avoid incorrect execution with lock.
 */
    sizeHint = nextSize(sizeHint);
    if(sizeHint <= size) return;

    cpLog(DEBUG_NEW_STACK,"%srehashing from %d (old size) to %d...%s",
	  "##########################################\n",
	  size,sizeHint,
	  "\n##########################################");

    Bucket* newBuckets = new Bucket[sizeHint];
    unsigned int i;


    for(i=0;i<size;i++)
    {
	Node* currNode;
	Node** prevNodePtr;
	SizeType newIndex;
#ifdef REHASH_THREAD	
	/// lock table when its a thread, to take care of race conditions
	rwLock.WriteLock();
#endif
	/// this allways needs to be there, even when this is not a thread,
	/// to take care of slow execution sequences of other methods
	buckets[i].rwLock.WriteLock();

#ifdef REHASH_THREAD
	rwLock.Unlock();
#endif
	prevNodePtr = &(buckets[i].first);
	currNode = *prevNodePtr;
	for(;currNode;)
	{
	    *prevNodePtr = currNode->next;
	    newIndex = hash(currNode->keyCopy,sizeHint);

#ifdef REHASH_THREAD
	    /// this is to take care of any conflicts with erase
	    /// but will only be useful if rehash is implemented as
	    /// a sperate thread. otherwise the whole table is locked anyway
	    /// by the caller. another option is to lock all initially
	     newBuckets[newIndex].rwLock.WriteLock();
#endif
	    currNode->next = newBuckets[newIndex].first;
	    currNode->myBucket = newBuckets+newIndex;
	    newBuckets[newIndex].first = currNode;

#ifdef REHASH_THREAD
	    newBuckets[newIndex].rwLock.Unlock();
#endif
	    currNode = *prevNodePtr;
	}
	buckets[i].rwLock.Unlock();
    }

#ifdef REHASH_THREAD
    rwLock.WriteLock();
#endif
    delete[] buckets;
    buckets = newBuckets;
    size = sizeHint;
#ifdef REHASH_THREAD
    rehashFlag = false;
    rwLock.Unlock();
#endif
}

Data
SipTransHashTable::giveDetails()
{
    return SipTransDebugger::printDebug(this);
}
