/*
* $Id: main.cxx,v 1.2 2007/03/02 07:29:49 lizhijie Exp $
*/
#include "sdp.hxx"
#include <string>


void
testEncode (SDPDescriptor& sd)
{
    cout << "------ Encode begins ------" << endl;
    // Session Description
    // v=0 by default - no other value as of now
    // o= owner/creator and session identifier
    // s= session name
    // i=* session information
    // u=* URI of description
    // e=* email address
    // p=* phone number
    // c=* connection information (not required if included in all media
    // b=* bandwidth information
    // Time Description
    // t= time the session is active
    // r=* zero or more repeated times
    // z=* time zone adjustment
    // k=* encryption key
    // Verify
    cout << sd.encode();

    // tbd: Return the string and plug it into decode()
    cout << "------- Encode ends -------" << endl;
}    // testEncode


/** This tests out the decoding of the SDP message, copy C'tor, and
= operator for the SDP class */
void
testDecode(SDPDescriptor& sdpDesc)
{
    //form a SDP string, and call decode, , and call writeDetails to check if the values have been assigned properly.
    string sdp;
    sdp.erase();

    sdp = "v=0\n";
    sdp += "o=mhandley 28908 23456 IN IP4 126.16.64.4\n";
    sdp += "s=SDP seminar\n";
    sdp += "i=information\n";
    sdp += "c=IN IP4 224.2.17.12/127\n";
    sdp += "a=recvonly\n";

    sdp += "m=audio 49230 RTP/AVP 96 97 98\n";
    sdp += "a=rtpmap:96 L8/8000\n";
    sdp += "a=rtpmap:97 L16/8000\n";
    sdp += "a=rtpmap:98 X-myrtpmap/8000\n";
    sdp += "c=IN IP4 128.2.124.5\n";

    sdp += "m=video 51372 RTP/AVP 31\n";
    sdp += "a=orient:portrait\n";
    sdp += "a=X-myAttrib:myValue\n";
    sdp += "a=ptime:100\n";
    sdp += "b=AS:64\n";

    sdpDesc.decode(sdp);

    cout << sdpDesc;


    cout << "Testing the copy operator";
    SDPDescriptor sdpDest;
    sdpDest = sdpDesc;

    cout << sdpDest;


    //test copy constructor.
    SDPDescriptor sdpCopyCtor(sdpDest);

    cout << sdpCopyCtor;

    cout << "Test Decode done" << endl;
}


void
main()
{
    cpLogSetPriority(LOG_DEBUG);

    SDPDescriptor sd;
    testDecode (sd);
    testEncode (sd);
}
