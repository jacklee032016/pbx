/*
* $Id: SdpSessionTest.cxx,v 1.2 2007/03/02 07:29:48 lizhijie Exp $
*/

#include "Verify.hxx"
#include "SdpSession.hxx"

int main()
{
    {
        SdpSession sdpDesc;
        SdpConnection connection;
        connection.setUnicast( "127.0.0.1"  );
        sdpDesc.setConnection( connection );

        SdpMedia* media = new SdpMedia;
	// Get RTP port number from UaDevice
	int rtpPort = 3456;

        media->setPort( rtpPort );   // which port to use when making a call
        // RTP doesn't have a suggestd number

        sdpDesc.addMedia( media );

	int verStr = 844111165;
	
	int sessionStr = 844111165;
	
	sdpDesc.setVersion(verStr);
	sdpDesc.setSessionId(sessionStr);

	sdpDesc.clearSdpTimeList();
	sdpDesc.addTime(SdpTime(0,0));

	cout << sdpDesc.encode() << endl;

	test_verify(sdpDesc.encode() == "v=0\r\no=- 844111165 844111165 IN IP4 0.0.0.0\r\ns=\r\nc=IN IP4 127.0.0.1\r\nt=2208988800 2208988800\r\nm=audio 3456 RTP/AVP 0\r\n");
	



	{
	    SdpSession copy;
	    copy = sdpDesc;
	}
	SdpSession copy2(sdpDesc);
	{
	    SdpSession copy;
	    copy = sdpDesc;
	}
	{
	    string s("v=0\r\no=CiscoSystemsSIP-IPPhone-UserAgent 2030 7663 IN IP4 10.22.76.105\r\ns=SIP Call\r\nc=IN IP4 0.0.0.0\r\nt=0 0\r\nm=audio 27774 RTP/AVP 0 100\r\na=rtpmap:0 PCMU/8000\r\na=rtpmap:100 telephone-event/8000\r\na=fmtp:100 0-15\r\n");
	    SdpSession session;
	    session.decode(s);
	    
	    test_verify(session.isHold());
	}

    }

    return test_return_code(2);
}
