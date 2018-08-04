#ifndef _Sip_Transaction_GC__hxx
#define _Sip_Transaction_GC__hxx
/*
* $Log: SipTransactionGC.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipTransactionGC.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "SipTransactionLevels.hxx"
#include "SipTransactionList.hxx"
#include "SipTransHashTable.hxx"
#include "ThreadIf.hxx"

namespace Assist
{
    
///////////////////////////////////
#define MESSAGE_CLEANUP_DELAY    (SipTransactionGC::msgCleanupDelay)
#define INVITE_CLEANUP_DELAY     (SipTransactionGC::invCleanupDelay)
#define ORPHAN_CLEANUP_DELAY     (SipTransactionGC::orphanCleanupDelay)
///////////////////////////////////

class SipTransDebugger;

class SipTransactionGC : public ThreadIf
{
    public:
	static SipTransactionGC* instance();
	static void shutdown();

	static int msgCleanupDelay;
	static int invCleanupDelay;
	static int orphanCleanupDelay;

    private:
	friend class SipTransDebugger;
	SipTransactionGC();
	~SipTransactionGC();
	SipTransactionGC(const SipTransactionGC&);
	SipTransactionGC& operator= (const SipTransactionGC&);
	bool operator==(const SipTransactionGC&);

    protected:
	virtual void thread();
	void trash(SipMsgContainer* item);
	bool doneFlag;

    public:
	void collect(SipMsgContainer* item, int delay=0);

    private:
	struct GCType;
	static SipTransactionGC *myInstance;
	static VMutex myLock; // used during instance creation ONLY

	SipTransactionList<GCType*> bins;
	typedef SipTransactionList<GCType*>::SipTransListNode BinsNodeType;
	VRWLock binsLock; // used to access the bins list

	struct GCType
	{
		const int myDelay;
		int currDelay;
		int currBin;
		SipTransactionList<SipMsgContainer*>  *bin;
		GCType(int delay): myDelay(delay), currDelay(0), currBin(0),
				   bin(0)
		{
		    assert(delay>=0);
		    bin = new SipTransactionList<SipMsgContainer*>[delay+2];
		}
	    private:
		GCType();
	};
};    

 
} 

#endif
