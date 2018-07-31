
#include "SdpHandler.hxx"
#include "PbxConfiguration.hxx"
#include "Sdp2Session.hxx"
#include "Sdp2Connection.hxx"
#include "Sdp2Media.hxx"

using namespace Assist;

using Assist::SDP::SdpSession;
using Assist::SDP::SdpConnection;
using Assist::SDP::SdpMedia;
using Assist::SDP::MediaAttributes;
using Assist::SDP::ValueAttribute;
using Assist::SDP::SdpRtpMapAttribute;


void addSdpToMsg( SipMsg& msg, Data rtpRate, int rtpPort )
{
	// add an appropriate SDP object to this message

	Sptr <SipSdp> sipSdp;
	sipSdp.dynamicCast ( msg.getContentData( 0 ) );

	if ( sipSdp != 0 )
	{
		SdpSession sdpDesc = sipSdp->getSdpDescriptor();
		list <SdpMedia*> mediaList;
		mediaList = sdpDesc.getMediaList();
		list <SdpMedia*> ::iterator mediaIterator = mediaList.begin();

		MediaAttributes* mediaAttrib;
		mediaAttrib = (*mediaIterator)->getMediaAttributes();
		(*mediaIterator)->getStringFormatList()->clear();

		if ( mediaAttrib == 0 )
		{
			mediaAttrib = new MediaAttributes();
			assert( mediaAttrib );
			(*mediaIterator)->setMediaAttributes( mediaAttrib );
		}

		ValueAttribute* attrib = new ValueAttribute();
		attrib->setAttribute( "ptime" );
		LocalScopeAllocator lo;
		attrib->setValue( rtpRate.getData(lo) );

#if 0
          /* added by lizhijie 2005.02.28  */
        SdpRtpMapAttribute* rtpAlawMapAttrib = new SdpRtpMapAttribute();
        rtpAlawMapAttrib->setPayloadType( 8 );
        rtpAlawMapAttrib->setEncodingName( "PCMA" );
        rtpAlawMapAttrib->setClockRate( 8000 );
        (*mediaIterator)->addFormat( rtpAlawMapAttrib->getPayloadType() );
	/*end of added */
#endif

		//add the rtpmap attribute for the default codec
		SdpRtpMapAttribute* rtpMapAttrib = new SdpRtpMapAttribute();
		rtpMapAttrib->setPayloadType( 0 );
		rtpMapAttrib->setEncodingName( "PCMU" );
		rtpMapAttrib->setClockRate( 8000 );
		(*mediaIterator)->addFormat( rtpMapAttrib->getPayloadType() );

		//add the rtpmap attribute for the dtmf in rtp
		SdpRtpMapAttribute* rtpMapEventAttrib = new SdpRtpMapAttribute();
		rtpMapEventAttrib->setPayloadType( 100 );
		rtpMapEventAttrib->setEncodingName( "telephone-event" );
		rtpMapEventAttrib->setClockRate( 8000 );

		// add the value attribute for telephone-event
		ValueAttribute* eventAttrib = new ValueAttribute();
		eventAttrib->setAttribute( "fmtp" );
		eventAttrib->setValue( "100 0-11" );

		// add the value attribute just created to the media attribute object
		mediaAttrib->addValueAttribute( attrib );
		
#if 0		
/*added by lizhijie, 2005.02.28 */
        mediaAttrib->addmap( rtpAlawMapAttrib );
/* end of added */
#endif

		mediaAttrib->addmap( rtpMapAttrib );
		(*mediaIterator)->addFormat( rtpMapEventAttrib->getPayloadType() );
		mediaAttrib->addmap( rtpMapEventAttrib );
		mediaAttrib->addValueAttribute( eventAttrib );
		sipSdp->setSdpDescriptor( sdpDesc );
		sipSdp->setRtpPort( rtpPort );


		// if the NAT address is set, do something with it
		if(PbxConfiguration::instance()->getNATAddress() != "")
		{
			Data host = PbxConfiguration::instance()->getNATAddress();
			cpDebug(LOG_DEBUG, "Replace connection address with NAT firewall WAN address %s", host.logData() );
			setHost(*sipSdp, host);
		}
	}
	else
	{
		// could not add the SDP for some reason
		cpLog(LOG_ERR, "could not add SDP to this object");
	}
}


