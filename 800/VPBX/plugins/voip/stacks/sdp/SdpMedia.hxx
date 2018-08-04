#ifndef _SDPMEDIA_H
#define _SDPMEDIA_H
/*
* $Log: SdpMedia.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:05:22  lizhijie
* no message
*
* $Id: SdpMedia.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include <vector>
#include <strstream>
#include <cassert>
#include "SdpExterns.hxx"
#include "SdpConnection.hxx"
#include "SdpBandwidth.hxx"
#include "SdpAttributes.hxx"
#include "SdpEncryptkey.hxx"

#include "cpLog.h"
#include "support.hxx"

///
enum SdpMediaType
{
    MediaTypeUnknown,
    MediaTypeAudio,
    MediaTypeVideo
};


/** Session Media
  * m=<media> <port> <transport> <fmt list> 
  * 
  * Besides the fields above, this class really holds the media block,
  * i.e. not just the "m=" line but all lines followed until next "m=" line
  */
class SdpMedia
{

    public:
        ///
        SdpMedia();
        ///
        SdpMedia(split_t& mediaDetails);

        ///
        SdpMediaType getMediaType()
        {
            return mediaType;
        }
        ///
        string getMediaTypeString()
        {
            return mediaTypeString;
        }
        ///
        void setMediaType( SdpMediaType newType );
        ///
        void setMediaType( string newType );
        ///
        int getPort()
        {
            return port;
        }
        ///
        void setPort(int newport)
        {
            port = newport;
        }
        ///
        int getNumPorts()
        {
            return numPorts;
        }
        ///
        void setNumPorts(int newnumPorts)
        {
            numPorts = newnumPorts;
        }
        ///
        TransportType getTransportType()
        {
            return transportType;
        }
        ///
        void setTransportType(TransportType newtransport);
        ///
        void addFormat( string fmt );
        /// deprecated
        void addFormat(int fmt)
        {
            formatList.push_back( itos( fmt ) );
            formatListInt.push_back( fmt );
        }
        ///
        vector < string > * getStringFormatList()
        {
            if (formatList.size() > 0) return &formatList;
            else return 0;
        }
        /// deprecated
        vector < int > * getFormatList()
        {
            if (formatListInt.size() > 0) return &formatListInt;
            else return 0;
        }
        ///
        void clearFormatList()
        {
            formatList.clear();
            formatListInt.clear();
        }
        ///
        MediaAttributes* getMediaAttributes()
        {
            return mediaAttrib;
        }

        ///
        void setMediaAttributes(MediaAttributes* attributes)
        {
            mediaAttrib = attributes;
        };
        ///
        SdpConnection* getConnection()
        {
            return connection;
        }
        ///
        void setConnection(const SdpConnection& conn) 
        { 
            if (!connection)
            {
                connection = new SdpConnection;
            }
            *connection = conn;
        }
        ///
        SdpBandwidth* getBandwidth()
        {
            return bandwidth;
        }
        ///
        SdpEncryptkey* getEncryptkey()
        {
            return encryptkey;
        }
        ///
        string getMediaInfo()
        {
            return mediaInfo;
        }
        ///
        void setMediaInfo(const string& info)
        {
            mediaInfo = info;
        }

        ///
        const SdpMedia& operator=(SdpMedia& media);

        ~SdpMedia()
        {
            if (mediaAttrib)
            {
                delete mediaAttrib;
            }
            if (connection)
            {
                delete connection;
            }
            if (bandwidth)
            {
                delete bandwidth;
            }
            if (encryptkey)
            {
                delete encryptkey;
            }
        }

        ///
        void encode (ostrstream& s);

    private:

	/// disable copy constructor
	SdpMedia(const SdpMedia&);
        ///
        string transportTypeString();
        ///
        void encodeMediaAnnouncement (ostrstream& s);

        SdpMediaType mediaType;
        string mediaTypeString;
        int port;
        int numPorts;
        TransportType transportType;     // RTP/AVP or UDP
        vector < string > formatList;
        vector < int > formatListInt;    // deprecated

        //maintain the MediaAttribute.
        MediaAttributes* mediaAttrib;

        //maintain the connection address in the c line.
        SdpConnection* connection;

        //maintain the bandwidth
        SdpBandwidth* bandwidth;

        //maintain the encryption key
        SdpEncryptkey* encryptkey;

        //maintain the media information
        string mediaInfo;

        ///
        static const int MaxFormatIntValue;
};


#endif
