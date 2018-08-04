/*
* $Log: SipUdp_impl.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:10  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipUdp_impl.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "global.h"
#include <cassert>
#include <sys/time.h>
#include <string>
#include <stdlib.h>
#include <stdio.h>

#include "Fifo.h"
#include "LockHelper.hxx"
#include "SipCommand.hxx"
#include "SipMsg.hxx"
#include "SipTransactionGC.hxx"
#include "SipTransactionLevels.hxx"
#include "SipUdp_impl.hxx"
#include "SipVia.hxx"
#include "Sptr.hxx"
#include "StatusMsg.hxx"

using namespace Assist;


/// this is a function to print messages
string printMsgBrief(Sptr<SipMsg> msg); 

 /*  retransmitOff from TransceiverSymbols.hxx */
bool SipUdp_impl::Udpretransmitoff = retransmitOff;

 /* retransmitTimeInitial from TransceiverSymbols.hxx */  
int SipUdp_impl::Udpretransmitimeinitial = retransmitTimeInitial; 

 /* retransmitTimeMax from TrasnceiverSymbols.hxx */
int SipUdp_impl::Udpretransmittimemax = retransmitTimeMax; 

SipUdp_impl::SipUdp_impl(Fifo<SipMsgContainer*>* fifo,
			 int port /*default argument*/ )
    :
    randomLosePercent(0),
    udpStack( NULL, port ),
    recFifo(fifo),
    sendFifo(),
    shutdown(false)
{
    udpStack.setModeBlocking(false);
    sendThread.spawn(sendThreadWrapper, this);
    receiveThread.spawn(rcvThreadWrapper, this);
}   


SipUdp_impl::~SipUdp_impl() 
{      
    shutdown = true;

    //insert shutdown message in the send fifo
    sendFifo.add(0);
    sendThread.join();
    receiveThread.join();
}


void*
SipUdp_impl::receiveMain()
{      
    NetworkAddress sender;
    while (1)
    {
        int len = 0;
        static const string::size_type MTU = 3600;
        char buf[MTU+1];
        buf[0] = '\0';
        len = 0; 
        len = udpStack.receiveTimeout( buf, MTU, &sender, 1, 0 );
        if ( len == 0 )
        {
	} 
	else if ( len < 0 )
	{
            cpDebug(LOG_DEBUG_STACK, "receiveTimeout error");
        }
        else if(recFifo->size() <= 10000 )
        {
            buf[len] = '\0';

            SipMsgContainer* sipMsg = new SipMsgContainer;
            sipMsg->msg.out = Data(buf, len);
            
            /******************************************************
             * will need to move this stuff into the stack later
             */
            Data toBeEaten = sipMsg->msg.out;
            
            cpDebug(LOG_DEBUG_STACK, "Received UDP Message \n%s" , 
                   toBeEaten.logData());
            
            sipMsg->msg.in = SipMsg::decode(toBeEaten);
            if(sipMsg->msg.in != 0)
            {
                sipMsg->msg.in->setReceivedIPName( sender.getIpName() );
                sipMsg->msg.in->setReceivedIPPort( Data(sender.getPort()) );
                recFifo->add(sipMsg);
                cpDebug(LOG_INFO, "Received UDP Message:\n\n<- HOST[%s] PORT[%d]\n\n%s", sender.getIpName().c_str(),
                      sender.getPort(), toBeEaten.logData());

            }
            else
            {
                // this message failed to decode
                cpDebug(LOG_DEBUG, 
                      "received message did not decode\n%s" , 
                      sipMsg->msg.out.logData());
                
                /// since not aceepted, hence delete
                delete sipMsg;
            }
        }
        else
        {
            cpDebug(LOG_DEBUG,  "***Ignoring received msg ***:%d" ,
		      recFifo->size());
        }

        if(shutdown)
        {
	    return 0;
        }
    }
    return 0;
}

  
void*
SipUdp_impl::sendThreadWrapper(void *p) 
{
    return static_cast<SipUdp_impl*>(p)->sendMain();
}

    
int
SipUdp_impl::udpSend(SipMsgContainer *sipMsg)
{
    // send it 
    int ret_status = 0;
    try
    {
        if(sipMsg->msg.netAddr == 0){
           cpLog(LOG_WARNING,"UDP SEND IS FAILED NULL MESSAGE :( To Send");
           return 0;	
        }
        const NetworkAddress& nAddr = *(sipMsg->msg.netAddr);
	
        assert(sipMsg->msg.out.length());
        cpDebug(LOG_INFO, "Sending UDP Message :\n\n-> HOST[%s] PORT[%d]\n\n%s",
              nAddr.getIpName().c_str(),nAddr.getPort(),
              sipMsg->msg.out.logData());
    
    	int lngth = sipMsg->msg.out.length(); 
        LocalScopeAllocator lo;
        //sipMsg->myLock.lock();
	//int count = sipMsg->retransCount;
        //sipMsg->myLock.unlock();
        //if(count == 0) return 0;
    	ret_status 
            = udpStack.transmitTo(sipMsg->msg.out.getData(lo),lngth,&nAddr);
    }
    catch(NetworkAddress::UnresolvedException& )
    {
        cpLog(LOG_ERR, "Failed to resolve the host name");
    }
    catch (UdpStackExceptionPortsInUse&)
    {
    	cpDebug(LOG_DEBUG_STACK,"All ports in use");
    }
    return ret_status;
} 


