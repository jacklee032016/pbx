#ifndef __MSG_CC_NEW_HXX__
#define __MSG_CC_NEW_HXX__
/*
* $Id: MsgCCNew.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "IsdnMsg.hxx"

namespace Assist
{

/* CC_NEW INDICATION */
/* this Msg is only defined in our software, not q931 */
class MsgCCNew : public IsdnMsg
{
	public:
		MsgCCNew(const msg_t *_msg, IsdnCtrlDevice *ctrl)  throw(IsdnMsgParserException &) ;
		MsgCCNew(IsdnCtrlDevice *ctrl);
		MsgCCNew(const MsgCCNew&);
		~MsgCCNew();

		MsgCCNew& operator =(const MsgCCNew&);
		bool operator ==(const MsgCCNew&);

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

