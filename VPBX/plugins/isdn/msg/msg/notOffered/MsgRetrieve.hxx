#ifndef __MSG_RETRIEVE_HXX__
#define __MSG_RETRIEVE_HXX__
/*
* $Id: MsgRetrieve.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "IsdnMsg.hxx"

namespace Assist
{

class IeChanId;

/* CC_RETRIEVE INDICATION */
class MsgRetrieve : public IsdnMsg
{
	public:
		MsgRetrieve(const iframe_t *_frm)  throw(IsdnMsgParserException &) ;
		MsgRetrieve(const MsgRetrieve&);
		MsgRetrieve(Sptr <IeChanId> _chanId);
		MsgRetrieve( );
		virtual ~MsgRetrieve();

		bool operator ==(const MsgRetrieve& src);  
		MsgRetrieve& operator =(const MsgRetrieve&);

		ISDN_METHOD getType() const;
		int getMsgType() const ;

		Sptr <IeChanId>  getChanId() const;
		void setChanId(Sptr <IeChanId> chanid);

#if WITH_DEBUG
		void debugInfo();
#endif

		virtual const char *getName();

	protected:
	
		int decode(const iframe_t *_frm)  throw(IsdnMsgParserException &) ;

    private:
		Sptr <IeChanId> 		ieChanId;
};
 
}

#endif

