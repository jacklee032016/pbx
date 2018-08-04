#ifndef __ISDN_MSG_HXX__
#define __ISDN_MSG_HXX__
/*
* $Id: IsdnMsg.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "global.h"
#include "vpbx_globals.h"

#include "Data.hxx"
#include "Sptr.hxx"
#include "VException.hxx"

#include "IsdnIE.hxx"

#include "as_msg.h"
#include "ask_isdn.h"
#include "l3ins.h"

#if USE_HASH_MAP 
#if defined(__linux__)
#include <hash_map>
#else
#undef USE_HASH_MAP
#endif
#endif

namespace Assist
{

class IeBearer;
class IeCause;
class IeCallId;
class IeChanId;
class IeProgress;
class IeNotify;
class IeDisplay;
class IeKeypad;
class IeSignal;
class IeConnectPN;
class IeCallingPN;
class IeCalledPN;
class IeUserUser;
class IeCallState;
class IeCallSub;
class IeRedirNR;
class IeRedirNR;
class IeFacility;		/* now only used in SETUP msg send to PBX */

class IeCharge;

class EndPoint;
class IsdnCtrlDevice;

typedef enum
{
	ISDN_TYPE_STATUS,	
	ISDN_TYPE_CC_TIMEOUT,	
	ISDN_TYPE_CC_NEW,
	ISDN_TYPE_SETUP,
	ISDN_TYPE_INFO,
	ISDN_TYPE_SETUP_ACK,
	ISDN_TYPE_PROCEEDING,
	ISDN_TYPE_PROGRESS,
	ISDN_TYPE_ALERTING,
	ISDN_TYPE_CONNECT,
	ISDN_TYPE_CONNECT_ACK,
	ISDN_TYPE_DISCONNECT,
	ISDN_TYPE_RELEASE,
	ISDN_TYPE_RELEASE_COMPLETE_IND,
	ISDN_TYPE_RELEASE_COMPLETE_CFM,
	ISDN_TYPE_NOTIFY,
	ISDN_TYPE_HOLD,
	ISDN_TYPE_RETRIEVE,
	ISDN_TYPE_SUSPEND,
	ISDN_TYPE_SUSPEND_ACK,
	ISDN_TYPE_SUSPEND_REJECT,
	ISDN_TYPE_RESUME,
	ISDN_TYPE_RESUME_ACK,
	ISDN_TYPE_RESUME_REJECT,
	ISDN_TYPE_FACILITY,
	ISDN_TYPE_RELEASE_CR,
	ISDN_TYPE_NEW_CR,
	ISDN_TYPE_UNKNOWN,
	
} ISDN_METHOD;


enum
{
	ERROR_MSG_IS_NULL = 1,
	ERROR_MSG_TOO_SHORT,	
};

#define ISDN_L3_MT_INVALIDATE				-1

class IsdnMsgParserException : public VException
{
	public:
		IsdnMsgParserException( const string& msg, const string& file, const int line, const int error = 0 );

		string getName( void ) const;
		
	protected:	
		int value;

};


/**
    This class is the base class for all ISDN Messages. It is just a container
    class of all IsdnIEs. 
*/
class IsdnMsg
{
	public:
		IsdnMsg(const msg_t *_msg, IsdnCtrlDevice *_ctrl)  throw(IsdnMsgParserException &);
		IsdnMsg(IsdnCtrlDevice *dev);
		IsdnMsg(const IsdnMsg&);

		virtual ~IsdnMsg();
		IsdnMsg& operator=(const IsdnMsg& newSipMsg);

		bool operator<(const IsdnMsg& src) const;
		bool operator ==(const IsdnMsg& src) const;
		
		/* enum type defined in C++ environments */
		virtual ISDN_METHOD getType() const = 0;

		/* MT defined in Q931  */
		virtual int getMsgType() const =0 ;
		/* CC msg type, such as CC_SETUP, used in create new msg_t */
		virtual int getCCType() const = 0;

		Sptr <IsdnMsg> clone();

		ostream& brief(ostream& strm) const;

		IsdnCtrlDevice *getCtrlDevice();
		
#if 0
		void setDevice(Sptr <CtrlDevice> _dev)
		{
			dev = _dev;
		};
		Sptr <EndPoint> getEndPoint()
		{
			return endPoint;
		};
		void setEndPoint(Sptr <EndPoint> _ep)
		{
			endPoint = _ep;
		};
#endif


//		void setFrameInfo(int _frameInfo);
		int getFrameInfo();

//		void setCallRef(int _callRef);
//		int getCallRef();

#if WITH_DEBUG
		virtual void debugInfo() ;
#endif
		/* send to DeviceCommandThread */
		int sendOut(Sptr <EndPoint> ep);

		virtual const char *getName() = 0;

	protected:
		virtual void encode( msg_t *msg) = 0;
		
		virtual int decode(const unsigned char *data) throw (IsdnMsgParserException &) =0;

		virtual int getMsgHeaderLength();

		IsdnCtrlDevice 			*myCtrl;			/* which port this msg belong to */
	private:
		/* read-only attributes: init in constructot */
		int 						frameInfo;		/* Frame info added in drivers */

		int 						callRef;
		
#if 0
		Sptr <EndPoint>			endPoint;	/* ISDN msg for this EndPoint */
#endif

};

ostream& operator<<(ostream& s, const IsdnMsg &msg);


#if USE_HASH_MAP
struct hash<IsdnMsg>
{
	size_t operator()(const IsdnMsg& s) const ;
};
#endif

/// this is now a public function to copy a pointer to a smart pointer
Sptr<IsdnMsg> copyPtrtoSptr(IsdnMsg* sipMsg);  
 
}

#endif

