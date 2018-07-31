#ifndef __MSG_CC_RELEASE_HXX__
#define __MSG_CC_RELEASE_HXX__
/*
* $Id: MsgCCRelease.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "IsdnMsg.hxx"

namespace Assist
{

/* CC_RELEASE INDICATION */
/* this Msg is only defined in our software, not q931 */
class MsgCCRelease : public IsdnMsg
{
	public:
		MsgCCRelease(const msg_t *_msg, IsdnCtrlDevice *ctrl)  throw(IsdnMsgParserException &) ;
		MsgCCRelease(IsdnCtrlDevice *ctrl);
		MsgCCRelease(const MsgCCRelease&);
		~MsgCCRelease();

		MsgCCRelease& operator =(const MsgCCRelease&);
		bool operator ==(const MsgCCRelease&);

		ISDN_METHOD getType() const;
		int getMsgType() const ;

#if WITH_DEBUG
		void debugInfo();
#endif
		int getCCType() const ;

		virtual const char *getName();

	protected:
	
		virtual void encode( msg_t *msg) ;
		int decode(const unsigned char *iesData)  throw(IsdnMsgParserException &) ;

	private:
		/* no info keep in this class */

};
 
}

#endif

