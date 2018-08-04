/*
* $Log: SdpMedia.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SdpMedia.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "global.h"
#include <string>
#include "support.hxx"

#include "SdpExceptions.hxx"
#include "SdpMedia.hxx"


const int SdpMedia::MaxFormatIntValue = 15;


///
SdpMedia::SdpMedia()
{
    mediaType = MediaTypeAudio;
    mediaTypeString = "audio";
    port = -1;
    numPorts = 0;
    transportType = TransportTypeRTP;
    formatList.clear();
    formatList.push_back( "0" );
    formatListInt.clear();
    formatListInt.push_back( 0 );
    mediaAttrib = 0;
    connection = 0;
    bandwidth = 0;
    encryptkey = 0;
    mediaInfo = "";
}    // SdpMedia::SdpMedia


///
SdpMedia::SdpMedia (split_t& mediaDetails)
{
    //initialize all pointers
    bandwidth = 0;
    mediaAttrib = 0;
    connection = 0;
    encryptkey = 0;
    split_t::iterator linecount;

    linecount = mediaDetails.begin();
    //print Media details
    split_t::iterator tempcount;
    tempcount = mediaDetails.begin();
    while (tempcount != mediaDetails.end())
    {
        cpDebug(LOG_DEBUG_STACK, "SdpMedia: %s", (*tempcount).c_str());
        tempcount++;
    }

    while (linecount != mediaDetails.end())
    {
        if ((*linecount)[0] == 'm')
        {
            string lineStr = (*linecount);
            lineStr.erase(0, 2);
            chomp (&lineStr);

            split_t mList(split(lineStr, " "));

            if (mList.size() < 4)
            {
                cpLog(LOG_ERR, "SdpMedia: m= size < 4");
                throw SdpExceptions(PARAM_NUMERR);
            }
            else
                cpDebug(LOG_DEBUG_STACK, "SdpMedia:m size >4");

            //parse media items.
            split_t::iterator mListIter;

            mListIter = mList.begin();

            mediaTypeString = *mListIter;
            if (mediaTypeString == string("audio"))
            {
                mediaType = MediaTypeAudio;
                cpDebug(LOG_DEBUG_STACK, "Found m=audio");
            }
            else if (mediaTypeString == string("video"))
            {
                mediaType = MediaTypeVideo;
                cpDebug(LOG_DEBUG_STACK, "Found m=video");
            }
            else
            {
                mediaType = MediaTypeUnknown;
                cpDebug(LOG_DEBUG_STACK, "Found m=%s", mediaTypeString.c_str() );
            }

            ++mListIter;
            /** port may be <port>/<num of ports>
                So, split mListIter again on /  */
            string portDetails = (*mListIter);

            split_t portList(split(portDetails, "/"));


            port = strtol(portList[0].c_str(), (char**)NULL, 10);
            cpDebug(LOG_DEBUG_STACK, "SDP port: %d " , port);

            if (portList.size() == 2)
            {
                //contains the number of ports.
                numPorts = atoi(portList[1].c_str());
            }
            else
            {
                numPorts = 0;
            }

            ++mListIter;

            if (*mListIter == SdpMediaTransportRTPAVP)
            {
                transportType = TransportTypeRTP;
            }
            else if (*mListIter == SdpMediaTransportUDP)
            {
                transportType = TransportTypeUDP;
            }

            //handle the format list.
            formatList.clear();
            formatListInt.clear();
            char format[ MaxFormatIntValue+1 ];
            format[ MaxFormatIntValue ] = '\0';
            char* endPtr = 0;
            int formatInt = 0;
            ++mListIter;
            while (mListIter != mList.end())
            {
                //these are all format list items
                cpDebug(LOG_DEBUG_STACK, "Media format: %s", (*mListIter).c_str() );
                formatList.push_back( *mListIter );

                strncpy( format, (*mListIter).c_str(), MaxFormatIntValue );
                formatInt = strtol( format, &endPtr, 10 );
                if( endPtr == format )    // Format is not an integer
                {
                    cpDebug(LOG_DEBUG, "Non-integer media format: %s", (*mListIter).c_str() );
                }
                else                      // Format is an integer
                {
                    cpDebug(LOG_DEBUG, "Integer media format: %d", formatInt );
                    formatListInt.push_back( formatInt );
                }
                ++mListIter;
            }

        }// if line = m.
        else if ((*linecount)[0] == 'a')
        {
            try
            {
                if (!mediaAttrib)
                {
                    //create the MediaAttribute object.
                    mediaAttrib = new MediaAttributes();
                    assert(mediaAttrib);
                }
                string attrib = (*linecount);
                attrib.erase(0, 2);
                chomp (&attrib);
                mediaAttrib->setAttribute(attrib);
            }
            catch (SdpExceptions& )
            {
                throw SdpExceptions(FAILED);
            }
        }
        else if ((*linecount)[0] == 'b')
        {
            try
            {
                string bw = (*linecount);
                bw.erase(0, 2);
                chomp (&bw);
                cpDebug(LOG_DEBUG_STACK, "Bandwidth, before sending to C'tor:%s", bw.c_str());
                bandwidth = new SdpBandwidth(bw);
                assert(bandwidth);
            }
            catch (SdpExceptions& )
            {
                throw SdpExceptions(FAILED);
            }
        }
        else if ((*linecount)[0] == 'k')
        {
            try
            {
                if (encryptkey)
                {
                    delete encryptkey;
                    encryptkey = 0;
                }

                string key = (*linecount);
                key.erase(0, 2);
                chomp (&key);
                cpDebug(LOG_DEBUG, "Encryption key, before sending to C'tor:%s", key.c_str());
                encryptkey = new SdpEncryptkey(key);
                assert(encryptkey);
            }
            catch (SdpExceptions& )
            {
                throw SdpExceptions(FAILED);
            }
        }
        else if ((*linecount)[0] == 'c')
        {
            try
            {
                string conn = (*linecount);
                conn.erase(0, 2);
                chomp (&conn);

                connection = new SdpConnection(conn);
                assert(connection);
            }
            catch (SdpExceptions& )
            {
                throw SdpExceptions(FAILED);
            }
        }
        else if ((*linecount)[0] == 'i')
        {
            string info = (*linecount);
            info.erase(0, 2);
            chomp (&info);

            mediaInfo = info;
        }
        else if ((*linecount)[0] == '\r' || (*linecount)[0] == '\n' )
        {
            //TODO make sure this is ok
            cpDebug(LOG_DEBUG_STACK, "Empty line" );
        }
        else
        {
            cpLog(LOG_ERR, "Unknown field in the media portion %s",
                  (*linecount).c_str());
            throw SdpExceptions(UNKNOWN_FIELD);
        }
        linecount++;
    }    // while
}    // SdpMedia::SdpMedia(split_t& mediaDetails)


