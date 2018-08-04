/*
* $Log: SipSentResponseDB.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:10  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipSentResponseDB.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/
#include "global.h"
#include "SipTransceiver.hxx"
#include "SipSentResponseDB.hxx"
#include "SipMsg.hxx"
#include "SipCommand.hxx"
#include "StatusMsg.hxx"
#include "SipVia.hxx"

using namespace Assist;

SipSentResponseDB::SipSentResponseDB()
    : SipTransactionDB()
{
}

SipSentResponseDB::SipSentResponseDB(int size)
    : SipTransactionDB(size)
{}

SipSentResponseDB::~SipSentResponseDB()
{
    /// the SipTransactionDB::~ will do the clean up
}

SipMsgContainer*
SipSentResponseDB::processSend(const Sptr<SipMsg>& msg)
{
    /// the only send in THIS db can be of the responses
    StatusMsg * response = dynamic_cast<StatusMsg*>(msg.getPtr());
    assert(response);

    SipMsgContainer * retVal = new SipMsgContainer();
    retVal->msg.in = msg;
    retVal->msg.type = msg->getType();
    retVal->msg.transport = msg->getVia(0).getTransport();

    /// if its a final response then update the transactionDB
    if(response->getStatusLine().getStatusCode() >= 200)
    {
	SipTransactionId id(*msg);
	SipTransactionList<SipTransLevel3Node *>::SipTransListNode*
	    level3Node = 0;
	SipTransLevel1Node * topNode = getTopNode(id,msg);
	if(topNode)
	{
	    SipTransactionList<SipTransLevel2Node *>::SipTransListNode*
		level2Node = 0;

	    level2Node = topNode->find(id);
	    if(level2Node)
	    {
		level3Node = level2Node->val->find(id);
	    }
	    else
	    {
		cpLog(DEBUG_NEW_STACK,
		      "ERROR, no Level2[%s] entry for outgoing response...",
		      id.getLevel2().logData());
	    }
	}
	if(level3Node)
	{
	    /// there's a transaction for this response message
	    /// hence add it to the transaction
	    if(level3Node->val->msgs.response)
	    {
		cpLog(DEBUG_NEW_STACK,"Collision\nOLD:%s\n\nNEW:%s",
		      level3Node->val->msgs.response->msg.out.logData(),
		      retVal->msg.out.logData());
	    }
	    else
	    {
		level3Node->val->msgs.response = retVal;
		retVal->level3Ptr = level3Node->val;

		/// make the transport retrans it repeatedly
		if((level3Node->val->myKey == INVITE_METHOD) &&
                   (SipTransceiver::myAppContext != APP_CONTEXT_PROXY))
                {
                    cpDebug(LOG_DEBUG_STACK, "Set UA INVITE final response retransmission" );
		    retVal->retransCount = MAX_RETRANS_COUNT;
                }
                cpDebug(LOG_DEBUG_STACK, "Retransmission count %d", retVal->retransCount );

                if(level3Node->val->myKey == INVITE_METHOD)
                {
                    SipTransactionGC::instance()->
                        collect(level3Node->val->msgs.request,
                                INVITE_CLEANUP_DELAY);
                }
	    }
	}

	/// release the lock acquired on top level node
	if(topNode)
        {
	    topNode->lock.unlock();
        }
    }

    return retVal;
}

SipMsgQueue*
SipSentResponseDB::processRecv(SipMsgContainer* msgContainer)
{
    /// the only receive in THIS db can be of the requests
    assert(dynamic_cast<SipCommand*>(msgContainer->msg.in.getPtr()));

    SipTransactionId id(*(msgContainer->msg.in));
    SipMsgQueue * retVal = 0;

    SipTransLevel1Node * topNode = getTopNode(id,msgContainer->msg.in);
    if(!topNode)
    {
	/// there was no transaction found/created for this message,
	/// so just proxy it up and hope for the best!!!
	retVal = new SipMsgQueue;
	retVal->push_back(msgContainer->msg.in);
        msgContainer->msg.in = 0;
	cpDebug(LOG_DEBUG,"No trasaction for %s",
	      msgContainer->msg.out.logData());

	///// leak fixed 04/11 @ 1230
	SipTransactionGC::instance()->
	    collect(msgContainer,
		    ORPHAN_CLEANUP_DELAY);
    }
    else
    {
        SipTransLevel2Node * level2Node = topNode->findOrInsert(id)->val;

	SipTransactionList<SipTransLevel3Node *>::SipTransListNode *nodePtr =
	    level2Node->findOrInsert(id);

	if(nodePtr->val->msgs.request)
	{
	    /// if there is a coresponding response then retrans it
	    if(nodePtr->val->msgs.response)
	    {
		cpDebug(LOG_INFO,"duplicate message: %s",
		      msgContainer->msg.out.logData());
		msgContainer->msg.in = nodePtr->val->msgs.response->msg.in;
		msgContainer->msg.netAddr = nodePtr->val->msgs.response->msg.netAddr;
                if(nodePtr->val->msgs.response->msg.out.length())
                {
		    msgContainer->msg.out 
                        = nodePtr->val->msgs.response->msg.out;
                }
                else
                { 
		    msgContainer->msg.out 
                        = nodePtr->val->msgs.response->msg.in->encode();
                }
		msgContainer->msg.type 
                    = nodePtr->val->msgs.response->msg.type;
		msgContainer->msg.transport 
                    = nodePtr->val->msgs.response->msg.transport;
		msgContainer->msg.netAddr 
                    = nodePtr->val->msgs.response->msg.netAddr;
		msgContainer->retransCount = FILTER_RETRANS_COUNT;
	    }
	    else
	    {
		/// we didn't find a matching response but since the request
		/// has been received already so assume that application will
		/// be sending a response (if it's not an ACK)
		/// and hence just drop this duplicate
		msgContainer->msg.in = 0;
		msgContainer->msg.out = "";
		msgContainer->retransCount = 0;
	    }
	}
	else
	{
	    /// this is a first time receive of this message
            int msgSeqNumber 
                  = msgContainer->msg.in->getCSeq().getCSeqData().convertInt();
#if 1
            if (SipTransceiver::myAppContext != APP_CONTEXT_PROXY)
            {
                if(!( (msgContainer->msg.in->getType() == SIP_ACK) ||
                      (msgContainer->msg.in->getType() == SIP_CANCEL) ))
                {
		    // Not ACK or CANCEL
                    if(level2Node->seqSet)
                    {
                      // Check the seq number of the message for the same call-leg.
                
                      // 1.  if the seq number is higher than previous one Msg is OK
                      // 2.  If sequence number is lower, it is an error
                      // 3.  If sequence number is equal and message is not an
                      //     ACK or CANCEL it is an error.  ACK and CANCELs w/
                      //     the same seqence number are OK.

                        int storedSeqNum = level2Node->seqNumber;
                        if( msgSeqNumber > storedSeqNum )
                        {
                            cpDebug(LOG_DEBUG, "***** Bumping seq to %d", msgSeqNumber);
                            //Bump up the next sequenece only if not ACK or CANCEL      
                            level2Node->seqNumber = msgSeqNumber;
                        }
                        else if( (msgSeqNumber < storedSeqNum) ||
                                 (msgSeqNumber == storedSeqNum) )
                        {             
                             // Error condition
                             Sptr<SipCommand> sipCmd;
                             sipCmd.dynamicCast(msgContainer->msg.in);
                             assert(sipCmd != 0);
                             Sptr<StatusMsg> statusMsg = new StatusMsg(*sipCmd, 400); 
                             Data reason = "Sequence number out of order";
                             statusMsg->setReasonPhrase(reason);
		             msgContainer->msg.in = statusMsg;
                             msgContainer->msg.type = statusMsg->getType();
                             msgContainer->msg.transport = statusMsg->getVia(0).getTransport();

		             msgContainer->msg.out = "";
	                     msgContainer->retransCount = 1;
                     
                             if(topNode)
                               topNode->lock.unlock();
                             processSend(statusMsg);
                             return 0;
                        }
                    }
                 }
                 else
                 {
                     level2Node->seqNumber = msgSeqNumber;
                     level2Node->seqSet = true;
                     cpDebug(LOG_DEBUG_STACK, "***** Setting seq to %d for callId %s", msgSeqNumber, msgContainer->msg.in->getCallId().encode().logData());
                 }
            }
#endif

	    nodePtr->val->msgs.request = msgContainer;
	    nodePtr->val->msgs.request->level3Ptr = nodePtr->val;

	    /// construct the msg queue to be sent up to application
	    retVal = new SipMsgQueue;
	    if(msgContainer->msg.in->getType() == SIP_ACK)
	    {
		SipTransactionList<SipTransLevel3Node *>::SipTransListNode
		    *curr = level2Node->level3.getLast();
		while(curr)
		{
		    /// look for the INVITE message
		    if(curr->val->myKey == INVITE_METHOD)
			break;
		    curr = level2Node->level3.getPrev(curr);
		}
		if( !curr )
                {
		    cpDebug(LOG_DEBUG_STACK,"No INVITE, try without Via branch");
		    curr = topNode->findLevel3AckInvite( id );
                }
		if(curr)
		{
		    cpDebug(LOG_DEBUG_STACK,"Found INVITE");
                    // add the other items of this transaction, and
                    // reduce memory usage by clearing the parsed
                    // message in the in pointer

		    if(curr->val->msgs.request)
                    {
			retVal->push_back(curr->val->msgs.request->msg.in);
                        curr->val->msgs.request->msg.in = 0;
                    }
		    if(curr->val->msgs.response)
                    {
			retVal->push_back(curr->val->msgs.response->msg.in);
                        curr->val->msgs.response->msg.in = 0;
                    }

		    /// also cancel the retrans of response
		    cancel(curr->val->msgs.response);
		    cpLog(DEBUG_NEW_STACK,"Stopping retrans of response[%s]",
			  curr->val->myKey.logData());
		}
		else
		{
		    cpDebug(LOG_DEBUG_STACK,"INVITE not Found");
		    /////////// ACK w/o INVITE !!!!
		    ////////// (may have been gc'd)
		}
	    }
	    retVal->push_back(msgContainer->msg.in);
	    /// also inform the cleanup
            if(msgContainer->msg.in->getType() != SIP_INVITE)
            {
                SipTransactionGC::instance()->
                    collect(nodePtr->val->msgs.request,
                            MESSAGE_CLEANUP_DELAY);
            }
            msgContainer->msg.in = 0;
	}
    }
    /// release the lock acquired on top level node
    if(topNode)
    {
        topNode->lock.unlock();
    }
    return retVal;
}

SipTransLevel1Node*
SipSentResponseDB::getTopNode(const SipTransactionId& id, 
                              const Sptr<SipMsg>& msg)
{
    SipTransHashTable::SipTransRWLockHelper helper;
    SipTransHashTable::Node * bktNode;

    if(msg->getType()==SIP_STATUS)
    {
	/// this finds if there's a transaction, and locks for READ
	bktNode = table.find(id.getLevel1(), &helper);
	if(bktNode)
	  if(bktNode->myNode->toTag == NOVAL)
	  {
              // this is the first response going out for this call
              // leg, hence / update the state info with To tag

              bktNode->myNode->toTag = msg->getTo().getTag();
	  }
	  else
	  {
	    /// make sure that response has the To tag same as previous
	    /// responses for this call leg
	    if(bktNode->myNode->toTag != msg->getTo().getTag())
	      {
		cpDebug(LOG_DEBUG_STACK,
		      "To tag of outgoing %d[%s] %s [%s] is incorrect",
                      msg->getType(),
		      msg->getTo().getTag().logData(),
		      "in existing transaction",
		      bktNode->myNode->toTag.logData());
                if(SipTransceiver::myAppContext != APP_CONTEXT_PROXY)
                {
                    SipTo to = msg->getTo();
		    cpDebug(LOG_DEBUG, "Setting to-tag of outgoing response from=%s to=%s", to.getTag().logData(), bktNode->myNode->toTag.logData() );
                    to.setTag(bktNode->myNode->toTag);
                    msg->setTo(to);
                }
	      }
	  }
    }
    else
    {
	// this creates a new transaction, if none exists, and locks
	// for write otherwise just returns the existing transaction
	// with read lock

	bktNode = table.findOrInsert(id.getLevel1(), &helper);
	if(bktNode->myNode == 0)
	{
	    bktNode->myNode = new SipTransLevel1Node();
	    bktNode->myNode->bucketNode = bktNode;
	}
	/// if there's a transaction, then check the to tag
	else if((bktNode->myNode->toTag!= NOVAL) &&
		(bktNode->myNode->toTag != msg->getTo().getTag()))
	{
	    cpDebug(LOG_DEBUG_STACK,"To tag of incoming %d[%s] %s [%s] is incorrect",
		  msg->getType(),
		  msg->getTo().getTag().logData(),
		  "in existing transaction",
		  bktNode->myNode->toTag.logData());
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
	return 0;
}
