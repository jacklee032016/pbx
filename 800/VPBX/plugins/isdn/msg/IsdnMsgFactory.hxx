#ifndef __ISDN_MSG_FACTORY_HXX__
#define __ISDN_MSG_FACTORY_HXX__
/*
* $Id: IsdnMsgFactory.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "global.h"
#include "vpbx_globals.h"

#include "IsdnMsg.hxx"

#include "Data.hxx"
#include "VException.hxx"

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
class IeFacility;
class IeProgress;
class IeNotify;
class IeDisplay;
class IeDate;
class IeKeypad;
class IeSignal;
class IeConnectPN;
class IeCallingPN;
class IeCalledPN;
class IeUserUser;
class IeComplete;
class IeRedirNR;
class IeRedirNR;

class MsgDecoder;

class CommonMsg;

class IsdnParserException : public VException
{
	public:
		IsdnParserException( const string& msg, const string& file, const int line, const int error = 0 );

		string getName( void ) const;

	private:
		
};


class IsdnMsgFactory
{
	public:
		static IsdnMsgFactory		*instance();
		
//		ISDN_METHOD getType(msg_t *rawMsg );
//		ISDN_METHOD getType(const iframe_t  *frm );
	
		IsdnMsg* decodeRawMsg(const msg_t *msg, IsdnCtrlDevice *ctrl);

#if WITH_Q931_DEBUG
		/* only support TE mode */
		void debugQ931Info(const iframe_t *_frm);
#endif
      
    protected:


    private:
		IsdnMsgFactory();

		~IsdnMsgFactory();
		
		IsdnMsg& operator=(const IsdnMsg& newSipMsg);

		static IsdnMsgFactory				*myInstance;
		
};

}
#endif

