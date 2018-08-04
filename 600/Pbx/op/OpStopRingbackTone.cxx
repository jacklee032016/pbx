/*
 * $Id: OpStopRingbackTone.cxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "global.h"
#include <cassert>
#include <cpLog.h>
#include "OpStopRingbackTone.hxx"
#include "UaHardwareEvent.hxx"
#include "UaDevice.hxx"
#include "UaCallInfo.hxx"

///
using namespace Vocal;


OpStopRingbackTone::OpStopRingbackTone()
{}


///
const Sptr < State >
OpStopRingbackTone::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "Stop Ringback Tone" );

    Sptr < UaCallInfo > call;
    call.dynamicCast( event->getCallInfo() );
    assert( call != 0 );

    Sptr < UaHardwareEvent > signal =
	new UaHardwareEvent( UaDevice::getDeviceQueue() );

	signal->setDeviceId( event->getDeviceId() );
	
	bool bRing = false;
	//added by lijie
	bool bCWing = false;
	
        Sptr < Contact > contact;
        Sptr < ContactList > contacts = call->getContactList();
        ContactList::iterator iter = contacts->begin();
        while ( iter != contacts->end() )
        {
            contact = (*iter);
            assert( contact != 0 );
            if ( contact->getStatus() == 180 || contact->getStatus() == 200 || contact->getStatus() == 183)
            {
			bRing = true;           	
			//added by lijie
			if(contact->getStatus() == 182)
				bCWing = true;
			break;
            }
            iter++;
        }
	if(bRing)
	{
   		 if ( call->getContact()->getRemoteRingback() )
    		{
        		signal->type = HardwareAudioType;
        		signal->signalOrRequest.request.type = AudioStop;
    		}
    		else
    		{
        		signal->type = HardwareSignalType;
			//added by lijie	
			if(!bCWing)
       	 		signal->signalOrRequest.signal = DeviceSignalLocalRingbackStop;
			else
		 	signal->signalOrRequest.signal = DeviceSignalStopCallWaitingBeep;
    		}

    		UaDevice::getDeviceQueue()->add( signal );
	}

    return 0;
}


///
OpStopRingbackTone::~OpStopRingbackTone()
{}


///
const char* const
OpStopRingbackTone::name() const
{
    return "OpStopRingbackTone";
}
