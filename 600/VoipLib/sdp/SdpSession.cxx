/*
 * $Id: SdpSession.cxx,v 1.2 2007/03/01 20:04:31 lizhijie Exp $
 */

#include "global.h"
#include "debug.h"

#include <cassert>
#include <time.h>
#include <strstream>

#include "SdpExceptions.hxx"
#include "SdpSession.hxx"


// Other files can include "SdpExterns.hxx" to reference the following
// const char []s

const char SdpNetworkTypeIN[] = "IN";

const char SdpAddressTypeIP4[] = "IP4";
const char SdpAddressTypeIP6[] = "IP6";

const char SdpMediaTransportRTPAVP[] = "RTP/AVP";
const char SdpMediaTransportUDP[] = "UDP";

const char SdpBandwidthModifierAS[] = "AS";  // Application specific maximum
const char SdpBandwidthModifierCT[] = "CT";  // Conference Total

const char SdpMediaTypeAudio[] = "audio";
const char SdpMediaTypeVideo[] = "video";

const char SdpAttributeRecvonly[] = "recvonly";
const char SdpAttributeSendrecv[] = "sendrecv";
const char SdpAttributeSendonly[] = "sendonly";

const char SdpEncryptkeyMethodClear[] = "clear";
const char SdpEncryptkeyMethodBase64[] = "base64";
const char SdpEncryptkeyMethodURI[] = "uri";
const char SdpEncryptkeyMethodPrompt[] = "prompt";