///
const SdpMedia&
SdpMedia::operator=(SdpMedia& newMedia)
{
    mediaType = newMedia.mediaType;
    mediaTypeString = newMedia.mediaTypeString;
    port = newMedia.port;
    numPorts = newMedia.numPorts;
    transportType = newMedia.transportType;
    mediaInfo = newMedia.mediaInfo;
    cpDebug(LOG_DEBUG_STACK, "Copied media, port, numPorts, transportType");

    vector < string > * newFormatList = newMedia.getStringFormatList();
    assert(newFormatList);

    formatList = *newFormatList;
    vector < int > * newIntFormatList = newMedia.getFormatList();
    if( newIntFormatList != 0 )
    {
        formatListInt = *(newMedia.getFormatList());
    }
    else
    {
        formatListInt.clear();
    }
    cpDebug(LOG_DEBUG_STACK, "SdpMedia: copied the format list");

    if (newMedia.mediaAttrib)
    {
        if (!mediaAttrib)
        {
            mediaAttrib = new MediaAttributes;
            assert(mediaAttrib);
        }
        *(mediaAttrib) = *(newMedia.mediaAttrib);
        //copy parent details.
//        *((SdpAttributes*)(mediaAttrib)) = *((SdpAttributes*)(newMedia.mediaAttrib));
        cpDebug(LOG_DEBUG_STACK, "SdpMedia: Copied Media attributes");
    }
    else //if not defined now, but previously defined, remove previous definition.
    {
        if (mediaAttrib)
        {
            delete mediaAttrib;
            mediaAttrib = 0;
        }
    }
    if (newMedia.connection)
    {
        if (!connection)
        {
            connection = new SdpConnection;
            assert(connection);
        }
        *(connection) = *(newMedia.connection);
        cpDebug(LOG_DEBUG_STACK, "SdpMedia: copied connection");
    }
    else
    {
        if (connection)
        {
            delete connection;
            connection = 0;
        }
    }
    if (newMedia.bandwidth)
    {
        if (!bandwidth)
        {
            bandwidth = new SdpBandwidth;
            assert(bandwidth);
        }
        *(bandwidth) = *(newMedia.bandwidth);
        cpDebug(LOG_DEBUG_STACK, "SdpMedia:copied bandwidth");
    }
    else
    {
        if (bandwidth)
        {
            delete bandwidth;
            bandwidth = 0;
        }
    }
    if (newMedia.encryptkey)
    {
        if (!encryptkey)
        {
            encryptkey = new SdpEncryptkey;
            assert(encryptkey);
        }
        *(encryptkey) = *(newMedia.encryptkey);
        cpDebug(LOG_DEBUG, "SdpEncryptkey:copied encryptkey");
    }
    else
    {
        if (encryptkey)
        {
            delete encryptkey;
            encryptkey = 0;
        }
    }
    return *(this);
}


