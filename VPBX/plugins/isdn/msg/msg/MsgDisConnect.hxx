#ifndef __MSG_DIS_CONNECT_HXX__
#define __MSG_DIS_CONNECT_HXX__

/*
* $Id: MsgDisConnect.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "IsdnMsg.hxx"

namespace Assist
{

class IeProgress;
class IeCause;

/* CC_DISCONNECT INDICATION */
class MsgDisConnect : public IsdnMsg
{
	public:
		MsgDisConnect(const msg_t *msg, IsdnCtrlDevice *ctrl)  throw(IsdnMsgParserException &) ;
		MsgDisConnect(IsdnCtrlDevice *ctrl);
		MsgDisConnect(Sptr <IeCause> _ieCause, IsdnCtrlDevice *ctrl);
		MsgDisConnect(const MsgDisConnect& src);

		virtual ~MsgDisConnect();

		MsgDisConnect& operator=(const MsgDisConnect&);
		bool operator ==(const MsgDisConnect& src); 

		virtual ISDN_METHOD getType() const;
		int getMsgType() const ;

		void setCause(Sptr <IeCause> cause);
		Sptr <IeCause> getCause();
		
		void setProgress(Sptr <IeProgress> _progress);
		Sptr <IeProgress> getProgress();
		
		void setCharge(Sptr <IeCharge> _charge);
		Sptr <IeCharge> getCharge();
		

#if WITH_DEBUG
		void debugInfo();
#endif

		int getCCType() const ;
		
		virtual const char *getName();

	protected:
	
		virtual void encode( msg_t *msg) ;
		int decode(const unsigned char *msg)  throw(IsdnMsgParserException &) ;
		
	private:
		/*p.19*/
		
		/* this IE is mandatory*/
		Sptr <IeCause> 		ieCause;

		/* optional, u-->n is not offered*/
		Sptr <IeProgress> 	ieProgress;

		/* following 2 IEs are option, and only used in n-->u */
		Sptr <IeDisplay>		ieDisplay;
		Sptr <IeSignal>		ieSignal;

		Sptr <IeCharge>		ieCharge;

		/*other IEs for locking shift (codeset 6)*/
		
};
 
}
 
#endif

