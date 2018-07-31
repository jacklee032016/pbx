#ifndef _SDPATTRIBUTES_H
#define _SDPATTRIBUTES_H
/*
* $Log: SdpAttributes.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:05:22  lizhijie
* no message
*
* $Id: SdpAttributes.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include <vector>
#include <string>
#include <strstream>
//#include <assert.h>
#include <cassert>
#include "SdpExterns.hxx"

#include "cpLog.h"

/**
 *  TERMINOLOGY	  (for more detailed description, please read RFC 2327)
 *
 *	Session Block : lines between "v=" line and "m=" line, "m=" line
 *			    not included
 *	Media Block : lines between two "m=" lines, the second "m+' line 
 *			    not included
 *	Session Attribute: attributes specified in the session block
 *	Media Attribute: attribute specified in the media block
 *	Property attribute : "a=" line in the form of "a=<attribute>"
 *	Value attribute : "a=" line in the form of "a=<attribute>:<value>"
 *	RTP Map Attribute : a type of media value attribute in the form of
 *		"a=rtpmap : <payload type> <encoding name> / <clock rate> [/<encodeing parameters>]"	
 *	
 *                      ********************                               
 *
 *  Several attribute classes defined. Here is an attemp to explain:
 *
 *	SdpAttributes : this is the basic attribute class
 *		1) Both Session & Media property attributes use this class.
 *		2) This class also served as base class of Media attribute,
 *	
 *	MediaAttributes : 
 *		1) Used as a container for attributes in one media block, 
 *			i.e. lines after "m=" line and before next "m=" line
 *		2) Hence this class is contained in the SdpMedia class.
 *
 *	SdpRtpMapAttribute :
 *		1) A special class for RTP Map Attribute because the format
 *			is different than others.	
 *		2) Hence this class is contained in the MediaAttributes class.
 */

/** A single value attribute
 *   a=<attribute>:<value>
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
        char value[256];
};


/** Attributes that are common to Session and Media
    a=<attribute>
    a=<attribute>:<value>
    EXCEPT:
    a=rtpmap:...
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
        void setAttribute(string& str);
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

/** rtpmap media attribute
 * a=rtpmap : <payload type> <encoding name> / <clock rate> [/<encodeing parameters>]
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



/** Media attributes: SdpAttributes class plus SdpRtpMapAttribute class
 */
class MediaAttributes : public SdpAttributes
{
    public:
        ///
        MediaAttributes()
        {
            rtpmap.clear();
        }

        void setAttribute(string& attrib);
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

#endif
