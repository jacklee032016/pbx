/*
 * $Id: UaCallContainer.cxx,v 1.2 2007/03/02 04:19:29 lizhijie Exp $
 */


#include "global.h"
#include <LockHelper.hxx>

#include "UaCallContainer.hxx"
#include "UaCallInfo.hxx"
#include "UaDevice.hxx"

using namespace Vocal;


/* added by lizhijie, 2005.02.17 */
UaCallInfoKey::UaCallInfoKey(const SipCallId &_callid, int _deviceId)
{
	callId = _callid;
	deviceId = _deviceId;
}

UaCallInfoKey::UaCallInfoKey(const UaCallInfoKey& src)
        :
        callId(src.callId),
        deviceId(src.deviceId)
{}


UaCallInfoKey::~UaCallInfoKey()
{
}

bool UaCallInfoKey::operator== ( const UaCallInfoKey& other ) const
{
	cpDebug(LOG_DEBUG_STACK, "UaCallInfoKey == operator");
	
	bool equal = false;
	equal = ((callId== other.callId) && ( deviceId == other.deviceId));

	cpDebug(LOG_DEBUG_STACK, "UaCallInfoKey == operator, returning %d", equal);
	return equal;
}

bool UaCallInfoKey::operator < ( const UaCallInfoKey& other ) const
{
//    Data selfstr = (const_cast < SipCallId* > (this))->encode(); // encode();
//    Data otherstr = (const_cast < SipCallId& > (other)).encode(); // other.encode();

	if ( deviceId < other.deviceId )
	{
		return true;
	}
//added by lijie
	else if(deviceId == other.deviceId)
	{
		if(callId < other.callId)
			return true;
		else
			return false;
	}
//added ends here	
	return false;
}

bool UaCallInfoKey::operator > ( const UaCallInfoKey& other ) const
{
//    Data selfstr = (const_cast < SipCallId* > (this))->encode(); // encode();
//    Data otherstr = (const_cast < SipCallId& > (other)).encode(); // other.encode();

	if ( deviceId > other.deviceId)
	{
		return true;
	}
//added by lijie	
	else if(deviceId == other.deviceId)
	{
		if(callId > other.callId)
			return true;
		else
			return false;
	}	
//added ends here
	return false;
}

const UaCallInfoKey& UaCallInfoKey::operator=(const UaCallInfoKey& src)
{
	if ( &src != this)
	{
		callId = src.callId;
		deviceId = src.deviceId;
	}
	return (*this);
}
/* end of added */

///
UaCallContainer::UaCallContainer()
{
	numOfValidCalls = 0;
}


///

UaCallContainer::~UaCallContainer()
{}


///

Sptr < UaCallInfo > UaCallContainer::newCall( const SipCallId& callId , int deviceId )
{
    Sptr < UaCallInfo > call = new UaCallInfo;
    assert( call != 0 );

    SipCallId callid = callId;

    //this is a workaround for the conferencing
    callid.setHost("\0");

	UaCallInfoKey key( callid, deviceId );
    LockHelper lockHelper( myMutex );
//    calls.insert( pair < SipCallId, Sptr < UaCallInfo > > ( callid, call ) );
	calls.insert( pair < UaCallInfoKey, Sptr < UaCallInfo > > ( key, call ) );

	cpDebug(LOG_DEBUG, "New Call %s with deviceId %d", callId.encode().logData() , deviceId );
	//added by lijie
	if(deviceId != AS_DEVICE_NOT_DEFINED)
	{
		numOfValidCalls++;
		UaDevice::LockDevice();
		UaDevice::setCanExit(false);
		UaDevice::UnlockDevice();
	}

	UaDevice::instance()->incCallCount(deviceId);	
	//UaDevice::instance()->reportDeviceInfo();
	//added ends here
	UaDevice::instance()->reportDeviceInfo(deviceId,NEW_CALL);

	return call;
}


