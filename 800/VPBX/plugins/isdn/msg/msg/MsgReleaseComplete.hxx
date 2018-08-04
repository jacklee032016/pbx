#ifndef __MSG_RELEASE_COMPLETE_HXX__
#define __MSG_RELEASE_COMPLETE_HXX__
/*
* $Id: MsgReleaseComplete.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "IsdnMsg.hxx"

namespace Assist
{

class IeCause;

/* CC_RELEASE_COMPLETE INDICATION */
class MsgReleaseComplete : public IsdnMsg
{
	public:
		MsgReleaseComplete(const msg_t *msg, IsdnCtrlDevice *ctrl)  throw(IsdnMsgParserException &);
		MsgReleaseComplete(IsdnCtrlDevice *ctrl );
		MsgReleaseComplete(Sptr <IeCause> _ieCause, IsdnCtrlDevice *ctrl);
		MsgReleaseComplete(const MsgReleaseComplete&);
		virtual ~MsgReleaseComplete();

		MsgReleaseComplete& operator =(const MsgReleaseComplete&);
		bool operator ==(const MsgReleaseComplete&);

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

		/* all IEs in the RELEASE_COMPLETE msg are optional, refer to p.25 */
		Sptr <IeCause> 		ieCause;

		/*following 2 IEs are just n-->u */
		Sptr <IeDisplay>		ieDisplay;

		Sptr <IeSignal>		ieSignal;

		/* other IEs with locking shift */
};
 
}

#endif

