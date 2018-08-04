/*
* $Id: IsdnStateBuilder.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#ifndef	__ISDN_STATE_BUILDER_HXX__
#define	__ISDN_STATE_BUILDER_HXX__

#include "EpState.hxx"
#include "IsdnEndPoint.hxx"

namespace Assist
{

/* EndPoint State for Destination interface : for incoming call */
class IsdnStateDest : public EpState
{
	public:
		IsdnStateDest(const string &name, int state):EpState(name, state){};
		virtual ~IsdnStateDest(){};
};

/* EndPoint State for Originate interface : for outgoing call */
class IsdnStateSrc : public EpState
{
	public:
		IsdnStateSrc(const string &name, int state):EpState(name, state){};
		virtual ~IsdnStateSrc(){};
};


#define ISDN_DEST_STATE_DECLARE(type, name, state)  \
class type : public IsdnStateDest \
{ \
public: \
	type() : IsdnStateDest(name, state){ initState();}; \
	~type(){}; \
	virtual void  initState(); \
}

#define ISDN_SRC_STATE_DECLARE(type, name, state)  \
class type : public IsdnStateSrc \
{ \
public: \
	type() : IsdnStateSrc(name, state) { initState();}; \
	~type(){}; \
	virtual void  initState(); \
}


/* connect is sent to the network, waiting for acknowledge */
ENDPOINT_STATE_DECLARE(IsdnStateConnect, 		"IsdnStateConnect", 			ISDN_EP_STATE_CONNECT);
/* connect is sent to the network, waiting for acknowledge */
ENDPOINT_STATE_DECLARE(IsdnStateError, 			"IsdnStateError", 				ISDN_EP_STATE_ERROR);
ENDPOINT_STATE_DECLARE(IsdnStateIdle, 			"IsdnStateIdle", 				ISDN_EP_STATE_IDLE);
/* after connect is break, enter into this state to wait RELEASE_CR msg send by local driver */
ENDPOINT_STATE_DECLARE(IsdnStateWaitClearCR, 	"IsdnStateWaitClearCR", 		ISDN_EP_STATE_WAIT_CLEAR_CR);



/* call is ringing. for a incoming Call in destination interface */
ISDN_DEST_STATE_DECLARE(IsdnStateAlertingIn, 	"IsdnStateAlertingIn", 			ISDN_EP_STATE_IN_ALERTING);
/* incoming disconnected */
ISDN_DEST_STATE_DECLARE(IsdnStateDisconnectIn, 	"IsdnStateDisconnectIn", 		ISDN_EP_STATE_IN_DISCONNECT);
/* call is proceeding.  for an incoming call in destination interface */
ISDN_DEST_STATE_DECLARE(IsdnStateProceedingIn, 	"IsdnStateProceedingIn", 		ISDN_EP_STATE_IN_PROCEEDING);
/* for an incoming connection in destination interface */
ISDN_DEST_STATE_DECLARE(IsdnStateSetupIn, 		"IsdnStateSetupIn", 			ISDN_EP_STATE_IN_SETUP);


/* call is ringing. for an outgoing call in originate interface */
ISDN_SRC_STATE_DECLARE(IsdnStateAlertingOut, 	"IsdnStateAlertingOut", 			ISDN_EP_STATE_OUT_ALERTING);
/* outgoing disconnected */
ISDN_SRC_STATE_DECLARE(IsdnStateDisconnectOut, 	"IsdnStateDisconnectOut", 		ISDN_EP_STATE_OUT_DISCONNECT);
/* call is proceeding. for an outgoing call in originate interface */
ISDN_SRC_STATE_DECLARE(IsdnStateProceedingOut, 	"IsdnStateProceedingOut", 		ISDN_EP_STATE_OUT_PROCEEDING);
/* for an outgoing call in originate interface */
ISDN_SRC_STATE_DECLARE(IsdnStateSetupOut, 		"IsdnStateSetupOut", 			ISDN_EP_STATE_OUT_SETUP);

}

#endif