///
Sptr < UaCallInfo > UaCallContainer::findCall( const SipCallId& callId , int deviceId )
{
    Sptr < UaCallInfo > call;

    LockHelper lockHelper( myMutex );

    SipCallId callid = callId;
    callid.setHost("\0");

	UaCallInfoKey key = UaCallInfoKey( callid, deviceId );
	CallMap::iterator iter = calls.find( key );
//	CallMap::iterator iter = calls.find( callid );

    if ( iter != calls.end() )
    {
        cpDebug(LOG_DEBUG, "Found Call %s", callId.encode().logData() );
        call = iter->second;
        assert( call != 0 );
		cpDebug(LOG_DEBUG, "Current State is %s", call->getState()->name() );
    }
    else
    {
        cpDebug(LOG_DEBUG, "Call %s with deviceId %d not found", callId.encode().logData() ,deviceId );
    }
    return call;
}


///
void UaCallContainer::deleteCall( const SipCallId& callId ,int deviceId )
{
    //Sptr < UaCallInfo > call;

    LockHelper lockHelper( myMutex );

    SipCallId callid = callId;
    callid.setHost("\0");

	UaCallInfoKey key(callid, deviceId);
	CallMap::iterator iter = calls.find( key );
//	CallMap::iterator iter = calls.find( callid );

	if ( iter != calls.end() )
	{
		cpDebug(LOG_DEBUG, "Delete Call %s with deviceId %d", callId.encode().logData() , deviceId );
		iter->second->removeTimer();
		calls.erase( iter );
		
		//added by lijie 2005-07-05	
		if(deviceId != AS_DEVICE_NOT_DEFINED)
		{
			numOfValidCalls--;
			UaDevice::LockDevice();
			if(!numOfValidCalls)
				UaDevice::setCanExit(true);
			else
				UaDevice::setCanExit(false);
			UaDevice::UnlockDevice();
		}	

		UaDevice::instance()->decCallCount(deviceId);		
		//UaDevice::instance()->reportDeviceInfo();
		UaDevice::instance()->reportDeviceInfo(deviceId,DELETE_CALL);
		//added ends here
	}
	else
	{
		cpDebug(LOG_DEBUG, "Call %s with deviceId %d not found", callId.encode().logData(), deviceId );
	}
}

bool UaCallContainer::replaceCallDeviceId( const SipCallId& callId , int oldDeviceId, int newDeviceID)
{
	Sptr < UaCallInfo > call;

	LockHelper lockHelper( myMutex );

	SipCallId callid = callId;
	callid.setHost("\0");

	UaCallInfoKey key = UaCallInfoKey( callid, oldDeviceId );
	CallMap::iterator iter = calls.find( key );

	if ( iter != calls.end() )
	{
		call = iter->second;
		assert( call != 0 );
		cpDebug(LOG_DEBUG, "replace : Found Call %s with deviceID %d", callId.encode().logData(), oldDeviceId);
		const UaCallInfoKey newkey = UaCallInfoKey(callid, newDeviceID );

		calls.erase( iter );
		calls.insert( pair < UaCallInfoKey, Sptr < UaCallInfo > > ( newkey, call ) );
		//iter->first = newkey;
		cpDebug(LOG_DEBUG, "CallInfo is replaced" );
//		assert( call != 0 );
//		cpDebug(LOG_DEBUG, "Current State is %s", call->getState()->name() );
		UaDevice::instance()->incCallCount(newDeviceID);
		UaDevice::instance()->decCallCount(oldDeviceId);

		//UaDevice::instance()->reportDeviceInfo();	
		UaDevice::instance()->reportDeviceInfo(newDeviceID,NEW_CALL);	
		UaDevice::instance()->reportDeviceInfo(newDeviceID,DELETE_CALL);	
		return true;
	}
	else
	{
		cpDebug(LOG_DEBUG, "Call %s with deviceId %d not found", callId.encode().logData() ,oldDeviceId );
	}

	return false;
}

