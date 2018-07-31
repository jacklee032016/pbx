#ifndef __MSG_SETUP_ACK_HXX__
#define __MSG_SETUP_ACK_HXX__
/*
* $Id: MsgSetupAck.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "IsdnMsg.hxx"

namespace Assist
{

class IeChanId;
class IeProgress;
class IeDisplay;
class IeSignal;

/* CC_SETUP_ACCNOWLEDGE INDICATION */
/* specs, p32, not offered in INS */
class MsgSetupAck : public IsdnMsg
{
	public:
		MsgSetupAck(const iframe_t *_frm)  throw(IsdnMsgParserException &) ;
		MsgSetupAck(const MsgSetupAck&);
		MsgSetupAck(Sptr <IeChanId> chanId, Sptr <IeProgress> progress);
		MsgSetupAck( );
		virtual ~MsgSetupAck();

		const MsgSetupAck& operator =(const MsgSetupAck&);

		bool operator ==(const MsgSetupAck& src);


		virtual ISDN_METHOD getType() const;
		int getMsgType() const ;

		Sptr <IeChanId>  getChanId() const;
		void setChanId(Sptr <IeChanId> chanId );

		Sptr <IeProgress>  getProgress() const;
		void setProgress(Sptr <IeProgress> progress);

		Sptr <IeDisplay>  getDisplay() const;
		void setDisplay(Sptr <IeDisplay> display);

		Sptr <IeSignal>  getSignal() const;
		void setSignal(Sptr <IeSignal> signal);

#if WITH_DEBUG
		void debugInfo();
#endif

		virtual const char *getName();

	protected:
	
		int decode(const iframe_t *_frm)  throw(IsdnMsgParserException &) ;

	private:

		Sptr <IeChanId> 		ieChanId;
		Sptr <IeProgress>		ieProgress;

		/* net --> user */
		Sptr <IeDisplay>		ieDisplay;
		Sptr <IeSignal>		ieSignal;
};
 
}

#endif

