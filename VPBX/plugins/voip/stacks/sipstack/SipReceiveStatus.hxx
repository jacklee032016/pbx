#ifndef SipReceiveStatus_HXX
#define SipReceiveStatus_HXX
/*
* $Log: SipReceiveStatus.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:06:38  lizhijie
* no message
*
* $Id: SipReceiveStatus.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "SipMsg.hxx"
#include "Sptr.hxx"
#include "Connection.hxx"

namespace Assist
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

}

#endif
