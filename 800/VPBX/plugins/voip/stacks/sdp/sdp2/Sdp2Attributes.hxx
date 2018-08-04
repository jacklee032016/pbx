#ifndef SDP2ATTRIBUTES_HXX_
#define SDP2ATTRIBUTES_HXX_
/*
* $Log: Sdp2Attributes.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:05:46  lizhijie
* no message
*
* $Id: Sdp2Attributes.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include <vector>
#include "Data.hxx"
#include <strstream>
#include <cassert>
#include "Sdp2Externs.hxx"
#include "cpLog.h"


/** Infrastructure common to VOCAL.
 */
namespace Assist 
{


/** Infrastructure in VOCAL related to logging.<br><br>
 */
namespace SDP
{


/**
    A single value attribute.

    <B>Example</B>
    <PRE>
    a=&lt;attribute>:&lt;value>
    </PRE>
    A complete example of usage is available in the SdpSession class
    description.<p>

    @see SdpSession
    <br>

 */
class ValueAttribute
{
    public:

        ///
        ValueAttribute();
        ///
        char* getAttribute();
        ///
        char* getValue();
        ///
        void setAttribute(const char* attrib);
        ///
        void setValue(const char* val);
        ///
        void encode (ostrstream& s);

    private:
        char attribute[256];
        char value[1256];
};


/** 
   <p>
   A collection of ValueAttributes.
   
   <b>TERMINOLOGY</b>
   (for more detailed description, please read RFC 2327)<p>

   <dl>
   <dt>Session Block
   <dd>lines between "v=" line and "m=" line, "m=" line  not included

   <dt>Media Block
   <dd>lines between two "m=" lines, the second "m=" line not included

   <dt>Session Attribute
   <dd>attributes specified in the session block

   <dt>Media Attribute
   <dd>attribute specified in the media block

   <dt>Property attribute
   <dd>"a=" line in the form of "a=<flag>", example "a=recvonly"

   <dt>Value attribute
   <dd>"a=" line in the form of "a=<attribute>:<value>"

   <dt>RTP Map Attribute
   <dd>a type of media value attribute in the form of 
   "a=rtpmap : &lt;payload type> &lt;encoding name> / &lt;clock rate> [&lt;encodeing parameters>]"
   </dl>
   <p>

   There are several attribute classes defined. Here is an attempt to
   explain the differences.<p>

   <dl>
   <dt>SdpAttributes
   <dd>this is the basic attribute class:
   <ul>
     <li>Both Session & Media property attributes use this class.
     <li>This class also served as base class of Media attribute,
   </ul>
   <dt>MediaAttributes
   <dd>
     <ul>
       <li>Used as a container for attributes in one media block, i.e. lines after "m=" line and before next "m=" line
       <li>Hence this class is contained in the SdpMedia class.
     </ul>
   <dt>SdpRtpMapAttribute
   <dd>
     <ul>
       <li>A special class for RTP Map Attribute because the format is different than others.	
       <li>Hence this class is contained in the MediaAttributes class.
     </ul>

    Attributes that are common to Session and Media.
    <p>
    <B>Examples</B>
    <PRE>
    a=&lt;attribute&gt;
    a=&lt;attribute>:&lt;value>
    </PRE>
    However, rtpmap is Not such an attribute.  
    A complete example of usage is available in the SdpSession class
    description.<p>

    @see SdpSession
    <br>
    @see SdpRtpMapAttribute SdpRtpMapAttribute
 */
class SdpAttributes
{
    public:
        ///
        SdpAttributes();

        ///
        SdpAttributes(const SdpAttributes& attr);

        ///
        ~SdpAttributes()
        { //delete attribvalue
            if (valueAttributes.size() > 0)
            {
                flushValueAttributes();
            }
        }

        ///
        const SdpAttributes& operator=(SdpAttributes& attrib);

        ///
        void setAttribute(Data& str);
        ///
        void setrecvonly();
        ///
        void setsendrecv();
        ///
        void setsendonly();
        ///
        bool getrecvonly();
        ///
        bool getsendrecv();
        ///
        bool getsendonly();

        ///
        void addValueAttribute (ValueAttribute* newValueAttribute)
        {
            valueAttributes.push_back(newValueAttribute);
        }
        ///
        vector < ValueAttribute* > * getValueAttributes()
        {
            return &valueAttributes;
        }
        ///
        void flushValueAttributes();

        /// Check if this is auser defined attribute
        bool isUserDefined(const char* str);

        ///
        void encode (ostrstream& s);

    private:
        bool recvonly;
        bool sendonly;
        bool sendrecv;

	void copyValueAttributes(const SdpAttributes& attrib);

        /// Value attributes
        vector < ValueAttribute* > valueAttributes;
};


const int encodingNameLen = 256;

/** 
    rtpmap media attribute - these are different than the standard
    SdpAttributes.
    <p>
    <B>Example:</B>
    <PRE>
    a=rtpmap : &lt;payload type> &lt;encoding name> / &lt;clock rate> [/&lt;encoding parameters>]
    </PRE>
    <p>
    An SdpRtpMapAttribute is typically contained in a MediaAttributes list, via
    MediaAttributes::addmap().
    <p>
    A complete example of usage is available in the SdpSession class
    description.<p>

    @see SdpSession
    @see SdpAttributes
    @see MediaAttributes
 */
class SdpRtpMapAttribute
{
    public:
        ///
        SdpRtpMapAttribute();
        ///
        void setPayloadType(int payload);
        ///
        int getPayloadType();
        ///
        void setEncodingName(const char* name);
        ///
        char* getEncodingName();
        ///
        void setClockRate(int rate);
        ///
        int getClockRate();
        ///
        void setEncodingParms(int parms);
        ///
        int getEncodingParms();

        ///
        void encode (ostrstream& s);

    private:
        int payloadType;
        char encodingName[encodingNameLen];
        int clockrate;

        /// Number of channels for audio streams
        int encodingParms;
};



/** 

    The MediaAttributes class is a subclass of the SdpAttributes and
    possibly contains one or more SdpRtpMapAttribute.
    <p>
    A complete example of usage is available in the SdpSession class
    description.<p>

    @see SdpSession
    @see SdpAttributes
    @see SdpRtpMapAttribute

 */
class MediaAttributes : public SdpAttributes
{
    public:
        ///
        MediaAttributes()
        {
            rtpmap.clear();
        }

        void setAttribute(Data& attrib);
        ///
        void addmap(SdpRtpMapAttribute* rtpattrib)
        {
            rtpmap.push_back(rtpattrib);
        }
        ///
        vector < SdpRtpMapAttribute* > * getmap()
        {
            return &rtpmap;
        }
        ///
        MediaAttributes& operator=(MediaAttributes& attrib);
        ///
        ~MediaAttributes()
        {
            if (rtpmap.size() > 0)
            {
                flushrtpmap();
            }
        }

        ///
        void flushrtpmap();

        ///
        void encode (ostrstream& s);


    private:
	/// disable copy constructor
	MediaAttributes(const MediaAttributes&);

        vector < SdpRtpMapAttribute* > rtpmap;
};


}


}

#endif