int getRtpPacketSize ( SipSdp& remoteSdp )
{
	Data rtpPacketSize;

	SdpSession sdpDesc = remoteSdp.getSdpDescriptor();
	// declare the media list pointer
	list <SdpMedia*> mediaList;

	// get the media list pointer from the session object
	mediaList = sdpDesc.getMediaList();
	if( mediaList.size() == 0 )
	{
		cpDebug(LOG_DEBUG, "No remote media in 180/183 SDP" );
		return -1;
	}
	else  // mediaList != 0
	{
		// declare the media list iterator and assign it to the
		// first media object on the list
		list < SdpMedia* > ::iterator mediaIterator = mediaList.begin();
		if( mediaIterator == mediaList.end() )
		{
			cpLog( LOG_ERR, "No remote media in 180/183 SDP" );
			return -1;
		}
		else  // We have remote media in 180/183 SDP
		{
			vector <Data>* formatList 	= (*mediaIterator)->getStringFormatList();

			if ( formatList != 0 && (*formatList)[0] == "0" )
			{
				cpDebug(LOG_DEBUG, "Getting media attribute format");
				// declare the media attribute list pointer
				MediaAttributes* mediaAttrib;
				// get the media attribute list pointer from the media
				// object pointed by the iterator
				mediaAttrib = (*mediaIterator)->getMediaAttributes();
				if ( mediaAttrib != 0 )
				{
					vector <ValueAttribute*> *valueAttribList = mediaAttrib->getValueAttributes();
					vector < ValueAttribute* > ::iterator attribIterator = valueAttribList->begin();
					while ( attribIterator != valueAttribList->end() )
					{
						char* attribName = (*attribIterator)->getAttribute();
						if ( strcmp(attribName, "ptime") == 0 )
						{
							rtpPacketSize = (*attribIterator)->getValue();
							return rtpPacketSize.convertInt();
							break;
						}
						attribIterator++;
					}
				}
			}
			else
			{
				return -1;
			}
		}
	}  // end   'else  // mediaList != 0'

	return -1;
}


void
setRtpPacketSize(SipSdp& sipSdp, int packetSize)
{
    char buf[10];
    sprintf(buf, "%d", packetSize);

    Data rtpPacketSize = buf;

    // add rtpPacketSize to sipSdp
    SdpSession sdpDesc = sipSdp.getSdpDescriptor();
    // get the media list pointer from the session object
    list < SdpMedia* > mediaList;

    mediaList = sdpDesc.getMediaList();
    // declare the media list iterator and assign it to the
    // first media object on the list
    list < SdpMedia* > ::iterator mediaIterator;
    mediaIterator = mediaList.begin();
    // get the media attribute list pointer from the media
    // object pointed by the iterator
    MediaAttributes* mediaAttrib;
    mediaAttrib = (*mediaIterator)->getMediaAttributes();
    if ( mediaAttrib != 0 )
    {
	vector < ValueAttribute* > * valueAttribList =
	    mediaAttrib->getValueAttributes();
	vector < ValueAttribute* > ::iterator attribIterator
	    = valueAttribList->begin();
        LocalScopeAllocator lo;
	while ( attribIterator != valueAttribList->end() )
	{
	    char* attribName = (*attribIterator)->getAttribute();
	    if ( strcmp(attribName, "ptime") == 0 )
	    {
		(*attribIterator)->setValue(rtpPacketSize.getData(lo));
		return;
		break;
	    }
	    attribIterator++;
	}
    }
    else
    {
	return;
    }
}


void
setHost(SipSdp& sipSdp, Data hostAddr)
{
    // set IP address and port 
    SdpConnection connection;
    LocalScopeAllocator lo;
    connection.setUnicast(hostAddr.getData(lo));
    if(hostAddr.find(":") != Data::npos)
    {
        connection.setAddressType(SDP::AddressTypeIPV6);
        sipSdp.setAddressType(SDP::AddressTypeIPV6);
    }
    sipSdp.setConnAddress(connection);
    sipSdp.setAddress( hostAddr );
}

void setStandardSdp(SipSdp& sipSdp, Data hostAddr, int port)
{
	setupMedia(sipSdp, hostAddr, port);

	// do the media stuff
	SdpSession sdpDesc = sipSdp.getSdpDescriptor();
	// declare the media list pointer
	list < SdpMedia* > mediaList;
	// get the media list pointer from the session object
	mediaList = sdpDesc.getMediaList();
	// declare the media list iterator and assign it to the first media object on the list
	list < SdpMedia* > ::iterator mediaIterator = mediaList.begin();

	vector <Data> * formatList = (*mediaIterator)->getStringFormatList();
	if ( formatList != 0 )
	{
		formatList->clear();
	}
	//using the default codec
	(*mediaIterator)->addFormat( 0 );
	setUlaw(sipSdp, 8000);

	setRtpPacketSize(sipSdp, PbxConfiguration::instance()->getNetworkRtpRate());
}


