#ifndef _Sip_Transaction_Levels__hxx
#define _Sip_Transaction_Levels__hxx
/*
 * $Id: SipTransactionLevels.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */


#include "SipTransactionId.hxx"
#include "SipTransactionList.hxx"
#include "SipTransHashTable.hxx"
#include "SipMsg.hxx"
#include "VRWLock.hxx"
#include "VMutex.h"
#include "NetworkAddress.h"
#include "cpLog.h"

namespace Vocal
{
    
#define DEBUG_NEW_STACK LOG_DEBUG_STACK

struct SipTransLevel1Node;
struct SipTransLevel2Node;
struct SipTransLevel3Node;
struct SipMsgPair;
struct SipMsgContainer;

static const char * const NOVAL = "";

/**
 * this is top level node that'll be accessible from the hash table. hence,
 * we'll not keep the key value here (it's uniquely accessible)
 */
struct SipTransLevel1Node
{
    /// state specific data
    Data toTag;
    Data fromTag;
    int  msgCount;

    /// next level list
    SipTransactionList<SipTransLevel2Node*> level2;

    /** lock for processing the call leg
      * for 1xx responses we'll need the read lock only, for other msgs
      * write lock
      * keep it simple, just lock all as write lock
      */
    VMutex lock;

    /// back reference into hashtable (needed by cleanup thread)
    SipTransHashTable::Node* bucketNode;

    SipTransLevel1Node()
	: toTag(NOVAL), fromTag(NOVAL), msgCount(0),level2(), lock(),
	  bucketNode(0)
	{}
    SipTransactionList<SipTransLevel2Node*>::SipTransListNode *
    findOrInsert(const SipTransactionId& id);

    SipTransactionList<SipTransLevel2Node*>::SipTransListNode *
    find(const SipTransactionId& id);

    SipTransactionList<SipTransLevel3Node*>::SipTransListNode *
    findLevel3AckInvite(const SipTransactionId& id);
};

struct SipTransLevel2Node
{
    /// the 2nd level key
    SipTransactionId::KeyTypeII myKey;
    bool seqSet;
    int seqNumber;

    /// next level list
    SipTransactionList<SipTransLevel3Node*> level3;

      /************ TO DO **************\
       * either put a ref to self's container in list of prev level*
       * or some thing to get the ref to self's container *
       * (in level2 and level3 nodes) *
       * modify transaction GC and sent res/req DBs *
      \*********************************/
    /// back reference to the top level node
    SipTransLevel1Node * topNode;
    SipTransactionList<SipTransLevel2Node*>::SipTransListNode *myPtr;

    SipTransLevel2Node() : myKey(), seqSet(false), seqNumber(-1), level3(), topNode(0), myPtr(0)
	{}

    SipTransactionList<SipTransLevel3Node*>::SipTransListNode *
    findOrInsert(const SipTransactionId& id);

    SipTransactionList<SipTransLevel3Node*>::SipTransListNode *
    find(const SipTransactionId& id);

    SipTransactionList<SipTransLevel3Node*>::SipTransListNode *
    findInvite();
};


struct SipMsgPair
{
    /// the request
    SipMsgContainer *request;
    /// and the response
    SipMsgContainer *response;

    SipMsgPair() : request(0), response(0)
	{}

    ~SipMsgPair()
	{
	    /// these are deleted explicitly by the cleanup
	}
};

struct SipTransLevel3Node
{
    /// the 3rd level key
    SipTransactionId::KeyTypeIII myKey;

    /// message sequence. we don't want to keep a msg queue here 'coz
    /// on every recieve a copy of msg queue is constructed anyway.
    /// atleast keep the operations fast by direct lookup.
    ///
    /// NOTE: the msg queue for ACK's will be constructed by combining
    /// it w/ level3 node of INVITE
    SipMsgPair msgs;

      /************ TO DO **************\
       * either put a ref to self's container in list of prev level *
       * or some thing to get the ref to self's container *
       * (in level2 and level3 nodes) *
       * modify transaction GC and sent res/req DBs *
      \*********************************/
    /// back reference to level2 node
    SipTransLevel2Node *level2Ptr;
    SipTransactionList<SipTransLevel3Node*>::SipTransListNode *myPtr;

    SipTransLevel3Node() : myKey(), msgs(), level2Ptr(0), myPtr(0)
	{}
};


struct SipMsgContainer
{
    struct m
    {
	/// refence to the message (for incoming messages)
	Sptr<SipMsg> in;
	/// reference to raw msg txt (for outgoing messages)
	Data         out;
        //Host
        Sptr<NetworkAddress> netAddr;
        //
        int  type;
        //
        Data  transport;
    } msg;

    /// transport specific data
    int retransCount;
    bool collected;

      /************ TO DO **************\
       * either put a ref to self's container in list of prev level *
       * or some thing to get the ref to self's container *
       * (in level2 and level3 nodes) *
       * modify transaction GC and sent res/req DBs *
      \*********************************/
    /// back reference to level3 node
    SipTransLevel3Node *level3Ptr;

    /// to control race between CANCEL and transport layer
    VMutex myLock;

    /// constructed with default value for stateless mode
    SipMsgContainer() : msg(), retransCount(1),collected(false),  level3Ptr(0), myLock()
	{}
};

 
} // namespace Vocal
#endif