// When adding a new const char [], add the corresponding extern declaration
// in "SdpExterns.hxx"


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
    vector < SdpMedia* > newMediaList = x.getMediaList();

    flushMediaList();

    if (newMediaList.size() > 0)
    {
        vector < SdpMedia* > ::iterator mediaiter;
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
    setUserName (string("-"));
    // User needs to call setAddress()

    setSessionName (string("-"));

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
string
SdpSession::networkTypeString()
{
    string s;

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
string
SdpSession::addressTypeString()
{
    string s;

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
    if (sessionInfo.size() > 0)
    {
        s << "i=" << sessionInfo << "\r\n";
    }
}    // SdpSession::encodeSessionInformation


///

void
SdpSession::encodeURI (ostrstream& s)
{
    if (uriInfo.size() > 0)
    {
        s << "u=" << uriInfo << "\r\n";
    }
}    // SdpSession::encodeURI


///

void
SdpSession::encodeEmailAddress (ostrstream& s)
{
    vector<string>::iterator iter = emailList.begin();

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
    vector<string>::iterator iter = phoneList.begin();

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
    vector < SdpTime > ::iterator iter = sdpTimeList.begin();

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
        vector<SdpZoneAdjustment>::iterator iter;
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
    vector < SdpMedia* > ::iterator mediaIterator = mediaList.begin();

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
static const int SdpTextPayloadLength = 1024;


///
string
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


bool SdpSession::decode(const string& buffer)
{
    split_t lines(split(buffer, "\n"));
    bool result = decode(lines);

    return result;
}


bool SdpSession::decode(split_t& lines)
{
    ODEBUG(DEBUG_SDP, 2, cout << "number of lines: " << lines.size() << endl; ) ;

    if (lines.size() < 3)
    {
        cpLog( LOG_ERR, "Sdp decode fail, too few lines" );
        isValid = false;
        return false;
    }

    sdpTimeList.clear();

    split_t::iterator linecount;

    linecount = lines.begin();

    {
        // lines[0] is the version line -- ignore

        string s = (*linecount);
        chomp (&s);

        if (!((*linecount) == "v=0\r" || (*linecount) == "v=0"))
            if (!(s == "v=0"))
            {
                // no good
                cerr << "SDP: did not find version in:" << endl;
                cerr << (*linecount) << endl;
                return false;
            }

        ++linecount;

        while (linecount != lines.end())
        {
            // this switch parses lines as they come

            if ((*linecount)[0] == 'o')
            {
                ODEBUG(DEBUG_SDP, 2, cout << "got o= line" << endl; ) ;
                // this is the options line -- do we care about it?

                // parse line 1 for the proper stuff

                s = (*linecount);
                s.erase(0, 2);
                chomp (&s);

                split_t optionsList(split(s, " "));


                if (optionsList.size() < 6)
                {
                    // not enough parameters
                    cpLog( LOG_ERR, "Parameter count < 6: %s", (*linecount).c_str() );
                    if ( linecount != lines.end() )
                    {
                        ++linecount;
                    }
                    continue;
                }

                username = optionsList[0];
                sessionId = atoi(optionsList[1].c_str());
                version = atoi(optionsList[2].c_str());
                if (optionsList[3] == SdpNetworkTypeIN)
                {
                    networkType = NetworkTypeInternet;
                }
                else
                {
                    cpLog( LOG_ERR, "Unknown network type %s", optionsList[3].c_str() );
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
                    cpLog( LOG_ERR, "Unknown address type %s", optionsList[4].c_str() );
                    if ( linecount != lines.end() )
                    {
                        ++linecount;
                    }
                    continue;
                }
                address = optionsList[5].c_str();
            }
            else if ((*linecount)[0] == 's')
            {
                ODEBUG(DEBUG_SDP, 2, cout << "got s= line" << endl; ) ;

                s = (*linecount);
                s.erase(0, 2);
                chomp (&s);

                sessionName = s;
            }
            else if((*linecount)[0] == 'i')
            {
                ODEBUG(DEBUG_SDP, 2, cout << "got i= line" << endl; ) ;

                s = (*linecount);
                s.erase(0, 2);
                chomp (&s);

                sessionInfo = s;
            }
            else if((*linecount)[0] == 'u')
            {
                ODEBUG(DEBUG_SDP, 2, cout << "got u= line" << endl; ) ;

                s = (*linecount);
                s.erase(0, 2);
                chomp (&s);

                uriInfo = s;
            }
	    else if((*linecount)[0] == 'e')
	    {
		// Collect all email addresses
                while ((linecount != lines.end()) && ((*linecount)[0] == 'e'))
                {
		    cpDebug(LOG_DEBUG, "Session: got %s", (*linecount).c_str());

                    s = (*linecount);
                    s.erase(0, 2);
                    chomp (&s);

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
		    cpDebug(LOG_DEBUG, "Session: got %s", (*linecount).c_str());

                    s = (*linecount);
                    s.erase(0, 2);
                    chomp (&s);

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
                ODEBUG(DEBUG_SDP, 2, cout << "got c= line" << endl; ) ;
                // this is a c= line

                s = (*linecount);
                s.erase(0, 2);
                chomp (&s);

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
                    cpDebug(LOG_DEBUG_STACK, "Session: got %s", (*linecount).c_str());

                    split_t timeDescriptionLines;  // Holding place for a
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
		s.erase(0,2);
                chomp (&s);

                split_t adjustmentList(split(s, " "));
                split_t::iterator iter;
                iter = adjustmentList.begin();

                while (iter != adjustmentList.end())
                {
                    SdpZoneAdjustment sza;

                    sza.setAdjustmentTime((*iter));
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
                s.erase(0, 2);
                chomp (&s);

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
						linecount[0].c_str());
		s = (*linecount);
		s.erase(0,2);
                chomp (&s);

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
                s = (*linecount).c_str();
                s.erase(0, 2);
                chomp (&s);

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
                string tempstr;
                //get all m lines until end of SDP msg.
                while (linecount != lines.end())
                {
                    split_t mediaDetails;

                    cpDebug(LOG_DEBUG_STACK, "Session:got the m= line");
                    cpDebug(LOG_DEBUG_STACK, "Session: %s", (*linecount).c_str());

                    (mediaDetails).push_back( (*linecount));  // substring copy.

                    ++linecount;

                    while ( (linecount != lines.end() ) && 
			    ((*linecount)[0] != 'm'))
                    {

                        cpDebug(LOG_DEBUG_STACK, "%s", (*linecount).c_str());

                        (mediaDetails).push_back( (*linecount));
                        ++linecount;
                    }
                    try
                    {
                        SdpMedia* mediaItm = new SdpMedia(mediaDetails);
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
                ODEBUG(DEBUG_SDP, 2, cout << "got unknown line" << endl; ) ;
                ODEBUG(DEBUG_SDP, 2, cout << "line: " << (*linecount) << endl; );
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
    vector < SdpMedia* > ::iterator iter = mediaList.begin();
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
               username.c_str());
        rc = false;
    }
    if (networkType != NetworkTypeInternet)
    {
        cpLog (LOG_WARNING, "SdpSession::conformToNcs - network type %s",
               networkTypeString().c_str());
        rc = false;
    }
    if (addressType != AddressTypeIPV4)
    {
        cpLog (LOG_WARNING, "SdpSession::conformToNcs - address type %s",
               addressTypeString().c_str());
        rc = false;
    }
    if (sessionName != "-")
    {
        cpLog (LOG_WARNING, "SdpSession::conformToNcs - sessionName %s",
               sessionName.c_str());
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
    else if (mediaList[0]->getMediaType() != MediaTypeAudio)
    {
        cpLog (LOG_WARNING, "SdpSession::conformToNcs - not audio media");
        rc = false;
    }
    else if (mediaList[0]->getTransportType() != TransportTypeRTP)
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
