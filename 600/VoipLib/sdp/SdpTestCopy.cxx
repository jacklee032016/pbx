/*
 * $Id: SdpTestCopy.cxx,v 1.2 2007/03/01 20:04:31 lizhijie Exp $
 */

#include "global.h"
#include "SdpSession.hxx"
#include <string>

void
copyTest()
{
    string sdp;
    sdp = "v=0\r\n";
    sdp += "o=mhandley 28908 23456 IN IP4 126.16.64.4\r\n";
    sdp += "s=SDP seminar\r\n";
    sdp += "i=information\r\n";
    sdp += "c=IN IP4 224.2.17.12/127\r\n";
    sdp += "t=1234567890 0\r\n";
    sdp += "a=recvonly\r\n";

    sdp += "m=audio 49230 RTP/AVP 96 97 98\r\n";
    sdp += "a=rtpmap:96 L8/8000\r\n";
    sdp += "a=rtpmap:97 L16/8000\r\n";
    sdp += "a=rtpmap:98 X-myrtpmap/8000\r\n";
    sdp += "c=IN IP4 128.2.124.5\r\n";

    sdp += "m=video 51372 RTP/AVP 31\r\n";
    sdp += "a=orient:portrait\r\n";
    sdp += "a=X-myAttrib:myValue\r\n";
    sdp += "a=ptime:100\r\n";
    sdp += "b=AS:64\r\n";

    SdpSession sdpDesc;
    sdpDesc.decode(sdp);

    SdpSession x;
    x = sdpDesc;
    if (x.encode() != sdpDesc.encode())
    {
        // error
        assert(0);
    }

    SdpSession y(x);

    if (x.encode() != y.encode())
    {
        assert(0);
    }

}


int
main()
{
    copyTest();
    return 0;
}
