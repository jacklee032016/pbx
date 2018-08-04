#ifndef UaBuilder_Hxx
#define UaBuilder_Hxx

/*
 * $Id: UaBuilder.hxx,v 1.2 2007/03/02 04:19:29 lizhijie Exp $
 */

#include <Builder.hxx>
#include "SipTransceiver.hxx"

#include "RegistrationManager.hxx"
#include "SubscribeManager.hxx"

namespace Vocal
{

/// Forward declarations
class UaDeviceEvent;
class UaDigitTimerEvent;
class UaTimerEvent;
class UaCallContainer;
class UaStateMachine;
class SipEvent;


/**
 *
 */
class UaBuilder : public Builder
{
public:
    ///
    UaBuilder();
    ///
    void process( const Sptr < SipProxyEvent > nextEvent );
    ///
    void setSipStack( const Sptr < SipTransceiver > stack );
    ///
    void startRegistration();
    ///
    void setSubscribeManager( const Sptr< SubscribeManager >
			      subscribeManager );
    ///
    ~UaBuilder();

	/**
	*    For that
	*    Initialize the calls (UaCallContainer) in this function one times
	*    not in process for every SipProxyEvent what with we have only
	*    one type of CallContainer(UaCallContainer).
	*    The same to initFeature()
	*    
	*    added by wuaiwu, 2005/10/14
	*/
	void initCallContainer();

	///
	void initFeature();
	///
	static Sptr < RegistrationManager > getRegManager() { return registrationManager;}
protected:
    ///

private:
    ///
    bool processSipEvent( const Sptr < SipEvent > sipEvent );
    ///
    bool processUaDeviceEvent( const Sptr < UaDeviceEvent > uaDeviceEvent );
    ///
    bool processUaDigitEvent( const Sptr < UaDigitTimerEvent > uaDigitEvent );
    ///
    bool processUaTimerEvent( const Sptr < UaTimerEvent > uaTimerEvent );
    ///
    bool handleStatusMsg( const Sptr < SipEvent > sipEvent );
    ///

    //modified by lijie 2005-06-03	
    //bool handleCallWaiting( Sptr< CallInfo > callInfo , int deviceId);
    bool handleCallWaiting( const Sptr < SipEvent > sipEvent  );
	
    ///
    void sendEvent( const Sptr < SipProxyEvent > nextEvent );
    void sendEvent( const Sptr < SipProxyEvent > nextEvent ,Sptr <CallInfo> call );

    ///
    

    ///
    static Sptr < RegistrationManager > registrationManager;
    ///
    Sptr < SipTransceiver > sipStack;
    ///
    Sptr < SubscribeManager > subManager;
    ///
    Sptr< UaCallContainer > calls;
    ///
    SipCallId callId;
    ///
    Sptr< CallInfo > callInfo;
    ///
    Sptr< UaStateMachine > stateMachine;
};
 
}

#endif
