/*
 * $Id: SipSentRequestDB.cxx,v 1.3 2007/06/04 17:59:33 lizhijie Exp $
 */

#include "global.h"
#include "SipSentRequestDB.hxx"
#include "SipMsg.hxx"
#include "SipCommand.hxx"
#include "StatusMsg.hxx"
#include "SipTransceiver.hxx"

using namespace Vocal;

SipSentRequestDB::SipSentRequestDB()
    : SipTransactionDB()
{
}


SipSentRequestDB::SipSentRequestDB(int size)
    : SipTransactionDB(size)
{
}


SipSentRequestDB::~SipSentRequestDB()
{
    // the SipTransactionDB::~ will do the clean up
}


SipMsgContainer*
SipSentRequestDB::processSend(const Sptr<SipMsg>& msg)
{
    // the only send in THIS db can be of the requests
    assert(dynamic_cast<SipCommand*>(msg.getPtr()));

    SipTransactionId id(*msg);
    SipMsgContainer * retVal = new SipMsgContainer();
    retVal->msg.in = msg;
    retVal->msg.type = msg->getType();

    Sptr<SipCommand> command;
    command.dynamicCast(msg);

    if ( command != 0 )
    {
        Sptr<BaseUrl> baseDest = command->getRequestLine().getUrl();
        Sptr<SipUrl> dest;
        dest.dynamicCast(baseDest);
        if(dest != 0)
        {
            cpDebug(LOG_DEBUG_STACK, "Setting transport %s", dest->getTransportParam().logData());
            retVal->msg.transport = dest->getTransportParam();
        }
    } 

    if(msg->getType() != SIP_ACK)
      retVal->retransCount = MAX_RETRANS_COUNT;

    SipTransLevel1Node * topNode = getTopNode(id,msg);
    SipTransactionList<SipTransLevel3Node *>::SipTransListNode *nodePtr =
	topNode->findOrInsert(id)->val->findOrInsert(id);

    if(nodePtr->val->msgs.request)
    {
	cpLog(LOG_ERR,"two identical requests from application...");
	cpLog(DEBUG_NEW_STACK,"Old...\n%s\n\nNew...\n%s",nodePtr->val->msgs.request->msg.out.logData(), retVal->msg.out.logData());
    }
    else
    {
	// insert the request into data base
	nodePtr->val->msgs.request = retVal;
	retVal->level3Ptr = nodePtr->val;

	// if this is a CANCEL then cancel all the active retranses...
	if(msg->getType() == SIP_CANCEL)
	{
	    SipTransLevel2Node * level2Node = topNode->find(id)->val;
	    
	    SipTransactionList<SipTransLevel3Node *>::SipTransListNode *curr =
		level2Node->level3.getLast();
	    while(curr)
	    {
		// we don't want to cancel the yet to be sent CANCEL
		if(curr != nodePtr)
		{
		    cpLog(DEBUG_NEW_STACK,"Canceling request %s",  curr->val->msgs.request->msg.out.logData());
		    cancel(curr->val->msgs.request);
		}
		curr = level2Node->level3.getPrev(curr);
	    }
	}
    }
    // release the lock acquired on top level node
    topNode->lock.unlock();

    // the transport will actually insert this msgs for cleanup, after the
    // retrans
   
    return retVal;
}