void
setUlaw(SdpSession& sdpDesc, int rate)
{
    // declare the media list pointer
    list < SdpMedia* > mediaList;
    // get the media list pointer from the session object
    mediaList = sdpDesc.getMediaList();
    // declare the media list iterator and assign it to the first media
    //object on the list

    assert(mediaList.size() != 0);

    list < SdpMedia* > ::iterator mediaIterator = mediaList.begin();

    assert(mediaIterator != mediaList.end());

    vector < Data > * formatList = (*mediaIterator)->getStringFormatList();
    if ( formatList != 0 )
    {
        formatList->clear();
    }

    //using the default codec
    (*mediaIterator)->addFormat( 0 );

    SdpMedia* media = 0;

    media = *mediaIterator;

    // declare the media attribute list pointer
    MediaAttributes* mediaAttrib;
    // get the media attribute list pointer from the media object pointed
    //by the iterator
    mediaAttrib = (*mediaIterator)->getMediaAttributes();

    if ( mediaAttrib == 0 )
    {
        mediaAttrib = new MediaAttributes();
        media->setMediaAttributes(mediaAttrib);
    }

    // get, then set, the media attribute
    mediaAttrib->flushValueAttributes();
    mediaAttrib->flushrtpmap();

    // create the new value attribute object
    ValueAttribute* attrib = new ValueAttribute();
    // set the attribute and its value
    attrib->setAttribute("ptime");
    LocalScopeAllocator lo;
    attrib->setValue( Data( PbxConfiguration::instance()->getNetworkRtpRate() ).getData(lo) );
    
    //add the rtpmap attribute for the default codec
    SdpRtpMapAttribute* rtpMapAttrib = new SdpRtpMapAttribute();
    rtpMapAttrib->setPayloadType(0);
    rtpMapAttrib->setEncodingName("PCMU");
    rtpMapAttrib->setClockRate(rate);
    
    // add the value attribute just created to the media attribute object
    mediaAttrib->addValueAttribute(attrib);
    mediaAttrib->addmap(rtpMapAttrib);
}

void setUlaw(SipSdp& sipSdp, int rate)
{
	// do the media stuff
	SdpSession sdpDesc = sipSdp.getSdpDescriptor();
	setUlaw(sdpDesc, rate);
	sipSdp.setSdpDescriptor(sdpDesc);
}

bool validUlaw(SipSdp& sdp)
{
	assert(0);
	return false;
}

void setupMedia(SipSdp& sipSdp, Data hostAddr, int port)
{
	setHost(sipSdp, hostAddr);

	// guarantees a valid SDP, at least vaguely.

	// do the media stuff
	SdpSession sdpDesc = sipSdp.getSdpDescriptor();
	// declare the media list pointer
    list < SdpMedia* > mediaList;
    // get the media list pointer from the session object
    mediaList = sdpDesc.getMediaList();

    list < SdpMedia* > ::iterator mediaIterator = mediaList.begin();

    SdpMedia* media = 0;

    if(mediaIterator != mediaList.end())
    {
        // xxx this should probably be more careful here
        // use the first one by default
        media = *mediaIterator;
    }
    else
    {
        media = new SdpMedia;
        media->setPort( port );
        sdpDesc.addMedia( media );
    }

    MediaAttributes* mediaAttrib;

    mediaAttrib = media->getMediaAttributes();
    if(!mediaAttrib)
    {
        mediaAttrib = new MediaAttributes();
        media->setMediaAttributes(mediaAttrib);
    }

    sipSdp.setSdpDescriptor( sdpDesc );
}


