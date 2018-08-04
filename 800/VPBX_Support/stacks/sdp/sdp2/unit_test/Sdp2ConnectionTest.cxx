/*
* $Log: Sdp2ConnectionTest.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: Sdp2ConnectionTest.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "Verify.hxx"
#include "Sdp2Connection.hxx"

using Assist::SDP::SdpConnection;
//using Assist::SDP::NetworkType;
//using Assist::SDP::NetworkType;

int main()
{
    cpLogSetPriority(LOG_DEBUG_STACK);

    Data s("IN IP4 0.0.0.0");
    SdpConnection conn(s);

    test_verify(conn.getNetworkType() == Assist::SDP::NetworkTypeInternet);

    test_verify(conn.getAddressType() == Assist::SDP::AddressTypeIPV4);
    
    test_verify(conn.encode() == "c=IN IP4 0.0.0.0\r\n"); 

    cout << conn.encode() << endl;

    return test_return_code(3);
}