SipMsgQueue*
SipSentRequestDB::processRecv(SipMsgContainer* msgContainer)
{
    // the only receive in THIS db can be of the responses
    StatusMsg * response = dynamic_cast<StatusMsg*>(
	msgContainer->msg.in.getPtr());
    assert(response);

    SipMsgQueue * retVal = 0;

    SipTransactionId id(*(msgContainer->msg.in));
    SipTransactionList<SipTransLevel3Node *>::SipTransListNode *
        level3Node=0;
    SipTransactionList<SipTransLevel2Node *>::SipTransListNode *
        level2Node=0;
    SipTransLevel1Node * topNode = getTopNode(id,msgContainer->msg.in);

    if(topNode)
    {
        level2Node = topNode->find(id);
        if(level2Node)
        {
            level3Node = level2Node->val->find(id);
        }
        else
        {
		cpLog(DEBUG_NEW_STACK, "ERROR, no Level2[%s] entry for incoming response...", id.getLevel2().logData());
        }
    }
    if(!level3Node)
    {
        // there's no transaction for this response message
        retVal = new SipMsgQueue;
        retVal->push_back(msgContainer->msg.in);
        cpDebug(LOG_DEBUG_STACK,"No transaction for %s",  msgContainer->msg.in->encode().logData());
        msgContainer->msg.in = 0;
        
	    // leak fixed 04/11 @ 1240
        SipTransactionGC::instance()->
            collect(msgContainer,
                    ORPHAN_CLEANUP_DELAY);
    }
    else
    {
        // and cancel all the retrans of request
        cancel(level3Node->val->msgs.request);
        cpDebug(DEBUG_NEW_STACK,"Stopping retrans of request[%s]", level3Node->val->myKey.logData());

        // If appContext is a proxy and a 200 or any provisional
        // response of INVITE do not filter it and give it to proxy

        int statusCode = response->getStatusLine().getStatusCode();
        if((statusCode < 200)  ||
           ((SipTransceiver::myAppContext == APP_CONTEXT_PROXY) && 
           (statusCode == 200) &&
           (response->getCSeq().getMethod() == INVITE_METHOD) ) )
        {
            // simply forward this response up to application
            retVal = new SipMsgQueue;
            retVal->push_back(msgContainer->msg.in);
            msgContainer->msg.in = 0;
            if ((statusCode == 200)&&
                (level3Node->val->myKey == INVITE_METHOD))
            {
		//Once 200 is received, why we need to keep it any longer ?? , changing it to 
		//be cleanup right away - SP
                SipTransactionGC::instance()->
                    collect(level3Node->val->msgs.request,
				    SipTransactionGC::invCleanupDelay);
                SipTransactionGC::instance()->
                    collect(msgContainer, SipTransactionGC::invCleanupDelay);
		SipTransactionGC::instance()->
		    collect(level3Node->val->msgs.response,
			    MESSAGE_CLEANUP_DELAY);
            }
            else
	    {
                SipTransactionGC::instance()->
                    collect(msgContainer, ORPHAN_CLEANUP_DELAY);
            }
        }
        else if(response->getStatusLine().getStatusCode() >= 200)
        {
            // if it is a final response, then process the transaction
            // there's a transaction, hence check for filtering
            
	    // fix to take care if 408 is recvd from udp sender, while
	    // there's also a response received by udp receiver. hence
	    // in case of BYE, this was going to else and overwriting
	    if(level3Node->val->msgs.response)
	    {
		SipTransactionList<SipTransLevel3Node *>::SipTransListNode *
		    curr = 0;
	       if(level3Node->val->myKey == INVITE_METHOD)
	       {
		   curr = level2Node->val->level3.getLast();
		   while(curr)
		   {
		       // look for the ACK message
		       if(curr->val->myKey == ACK_METHOD &&
			  curr->val->msgs.request)
		       {
			   cpLog(DEBUG_NEW_STACK,"duplicate message: %s",
				 msgContainer->msg.out.logData());

                           msgContainer->msg.in = 0;
			   msgContainer->msg.out =
			       curr->val->msgs.request->msg.out;
                           msgContainer->msg.type 
                               = curr->val->msgs.request->msg.type;
                           msgContainer->msg.transport =  
                               curr->val->msgs.request->msg.transport;
                           msgContainer->msg.netAddr =  
                               curr->val->msgs.request->msg.netAddr;
			   
			   msgContainer->retransCount = FILTER_RETRANS_COUNT;
			   
			   break;
		       }
		       curr = level2Node->val->level3.getPrev(curr);
		   }
	       }
	       if(!curr)
	       {
                   // we didn't find an ACK in our transaction, but
                   // since there's a response, lets assume the
                   // application is processing it, hence just drop
                   // the response

		   msgContainer->msg.in = 0;
		   msgContainer->msg.out = "";
		   msgContainer->retransCount = 0;
	       }
	    }
	    else
	    {
		level3Node->val->msgs.response = msgContainer;
		level3Node->val->msgs.response->level3Ptr = level3Node->val;
		
		retVal = new SipMsgQueue;
		if(level3Node->val->msgs.request)
                {
		    retVal->push_back(level3Node->val->msgs.request->msg.in);
                    level3Node->val->msgs.request->msg.in = 0;
                }
		retVal->push_back(msgContainer->msg.in);

                //Clean up Object leave the raw data
                if(!msgContainer->msg.out.length())
                {
                    msgContainer->msg.out = msgContainer->msg.in->encode(); 
                }
                msgContainer->msg.in = 0;

		// also inform the cleanup
                if (level3Node->val->myKey == INVITE_METHOD)
                {
                    SipTransactionGC::instance()->
                        collect(level3Node->val->msgs.request,
                        INVITE_CLEANUP_DELAY);
                }
		else
		{
                    SipTransactionGC::instance()->
                        collect(level3Node->val->msgs.request,
                        MESSAGE_CLEANUP_DELAY);
		}

		SipTransactionGC::instance()->
		    collect(level3Node->val->msgs.response,
			    MESSAGE_CLEANUP_DELAY);
	    }
        }
    }
    // release the lock acquired on top level node
    if(topNode)
    {
        topNode->lock.unlock();
    }
    return retVal;
}

SipTransLevel1Node *SipSentRequestDB::getTopNode(const SipTransactionId& id, const Sptr<SipMsg>& msg)
{
	SipTransHashTable::SipTransRWLockHelper helper;
	SipTransHashTable::Node * bktNode;

	if(msg->getType()==SIP_STATUS)
	{
		// this finds if there's a transaction, and locks for READ
		bktNode = table.find(id.getLevel1(), &helper);

		// if there's a transaction, then check the from tag
		if(bktNode && bktNode->myNode->fromTag != msg->getFrom().getTag())
		{
			cpDebug(LOG_DEBUG,	"From tag of incoming response[%s] %s [%s] is incorrect",
				msg->getFrom().getTag().logData(), "in existing transaction", bktNode->myNode->fromTag.logData());
		}
	}
	else
	{
		// this creats a new transaction, if none exists, and locks for write
		// otherwise just returns the existing transaction with read lock
		bktNode = table.findOrInsert(id.getLevel1(), &helper);
		if(bktNode->myNode == 0)
		{
			bktNode->myNode = new SipTransLevel1Node();
			bktNode->myNode->fromTag = msg->getFrom().getTag();
			bktNode->myNode->bucketNode = bktNode;
		}
	}

	if(bktNode)
	{
		// acquire a lock on the top level node, before releasing the
		// bucket node: due to race w/ cleanup thread!!!
		bktNode->myNode->lock.lock();

		return bktNode->myNode;
	}
	else
	{
		return 0;
	}
}