void
setAvt(SdpSession& sdpDesc, int payloadType)
{
   // declare the media list pointer
    list < SdpMedia* > mediaList;
    // get the media list pointer from the session object
    mediaList = sdpDesc.getMediaList();

    list < SdpMedia* > ::iterator mediaIterator = mediaList.begin();

    assert(mediaIterator != mediaList.end());

    SdpMedia* media = *mediaIterator;

    MediaAttributes* mediaAttrib;

    mediaAttrib = media->getMediaAttributes();

    assert(mediaAttrib != 0);

    media->addFormat(payloadType);
    SdpRtpMapAttribute* rtpMapAttribDtmf = new SdpRtpMapAttribute();
    rtpMapAttribDtmf->setPayloadType(payloadType);
    rtpMapAttribDtmf->setEncodingName("telephone-event");
    rtpMapAttribDtmf->setClockRate(8000);
    
    // add the value attribute for telephone-event
    ValueAttribute* eventAttrib = new ValueAttribute();
    eventAttrib->setAttribute( "fmtp" );

    char buf[256];

    sprintf(buf, "%d 0-15", payloadType);

    eventAttrib->setValue( buf );
    
    mediaAttrib->addmap(rtpMapAttribDtmf);
    mediaAttrib->addValueAttribute( eventAttrib );
}

#if 0
void setAvt(SipSdp& sipSdp, int payloadType)
{
    setupMedia(sipSdp,
               theSystem.gethostAddress(),
               //"192.168.3.172",
               UaDevice::instance()->getRtpPort(AS_DEVICE_NOT_DEFINED));  /* how to change this method here, lizhijie 2005.02.13 */

    // do the media stuff
    SdpSession sdpDesc = sipSdp.getSdpDescriptor();
    setAvt(sdpDesc, payloadType);
    sipSdp.setSdpDescriptor(sdpDesc);
}
#endif

bool checkType(SipSdp& sdp, int& realPayloadType, Data eventName)
{
    list < SdpMedia* > remoteMediaList 
        = sdp.getSdpDescriptor().getMediaList();

    if (remoteMediaList.size() < 1)
    {
        cpLog(LOG_ERR, "did not find any media lists");
        return false;
    }

    if (remoteMediaList.size() > 1)
    {
        cpLog(LOG_WARNING, "Found %d media descriptors. Only using first one!",
              remoteMediaList.size());
    }

    MediaAttributes *mediaAttributes 
        = (*remoteMediaList.begin())->getMediaAttributes();
    if(mediaAttributes)
    {
        cpDebug(LOG_DEBUG, "Has Media Attribute(s)" );
        vector < SdpRtpMapAttribute* > *rtpMaps = mediaAttributes->getmap();
        if(rtpMaps->size() == 0)
        {
            cpLog(LOG_ERR, "Empty RTP MAP attribute");
            return false;
        }
        vector < SdpRtpMapAttribute* > ::iterator rtpMap;
        //As suggested by Steve Frare, though this is just a hack
        //I am putting it till we investigate more and see why it
        //should throw an exception  in some cases on Windows
      try {
        rtpMap = rtpMaps->begin();
        while (rtpMap != rtpMaps->end())
        {
            Data encodingName = (*rtpMap)->getEncodingName();
            
            if (isEqualNoCase(encodingName, eventName))
            {
                realPayloadType = (*rtpMap)->getPayloadType();

                cpDebug(LOG_DEBUG, 
                      "Has payload type %s, value %d",
                      encodingName.logData(),
                      realPayloadType);
                return true;
            }
            else
            {
                LocalScopeAllocator lo1;
                LocalScopeAllocator lo2;
                cpDebug(LOG_DEBUG, "payload type %s != %s",
                      eventName.getData(lo1),
                      encodingName.getData(lo2));
            }
            ++rtpMap;
        }
    }
    catch(...)
    {
        cpLog( LOG_ERR, "SdpHandler::checkType exception.");
    }
    }
    else
    {
        cpDebug(LOG_DEBUG, "No Media Attribute" );
        return false;
    }
    return false;
}


