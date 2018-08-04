#ifndef REFERMSG_H
#define REFERMSG_H
/*
 * $Id: ReferMsg.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */

#include "SipCommand.hxx"
#include "StatusMsg.hxx"

namespace Vocal
{

///
class ReferMsg : public SipCommand
{
    public:
        ///
        enum ReferMsgForward { referMsgForward };

        ///
        ReferMsg();

        /** Constructor based on a status received and the Refer-To 
          * URI(for UAC use)
          */
        ReferMsg( const StatusMsg&, Sptr <BaseUrl> referToUri );

        /** Construct a REFER base on another REFER for forwarding to the next
         *  hop (for proxy use)
         */
        ReferMsg( const ReferMsg&, enum ReferMsgForward );

        /** construct a REFER based on a request received and the Refer-To URI
         *  (for UAS use)
         */
        ReferMsg(const SipCommand& command, Sptr <BaseUrl> referTo );


        ///
        ReferMsg( const Data& data );

        ///
        const ReferMsg& operator =( const ReferMsg& );

        ///
        bool  operator ==( const ReferMsg& );

        ///
        ReferMsg( const ReferMsg& );

        ///
        virtual Method getType() const;

        ///
        virtual ~ReferMsg();

    private:
        ///
        void setReferDetails(const SipMsg&, Sptr <BaseUrl> url);
};
 
} // namespace Vocal

#endif
