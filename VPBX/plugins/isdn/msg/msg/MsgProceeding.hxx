#ifndef __MSG_PROCEEDING_HXX__
#define __MSG_PROCEEDING_HXX__
/*
* $Id: MsgProceeding.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "IsdnMsg.hxx"

namespace Assist
{

class IeChanId;
class IeProgress;
class IeDisplay;

/* CC_PROCEEDING INDICATION */
class MsgProceeding : public IsdnMsg//Command
{
	public:
		MsgProceeding(const msg_t *msg, IsdnCtrlDevice *ctrl)   throw(IsdnMsgParserException &) ;
		MsgProceeding( IsdnCtrlDevice *ctrl);
		MsgProceeding(const MsgProceeding&);
//		MsgProceeding(Sptr <IeChanId> chanId, Sptr <IeProgress> progress);
		virtual ~MsgProceeding();

		MsgProceeding& operator =(const MsgProceeding&);
		bool operator ==(const MsgProceeding& src); 

		virtual ISDN_METHOD getType() const;
		int getMsgType() const ;

		Sptr <IeChanId> getChanId() const;
		void setChanId( Sptr <IeChanId> chanId);

		Sptr <IeProgress> getProgress() const;
		void setProgress(Sptr <IeProgress> progress);

		Sptr <IeDisplay> getDisplay() const;
		void setDisplay(Sptr <IeDisplay> display);

		/* for PROCEEDING msg send by NT side device */
		Sptr <IeFacility> getFacility() const;
		void setFacility(Sptr <IeFacility> facility);
		
#if WITH_DEBUG
		void debugInfo();
#endif

		int getCCType() const ;

		virtual const char *getName();

	protected:
	
		virtual void encode( msg_t *msg) ;
		int decode(const unsigned char *iesData)  throw(IsdnMsgParserException &) ;

	private:
		/* p.16 , no IE in the PROCEEDING L3 msg is mandatory*/
		/* Option IE. */
		Sptr <IeChanId> 				ieChanId;
		
		/* Option IE */
		Sptr <IeProgress> 			ieProgress;

		/* Option IE, n-->u */
		Sptr <IeDisplay>				ieDisplay;

		/* 2006.06.14 */
		Sptr <IeFacility>				ieFacility;
		
};
 
}

#endif

