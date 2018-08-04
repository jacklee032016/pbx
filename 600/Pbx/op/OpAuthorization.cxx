#include "OpAuthorization.hxx"
#include "global.h"
#include "SipEvent.hxx"
#include "UaCallInfo.hxx"
#include "UaDevice.hxx"
#include "UaStateMachine.hxx"
#include "UaConfiguration.hxx"
#include "UaTimerEvent.hxx"
#include "SipCallId.hxx"
#include "UaCallInfo.hxx"
#include "UaCallContainer.hxx"


#include "Verify.hxx"
#include "Md5.hxx"



using Vocal::Crypto::Md5;



using namespace Vocal;


OpAuthorization::OpAuthorization()
{

}


OpAuthorization::~OpAuthorization()
{
}


const char* const
OpAuthorization::name() const
{
    return "OpAuthorization";
}


const Sptr < State >
OpAuthorization::process( const Sptr < SipProxyEvent > event )
{
	cpDebug(LOG_DEBUG, "OpAuthorization::process");
	Sptr <UaTimerEvent> uatimer;
	uatimer.dynamicCast(event);
	if (uatimer == 0)
		return 0;

	//
	//SipCallId overdueCallId = uatimer->getCallId();
	//assert(overdueCallId);

	//int overdueDeviceId = event->getDeviceId();
	//assert(overdueDeviceId >= 0);

	//Sptr <UaCallContainer> calls;
	Sptr <UaCallInfo> callInfo;
	callInfo.dynamicCast( event->getCallInfo() );
	
	if (callInfo != 0)
	{
		cpDebug(LOG_DEBUG, "------------ DELETE CALLS -----------");
		Sptr < UaCallContainer > calls;
	    	calls.dynamicCast( event->getCallContainer() );
	   	assert( calls != 0 );
	    	calls->deleteCall( *(UaDevice::instance()->getCallId( event->getDeviceId() )) ,event->getDeviceId());
	   	UaDevice::instance()->setCallId( 0 ,event->getDeviceId() );
	}
	//assert(callinfo);
	//
		
	return 0;
	
}

