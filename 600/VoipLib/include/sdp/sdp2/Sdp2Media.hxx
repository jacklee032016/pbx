#ifndef SDP2MEDIA_HXX_
#define SDP2MEDIA_HXX_
/*
 * $Id: Sdp2Media.hxx,v 1.2 2007/03/01 20:04:27 lizhijie Exp $
 */

#include <strstream>
#include <cassert>
#include <list>
#include "Sdp2Externs.hxx"
#include "Sdp2Connection.hxx"
#include "Sdp2Bandwidth.hxx"
#include "Sdp2Attributes.hxx"
#include "Sdp2Encryptkey.hxx"

#include "cpLog.h"
#include "support.hxx"


/** Infrastructure common to VOCAL.
 */
namespace Vocal 
{


/** Infrastructure in VOCAL related to logging.<br><br>
 */
namespace SDP
{


///
enum SdpMediaType
{
    MediaTypeUnknown,
    MediaTypeAudio,
    MediaTypeVideo
};


/** 
    SdpMedia describes the media lines in an SDP.
    <p>
    <b>Example</b>
    <pre>
    m=&lt;media> &lt;port> &lt;transport> &lt;fmt list> 
    </pre>
    In addition to the m= lines, this class holds the entire media block, 
    i.e. not just the "m=" line but all lines followed until next "m=" line.
    <p>

    A complete example of usage is available in the SdpSession class
    description.<p>

    @see SdpSession

  */
class SdpMedia
{

    public:
        ///
        SdpMedia();
        ///
        SdpMedia(list<Data>& mediaDetails);

        ///
        SdpMediaType getMediaType()
        {
            return mediaType;
        }
        ///
        Data getMediaTypeString()
        {
            return mediaTypeString;
        }
        ///
        void setMediaType( SdpMediaType newType );
        ///
        void setMediaType( Data newType );
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
        void addFormat( Data fmt );
        /// deprecated
        void addFormat(int fmt)
        {
            formatList.push_back( itos( fmt ) );
            formatListInt.push_back( fmt );
        }
        ///
        vector < Data > * getStringFormatList()
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
        void setBandwidth(SdpBandwidth& bWidth)
        {
            delete bandwidth;
            Data aVal = bWidth.modifierString();
            aVal += ":";
            aVal += Data(bWidth.getValue());
            bandwidth = new SdpBandwidth(aVal);
        }
        ///
        SdpEncryptkey* getEncryptkey()
        {
            return encryptkey;
        }
        ///
        Data getMediaInfo()
        {
            return mediaInfo;
        }
        ///
        void setMediaInfo(const Data& info)
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
        Data transportTypeString();
        ///
        void encodeMediaAnnouncement (ostrstream& s);

        SdpMediaType mediaType;
        Data mediaTypeString;
        int port;
        int numPorts;
        TransportType transportType;     // RTP/AVP or UDP
        vector < Data > formatList;
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
        Data mediaInfo;

        ///
        static const int MaxFormatIntValue;
};


}


}

#endif
