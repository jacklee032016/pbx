/*
* $Log: SipTransceiver.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:10  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipTransceiver.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "global.h"
#include <cstdlib>
#include <cassert>
#include <sys/time.h>
#include <string>
#include <Sptr.hxx>
#include "SipCommand.hxx"
#include "StatusMsg.hxx"
#include "InviteMsg.hxx"
#include "CancelMsg.hxx"
#include "AckMsg.hxx"
#include "ByeMsg.hxx"
#include "RegisterMsg.hxx"
#include "OptionsMsg.hxx"
#include "InfoMsg.hxx"
#include "ReferMsg.hxx"
#include "TransferMsg.hxx"
#include "SipVia.hxx"
#include "SipFrom.hxx"
#include "SipContact.hxx"
#include "SipCSeq.hxx"
#include "SipSnmpDetails.hxx"
#include "SipTransactionId.hxx"
#include "SipTransceiver.hxx"
#include "SystemInfo.hxx"

using namespace Assist;


class TransactionDBStatus: public SipDebuggingStatus
{
    public:
        TransactionDBStatus(SipTransactionDB& x, Data label )
            :myItem(x),
             myLabel(label + ": ")
            {
            }

        Data status()
            {
                return myLabel + myItem.getDetails();
            }

    private:
        SipTransactionDB& myItem;
        Data myLabel;
};


class SipUdpStatus : public SipDebuggingStatus
{
    public:
        SipUdpStatus(SipUdpConnection& x, int port)
            :myItem(x),
             myLabel()
            {
                char buf[256];
                sprintf(buf, "UDP Stack (port %d): ", port);
                myLabel = buf;
            }
        Data status()
            {
                return myLabel + myItem.getDetails();
            }
    private:
        SipUdpConnection& myItem;
        Data myLabel;
};


SipAppContext SipTransceiver::myAppContext = APP_CONTEXT_GENERIC;

SipTransceiver::SipTransceiver( Data adata, 
				int siplistenPort, 
				bool nat  /*Default Agruments*/,
				SipAppContext aContext  /*Default Agruments*/)
    :
    recvdMsgsFifo(),
    udpConnection(0),
    tcpConnection(0),
    sentRequestDB(),
    sentResponseDB()
{
    myAppContext = aContext;
    cpDebug(LOG_DEBUG_STACK, "SipStack listening on port %d ", siplistenPort );
    cpDebug(LOG_DEBUG_STACK, "SipStack context %s ", (myAppContext == APP_CONTEXT_PROXY) ? "Proxy" : "Non-Proxy" );

    udpConnection = new SipUdpConnection( &recvdMsgsFifo, siplistenPort );

    if ( udpConnection == 0 )
    {
        cpDebug(LOG_INFO, "SipTransceiver::unable to instantiate UDP connections");
    }

    tcpConnection = new SipTcpConnection( &recvdMsgsFifo, siplistenPort );

    if ( tcpConnection == 0 )
    {
        cpDebug(LOG_INFO, "SipTransceiver::unable to instantiate TCP connections");
    }
    application = adata;
    Data nameport;
    nameport += adata;
    nameport += ":";
    nameport += Data(siplistenPort);
    //sipAgent = new SipAgent(nameport);
    natOn = nat;

    Assist::theSystem.setSystemPort(siplistenPort);

    char* port;
    if((port = getenv("VOCAL_SIP_REPORTER")))
    {
        debugger = new SipDebuggingInterface(atoi(port)); 
        debugger->add(new TransactionDBStatus(sentRequestDB, "Request DB"));
        debugger->add(new TransactionDBStatus(sentResponseDB, "Response DB"));
        debugger->add(new SipUdpStatus(*udpConnection, siplistenPort));
    }

    myLocalNamePort = nameport;
}


SipTransceiver::~SipTransceiver()
{
    // xxx - this needs to close up shop
    SipTransactionGC::shutdown();
}


void
SipTransceiver::sendAsync(SipCommand& sipMessage)
{
    if ( !(sipMessage.checkMaxForwards()) )
    {
        cpDebug(LOG_DEBUG_STACK, "SipTransceiver: sendAsync: %s",
              sipMessage.encode().logData());
        cpDebug(LOG_DEBUG_STACK, "reached MaxForwards limit, not sending");

        return ;
    }

    SipMsgContainer* msgPtr 
	= sentRequestDB.processSend(
	    copyPtrtoSptr(&sipMessage));

    if(msgPtr)
    {
	//// should this only be for actually sent messages? /////////
	if (sipAgent != 0)
	{
	    updateSnmpData(copyPtrtoSptr(&sipMessage), OUTS);
	}
	//////////////////////////////////////////////////////////////
	
	send(msgPtr);
    }
}


