#ifndef SipReceiveStatus_HXX
#define SipReceiveStatus_HXX
/*
 * $Id: SipReceiveStatus.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */

#include "SipMsg.hxx"
#include "Sptr.hxx"
#include "Connection.hxx"

namespace Vocal
{


/// data container for SipReceiveStatus header - only used inside SIP stack
class SipReceiveStatus
{
    public:
        SipReceiveStatus();
        SipReceiveStatus( const SipReceiveStatus& other);
        SipReceiveStatus& operator = (const SipReceiveStatus& other) const;
        ///
        Sptr < Connection > tcpConnection;
        ///
        bool tcpValid;
        ///
        Sptr < SipMsg > sipmsg;
        ///
        bool sweepMsg;
};

} // namespace Vocal


#endif
