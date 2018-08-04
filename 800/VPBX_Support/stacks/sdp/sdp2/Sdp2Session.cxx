/*
* $Log: Sdp2Session.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: Sdp2Session.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "global.h"
#include "debug.h"

#include <cstdio>
#include <time.h>
#include <strstream>
#include <deque>

#include "Sdp2Exceptions.hxx"
#include "Sdp2Session.hxx"
#include "Sdp2Externs.hxx"


//using Assist::SDP;
using Assist::SDP::SdpSession;
using Assist::SDP::SdpConnection;
using Assist::SDP::SdpProtocolType;
using Assist::SDP::NetworkType;
using Assist::SDP::SdpBandwidth;
using Assist::SDP::SdpTime;
// Other files can include "Sdp2Externs.hxx" to reference the following
// const char []s

const char Assist::SDP::SdpNetworkTypeIN[] = "IN";

const char Assist::SDP::SdpAddressTypeIP4[] = "IP4";
const char Assist::SDP::SdpAddressTypeIP6[] = "IP6";

const char Assist::SDP::SdpMediaTransportRTPAVP[] = "RTP/AVP";
const char Assist::SDP::SdpMediaTransportUDP[] = "UDP";
const char Assist::SDP::SdpMediaTransportMPA[] = "MPA/16000";

const char Assist::SDP::SdpBandwidthModifierAS[] = "AS";  // Application specific maximum
const char Assist::SDP::SdpBandwidthModifierCT[] = "CT";  // Conference Total
const char Assist::SDP::SdpBandwidthModifierRR[] = "RR";  

const char Assist::SDP::SdpMediaTypeAudio[] = "audio";
const char Assist::SDP::SdpMediaTypeVideo[] = "video";

const char Assist::SDP::SdpAttributeRecvonly[] = "recvonly";
const char Assist::SDP::SdpAttributeSendrecv[] = "sendrecv";
const char Assist::SDP::SdpAttributeSendonly[] = "sendonly";

const char Assist::SDP::SdpEncryptkeyMethodClear[] = "clear";
const char Assist::SDP::SdpEncryptkeyMethodBase64[] = "base64";
const char Assist::SDP::SdpEncryptkeyMethodURI[] = "uri";
const char Assist::SDP::SdpEncryptkeyMethodPrompt[] = "prompt";

// When adding a new const char [], add the corresponding extern declaration
// in "Sdp2Externs.hxx"


void split(deque<Data>& slist, Data str, const char* item)
{
    bool finished = false;

    while (!finished)
    {
	Data x = str.parse(item, &finished);
	if(finished)
	{
	    x = str;
	}
	slist.push_back(x);
    }
}


///
SdpSession::SdpSession()
        : emailList(), phoneList(), sdpTimeList()
{
    protocolType = SdpProtocolTypeSDP;

    protocolVersion = 0;

    username = "-";
    // Use NTP time
    sessionId = static_cast < unsigned int > (time(NULL) + SdpTime::NTP_UX_DIFF);
    // Use NTP time
    version = static_cast < unsigned int > (time(NULL) + SdpTime::NTP_UX_DIFF);

    networkType = NetworkTypeInternet;

    addressType = AddressTypeIPV4;

    address = "0.0.0.0";

    sessionName = "-";

    sessionName = "";

    uriInfo = "";

    emailList.clear();

    phoneList.clear();

    connection = 0;

    attribute = 0;

    bandwidth = 0;
    
    encryptkey = 0;

    SdpTime time;
    sdpTimeList.push_back (time);

    zoneAdjustmentList.clear();

    flushMediaList();

    isValid = false;
}    // SdpSession::SdpSession

///
SdpSession::SdpSession (const SdpSession& x)
{
    connection = 0;
    attribute = 0;
    bandwidth = 0;
    encryptkey = 0;
    flushMediaList();
    *this = x;
}    // SdpSession::SdpSession


///
SdpSession::~SdpSession()
{
    if (connection)
    {
        delete connection;
    }
    if (attribute)
    {
        delete attribute;
    }
    if (bandwidth)
    {
        delete bandwidth;
    }
    if (encryptkey)
    {
        delete encryptkey;
    }
    flushMediaList();
}    // SdpSession::~SdpSession


SdpSession&
SdpSession::operator= (const SdpSession& x)
{
    protocolVersion = x.protocolVersion;
    username = x.username;
    sessionId = x.sessionId;
    version = x.version;
    networkType = x.networkType;
    addressType = x.addressType;
    address = x.address;

    sessionName = x.sessionName;

    sessionInfo = x.sessionInfo;

    uriInfo = x.uriInfo;

    emailList = x.emailList;

    phoneList = x.phoneList;

    if (x.connection)
    {
        if (!connection)
        {
            connection = new SdpConnection;
            assert(connection);
        }
        *(connection) = *(x.connection);
        cpDebug(LOG_DEBUG_STACK, "Copied connection");
    }
    else
    {
        if (connection)
        {
            delete connection;
            connection = 0;
        }
    }

    if (x.bandwidth)
    {
        if (!bandwidth)
        {
            bandwidth = new SdpBandwidth;
            assert(bandwidth);
        }

        *(bandwidth) = *(x.bandwidth);
        cpDebug(LOG_DEBUG_STACK, "copied bandwidth");
    }
    else
    {
        if (bandwidth)
        {
            delete bandwidth;
            bandwidth = 0;
        }
    }

    sdpTimeList.clear();
    sdpTimeList = x.getSdpTimeList();

    zoneAdjustmentList = x.zoneAdjustmentList;;

    if (x.encryptkey)
    {
        if (!encryptkey)
        {
            encryptkey = new SdpEncryptkey;
            assert(encryptkey);
        }

        *(encryptkey) = *(x.encryptkey);
        cpDebug(LOG_DEBUG_STACK, "copied encryptkey");
    }
    else
    {
        if (encryptkey)
        {
            delete encryptkey;
            encryptkey = 0;
        }
    }

    if (x.attribute)
    {
        if (!attribute)
        {
            attribute = new SdpAttributes;
            assert(attribute);
        }

        *(attribute) = *(x.attribute);
        cpDebug(LOG_DEBUG_STACK, "copied attribute");
    }
    else
    {
        if (attribute)
        {
            delete attribute;
            attribute = 0;
        }
    }

    //copy contents of objects pointed by the mediaList.
    // we need to flush the previous values
    list < SdpMedia* > newMediaList = x.getMediaList();

    flushMediaList();

    if (newMediaList.size() > 0)
    {
        list < SdpMedia* > ::iterator mediaiter;
        mediaiter = newMediaList.begin();

        while (mediaiter != newMediaList.end())
        {
            cpDebug(LOG_DEBUG_STACK, "Obtained one mediaiter");
            if (*mediaiter)
            {
                // create new of media and assign values.
                SdpMedia* media = new SdpMedia;
                assert(media);

                *media = *(*mediaiter);

                //store in the vector.
                addMedia(media);

            }
            ++mediaiter;
        }
    }
    buf[0] = '\0';
    isValid = x.isValid;
    return (*this);
}    // SdpSession::operator=


#if 0
bool
SdpSession::operator== (const SdpSession& x) const
{
    /// xxx not implemented
    assert(0);
    return false;
}
#endif


void
SdpSession::clearSdpTimeList ()
{
    sdpTimeList.clear();
    isValid = false;
}    // SdpSession::addTime


void
SdpSession::addTime (const SdpTime& time)
{
    sdpTimeList.push_back (time);
    isValid = true;
}    // SdpSession::addTime


///
void
SdpSession::setBandwidth (SdpBandwidth& bw)
{
    if (!bandwidth)
    {
        bandwidth = new SdpBandwidth;
    }
    *bandwidth = bw;
}    // SdpSession::setBandwidth


///
void
SdpSession::setNcs ()
{
    setUserName (Data("-"));
    // User needs to call setAddress()

    setSessionName (Data("-"));

    // No "i="
    // No "u="
    // No "e="
    // No "p="

    SdpConnection conn;
    // User needs to call setUnicast()
    setConnection (conn);

    SdpBandwidth bw;
    setBandwidth (bw);

    sdpTimeList.clear();
    SdpTime time;
    addTime (time);

    // No "k="

    // User add attribute("a=") lines

    SdpMedia* media = new SdpMedia;
    // User needs to call SdpMedia::setPort()
    addMedia (media);
}    // SdpSession::setNcs


///
void
SdpSession::setProtocolType (SdpProtocolType protocol)
{
    protocolType = protocol;
    // Setup this SDP according to the corresponding protocol
    if (protocol == SdpProtocolTypeNCS)
    {
        setNcs ();
    }
}    // SdpSession::setProtocolType


///
SdpProtocolType
SdpSession::getProtocolType ()
{
    return protocolType;
}    // SdpSession::getProtocolType


///
void
SdpSession::encodeVersion (ostrstream& s)
{
    // v=0 by default - no other value as of now
    s << "v=" << protocolVersion << "\r\n";
}    // SdpSession::encode


///
Data
SdpSession::networkTypeString()
{
    Data s;

    switch (networkType)
    {
        case NetworkTypeInternet:
        {
            s = SdpNetworkTypeIN;
            break;
        }
        default:
        {
	    // Network type corrupted
	    assert(0);
            break;
        }
    }
    return s;
}    // SdpSession::networkTypeString


///
Data
SdpSession::addressTypeString()
{
    Data s;

    switch (addressType)
    {
        case AddressTypeIPV4:
        {
            s = SdpAddressTypeIP4;
            break;
        }
        case AddressTypeIPV6:
        {
            s = SdpAddressTypeIP6;
            break;
        }
        default:
        {
	    // Address type corrupted
	    assert(0);
            break;
        }
    }
    return s;
}    // SdpSession::addressTypeString


///
void
SdpSession::encodeOrigin (ostrstream& s)
{
    s << "o=" << username
    << ' '
    << sessionId
    << ' '
    << version
    << ' '
    << networkTypeString()
    << ' '
    << addressTypeString()
    << ' '
    << address
    << "\r\n";
}    // SdpSession::encodeOrigin


///
void
SdpSession::encodeSessionName (ostrstream& s)
{
    s << "s=" << sessionName << "\r\n";
}    // SdpSession::encodeSessionName


///
void
SdpSession::encodeSessionInformation (ostrstream& s)
{    
    if (sessionInfo.length() > 0)
    {
        s << "i=" << sessionInfo << "\r\n";
    }
}    // SdpSession::encodeSessionInformation


///

void
SdpSession::encodeURI (ostrstream& s)
{
    if (uriInfo.length() > 0)
    {
        s << "u=" << uriInfo << "\r\n";
    }
}    // SdpSession::encodeURI


///

void
SdpSession::encodeEmailAddress (ostrstream& s)
{
    list<Data>::iterator iter = emailList.begin();

    while (iter != emailList.end())
    {
        s << "e=" << *iter << "\r\n";
        ++iter;
    }
}    // SdpSession::encodeEmailAddress


///

void
SdpSession::encodePhoneNumber (ostrstream& s)
{
    list <Data>::iterator iter = phoneList.begin();

    while (iter != phoneList.end())
    {
                s << "p=" << *iter << "\r\n";
        ++iter;
    }
}    // SdpSession::encodePhoneNumber


///

void
SdpSession::encodeTime (ostrstream& s)
{
    list < SdpTime > ::iterator iter = sdpTimeList.begin();

    while (iter != sdpTimeList.end())
    {
        iter->encode (s);
        ++iter;
    }
}    // SdpSession::encodeTime


///
void
SdpSession::encodeTimeZoneAdjustment (ostrstream& s)
{
    if (zoneAdjustmentList.size() > 0)
    {
        list <SdpZoneAdjustment>::iterator iter;
        iter = zoneAdjustmentList.begin();

        s << "z=" << (*iter).getAdjustmentTime() << ' ' << (*iter).getOffset();
        ++iter;

        while (iter != zoneAdjustmentList.end())
        {
            s << ' ' << (*iter).getAdjustmentTime() << ' ' << (*iter).getOffset();
            ++iter;
        }
    
        s << "\r\n";
    }
}    // SdpSession::encodeTimeZoneAdjustment


///

void
SdpSession::encodeMedia (ostrstream& s)
{
    list < SdpMedia* > ::iterator mediaIterator = mediaList.begin();
    while (mediaIterator != mediaList.end())
    {
        (*mediaIterator)->encode (s);
        ++mediaIterator;
    }
}    // SdpSession::encodeMedia


///
void
SdpSession::setConnection (const SdpConnection& conn)
{
    if (!connection)
    {
        connection = new SdpConnection;
    }
    *connection = conn;
} // SdpSession::setConnection


///
SdpConnection*
SdpSession::getConnection ()
{
    return connection;
} // SdpSession::getConnection


///
static const int SdpTextPayloadLength = 2024;


///
Data
SdpSession::encode()
{

    buf[0] = '\0';
    ostrstream s (buf, SdpTextPayloadLength);

    if (!isValid)
    {
        //TODO throw SdpExceptionInvalid ("Invalid SDP" , __FILE__, __LINE__ );
        // or
        // throw SDPExceptions (InvalidContent);
    }

    else
    {
        // Encoding order is important

        // v=0 by default - no other value as of now
        encodeVersion (s);

        // o= owner/creator and session identifier
        encodeOrigin (s);

        // s= session name
        encodeSessionName (s);

        // i=* session information
        encodeSessionInformation (s);

        // u=* URI of description
        encodeURI (s);

        // e=* email address
        encodeEmailAddress (s);

        // p=* phone number
        encodePhoneNumber (s);

        // c=* connection information (not required if included in all media)
        if (connection)
        {
            connection->encode (s);
        }

        // b=* bandwidth information
        if (bandwidth)
        {
            bandwidth->encode (s);
        }

        // Time Description
        encodeTime (s);

        // z=* time zone adjustment
        encodeTimeZoneAdjustment (s);

        // k=* encryption key
	if ( encryptkey )
	{
	    encryptkey->encode(s);
	}

        // a=* zero or more session attribute lines
        if (attribute)
        {
            attribute->encode (s);
        }

        // Media Description
        encodeMedia (s);
    }
    s << ends;
    return s.str();
}    // SdpSession::encode


bool SdpSession::decode(Data buffer)
{
    list <Data> lines;

    while(buffer.length() > 0)
    {
        bool mFail=true;
        //If CRLF is found mFail will be set to false
	Data tmp = buffer.getLine(&mFail);
	if(tmp.length())
	    lines.push_back(tmp);
        //See if no more lines
        if(mFail)
        {
             //No CRLF found, take the remaining buffer as the 
             //line
             if(buffer.length()) lines.push_back(buffer);
             break;
        }
    }

    bool result = decode(lines);

    return result;
}


bool SdpSession::decode(list <Data>& lines)
{
    cpDebug(LOG_DEBUG, "number of lines: %d" , lines.size());

    if (lines.size() < 3)
    {
        cpLog( LOG_ERR, "Sdp decode fail, too few lines" );
        isValid = false;
        return false;
    }

    sdpTimeList.clear();

    list<Data>::iterator linecount;

    linecount = lines.begin();

    {
        // lines[0] is the version line -- ignore

        Data s = (*linecount);

        if (!((*linecount) == "v=0\r" || (*linecount) == "v=0"))
            if (!(s == "v=0"))
            {
                // no good
                cpLog(LOG_ERR,  "SDP: did not find version in:%s", (*linecount).logData());
                return false;
            }

        ++linecount;

        while (linecount != lines.end())
        {
            // this switch parses lines as they come

            if ((*linecount)[0] == 'o')
            {
                cpDebug(LOG_DEBUG, "got o= line" ) ;
                // this is the options line -- do we care about it?

                // parse line 1 for the proper stuff



                s = (*linecount);
		s.parse("=");

                deque<Data> optionsList;
		split(optionsList, s, " ");

                if (optionsList.size() < 6)
                {
                    // not enough parameters
                    cpLog( LOG_ERR, "Parameter count < 6: %s", (*linecount).logData() );
                    if ( linecount != lines.end() )
                    {
                        ++linecount;
                    }
                    continue;
                }

                username = optionsList[0];

		LocalScopeAllocator localSession;
		LocalScopeAllocator localVersion;


                sessionId = atoi(optionsList[1].getData(localSession));
                version = atoi(optionsList[2].getData(localVersion));
                if (optionsList[3] == SdpNetworkTypeIN)
                {
                    networkType = NetworkTypeInternet;
                }
                else
                {
                    cpLog( LOG_ERR, "Unknown network type %s", optionsList[3].logData() );
                    if ( linecount != lines.end() )
                    {
                        ++linecount;
                    }
                    continue;
                }

                if (optionsList[4] == SdpAddressTypeIP4)
                {
                    addressType = AddressTypeIPV4;
                }
                else if (optionsList[4] == SdpAddressTypeIP6)
                {
                    addressType = AddressTypeIPV6;
                }
                else
                {
                    cpLog( LOG_ERR, "Unknown address type %s", optionsList[4].logData() );
                    if ( linecount != lines.end() )
                    {
                        ++linecount;
                    }
                    continue;
                }
		LocalScopeAllocator lAddress;
                address = optionsList[5].getData(lAddress);
            }
            else if ((*linecount)[0] == 's')
            {
                cpDebug(LOG_DEBUG, "got s= line" ) ;

                s = (*linecount);
		s.parse("=");

                sessionName = s;
            }
            else if((*linecount)[0] == 'i')
            {
                cpDebug(LOG_DEBUG, "got i= line") ;

                s = (*linecount);
		s.parse("=");

                sessionInfo = s;
            }
            else if((*linecount)[0] == 'u')
            {
                cpDebug(LOG_DEBUG, "got u= line") ;

                s = (*linecount);
		s.parse("=");

                uriInfo = s;
            }
	    else if((*linecount)[0] == 'e')
	    {
		// Collect all email addresses
                while ((linecount != lines.end()) && ((*linecount)[0] == 'e'))
                {
		    cpDebug(LOG_DEBUG, "Session: got %s", (*linecount).logData());

                s = (*linecount);
		s.parse("=");

//                    s = (*linecount);
//                    s.erase(0, 2);
//                    chomp (&s);

                    emailList.push_back(s);
		    ++linecount;
		}
                if (linecount != lines.end())
		{
		    // Not done yet, adjust line position 
		    --linecount;
		}
            }
	    else if((*linecount)[0] == 'p')
	    {
		// Collect all phone numbers
                while ((linecount != lines.end()) && ((*linecount)[0] == 'p'))
                {
		    cpDebug(LOG_DEBUG, "Session: got %s", (*linecount).logData());
                s = (*linecount);
		s.parse("=");

//                    s = (*linecount);
//                    s.erase(0, 2);
//                    chomp (&s);

                    phoneList.push_back(s);
		    ++linecount;
		}

                if (linecount != lines.end())
		{
		    // adjust line position for the next description
		    --linecount;
		}
            }
            else if ((*linecount)[0] == 'c')
            {
                cpDebug(LOG_DEBUG, "got c= line") ;
                // this is a c= line

                s = (*linecount);
		s.parse("=");

//                s = (*linecount);
//                s.erase(0, 2);
//                chomp (&s);

                //create the connection object, and store details there.
                try
                {
                    if (!connection)
                    {
                        connection = new SdpConnection(s);
                        assert (connection);
                    }
                    else
                    {
                        SdpConnection conn (s);
                        (*connection) = conn;
                    }
                }
                catch (SdpExceptions& exp)
                {
                    switch (exp.value)
                    {
                        case UNKNOWN_NETTYPE :
			    return false;
			    break;
			case UNKNOWN_ADDRTYPE :
			    return false;
			    break;
                        case PARAM_NUMERR :
			    return false;
			    break;
                        default:
			    // Unknown exception
			    assert(0);
			    break;
                    }
                }
            }
            else if ((*linecount)[0] == 't')
            {
                // Collect all Time Descriptions
                while ((linecount != lines.end()) && ((*linecount)[0] == 't'))
                {
                    cpDebug(LOG_DEBUG_STACK, "Session: got %s", (*linecount).logData());

                    list<Data> timeDescriptionLines;  // Holding place for a
						   // Time Description
                    timeDescriptionLines.push_back ((*linecount));
                    ++linecount;
                    // Get the optional "r=" line
                    while ((linecount != lines.end()) && ((*linecount)[0] == 'r'))
                    {
                        timeDescriptionLines.push_back ((*linecount));
                        ++linecount;
                    }

                    try
                    {
                        SdpTime time (timeDescriptionLines);
                        sdpTimeList.push_back (time);
                    }
                    catch (SdpExceptions& exp)
                    {
                        if (exp.value == FAILED)
                        {
                            return false;
                        }
                    }
                }

                if (linecount != lines.end())
                {
                    // adjust line position for the next description
                    --linecount;
                }
            }
	    else if((*linecount)[0] == 'z')
	    {
		cpDebug(LOG_DEBUG, "Session: got the \"z=\" line");
                s = (*linecount);
		s.parse("=");
//		s = (*linecount);
//		s.erase(0,2);
//                chomp (&s);

                vector<Data> adjustmentList;
		bool finished = false;

		while(!finished)
		{
		    Data item = s.parse(" ", &finished);
		    if(finished)
		    {
			item = s;
		    }
		    adjustmentList.push_back(item);
		}

		vector<Data>::iterator iter = adjustmentList.begin();
		while(iter != adjustmentList.end())
		{
                    SdpZoneAdjustment sza;

                    sza.setAdjustmentTime(*iter);

                    ++iter;

                    if (iter != adjustmentList.end())
                    {
			sza.setOffset((*iter));
                        zoneAdjustmentList.push_back(sza);
                        ++iter;
                    }
                    else    // <adjustment time> didn't followed by an <offset>
                    {
			sza.setOffset("0");
                        zoneAdjustmentList.push_back(sza);
                        break;
                    }
                }
	    }
            else if ((*linecount)[0] == 'b')
            {
                cpDebug(LOG_DEBUG_STACK, "Session: got the \"b=\" line");
                s = (*linecount);
		s.parse("=");
//                s = (*linecount);
//                s.erase(0, 2);
//                chomp (&s);

                //create the bandwidth object, and store details there.
                try
                {
                    if (!bandwidth)
                    {
                        bandwidth = new SdpBandwidth(s);
                        assert (bandwidth);
                    }
                    else
                    {
                        SdpBandwidth bw (s);
                        (*bandwidth) = bw;
                    }
                }
                catch (SdpExceptions& )
                {
                    //bandwidth is optional. So do nothing if incorrect.
                }

            }
	    else if((*linecount)[0] == 'k')
	    {
		cpDebug(LOG_DEBUG, "Session: got the \"k=\" line: %s", 
						(*linecount).logData());
                s = (*linecount);
		s.parse("=");
//                chomp (&s);

		//create the encrypttion key object, and store details there.
		try
		{
		    if (!encryptkey)
		    {
			encryptkey = new SdpEncryptkey(s);
			assert (encryptkey);
		    }
                    else
                    {
                        SdpEncryptkey key(s);
                        (*encryptkey) = key;
                    }
		}
		catch (SdpExceptions& )
		{
		    //Encryption key is optional. So do nothing if incorrect.
		}
            }
            else if ((*linecount)[0] == 'a')
            {
                //this is the attribute line.
                cpDebug(LOG_DEBUG_STACK, "Session: got the a=line");
                s = (*linecount);
		s.parse("=");
//                s.erase(0, 2);
//                chomp (&s);

                //create the attribute object, if not previously created.
                if (!attribute)
                {
                    cpDebug(LOG_DEBUG_STACK, "create a session attribute.");
                    attribute = new SdpAttributes();
                    assert(attribute);
                }

                try
                {
                    attribute->setAttribute(s);
                }
                catch (SdpExceptions& )
                {
                    //attribute is optional. So do nothing if incorrect.
                }

            }
            else if ((*linecount)[0] == 'm')
            {
                Data tempstr;
                //get all m lines until end of SDP msg.
                while (linecount != lines.end())
                {
		    list<Data> mediaDetails;

                    cpDebug(LOG_DEBUG_STACK, "Session:got the m= line");
                    cpDebug(LOG_DEBUG_STACK, "Session: %s", (*linecount).logData());
                    
                    if((*linecount)[0] == 'm')
                    {
                        (mediaDetails).push_back( (*linecount));  // subData copy.
                    }

                    ++linecount;

                    while ( (linecount != lines.end() ) && 
			    ((*linecount)[0] != 'm'))
                    {

                        cpDebug(LOG_DEBUG_STACK, "%s", (*linecount).logData());

                        (mediaDetails).push_back( (*linecount));
                        ++linecount;
                    }
                    try
                    {
                        SdpMedia* mediaItm = new SdpMedia(mediaDetails);
                        ostrstream ostr;
                        mediaItm->encode(ostr);
                        assert(mediaItm);
                        addMedia(mediaItm);
                    }
                    catch (SdpExceptions& exp)
                    {
                        if (exp.value == FAILED)
                            return false;
                    }

                } // end of processing of one m line
            }// end of processing all m lines
            else
            {
                cpDebug(LOG_DEBUG, "got unknown line") ;
                cpDebug(LOG_DEBUG, "line: %s", (*linecount).logData());
            }
            // We might have reached the end in processing m field.
            // In that case, we don't increment linecount
            if ( linecount != lines.end() )
            {
                ++linecount;
            }
        }
    }
    isValid = true;
    return true;
}

///
void
SdpSession::flushMediaList()
{
    if (mediaList.empty())
    {
        return ;
    }
    list < SdpMedia* > ::iterator iter = mediaList.begin();
    while (iter != mediaList.end())
    {
        if (*iter)
        {
            delete (*iter);
        }
        ++iter;
    }
    mediaList.clear();
}    // SdpSession::flushMediaList


///
void
SdpSession::reset()
{
    protocolType = SdpProtocolTypeSDP;

    protocolVersion = 0;

    username = "-";
    //TODO Use NTP time
    sessionId = static_cast < unsigned int > (time(NULL));
    //TODO Use NTP time
    version = static_cast < unsigned int > (time(NULL));
    networkType = NetworkTypeInternet;
    addressType = AddressTypeIPV4;
    address = "0.0.0.0";

    sessionName = "-";

    if ( 0 != connection )
    {
	delete connection;
	connection = 0;
    }

    if ( 0 != attribute )
    {
	delete attribute;
	attribute = 0;
    }

    if ( 0 != bandwidth )
    {
	delete bandwidth;
	bandwidth = 0;
    }

    if ( 0 != encryptkey )
    {
	delete encryptkey;
	encryptkey = 0;
    }

    SdpTime time;
    sdpTimeList.clear();
    sdpTimeList.push_back (time);

    isValid = false;

    flushMediaList();

    // Take care of some of the new components added.
    zoneAdjustmentList.clear();
    phoneList.clear();
    emailList.clear();
    uriInfo = "";
    sessionInfo = "";

}    // SdpSession::SdpSession

///
bool
SdpSession::conformToSdp ()
{
    bool rc = true;

    if (protocolVersion != 0)
    {
        cpLog (LOG_WARNING, "SdpSession::conformToSdp - protocol version %d",
               protocolVersion);
        rc = false;
    }
    if (networkType == NetworkTypeUnknown)
    {
        cpLog (LOG_WARNING, "SdpSession::conformToSdp - unknown network type");
        rc = false;
    }
    if (addressType == AddressTypeUnknown)
    {
        cpLog (LOG_WARNING, "SdpSession::conformToSdp - unknown address type");
        rc = false;
    }
    if (sdpTimeList.empty())
    {
        cpLog (LOG_WARNING, "SdpSession::conformToSdp - no time description");
        rc = false;
    }
    else
    {
        cpLog (LOG_DEBUG_STACK, "SdpSession::conformToSdp - PASSED");
    }
    return rc;
}    // SdpSession::conformToSdp


///
bool
SdpSession::conformToNcs ()
{
    bool rc = true;

    if (protocolVersion != 0)
    {
        cpLog (LOG_WARNING, "SdpSession::conformToNcs - protocol version %d",
               protocolVersion);
        rc = false;
    }
    if (username != "-")
    {
        cpLog (LOG_WARNING, "SdpSession::conformToNcs - username %s",
               username.logData());
        rc = false;
    }
    if (networkType != NetworkTypeInternet)
    {
        cpLog (LOG_WARNING, "SdpSession::conformToNcs - network type %s",
               networkTypeString().logData());
        rc = false;
    }
    if (addressType != AddressTypeIPV4)
    {
        cpLog (LOG_WARNING, "SdpSession::conformToNcs - address type %s",
               addressTypeString().logData());
        rc = false;
    }
    if (sessionName != "-")
    {
        cpLog (LOG_WARNING, "SdpSession::conformToNcs - sessionName %s",
               sessionName.logData());
        rc = false;
    }
    // No "i="
    // No "u="
    // No "e="
    // No "p="
    if (connection == 0)
    {
        cpLog (LOG_WARNING, "SdpSession::conformToNcs - no connection");
        rc = false;
    }
    else if (connection->getNetworkType() != NetworkTypeInternet)
    {
        cpLog (LOG_WARNING, "SdpSession::conformToNcs - network type %d",
               connection->getNetworkType());
        rc = false;
    }
    else if (connection->getAddressType() != AddressTypeIPV4)
    {
        cpLog (LOG_WARNING, "SdpSession::conformToNcs - address type %d",
               connection->getAddressType());
        rc = false;
    }
    else if (connection->getMulticast() != 0)
    {
        cpLog (LOG_WARNING, "SdpSession::conformToNcs - not unicast");
        rc = false;
    }
    if (bandwidth == 0)
    {
        cpLog (LOG_WARNING, "SdpSession::conformToNcs - no bandwidth");
        rc = false;
    }
    else if (bandwidth->getModifier() != BandwidthModifierTypeAS)
    {
        cpLog (LOG_WARNING, "SdpSession::conformToNcs - bandwidth modifier %d",
               bandwidth->getModifier());
        rc = false;
    }
    if (sdpTimeList.empty())
    {
        cpLog (LOG_WARNING, "SdpSession::conformToNcs - no time description");
        rc = false;
    }
    //TODO more time checking of stopTime == 0, no Repeat, no Time Zones
    // No "k="
    //TODO attribute("a=") lines like a=ptime:<value>
    if (mediaList.empty())
    {
        cpLog (LOG_WARNING, "SdpSession::conformToNcs - no media");
        rc = false;
    }
    else if ((*mediaList.begin())->getMediaType() != MediaTypeAudio)
    {
        cpLog (LOG_WARNING, "SdpSession::conformToNcs - not audio media");
        rc = false;
    }
    else if ((*mediaList.begin())->getTransportType() != TransportTypeRTP)
    {
        cpLog (LOG_WARNING, "SdpSession::conformToNcs - not RTP/AVP");
        rc = false;
    }
    else
    {
        cpLog (LOG_DEBUG_STACK, "SdpSession::conformToNcs - PASSED");
    }
    return rc;
}    // SdpSession::conformToNcs


/// Verify if this a well formed SDP according to protocol
bool
SdpSession::verify (SdpProtocolType protocol /*Default Argument*/)
{
    bool rc = true;
    if (protocol == SdpProtocolTypeSDP)
    {
        rc = conformToSdp ();
    }
    else if (protocol == SdpProtocolTypeNCS)
    {
        rc = conformToNcs ();
    }
    //TODO verify other protocol types
    if (!rc)
    {
        cpLog (LOG_WARNING, "SdpSession::verify - FAILED");
    }
    return rc;
}    // SdpSession::verify


///
void
SdpSession::setHold ()
{
    if (connection)
    {
        connection->setHold();
    }
    else
    {
        // TODO should we do something here
        cpLog (LOG_ERR, "SdpSession::setHold: Session connection doesn't exist");
    }

    return ;
}

///
bool
SdpSession::isHold ()
{
    if (connection)
    {
        return connection->isHold();
    }
    else
    {
        // TODO should we do something here
        cpLog (LOG_ERR, "SdpSession::isHold: Session connection doesn't exist");
        return false;
    }
}
