#ifndef REFERMSG_H
#define REFERMSG_H
/*
* $Log: ReferMsg.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:06:38  lizhijie
* no message
*
* $Id: ReferMsg.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "SipCommand.hxx"
#include "StatusMsg.hxx"

namespace Assist
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
 
}

#endif