void
SipTransceiver::sendAsync(Sptr<SipCommand> sipMessage, const Data& host,
                       const Data& port) 
{
    if ( !(sipMessage->checkMaxForwards()) )
    {
        cpDebug(LOG_DEBUG_STACK, "SipTransceiver: sendAsync: %s",
              sipMessage->encode().logData());
        cpDebug(LOG_DEBUG_STACK, "reached MaxForwards limit, not sending");

        return ;
    }

    SipMsgContainer* msgPtr = sentRequestDB.processSend(sipMessage);
    
    if(msgPtr)
    {
	//// should this only be for actually sent messages? /////////
	if (sipAgent != 0)
	{
	    updateSnmpData(sipMessage, OUTS);
	}
	//////////////////////////////////////////////////////////////

	send(msgPtr, host, port);
    }
}


void
SipTransceiver::sendReply(StatusMsg& sipMessage)
{
    //// should this not be symmetric to what we are doing in sendAsync????
    //// (its missing all the checks and the snmp update)

    SipMsgContainer *sipMsg 
	= sentResponseDB.processSend(
	    copyPtrtoSptr(&sipMessage));
    if(sipMsg)
    {
	send(sipMsg);
    }
}


void
SipTransceiver::sendReply(Sptr<StatusMsg> sipMessage)
{
    //// should this not be symmetric to what we are doing in sendAsync????
    //// (its missing all the checks and the snmp update)
    
    SipMsgContainer *sipMsg = sentResponseDB.processSend(sipMessage);
    if(sipMsg)
    {
	send(sipMsg);
    }
}


void
SipTransceiver::send(SipMsgContainer *sipMsg, const Data& host,
                       const Data& port)
{
    if ((sipMsg->msg.transport == TCP) ||
        (sipMsg->msg.transport == "tcp"))
    {
        if (tcpConnection != 0)
        {
            //send on tcp.
            tcpConnection->send(sipMsg, host, port);
        }
        else
        {
            cpDebug(LOG_INFO, "TCP connection not instantiated");

	    /// hence delete the msg container
	    SipTransactionGC::instance()->
                collect(sipMsg, ORPHAN_CLEANUP_DELAY);
        }
    }
    else
    {
        if (udpConnection != 0)
        {
            //send on udp.
            udpConnection->send(sipMsg, host, port);
        }
        else
        {
            cpDebug(LOG_INFO, "UDP connection not instantiated");

	    /// hence delete the msg container
	    SipTransactionGC::instance()->
                collect(sipMsg, ORPHAN_CLEANUP_DELAY);
        }
    }
}

Sptr < SipMsgQueue > SipTransceiver::receive(int timeOut)
{
    Sptr < SipMsgQueue > msgQPtr = 0;
  
    timeval start, now;
  
    if ( timeOut >= 0 )
    {
	gettimeofday(&start, 0);
    }
  
    while (msgQPtr == 0)
    {
	int timePassed = 0;
	if ( timeOut >= 0 )
        {
	    gettimeofday(&now, 0);
	  
	    timePassed = ( now.tv_sec - start.tv_sec ) * 1000
		+ ( now.tv_usec - start.tv_usec ) / 1000;
	  
	    if (timePassed >= timeOut)
            {
		//cpDebug(LOG_DEBUG_STACK, "timeout value exceeded");
		return 0;
            }
	}
	recvdMsgsFifo.block(timeOut-timePassed);
      
	if ( !recvdMsgsFifo.messageAvailable() )
	{
	    continue;
	}
      
      
	SipMsgContainer *msgPtr = recvdMsgsFifo.getNext();
	if ( msgPtr == 0)
	{
	    assert(0);
//            cpLog(LOG_CRIT, "received NULL");
	    continue;
	}

	/********************** TO DO ****************************
	 * not doing 'coz need to bring some stuff from udp impl
	 * to here, so will do after 500 cps
         * msgPtr->msg.in = SipMsg::decode(msgPtr->msg.out);
	 *********************************************************/

	/*****************************************************************
	 * how does this affect the transactions, i.e. it is only being
	 * done on received messages, so should this be before or after
	 * going thru the data base (i.e. is it visible to transactionDB)?
	 *****************************************************************/

	/*********** decided to remove it from the stack ******************/
        cpDebug(LOG_DEBUG, "Received: %s",msgPtr->msg.in->briefString().c_str());
        if ( natOn == true)
	{
	//changes for taking care of the NAT traversals
	  SipVia natVia = msgPtr->msg.in->getVia(0);
	  LocalScopeAllocator lo; 
          //cpLog (LOG_DEBUG, "natVia = %s", natVia.encode().logData());
	  string addr1 = natVia.getHost().getData(lo);
	  string addr2 = msgPtr->msg.in->getReceivedIPName().getData(lo);

          //addr2 can be empty if stack had generated the message like 408
          if(addr2 != "")
          {
	    NetworkAddress netaddr1(addr1);
	    NetworkAddress netaddr2(addr2);
	  
	    if ( netaddr1.getIpName() != netaddr2.getIpName() || (addr1 == ""))
	    {
	      natVia.setReceivedhost(msgPtr->msg.in->getReceivedIPName());
	      natVia.setReceivedport(msgPtr->msg.in->getReceivedIPPort());
	      //remove the first item from the via list
	      msgPtr->msg.in->removeVia(0);
	      //insert natvia in the vector via list
	      msgPtr->msg.in->setVia(natVia, 0);
	    }
          }
	}
	//---NAT
/*	*********************************************************************/
	
      
        cpDebug(LOG_DEBUG, "Received: %s",msgPtr->msg.in->briefString().c_str());
	cpDebug(LOG_DEBUG_STACK, "msg is; %s",msgPtr->msg.out.logData());
      
	SipMsgQueue *msgQ = 0;
	Sptr<SipMsg> sipPtr = msgPtr->msg.in;

	if(msgPtr->msg.in->getType() == SIP_STATUS)
	    msgQ = sentRequestDB.processRecv(msgPtr);
	else
	    msgQ = sentResponseDB.processRecv(msgPtr);
      
	if(msgQ)
	{
	    msgQPtr = msgQ;
	  
	    //need to have snmpDetails for this.
	    if (sipAgent != 0)
	    {
		updateSnmpData(sipPtr, INS);
	    }
	}
	else if(msgPtr->msg.in != 0)
	{
	    send(msgPtr);
	}
	else if(msgPtr->msg.out.length())
	{
	    send(msgPtr);
	}
	else
	{
	    delete msgPtr;
	}
    }
    return msgQPtr;
}


