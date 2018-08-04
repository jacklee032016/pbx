/*
 * $Id: SubscribeManager.cxx,v 1.2 2007/03/02 04:19:29 lizhijie Exp $
 */

static const char* const version =
    "$Id: SubscribeManager.cxx,v 1.2 2007/03/02 04:19:29 lizhijie Exp $";

#include "global.h"
#include <cassert>

#include <SipVia.hxx>

#include "SubscribeManager.hxx"
#include "UaConfiguration.hxx"
#include "VCondition.h"

using namespace Vocal;

///
SubscribeManager::SubscribeManager( Sptr < SipTransceiver > sipstack )
{
    cpDebug(LOG_DEBUG, "Starting Subscribe Mananger");

    UaConfiguration* config = UaConfiguration::instance();

    // First Subscribe message
    SipCallId callId;

    Sptr < SipUrl > toUrl;
    try
    {
        toUrl = new SipUrl( Data("sip:") + Data( config->getSubscribeTo() ) );

    }
    catch ( SipUrlParserException& )
    {
        cpLog( LOG_ERR, "Invalid url %s", config->getSubscribeTo().c_str() );
        cpLog( LOG_ERR, "need to handle this case" );
    }

    SipTo to( toUrl );
    to.setUser( config->getUserName() );
    subscribeMsg = new SubscribeMsg( callId, to, atoi( config->getLocalSipPort().c_str() ) );

    SipExpires sipExpires;
    sipExpires.setDelta( Data(config->getSubscribeExpire() ));
    subscribeMsg->setExpires( sipExpires );

    delay = 1000 * config->getSubscribeExpire();

    // add event header
    SipSubsNotifyEvent notifyEvent;
    notifyEvent.setEventType( EVENT_MWI );
    subscribeMsg->setSubsNotifyEvent( notifyEvent );

    //allocation a fifo for the subscribe messages
    myQ = new Fifo < Sptr < SubscribeMsg > > ;

    //add the first message to the Q
    Sptr < SubscribeMsg > msg = new SubscribeMsg(*subscribeMsg);
    msgId = myQ->addDelayMs( msg, 0 );

    // set sipstack
    sipStack = sipstack;
}

///
SubscribeManager::~SubscribeManager()
{}

///

void
SubscribeManager::subscribeMain()
{

    cpDebug(LOG_DEBUG, "SubscribeManager::subscribeMain" );

    cpDebug(LOG_DEBUG, "SubscribeManager::subscribeMain: getting next subscribe message");

    Sptr < SubscribeMsg > subMsg = myQ->getNext();

    if ( subMsg == 0 )
    {
        //sleep for a second
        vusleep( 1000000 );
        return ;
    }

    subscribeMutex.lock();

    if ( 0 != sipStack )
    {
        cpDebug(LOG_DEBUG, "sending subscribe message");
        sipStack->sendAsync( *subMsg );
    }

    //add another subscribe message to Q in case response never comes
    SipCSeq cseq = subscribeMsg->getCSeq();
    cseq.incrCSeq();
    subscribeMsg->setCSeq( cseq );

    Sptr < SubscribeMsg > msg = new SubscribeMsg(*subscribeMsg);
    msgId = myQ->addDelayMs(msg, delay);

    subscribeMutex.unlock();
}


///
bool
SubscribeManager::handleSubscribeResponse(const StatusMsg& statusMsg)
{

    bool ret = true;

    // check if response corresponds to the original subscribe message
    if ( subscribeMsg->computeCallLeg() == statusMsg.computeCallLeg() )
    {
         subscribeMutex.lock();


	 cpDebug(LOG_DEBUG, "SubscribeManager::handling response to a subscribe message");

	 myQ->cancel( msgId );

	 cpDebug(LOG_DEBUG, "SubscribeManager::updating subscribe information");
	 updateSubscribeMsg(statusMsg);

	 int expire;
	 if ( statusMsg.getStatusLine().getStatusCode() > 200 )
	 {
	     cpLog( LOG_ERR, "Subscribe failed" );
	     cpLog( LOG_ERR, "Subscribe will be tried again in 60 seconds" );
	     expire = 60;
	     ret = false;
	 }
	 else
	 {
	     expire = delay;
	 }

	 cpDebug(LOG_DEBUG, "The new delay is %d", expire);

	 msgId = myQ->addDelayMs(subscribeMsg, delay);

	 subscribeMutex.unlock();
      
    } else
    {
        cpLog( LOG_ERR, "the subscribe message response is for an unkown person.  Ignored" );
        return ret = false;
    }


    return ret;
}

///
void
SubscribeManager::updateSubscribeMsg( const StatusMsg& statusMsg )
{
    // for now, just update expire if status is 200
    // to be done

    //add another subscribe message to Q in case response never comes
    SipCSeq cseq = subscribeMsg->getCSeq();
    cseq.incrCSeq();
    subscribeMsg->setCSeq( cseq );
}

///
bool
SubscribeManager::handleNotify( const NotifyMsg& msg )
{

    if ( subscribeMsg == 0 )
    {
        cpLog( LOG_ERR, "No existing subscribe message" );
        return false;
    }

    if ( subscribeMsg->computeCallLeg() == msg.computeCallLeg() )
    {
	NotifyMsg notMsg = msg;
	StatusMsg status( notMsg, 200 /*OK*/ );
	sipStack->sendReply( status );
	
	SipSubsNotifyEvent notifyEvent = msg.getSubsNotifyEvent();
	if ( notifyEvent.getEventType() != EVENT_MWI )
	  {
	    cpLog( LOG_ERR, "This notify is not for message waiting" );
	    return 0;
	  }
	
	// for now, just print something
	//cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
	//cout << "You have new Voice Mail messages !!! " << endl;
	//cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
	return true;	
    } else
    {
        cpLog( LOG_ERR, "Notify does not match subscribe" );
        return false;
    }
}
