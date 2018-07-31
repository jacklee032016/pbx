#ifndef __MSG_CONNECT_HXX__
#define __MSG_CONNECT_HXX__

#include "IsdnMsg.hxx"
/*
* $Id: MsgConnect.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

namespace Assist
{

class IeChanId;
class IeConnectPN;
#ifdef CENTREX
class IeCentrex;
#endif

/* CC_CONNECT INDICATION */
class MsgConnect : public IsdnMsg//Command
{
	public:
		MsgConnect(const msg_t *msg, IsdnCtrlDevice *ctrl)   throw(IsdnMsgParserException &) ;
		MsgConnect(IsdnCtrlDevice *ctrl);
		MsgConnect(int _dlci, Sptr <IeChanId> chanId, IsdnCtrlDevice *ctrl);
		MsgConnect(const MsgConnect& src);

		virtual ~MsgConnect();

		MsgConnect& operator=(const MsgConnect&);
		bool operator ==(const MsgConnect& src); 

		virtual ISDN_METHOD getType() const;
		int getMsgType() const ;

#if 0
		/* Connect PN is not defined in INS */
		void setConnectPN(Sptr <IeConnectPN> connectPn);
		Sptr <IeConnectPN> getConnectPN();
#endif

		void setChanId(Sptr <IeChanId> chanId);
		Sptr <IeChanId> getChanId();
		
#ifdef CENTREX
		void setCentrex(Sptr <IeCentrex> centrex);

		Sptr <IeCentrex> getCentrex();
#endif

#if WITH_DEBUG
		void debugInfo();
#endif

		int getCCType() const ;
		
		virtual const char *getName();

	protected:
	
		virtual void encode( msg_t *msg) ;
		int decode(const unsigned char *iesData)  throw(IsdnMsgParserException &) ;
		
	private:
		int dlci;

		/* No mandatory IEs in the CONNECT msg */
		/* only optionally used in u-->n, eg. callee can select B channel */
		Sptr <IeChanId> 		ieChanId;

		/*optional IE, both directions */
		Sptr <IeProgress> 	ieProgress;

		/* optional IE, n-->u */
		Sptr <IeDisplay>		ieDisplay;

		/* optional IE, n-->u */
		Sptr <IeSignal>		ieSignal;
#if 0		
		Sptr <IeConnectPN> 	ieConnectPN;
#endif

#ifdef CENTREX
		Sptr <IeCentrex> 		ieCentrex;
#endif
};
 
}
 
#endif

