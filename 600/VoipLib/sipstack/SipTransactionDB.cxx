/*
 * $Id: SipTransactionDB.cxx,v 1.2 2007/03/01 20:04:32 lizhijie Exp $
 */

#include "global.h"
#include "SipTransactionDB.hxx"

using namespace Vocal;

SipTransactionDB::SipTransactionDB()
    : table()
{
}


SipTransactionDB::SipTransactionDB(int size)
    : table(size)
{
}


SipTransactionDB::~SipTransactionDB()
{
    /// clean up the table...

    /// TODO: GC all the items...
}


Data
SipTransactionDB::getDetails()
{
    return table.giveDetails();
}


void
SipTransactionDB::cancel(SipMsgContainer *msg)
{
    if(!msg) 
    {
        return;
    }

    /// just set the retrans count to 0, and the transport stack will
    /// feed it to cleanup thread
    msg->myLock.lock();
    msg->retransCount = 0;
    msg->myLock.unlock();
}


SipTransactionDB::CallLegVector
SipTransactionDB::getCallLegMsgs(Sptr<SipMsg>& sipMsg)
{
    CallLegVector retVal;
    SipTransactionId id(*sipMsg);

    SipTransLevel1Node * topNode = getTopNode(id,sipMsg);
    if(topNode)
    {
	SipTransactionList<SipTransLevel2Node*>::SipTransListNode*
            level2Node = topNode->level2.getFirst();
	while(level2Node)
	{
	    SipTransactionList<SipTransLevel3Node*>::SipTransListNode 
		*level3Node = level2Node->val->level3.getFirst();
	    while(level3Node)
	    {
		if(level3Node->val->msgs.request)
                {
                    Data toBeEatenData 
                        = level3Node->val->msgs.request->msg.out;
                    SipMsg* sipMsg = SipMsg::decode(toBeEatenData);
                    sipMsg->setSendAddress(*(level3Node->val->msgs.request->msg.netAddr));
                    retVal.push_back(sipMsg);
		    //retVal.push_back(SipMsg::decode(toBeEatenData));
                }
		if(level3Node->val->msgs.response)
                {
                    Data toBeEatenData 
                        = level3Node->val->msgs.response->msg.out;
		    retVal.push_back(SipMsg::decode(toBeEatenData));
                }
		level3Node = level2Node->val->level3.getNext(level3Node);
	    }
	    level2Node = topNode->level2.getNext(level2Node);
	}
    }

    /// release the lock acquired on top level node
    if(topNode)
    {
	topNode->lock.unlock();
    }
    return retVal;
}