void*
SipUdp_impl::rcvThreadWrapper(void *p) 
{
    return static_cast<SipUdp_impl*>(p)->receiveMain();
}       


void* 
SipUdp_impl::sendMain()
{   
    while(1) 
    {
	int ret_status = 0;
    	RetransmitContents* retransmit = sendFifo.getNext();

	if(shutdown)
	{
	    return 0;
	}

        if (retransmit != 0)
    	{
    	    SipMsgContainer *sipMsg = retransmit->getMsg();
    	    if (sipMsg != 0)   
            {
		int count = sipMsg->retransCount;
		if (count)
                {
		    cpDebug(LOG_DEBUG_STACK, "retransmission count = %d", count);

		    //get host, and port, and send it off.
                     
		    if(!randomLosePercent || 
		       ((random() % 100) >= randomLosePercent))
                    {
			//
			ret_status = udpSend(sipMsg);
			switch(ret_status)
                        {
                        case 0:
			    break;
                        case 1:
			    cpDebug(LOG_INFO, "ECONNREFUSED! ");
			    ret_status = 403;
			    break;
                        case 2:
			    cpDebug(LOG_INFO, "EHOSTDOWN! ");
			    ret_status = 408;
			    break;
                        case 3:
			    cpDebug(LOG_INFO, "EHOSTUNREACH! ");
			    ret_status = 404;
			    break;
                        default:
			    cpDebug(LOG_INFO, "ERROR!!!");
			    ret_status = 400;
                        }
			if ( ret_status)
                        {
			    if  ((sipMsg != 0) && 
                                 (sipMsg->msg.type != SIP_STATUS)) 
                            {
                                Data toBeEaten = sipMsg->msg.out;
                                Sptr<SipMsg> mmsg = SipMsg::decode(toBeEaten);
			        Sptr<SipCommand> commandMsg;
			        commandMsg.dynamicCast(mmsg);
				SipMsgContainer * retVal = new SipMsgContainer;
				
				retVal->msg.in=new StatusMsg((*commandMsg),
							     ret_status);
				retVal->msg.out=retVal->msg.in->encode();
				recFifo->add(retVal);
                            }
                        }
                    }
		    else
                    {
			cpDebug(LOG_INFO, "XXX random lose: message not sent!");
                    }
		    
                    //update retransDetails
		    count--;

		    if(Udpretransmitoff)
		      count = 0;

		    //// required or else will send 408 up for retransoff!
		    else
		    {
			if ( ret_status != 0)
                        {
			    count = 0;
                        }
			else if ((!count) && (ret_status != 0))
                        {
			    /// flag the message got All its chances
			    retransmit->setCount(1);
                        }
		    }
		    sipMsg->myLock.lock();
		    /// check if it was canceled already
		    if(sipMsg->retransCount)
                    {
			sipMsg->retransCount = count;
                    }
		    else
                    {
                        count = 0;
                    }
		    sipMsg->myLock.unlock();

		    //increment the time to send out.

		    //// we don't want to hold on to messages that don't
		    //// need to be retransmitted
		    if(count)
		    {
			int timeToGo = retransmit->getTimeToGo();
			if (timeToGo == 0)
			{
			    retransmit->setTimeToGo(retransmitTimeInitial);
			}
			else if(timeToGo < retransmitTimeMax)
			{
			    retransmit->setTimeToGo(timeToGo * 2);
			}
		    }
		    else
			retransmit->setTimeToGo(0);

		    //add into Fifo.
		    sendFifo.addDelayMs( retransmit, 
			 retransmit->getTimeToGo());
                }
		else //i.e. count == 0
                {
		    /// check if this message got all it's chances!
                    if (retransmit->getCount())
		    {
			//send 408 if a command, other than an ACK.
			if ((sipMsg->msg.type != SIP_ACK) && 
                            (sipMsg->msg.type != SIP_STATUS)) 
			{
	                    cpDebug(LOG_DEBUG_STACK, "THE COUNT IS [ %d ]\n",retransmit->getCount());
	                    cpDebug(LOG_DEBUG_STACK, "The SipMeaasge is \n%s\n",sipMsg->msg.out.logData());
                            Data toBeEaten = sipMsg->msg.out;
                            Sptr<SipMsg> mmsg = SipMsg::decode(toBeEaten);
                            Sptr<SipCommand> commandMsg;
                            commandMsg.dynamicCast(mmsg);
                            SipMsgContainer* statusMsg = new SipMsgContainer;
                            statusMsg->msg.in 
                                = new StatusMsg((*commandMsg), 408);
                            //statusMsg->msg.out = statusMsg->msg.in->encode();
                            recFifo->add(statusMsg);
			}
		    }

		    /// feed it to the GC...
                    if(sipMsg->level3Ptr == 0)
                    {
                        SipTransactionGC::instance()->
                            collect(sipMsg, ORPHAN_CLEANUP_DELAY);
                    }
                    else if (sipMsg->msg.type != SIP_INVITE)
                    {
                        SipTransactionGC::instance()->
                            collect(sipMsg, MESSAGE_CLEANUP_DELAY);
                    }

		    delete retransmit;
		  }
	      }//if sipMsg
	}//if retransmit.
    }//end while
}


