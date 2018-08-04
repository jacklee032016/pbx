#ifndef UaCallContainer_H
#define UaCallContainer_H

/*
 * $Id: UaCallContainer.hxx,v 1.2 2007/03/02 04:19:29 lizhijie Exp $
 */

#include <Sptr.hxx>
#include <CallContainer.hxx>

#include "UaCallInfo.hxx"

namespace Vocal
{

/* added by lizhijie , 2005.02.17 */
class UaCallInfoKey
{
	public:
		UaCallInfoKey(const SipCallId &_callid, int _deviceId);
		UaCallInfoKey(const UaCallInfoKey& src);
		~UaCallInfoKey();
		
		bool operator == ( const UaCallInfoKey& other ) const;

		bool operator != (const UaCallInfoKey& other) const
		{ 
			return !(*this == other);
		}

		bool operator < ( const UaCallInfoKey& other ) const;
		bool operator > ( const UaCallInfoKey& other ) const;
		
		const UaCallInfoKey& operator = ( const UaCallInfoKey& src );
	

	protected:
		SipCallId		callId;
		int 			deviceId;
		
};

/**
 */
class UaCallContainer : public CallContainer
{
	public:
		///
		UaCallContainer();
		///
		~UaCallContainer();

		///
		Sptr < UaCallInfo > findCall( const SipCallId& callId , int deviceId);
		///
		Sptr < UaCallInfo > newCall( const SipCallId& callId ,int deviceId );
		///
		void deleteCall( const SipCallId& callId , int deviceId);

		bool replaceCallDeviceId( const SipCallId& callId , int oldDeviceId, int newDeviceID);

	protected:

	private:
#if 0		
		typedef map < SipCallId, Sptr < UaCallInfo > > CallMap;
#endif
		typedef map < UaCallInfoKey, Sptr < UaCallInfo > > CallMap;

		CallMap calls;

		int numOfValidCalls;

};
 
}

#endif
