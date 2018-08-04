#ifndef SUBSCRIBEMSG_H
#define SUBSCRIBEMSG_H
/*
 * $Id: SubscribeMsg.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */

#include "SipCommand.hxx"
#include "symbols.hxx"

namespace Vocal
{
    
///
class SubscribeMsg : public SipCommand
{
    public:
        enum SubscribeMsgForward { subscribeMsgForward };
        /**
	   Default constructor.  This constructor sets the same from
	   and to (i.e., the host creating the subscribe msg) and a
	   new call-id.  The request line is the UAS which notifies of
	   the different events.
	*/
        SubscribeMsg();
        ///
        SubscribeMsg(const Data& data);
        /**
	   call-member subscriptions.  The from, to, and call-id of the
	   call-leg is used in building the subscribe msg.  The request
	   URL of the UAS must be specified.
	*/
        SubscribeMsg(const SipCallLeg& callLeg, Sptr <BaseUrl> reqUrl);
        /**
	   third-party subscriptions.  The to is used in the to header
	   and also in the request line.  A port can be specified,
	   otherwise, it defaults to 5060.
	*/
        SubscribeMsg(const SipCallId& callId, const SipTo& to, int port = 5060);

        /// copy constructor
        SubscribeMsg(const SubscribeMsg&);

        ///
        SubscribeMsg( const SubscribeMsg&, enum SubscribeMsgForward );

        ///
        SubscribeMsg& operator =(const SubscribeMsg&);
	/// 
	bool operator ==(const SubscribeMsg& src); 
	///
        virtual ~SubscribeMsg();
        ///
        virtual Method getType() const;
    private:
        //set the via, expires, cseq fields and the request line.
        void setSubscribeDetails(Sptr <BaseUrl>  fromUrl, Sptr <BaseUrl> toUrl);
};

 
} // namespace Vocal

#endif