void 
SipUdp_impl::getHostPort(Sptr<SipMsg> sipMessage, Data* host, int* port)
{       
    if ( sipMessage->getType() == SIP_STATUS  )
    {
    	assert( sipMessage->getNumVia() > 0 );
	
    	if ( sipMessage->getNumVia() > 0 )
    	{
    	    const SipVia& via = sipMessage->getVia( 0 );
	    // check for viaReceived parameter in the via header
	    if ( via.isViaReceived() == true)
	    {
		*host = via.getReceivedhost();
                *port = via.getReceivedport().convertInt();
	    } 
	    else
	    {
                *host = via.getMaddr();
                if( host->length() == 0 )
                {
                    *host = via.getHost();
                }
		*port = via.getPort();
		cpDebug(LOG_DEBUG_STACK,"got host from Via of %s",host->logData());
		cpDebug(LOG_DEBUG_STACK,"got port from Via of %d",*port);
	    }
	}
    }
    else 
    {
        Sptr<SipCommand> command;
        command.dynamicCast(sipMessage);
        if ( command != 0)
        {
            Sptr<SipUrl> dest = command->postProcessRouteAndGetNextHop();

	    if(dest != 0)
	    {
                Data maddr = dest->getMaddrParam();
                if(maddr.length() > 0)
		    *host = maddr;
                else
		    *host = dest->getHost();
		*port = dest->getPort().convertInt();
            
		cpDebug(LOG_DEBUG_STACK, "got host from start line: %s:%d", host->logData(), *port);
	    }
	    else
	    {
		cpLog(LOG_ERR, "attempting to send message to a non-sip URL, discarding");
		assert(0);
	    }
        }
        else
        {
            assert(0);
        }
    }//if sipCommand

    //If port is zero set the default port
    if(*port == 0)
    {
        *port = SIP_PORT;
    }
}

    
void
SipUdp_impl::send(SipMsgContainer* msg, const Data& host, const Data& port)
{ 
    RetransmitContents* retransDetails = new RetransmitContents(msg, 0);

    if(!msg->msg.out.length())
    {
        Data nhost; 
        int nport;

        assert(msg->msg.in != 0);
	if(host.length())
	{
		nhost = host;
		nport = port.convertInt();
	}
	else
		getHostPort(msg->msg.in, &nhost, &nport);
        msg->msg.out = msg->msg.in->encode();
        try
        {
            if(msg->msg.netAddr == 0 )
                msg->msg.netAddr = new NetworkAddress(nhost.convertString(), nport);
        }
        catch(NetworkAddress::UnresolvedException& e)
        {
//            cpLog(LOG_ERR, "host: %s\n nhost: %s", host, nhost);
            cpLog(LOG_ERR, "Destination (%s) is not reachable, reason:%s.",
                            nhost.logData(), e.getDescription().c_str());
        }
    }

    // free the msg.in ptr if this is a status msg for a non-INVITE msg
    // or if it is an ACK message

    if((msg->msg.in != 0) && 
       (((msg->msg.in->getType() == SIP_STATUS) &&
         (msg->msg.in->getCSeq().getMethod() != "INVITE")) || 
        (msg->msg.in->getType() == SIP_ACK)))
    {
        // clear msg.in
        msg->msg.in = 0;
    }

    //if this has to be retransmitted.
    if (msg->retransCount > 1)
    {
        cpDebug(LOG_DEBUG_STACK, 
	      "Msg is for retransmission:\n %s", 
	      msg->msg.out.logData());

	//////// will have to go after moving retrans count etc to stack!!!
	msg->retransCount = sipmaxRetrans;
	///////////////////////////////////////////////////////////////////
    }
    cpDebug(LOG_DEBUG_STACK,
	      "$$$$$$$$$$$$$$$Msg is for retransmission:\n %s",
	      msg->msg.out.logData());

    //send immediately.
    sendFifo.addDelayMs( retransDetails , 0);
}


void
SipUdp_impl::reTransOn()
{
    Udpretransmitoff = false;
}


void
SipUdp_impl::reTransOff()
{
    Udpretransmitoff = true;
}


void
SipUdp_impl::setRetransTime(int initial, int max)
{
    Udpretransmitimeinitial = initial;
    Udpretransmittimemax = max;
}


void
SipUdp_impl::setRandomLosePercent(int percent)
{
    if((percent >=0) && (percent <= 100))
    {
	randomLosePercent = percent;
    }
}


void
SipUdp_impl::printSize()
{
    cpDebug(LOG_INFO, "recFifo: %d, sendFifo: %d",
	  recFifo->size(),sendFifo.size());
}


Data
SipUdp_impl::getDetails() const
{
    char buf[256];
    sprintf(buf, "receive fifo: %d    send fifo: %d",
            recFifo->size(),sendFifo.size());
    sprintf(buf, "\npkts received: %d   pkts sent: %d",
            udpStack.getPacketsReceived(),udpStack.getPacketsTransmitted());
    return buf;
}