void
doAnswerStuff(SipSdp& local, Sptr<SipSdp> remote, Data hostAddr, int port)
{
    SdpSession sdpDesc;

    SdpMedia* media = new SdpMedia;

    media->setPort( port );   // which port to use when making a call
    // RTP doesn't have a suggested number

    sdpDesc.addMedia( media );

    //To further perform any other operation on that particular media
    //item, use loc.

    MediaAttributes* mediaAttrib = new MediaAttributes();
    assert(mediaAttrib);
    media->setMediaAttributes(mediaAttrib);

    // create the new value attribute object
    ValueAttribute* attrib = new ValueAttribute();
    // set the attribute and its value
    attrib->setAttribute("ptime");
    LocalScopeAllocator lo;
    attrib->setValue( Data( PbxConfiguration::instance()->getNetworkRtpRate() ).getData(lo) );
    mediaAttrib->addValueAttribute(attrib);

    setUlaw(sdpDesc, 8000);

    if(remote != 0)
    {
        // Add RTP formats 'PCMU' and 'telephone-event' to local session
        // description, if supported by remote session description.

        int payloadType;
    
        if(checkType(*remote, payloadType, "telephone-event"))
        {
            cpDebug(LOG_DEBUG, 
                  "Has DTMF NTE (RFC2833) in payload type: %d.", 
                  payloadType);
            if(payloadType == 100)
            {
                // there's ULAW in the remote
                setAvt(sdpDesc, payloadType);
            }
            else
            {
                // xxx fix this when dynamic payload types work.
                cpDebug(LOG_DEBUG, "%d != 100, so will not respond", payloadType);
            }
        }
    }
    // always accept AVT tones.
    setAvt(sdpDesc, 100);

    local.setSdpDescriptor( sdpDesc );

    local.setUserName( theSystem.getUserName() );
    local.setSessionName( theSystem.getSessionName() );

    setHost(local, hostAddr);
}

#if 0
void setAudioRequest( struct Assist::HardwareAudioRequest* request,
                      SipSdp& remoteSdp,
                      SipSdp& localSdp)
{
	request->type = AudioStart;

	// Set remote host and port
	LocalScopeAllocator lo;
	strcpy( request->remoteHost, remoteSdp.getConnAddress().getData(lo) );
	request->remotePort = remoteSdp.getRtpPort();

	// set the local host and port
	request->localPort = localSdp.getRtpPort();
	strcpy( request->localHost, localSdp.getConnAddress().getData(lo) );

	// set the audio rate (should I use the remote rate?)
	request->rtpPacketSize = PbxConfiguration::instance()->getNetworkRtpRate();

	//TODO Why echo cancellation always true?
	request->echoCancellation = true;

	//TODO: this will be removed once 183 is integrated.

	//modified by lijie 2005-06-03
	//request->sendRingback = false;
	request->actionType = ActionNull;

	cpDebug(LOG_DEBUG, "Start audio( duplex ):" );
	cpDebug(LOG_DEBUG, "localHost     = %s", request->localHost);
	cpDebug(LOG_DEBUG, "localPort     = %d", request->localPort);
	cpDebug(LOG_DEBUG, "remoteHost    = %s", request->remoteHost);
	cpDebug(LOG_DEBUG, "remotePort    = %d", request->remotePort);
	cpDebug(LOG_DEBUG, "rtpPacketSize = %d", request->rtpPacketSize);
}

void sendRemoteRingback(SipSdp& remoteSdp, int deviceId)
{
	LocalScopeAllocator lo;
	Sptr <UaHardwareEvent> ringbackSignal = new UaHardwareEvent( UaDevice::getDeviceQueue() );

	ringbackSignal->type = HardwareAudioType;
	ringbackSignal->signalOrRequest.request.type = AudioStart;
	ringbackSignal->signalOrRequest.request.localPort = 0;
	
	ringbackSignal->signalOrRequest.request.remotePort = remoteSdp.getRtpPort();
	strcpy(ringbackSignal->signalOrRequest.request.remoteHost,    remoteSdp.getConnAddress().getData(lo));

	ringbackSignal->signalOrRequest.request.echoCancellation = true;
	//modified by ljiie 2005-06-03
	//ringbackSignal->signalOrRequest.request.sendRingback = true;
	ringbackSignal->signalOrRequest.request.actionType = StartRingBack;
	ringbackSignal->signalOrRequest.request.rtpPacketSize = getRtpPacketSize(remoteSdp);
	if(ringbackSignal->signalOrRequest.request.rtpPacketSize <= 0)
	{
		ringbackSignal->signalOrRequest.request.rtpPacketSize = PbxConfiguration::instance()->getNetworkRtpRate();
	}

	//added by lijie 2005-05-08
	ringbackSignal->setDeviceId(deviceId);	
	UaDevice::getDeviceQueue()->add( ringbackSignal );
}

//added by lijie on 2005-06-03
void stopAudio( int deviceId )
{
	Sptr <UaHardwareEvent> signal = new UaHardwareEvent( UaDevice::getDeviceQueue() );
	signal->type = HardwareAudioType;

	struct HardwareAudioRequest* request = &(signal->signalOrRequest.request);
	request->type = AudioStop;

	signal->setDeviceId(deviceId);
	UaDevice::getDeviceQueue()->add( signal );
}
#endif

