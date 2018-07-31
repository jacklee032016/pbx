#ifndef __VOIP_CALL_HXX__
#define __VOIP_CALL_HXX__

#include "Sptr.hxx"
#include "InviteMsg.hxx"
#include "AckMsg.hxx"
#include "TransferMsg.hxx"
#include "SipRoute.hxx"

#include "Contact.hxx"
#include "SipSdp.hxx"
#include "SystemInfo.hxx"

namespace Assist
{

class SipSdp;
    
typedef list<Sptr <Contact> > 			ContactList;
typedef vector <Sptr <SipRoute> > 		RouteList;

/*  */
class SipContext //: public CallInfo
{
	public:
		SipContext();
		~SipContext();

		void setOriginator( const bool yorn );
		bool isOriginator();

		void setCallee( const SipUrl url );
		SipUrl getCallee();

		Sptr <Contact> getContact();
		void setContact( Sptr <Contact> contact );

		Sptr <ContactList> getContactList();
		void addContact( const Sptr <Contact> contact );

		/// set the contact from a message
		void setContactMsg( Sptr<InviteMsg> msg );
		/// set the contact from a message
		void setContactMsg( InviteMsg& msg );

		Sptr <Contact> findContact( const SipMsg& msg );
		void removeContact( const Sptr <Contact> contact );
		void clearContactList();

		void setCaller( const SipUrl url );
		SipUrl getCaller();

		void setLocalSdp( const Sptr <SipSdp> sdp );
		Sptr <SipSdp> getLocalSdp();

		void setRemoteSdp( const Sptr <SipSdp> sdp );
		Sptr <SipSdp> getRemoteSdp();

		void setLocal2Sdp( const Sptr <SipSdp> sdp );

		Sptr <SipSdp> getLocal2Sdp();
		void setRemote2Sdp( const Sptr <SipSdp> sdp );

		Sptr <SipSdp> getRemote2Sdp();

		void setRingInvite( Sptr <InviteMsg> invite );
		Sptr <InviteMsg> getRingInvite();

		//added by lijie
		void setCalleeIdle(bool bIdle);
		bool getCalleeIdle();
		void setTimerSet(bool bSet);
		bool getTimerSet();
		void setAudioSet(bool bSet);
		bool getAudioSet();
		//added ends here

		void setRing2Invite( Sptr <InviteMsg> invite );
		Sptr <InviteMsg> getRing2Invite();
		
		void setCallerRoute1List( SipRecordRouteList recordrouteList );
		void setCalleeRoute1List( SipRecordRouteList recordrouteList );
		void addRoute1( const Sptr <SipRoute> sipRoute );
		vector < SipRoute* > getRoute1List();

		void setCallerRoute2List( SipRecordRouteList recordrouteList );
		void setCalleeRoute2List( SipRecordRouteList recordrouteList );
		void addRoute2( const Sptr <SipRoute> sipRoute );
		vector <SipRoute*> getRoute2List();

		void setXferMsg( Sptr <TransferMsg> transfer );
		Sptr <TransferMsg> getXferMsg();

		void setAckMsg( Sptr <AckMsg> ack );
		Sptr<AckMsg> getAckMsg();

		/* added by wuaiwu, 2005/07/07 */
		void setAuthorization(bool m);
		bool getAuthorization() const;

		// void setAuthenTimerId(FifoEventId r);
		// FifoEventId getAuthenTimerId() const;
		/* end of added */
	protected:

	private:
		bool 						isCalleeIdle;
		bool 						bTimerSet;
		bool 						bAudioSet;
		//added ends here

		bool 						originator;
		SipUrl 						callee;

		Sptr <Contact> 				currentContact;
		Sptr <ContactList> 			contactList;

		SipUrl 						caller;

		Sptr <SipSdp> 				localSdp;
		Sptr <SipSdp> 				remoteSdp;
		Sptr <SipSdp> 				local2Sdp;
		Sptr <SipSdp> 				remote2Sdp;
		
		Sptr <InviteMsg> 				ringInvite;
		Sptr <InviteMsg> 				ring2Invite;

		Sptr <RouteList> 				route1List;
		Sptr <RouteList> 				route2List;

		Sptr <TransferMsg> 			xferMsg;
		Sptr <AckMsg> 				ackMsg;

		bool 						isAuthorization;

};
 
}

#endif

