/*
 * $Id: SipThread.cxx,v 1.5 2007/06/18 11:11:17 lizhijie Exp $
 */

#include "global.h"
#include "cpLog.h"
#include "SipThread.hxx"
#include "SipVia.hxx"
#include "SipEvent.hxx"
#include "SipMsg.hxx"
#include "SipTransceiver.hxx"
#include "SystemInfo.hxx"

using namespace Vocal;

// the inline function only used in this file
inline void SipEvent::setSipReceive( const Sptr < SipMsgQueue > sipRcv )
{
	setSipMsgQueue(sipRcv);
}
SipThread::SipThread( 
	const Sptr < SipTransceiver > sipStack, 
	const Sptr < Fifo < Sptr < SipProxyEvent > > > outputFifo ,
	bool callLegHistory)
	: mySipStack(sipStack), myOutputFifo(outputFifo), myCallLegHistory(callLegHistory)
{
	assert ( mySipStack != 0 );
	assert ( myOutputFifo != 0 );
}



SipThread::~SipThread()
{
}

bool SipThread::discardMessage( Sptr < SipMsg > &sipMsg) const
{
    int numberOfVia = sipMsg->getNumVia();

    // check required only if > 2 via's in msg
    //
    if ( numberOfVia <= 2 )
    {
        return false;
    }

    SipVia mySipVia;
    mySipVia.setHost(theSystem.gethostAddress());

    for ( int i=0, loopCount = 0; i < numberOfVia; i++ )
    {
        const SipVia& recvSipVia = sipMsg->getVia(i);

        if ( recvSipVia == mySipVia )
        {
            loopCount++;

            if ( loopCount > 2 )
            {
                cpDebug(LOG_DEBUG, "Discarding Msg, loop > %d. %s", loopCount, recvSipVia.encode().logData());

                return ( true );
            }
        }
    }

    return ( false );
}



void SipThread::thread()
{
	cpLogSetLabelThread (VThread::selfId(), "bcSip");
	cpDebug(LOG_DEBUG, "PID of SipThread is %d" ,getpid() );

	while ( true )
	{
		try
		{
			assert ( mySipStack != 0 );
			assert ( myOutputFifo != 0 );

			Sptr <SipMsgQueue> sipRcv( mySipStack->receive(1000) );

			if ( sipRcv != 0 )
			{
				Sptr <SipMsg> sipMsg = sipRcv->back();

				if ( sipMsg != 0 )
				{
					Sptr < SipEvent > nextEvent = new SipEvent(myOutputFifo);

					if ( nextEvent != 0 )
					{
						nextEvent->setSipReceive(sipRcv);
						nextEvent->setSipStack(mySipStack);

						//nextEvent->setDeviceId(AS_DEVICE_NOT_DEFINED);

						if(myCallLegHistory)
							nextEvent->setCallLeg();

						myOutputFifo->add(nextEvent);
					}
				}
			}
		}
		catch ( VException& v)
		{
			cpDebug(LOG_DEBUG, "VException from SIP  stack %s", v.getDescription().c_str() );
		}
		catch ( ... )
		{
			cpDebug(LOG_DEBUG, "Exception from SIP stack" );
		}

		if ( isShutdown() == true )
		{
			return;
		}
	}
}

