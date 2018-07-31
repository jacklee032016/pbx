#ifndef __MSG_ALERTING_HXX__
#define __MSG_ALERTING_HXX__
/*
* $Id: MsgAlerting.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "IsdnMsg.hxx"

namespace Assist
{

class IeChanId;
class IeProgress;

/* CC_ALERTING INDICATION */
class MsgAlerting : public IsdnMsg//Command
{
	public:
		MsgAlerting(const msg_t *msg, IsdnCtrlDevice *ctrl)   throw(IsdnMsgParserException &) ;
		MsgAlerting(IsdnCtrlDevice *ctrl);
//		MsgAlerting( Sptr <IeChanId> chanId, Sptr <IeProgress> progress, IsdnCtrlDevice *ctrl);
		MsgAlerting(const MsgAlerting&);
		
		virtual ~MsgAlerting();
		
		MsgAlerting& operator =(const MsgAlerting&);
		bool operator ==(const MsgAlerting& src) ;

		ISDN_METHOD getType() const;
		int getMsgType() const ;


		void setChanId( Sptr <IeChanId> chanId);
		
		Sptr <IeChanId> getChanId();

		void setProgress(Sptr <IeProgress> progress);

		Sptr <IeProgress> getProgress();

#if WITH_DEBUG
		void debugInfo();
#endif

		int getCCType() const ;
		
		virtual const char *getName();

	protected:
	
		virtual void encode( msg_t *msg) ;
		
		/* data only keep IEs */
		int decode(const unsigned char *ieDatas)  throw(IsdnMsgParserException &) ;
		
	private:
		/* all IEs in ALERTING are optional, p.15 */

		Sptr <IeChanId> 			ieChanId;
		Sptr <IeProgress> 		ieProgress;


		/* following 2 IEs only are used in n-->u */
		Sptr <IeDisplay>			ieDisplay;
		Sptr <IeSignal>			ieSignal;
		
};
 
}

#endif