void
SipTransceiver::reTransOff()
{
    cpDebug(LOG_DEBUG_STACK, "*** Retransmission is turned off ***");
    SipUdpConnection::reTransOff();
}


void
SipTransceiver::reTransOn()
{
    cpDebug(LOG_DEBUG_STACK, "*** Retransmission is turned on ***");
    SipUdpConnection::reTransOn();
}


void
SipTransceiver::setRetransTime(int initial, int max)
{
    cpDebug(LOG_DEBUG_STACK, "Initial time set to %d ms", initial);
    cpDebug(LOG_DEBUG_STACK, "Max time set to %d ms", max);
    SipUdpConnection::setRetransTime(initial, max);
}


void
SipTransceiver::setRandomLosePercent(int percent)
{
    if (udpConnection != 0)
    {
        udpConnection->setRandomLosePercent(percent);
    }
}


SipTransactionDB::CallLegVector
SipTransceiver::getCallLegMsgs(Sptr < SipMsg > sipmsg)
{
    //// will it always be on SentRequest???
    return (sentRequestDB.getCallLegMsgs(sipmsg));
}


void
SipTransceiver::updateSnmpData(Sptr < SipMsg > sipMsg, SnmpType snmpType)
{
    //come here only if snmpAgent is valid.

    if (sipAgent == 0)
        return ;

    Method type = sipMsg->getType();
    if (type == SIP_STATUS)
    {
        if (snmpType == INS)
        {
            sipAgent->updateSipCounter(sipSummaryInResponses);
        }
        else if (snmpType == OUTS)
        {
            sipAgent->updateSipCounter(sipSummaryOutResponses);
        }

        //get the status code of this msg.
        Sptr < StatusMsg > statusMsg;
        statusMsg.dynamicCast(sipMsg);

        int statusCode = statusMsg->getStatusLine().getStatusCode();

        if (snmpType == INS)
        {
            sipAgent->updateStatusSnmpData(statusCode, INS);
        }
        else if (snmpType == OUTS)
        {
            sipAgent->updateStatusSnmpData(statusCode, OUTS);
	}

    }

    else
    {
        //this is a command.
        if (snmpType == INS)
        {
            sipAgent->updateSipCounter(sipSummaryInRequests);

            sipAgent->updateCommandSnmpData(type, INS);
        }
        else if (snmpType == OUTS)
        {
            sipAgent->updateSipCounter(sipSummaryOutRequests);

            sipAgent->updateCommandSnmpData(type, OUTS);
        }
    }
}


void
SipTransceiver::printSize()
{
    cpLog(LOG_ERR, "%s:::::\n%s\n%s\n\n%s\n%s\n\n%s\n%s",
	  "Sip Stack Size",
	  "SentResponseDB size...", sentResponseDB.getDetails().logData(),
	  "SentRequestDB size...", sentRequestDB.getDetails().logData(),
          "UDP stack details...", udpConnection->getDetails().logData());
}


Data
SipTransceiver::getLocalNamePort() const
{
    return myLocalNamePort;
}
