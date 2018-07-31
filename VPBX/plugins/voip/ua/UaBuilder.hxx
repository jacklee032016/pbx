#ifndef __UA_BUILDER_HXX__
#define __UA_BUILDER_HXX__
/*
* $Id: UaBuilder.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "SipTransceiver.hxx"

#include "RegistrationManager.hxx"
#include "SubscribeManager.hxx"

namespace Assist
{

class VoipDeviceMgr;
class SipEvent;
class SubscribeThread;
class SubscribeManager;
class RegistrationManager;

class UaBuilder
{
	public:
		UaBuilder(VoipDeviceMgr *deviceMgr, unsigned short  defaultSipPort = 5060,
                        Data                    applName = "Assist SIP UserAgent", 
                        bool                    filteron = true, 
                        bool                    nat = false,
                        SipAppContext           aContext=APP_CONTEXT_GENERIC);
		
		~UaBuilder();
		void run();

		const Sptr <SipTransceiver> getSipStack();

		void startRegistration();

		static Sptr <RegistrationManager> getRegManager()
		{
			return myRegistrationManager;
		}
		
		bool processSipMsg( const Sptr <SipMsg> sipMsg, Sptr <SipMsgQueue> sipRcv);


	protected:

	private:
		void replyBusyHere(Sptr <InviteMsg> inviteMsg);


		bool handleStatusMsg( const Sptr <SipMsg> inviteMsg );

		//bool handleCallWaiting( Sptr< CallInfo > callInfo , int deviceId);
		bool handleCallWaiting( const Sptr <InviteMsg> inviteMsg );


		static Sptr <RegistrationManager> 		myRegistrationManager;

		Sptr <SipTransceiver> 					mySipStack;
		Sptr <SubscribeManager> 				mySubscribeManager;
		
		Sptr <SubscribeThread> 				mySubscribeThread;

#if	__CDR_OPEN__
		Sptr <RecordThread> 					myRecordThread;
#endif

#if 0        
       	 Sptr < LoadGenThread > 				loadGenThread;
#endif

		VoipDeviceMgr						*myDeviceMgr;
};
 
}

#endif
