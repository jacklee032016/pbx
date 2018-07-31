#ifndef __MSG_RELEASE_HXX__
#define __MSG_RELEASE_HXX__
/*
* $Id: MsgRelease.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "IsdnMsg.hxx"

namespace Assist
{

class IeCause;
class IeDisplay;
class IeSignal;

/* CC_RELEASE INDICATION */
class MsgRelease : public IsdnMsg
{
	public:
		MsgRelease(const msg_t *_msf, IsdnCtrlDevice *ctrl)  throw(IsdnMsgParserException &) ;
		MsgRelease(IsdnCtrlDevice *ctrl );
		MsgRelease(Sptr <IeCause> _cause,IsdnCtrlDevice *ctrl);
		MsgRelease(const MsgRelease&);
		~MsgRelease();

		MsgRelease& operator =(const MsgRelease&);
		bool operator ==(const MsgRelease&);

		virtual ISDN_METHOD getType() const;
		int getMsgType() const ;

		Sptr <IeCause> getCause() const;
		void setCause(Sptr <IeCause> cause);

		Sptr <IeDisplay> getDisplay();
		void setDisplay(Sptr <IeDisplay> _display);

		Sptr <IeSignal> getSignal();
		void setSignal(Sptr <IeSignal> _signal);

#if WITH_DEBUG
		void debugInfo();
#endif

		int getCCType() const ;
		
		virtual const char *getName();

	protected:
	
		virtual void encode( msg_t *msg) ;
		int decode(const unsigned char *iesData)  throw(IsdnMsgParserException &) ;

    private:
		/* all IEs in the RELEASE msg are optional */
		Sptr <IeCause > 		ieCause;

		/*following 2 IEs are just n-->u */
		Sptr <IeDisplay>		ieDisplay;

		Sptr <IeSignal>		ieSignal;

		/* other IEs with locking shift */
};
 
}

#endif

