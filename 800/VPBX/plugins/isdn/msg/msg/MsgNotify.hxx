#ifndef __MSG_NOTIFY_HXX__
#define __MSG_NOTIFY_HXX__
/*
* $Id: MsgNotify.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "IsdnMsg.hxx"

namespace Assist
{

class IeNotify;
class IeRedirDN;

/* CC_NOTIFY INDICATION */
class MsgNotify : public IsdnMsg //Command
{
	public:
		MsgNotify(const msg_t *msg, IsdnCtrlDevice *ctrl)  throw(IsdnMsgParserException &) ;
		MsgNotify( IsdnCtrlDevice *ctrl);
		MsgNotify( Sptr <IeNotify> _ieNotify, IsdnCtrlDevice *ctrl);
		MsgNotify(const MsgNotify&);
		
		virtual ~MsgNotify();

		MsgNotify& operator =(const MsgNotify&);
		bool operator ==(const MsgNotify&);

		virtual ISDN_METHOD getType() const;
		int getMsgType() const ;

		Sptr <IeNotify> getNotify();
		void setNotify(Sptr<IeNotify> _notify);

		Sptr <IeDisplay> getDisplay();
		void setDisplay(Sptr <IeDisplay> _display);

#if WITH_DEBUG
		void debugInfo();
#endif

		int getCCType() const ;
//		virtual msg_t *encode( Sptr <IsdnEndPoint> isdnEp);
		
		virtual const char *getName();

	protected:
	
		virtual void encode( msg_t *msg) ;
		int decode(const unsigned char *iesData)  throw(IsdnMsgParserException &) ;
		
	private:
		/* only 2 IEs in NOTIFY msg*/
		/* This is a mandatory IE */
		Sptr <IeNotify> 					ieNotify;

		/* optional IE */
		Sptr <IeDisplay> 					ieDisplay;
#if 0		
		/* this IE not defined in NOTIFY msg */
		Sptr <IeRedirDN> 					ieRedirDN;
#endif
};
 
}

#endif

