#ifndef _Sip_Trans_Hash_Table__hxx
#define _Sip_Trans_Hash_Table__hxx
/*
* $Log: SipTransHashTable.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:06:38  lizhijie
* no message
*
* $Id: SipTransHashTable.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "VRWLock.hxx"
#include "SipTransactionId.hxx"

#include "cpLog.h"

namespace Assist
{
    
#define DEBUG_NEW_STACK LOG_DEBUG_STACK


/**
 * define this if we want to do rehash in the background as a seperate thread
 */
//#define REHASH_THREAD

#ifdef REHASH_TREAD
#define REHASH_THREASHOLD 10
#endif

/**
 * define this if we want to lock nodes in lookups. it may not be
 * required 'coz we'll process a msg completely before working with another
 * msg. (and only one side of the TransactionDB will be "creating" new nodes)
 *
 * note that this is different from Top Level node locking, which will still
 * be there (but within Level1Node structure).
 *
 *
 * ^^^ and this is required, to control race between findxxxx and the cleanup
 * thread. specifically, some findxxxx caller might be waiting on readlock of
 * the existing top level node, while cleanup has the write lock and deletes
 * the node (which is illegal, coz the lock allows to new/delete on the lower
 * levels, but not the "*this" level)
 *
 * ^^^ is necessary *just* to flush out any slow executing find/inserts
 */
#define USE_NODE_LOCK

/// forward declarations...
struct SipTransLevel1Node;


/**
 * this is a *special purpose* implementation of hash table to be used by
 * TransactionDB.
 *
 * reason for not using stl hash table instead is that we don't want to lock
 * the *whole* hash table, when doing a lookup. in this implementation, it only
 * locks the hashed bucket, which is quickly unlocked once the collision list
 * has been processed.
 *
 * this structure will be concurrently operated by three threads, viz., Worker
 * (thru transceiver, for outgoing messages), Sip (thru filter, for 
 * incoming messages) and the TransactionDB's cleanup (will call erase)
 *
 * "how to use it" instructions for SentResp and SentReq ...
 *
 * ... lookups for requests will be using the findOrInsert method, while for
 * responses will be using find (it does'nt makes sense to creat a new call leg
 * for a response!)
 */


class SipTransHashTable
{
    public:
	typedef SipTransactionId::KeyTypeI KeyType;
	typedef unsigned long SizeType;
	struct Bucket;
	struct Node;
#ifdef USE_NODE_LOCK
	class SipTransRWLockHelper;
#endif
	SipTransHashTable();
	SipTransHashTable(SizeType sizeHint);

/// this might be a problem, may be not, by conflicting w/ erase being called
/// from cleanup thread. hence, should be called by user only when cleanup
/// thread has been "joined"
	~SipTransHashTable();

	Node* find(const KeyType& key
#ifdef USE_NODE_LOCK
		   , SipTransRWLockHelper* rwHelper=0
#endif
	    );
	Node* findOrInsert(const KeyType& key
#ifdef USE_NODE_LOCK
			   , SipTransRWLockHelper* rwHelper=0
#endif
	    );

#ifdef USE_NODE_LOCK
	/// once processing is done, user should call this
	void release(Node* node);
	/// OR use RW helper!!! (**** safer ***)
#endif

	Node& operator[](const KeyType& key);

	/// method specifically for cleanup to acquire lock for top level node
	void lock(Node * node, SipTransRWLockHelper * helper = 0);

	/// this method will be used to erase a node from hash
	void erase(Node* node);

	/// and to release the locks
	void unLock(Node * node);

	///// debugging method
	Data giveDetails();

    private:
	SipTransHashTable& operator= (const SipTransHashTable&);
	bool operator==(const SipTransHashTable&);
	SipTransHashTable(const SipTransHashTable&);

    private:
	friend class SipTransDebugger;
	Bucket * buckets;
	SizeType size;
	SizeType count;
#ifdef REHASH_THREAD
	bool rehashFlag;
#endif
	VRWLock rwLock;

	static SizeType nextSize(SizeType sizeHint);
	static SizeType hash(const SipTransactionId::KeyTypeI& key, SizeType size);

	/// when this is called, the table is already locked for write
	/// TODO
	void rehash(SizeType sizeHint);
#ifdef REHASH_THREAD
	///// declare the thread wrapper
#endif
	void rehashFunc(SizeType sizeHint);

    public:
	struct Bucket
	{
		Bucket():first(0), rwLock()
		{}

	    private:
		Bucket& operator= (const Bucket&);
		Bucket(const Bucket&);

	    private:
		friend class SipTransDebugger;
		friend class SipTransHashTable;
		Node* first;
		/**
		 * this lock is required 'coz operations of TransactionDB will span beyond
		 * the scope of hashtable's method, i.e., lookup. hence, we need some
		 * way of guaranteeing thread safety during erase, rehash and insert
		 *
		 * actually ^^^^^ is NOT correct, although we need the lock, but just
		 * during the scope of the method. outside the scope of hashtable, users
		 * only use the myNode member, and that is not changed/touched by the
		 * hashtable methods
		 */
		VRWLock rwLock;
	};

	struct Node
	{
		SipTransLevel1Node* myNode;
		SipTransHashTable * myTable;
		Node(const KeyType& key)
		    : myNode(0),
		      myTable(0),
		      myBucket(0),
		      next(0),
		      keyCopy(key)
#ifdef USE_NODE_LOCK
		    ,rwLock()
#endif
		{}

	    private:
		Node& operator= (const Node&);
		Node(const Node&);

	    private:
		friend class SipTransDebugger;
		friend class SipTransHashTable;
		Bucket* myBucket;

		Node *next;
		const KeyType keyCopy;

/**
 * don't need a RW lock here, 'coz only hash table will modify these nodes, and
 * if bucket is locked then don't need to worry about erase or rehash
 *
 * actually ^^^^ is wrong, we need a rw lock here so that reference to bucket
 * can be safely accessed (race conditions with rehash)
 */

/**
 * even ^^^^ is wrong, actually lock the table first to acheive lock on bucket
 * and hence don't need the lock here
 */

#ifdef USE_NODE_LOCK
/**
 * all of ^^^^ holds, but for the hashtable. for the transactiobDB we still
 * need to lock the node - to take care of find Vs insert
 */
		VRWLock rwLock;
#endif
	};

#ifdef USE_NODE_LOCK
/**
 * this is a RW lock helper specific to the SipTransactionDB and the hash table
 * above. it will be used to take care of read/write lock/unlock on the nodes
 * being passed in between the two classes.
 *
 * user can also directly lock/unlock the nodes, this utility class is just
 * to make the exchange of locked objects safer.
 */
	class SipTransRWLockHelper
	{
	    public:
		SipTransRWLockHelper()
		    : rwLockPtr(0)
		{}

		~SipTransRWLockHelper()
		{
		    if(rwLockPtr)
			rwLockPtr->Unlock();
		}

		void readLock(VRWLock* lockPtr)
		{
		    this->rwLockPtr = lockPtr;
		    this->rwLockPtr->ReadLock();
		}

		void writeLock(VRWLock* lockPtr)
		{
		    this->rwLockPtr = lockPtr;
		    this->rwLockPtr->WriteLock();
		}

		VRWLock * rwLockPtr;
	};
#endif //USE_NODE_LOCK

};
 
}

#endif
