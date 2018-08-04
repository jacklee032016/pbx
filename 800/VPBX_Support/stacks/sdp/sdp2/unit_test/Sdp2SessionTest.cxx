/*
* $Log: Sdp2SessionTest.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: Sdp2SessionTest.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "Verify.hxx"
#include "Sdp2Session.hxx"

using Assist::SDP::SdpSession;
using Assist::SDP::SdpConnection;
using Assist::SDP::SdpMedia;
using Assist::SDP::MediaAttributes;
using Assist::SDP::SdpTime;
using Assist::SDP::SdpRtpMapAttribute;

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
	sdpDesc.setUserName("Pingtel(VxWorks)");

	sdpDesc.clearSdpTimeList();
	sdpDesc.addTime(SdpTime(0,0));

	//cout << sdpDesc.encode() << endl;

	sdpDesc.setUserName("XXX");
	//cout << sdpDesc.encode() << endl;


	test_verify(sdpDesc.encode() == "v=0\r\no=XXX 844111165 844111165 IN IP4 0.0.0.0\r\ns=\r\nc=IN IP4 127.0.0.1\r\nt=2208988800 2208988800\r\nm=audio 3456 RTP/AVP 0\r\n");

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
	    Data s("v=0\r\no=CiscoSystemsSIP-IPPhone-UserAgent 2030 7663 IN IP4 10.0.0.1\r\ns=SIP Call\r\nc=IN IP4 0.0.0.0\r\nt=0 0\r\nm=audio 27774 RTP/AVP 0 100\r\na=rtpmap:0 PCMU/8000\r\na=rtpmap:100 telephone-event/8000\r\na=fmtp:100 0-15\r\n");
	    SdpSession session;
	    session.decode(s);
	    
	    test_verify(session.isHold());
//	    cout << session.getConnection()->getUnicast();
	}
	{
	    Data s("v=0\r\no=CiscoSystemsSIP-IPPhone-UserAgent 8161 23814 IN IP4 10.0.0.1\r\ns=SIP Call\r\nc=IN IP4 10.0.0.1\r\nt=0 0\r\nm=audio 17416 RTP/AVP 0 100\r\na=rtpmap:0 PCMU/8000\r\na=rtpmap:100 telephone-event/8000\r\na=fmtp:100 0-15\r\n");

	    SdpSession session;
	    session.decode(s);

	    test_verify(session.getConnection()->getUnicast() == "10.0.0.1");
	    test_verify((*session.getMediaList().begin())->getPort() == 17416);

	    MediaAttributes *mediaAttributes 
		= (*session.getMediaList().begin())->getMediaAttributes();
	    test_verify(mediaAttributes != 0);

	    vector < SdpRtpMapAttribute* > *rtpMaps 
		= mediaAttributes->getmap();
	    vector < SdpRtpMapAttribute* > ::iterator rtpMap;
	    rtpMap = rtpMaps->begin();
	    test_verify(rtpMap != rtpMaps->end());

	    test_verify(Data((*rtpMap)->getEncodingName()) == "PCMU");
	    ++rtpMap;
	    test_verify(Data((*rtpMap)->getEncodingName()) 
			== "telephone-event");
	    ++rtpMap;
	    test_verify(rtpMap == rtpMaps->end());
	}
	{
		Data s("v=0\r\no=- 99513809 99513809 IN IP4 128.107.140.140\r\ns=VOVIDA Session\r\nc=IN IP4 128.107.140.140\r\nt=962892393 0\r\nm=audio 3456 RTP/AVP 0\r\na=rtpmap:0 PCMU/8000\r\n");

		SdpSession session;
		session.decode(s);
		test_verify(session.getConnection()->getUnicast() == "128.107.140.140");
	}
	{
		Data s("v=0\r\no=- 99513809 99513809 IN IP4 128.107.140.140\r\ns=VOVIDA Session\r\nc=IN IP4 128.107.140.140\r\nt=962892393 0\r\nm=audio 3456 RTP/AVP 0\r\na=rtpmap:0 PCMU/8000\r\na=ptime:20\r\n");

		SdpSession session;
		session.decode(s);
		test_verify(session.getConnection()->getUnicast() == "128.107.140.140");
	}

        {
               Data s("v=0\r\no=- 1025036129 1025036129 IN IP4 192.168.123.10\r\ns=SIP Call\r\nt=0 0\r\nm=audio 4136 RTP/AVP 0 100\r\nc=IN IP4 192.168.123.10\r\na=rtpmap:0 pcmu/8000\r\na=rtpmap:100 telephone-event/8000\r\na=fmtp:100 0-15");
		SdpSession session;
		session.decode(s);
                list<SdpMedia*> mediaList = session.getMediaList();
                for(list<SdpMedia*>::iterator itr = mediaList.begin(); itr != mediaList.end(); itr++)
                {
                    if((*itr)->getMediaType() == Assist::SDP::MediaTypeAudio)
                    {
		        test_verify((*itr)->getConnection()->getUnicast() == "192.168.123.10");
                    }
                }
        }


    }

    return test_return_code(12);
}
