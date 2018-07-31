#ifndef __MSG_FACILITY_HXX__
#define __MSG_FACILITY_HXX__
/*
* $Id: MsgFacility.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "IsdnMsg.hxx"

namespace Assist
{

//class IeFacility;
class IeDisplay;

/* CC_FACILITY INDICATION */
class MsgFacility : public IsdnMsg//Command
{
	public:
		MsgFacility(const msg_t *_msg, IsdnCtrlDevice *ctrl)   throw(IsdnMsgParserException &) ;
		MsgFacility(IsdnCtrlDevice *ctrl);
		MsgFacility(const MsgFacility&);
		~MsgFacility();

		MsgFacility& operator =(const MsgFacility&);

		bool operator ==(const MsgFacility&);

		virtual ISDN_METHOD getType() const;
		int getMsgType() const ;

#if 0
		Sptr <IeFacility> getFacility() const;
		void setFacility(Sptr <IeFacility> _facility);
#endif

		Sptr <IeDisplay> getDisplay() const;
		void setDisplay(Sptr <IeDisplay> _display);

#if WITH_DEBUG
		void debugInfo();
#endif

		int getCCType() const ;
		
		virtual const char *getName();

	protected:
	
		virtual void encode( msg_t *msg) ;
		int decode(const unsigned char *iesData)  throw(IsdnMsgParserException &) ;

    private:
#if 0
		Sptr <IeFacility> 			ieFacility;
#endif
		Sptr <IeDisplay>			ieDisplay;
};
 
}

#endif