///
string
SdpMedia::transportTypeString ()
{
    string s;

    switch (transportType)
    {
        case TransportTypeRTP:
        {
            s = SdpMediaTransportRTPAVP;
            break;
        }
        case TransportTypeUDP:
        {
            s = SdpMediaTransportUDP;
            break;
        }
        default:
        {
            //TODO Throw some exception
            break;
        }
    }
    return s;
}    // SdpMedia::transportTypeString


///
void
SdpMedia::encodeMediaAnnouncement (ostrstream& s)
{
    s << "m="
      << mediaTypeString
      << ' '
      << port;

    if (numPorts > 0)
    {
        s << '/' << numPorts;
    }
    s << ' ' << transportTypeString();

    vector < string > ::iterator iter = formatList.begin();
    while (iter != formatList.end())
    {
        s << ' ' << *iter;
        ++iter;
    }
    s << "\r\n";
}    // SdpMedia::encodeMediaAnnouncement


///
void
SdpMedia::setTransportType(TransportType newtransport)
{
    //TODO relax these checks?
    switch (newtransport)
    {
        case TransportTypeRTP:
        if (((port > 1023) && (port < 65536)) && (0 == (port % 2)))
            transportType = newtransport;
        else
            cpLog(LOG_ERR, "for RTP, port: %d should be within 1024 - 65536 and even", port);
        break;
        case TransportTypeUDP:
        if ((port > 1023) && (port < 65536))
            transportType = newtransport;
        else
            cpLog(LOG_ERR, "for UDP, port: %d should be within 1024 - 65536", port);
        break;
        default:
        cpLog(LOG_ERR, "Unknown transport type %d", newtransport);
    }
}


///
void 
SdpMedia::addFormat( string fmt )
{
    formatList.push_back( fmt );

    char format[ MaxFormatIntValue+1 ];
    format[ MaxFormatIntValue ] = '\0';
    strncpy( format, fmt.c_str(), MaxFormatIntValue );
    char* endPtr = 0;
    int formatInt = 0;
    formatInt = strtol( format, &endPtr, 10 );
    if( endPtr != format )    // Format is an integer
    {
        formatListInt.push_back( formatInt );
    }
}


///
void
SdpMedia::encode (ostrstream& s)
{
    // m= media name and transport address
    encodeMediaAnnouncement (s);

    // i=* media title
    if (mediaInfo.size() > 0)
    {
        s << "i=" << mediaInfo << "\r\n";
    }

    // c=* connection information (not required if included at session-level)
    if (connection)
    {
        connection->encode (s);
    }

    // b=* bandwidth information
    if (bandwidth)
    {
        bandwidth->encode (s);
    }

    // k=* encryption key
    if (encryptkey)
    {
        encryptkey->encode (s);
    }

    // a=* zero or more session attribute lines
    if (mediaAttrib)
    {
        mediaAttrib->encode (s);
    }
}    // SdpMedia::encode


void
SdpMedia::setMediaType( SdpMediaType newType )
{
    switch( newType )
    {
        case MediaTypeAudio:
        {
            mediaTypeString = "audio";
        }
        break;
        case MediaTypeVideo:
        {
            mediaTypeString = "video";
        }
        break;
        default:
        {
            cpLog( LOG_ERR, "Unknown media type %d", newType );
            mediaTypeString = "unknown";
        }
    }
}    // SdpMedia::setMediaType


void
SdpMedia::setMediaType( string newType )
{
    mediaTypeString = newType;
    if( newType == "audio" )
    {
        mediaType = MediaTypeAudio;
    }
    else if( newType == "video" )
    {
        mediaType = MediaTypeVideo;
    }
    else
    {
        mediaType = MediaTypeUnknown;
    }
}    // SdpMedia::setMediaType
