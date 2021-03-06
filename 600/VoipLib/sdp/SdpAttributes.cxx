/*
 * $Id: SdpAttributes.cxx,v 1.2 2007/03/01 20:04:31 lizhijie Exp $
 */

#include "global.h"
#include "support.hxx"
#include "SdpExceptions.hxx"
#include "SdpAttributes.hxx"


/******************** SdpAttributes class methods ***********************/

bool
SdpAttributes::isUserDefined (const char* str)
{
    if (strlen(str) < 2)
        return false;


    if ( (*str == 'X') && (*(str++) == '-') )
        return true;
    else
        return false;
}


/* Parse s and set individual values.
   we are sure we dont get the rtpmap string here, since this is
   called from the Session part, i.e. rtpmap attribute is set
   only within the media attribute 
*/
void
SdpAttributes::setAttribute (string& s)
{

    /* the line can be of the form 
	a=<attribute> 			i.e. Property Attribute OR 
	a=<attribute>:<value> 		i.e. Value Attribute	OR 
	a=<userdefined attr>:<value>	i.e. User-defined Attribute
    */

    cpDebug(LOG_DEBUG_STACK, "SdpAttributes: %s", s.c_str());

    split_t attributeList(split(s, ":"));

    if (attributeList.size() < 1)
    {
        delete(&attributeList);
        cpLog(LOG_ERR, "Attribute: param count < 1 on attributeList");
        throw SdpExceptions(PARAM_NUMERR);
    }
    else if (attributeList.size() == 1)	// Property Attribute
    {
        if (attributeList[0] == "recvonly")
        {
            setrecvonly();
        }
        else if (attributeList[0] == "sendrecv")
        {
            setsendrecv();
        }
        else if (attributeList[0] == "sendonly")
        {
            setsendonly();
        }
    }
    else if ( attributeList.size() == 2 ) // Value or User-defined attribute
    {                                       
        cpLog (LOG_DEBUG_STACK, "Attrib, value: %s, %s",
               attributeList[0].c_str(),
               attributeList[1].c_str());

        ValueAttribute* valueAttribute = new ValueAttribute;
        assert (valueAttribute);

        valueAttribute->setAttribute (attributeList[0].c_str());
        valueAttribute->setValue (attributeList[1].c_str());

        addValueAttribute (valueAttribute);
    }
    else if ( attributeList.size() > 2 ) // Assume a value attribute but the value string has : in it
    {
        string value;
        
        value = attributeList[1];
        for (unsigned int i = 2; i <  attributeList.size(); i++)
        {
            value += ":";
            value += attributeList[i];
        }

        cpLog (LOG_DEBUG_STACK, "Attrib, value: %s, %s",
               attributeList[0].c_str(),
               value.c_str());

        ValueAttribute* valueAttribute = new ValueAttribute;
        assert (valueAttribute);

        valueAttribute->setAttribute (attributeList[0].c_str());
        valueAttribute->setValue (value.c_str());

        addValueAttribute (valueAttribute);
    }
}


SdpAttributes::SdpAttributes()
    : recvonly(false),
      sendonly(false),
      sendrecv(false)
{
}


SdpAttributes::SdpAttributes(const SdpAttributes& attr)
    : recvonly(attr.recvonly),
      sendonly(attr.sendonly),
      sendrecv(attr.sendrecv)
{
    //copy user defined attributes from attrib to this.
    //copy individual members.
    copyValueAttributes(attr);
}


void
SdpAttributes::setrecvonly ()
{
    recvonly = true;
}

void
SdpAttributes::setsendrecv ()
{
    sendrecv = true;
}

void
SdpAttributes::setsendonly ()
{
    sendonly = true;
}


bool
SdpAttributes::getrecvonly ()
{
    return recvonly;
}

bool
SdpAttributes::getsendrecv ()
{
    return sendrecv;
}

bool
SdpAttributes::getsendonly ()
{
    return sendonly;
}

void
SdpAttributes::flushValueAttributes ()
{
    vector < ValueAttribute* > ::iterator iter;
    for( iter = valueAttributes.begin(); iter != valueAttributes.end() ; ++iter)
    {
        if (*iter)
        {
            delete (*iter);
        }
    }
    valueAttributes.clear();
}

const SdpAttributes&
SdpAttributes::operator=(SdpAttributes& attrib)
{

    recvonly = attrib.getrecvonly();
    sendonly = attrib.getsendonly();
    sendrecv = attrib.getsendrecv();
    //copy user defined attributes from attrib to this.
    //copy individual members.
    copyValueAttributes(attrib);

    return *(this);
}


void
SdpAttributes::encode (ostrstream& s)
{
    if (recvonly)
    {
        s << "a=" << SdpAttributeRecvonly << "\r\n";
    }
    else if (sendonly)
    {
        s << "a=" << SdpAttributeSendonly << "\r\n";
    }
    else if (sendrecv)
    {
        s << "a=" << SdpAttributeSendrecv << "\r\n";
    }

    vector < ValueAttribute* > ::iterator iter = valueAttributes.begin();
    while (iter != valueAttributes.end())
    {
        (*iter)->encode (s);
        iter++;
    }
}    // SdpAttributes::encode


void
SdpAttributes::copyValueAttributes(const SdpAttributes& attrib)
{
    const vector < ValueAttribute* > * newValueAttributes 
	= &attrib.valueAttributes;
    assert(newValueAttributes);

    flushValueAttributes();

    vector < ValueAttribute* > ::const_iterator iter;
    for(iter = newValueAttributes->begin() ;
	iter != newValueAttributes->end() ;
	++iter)
    {
	if (*iter)
	{
	    ValueAttribute* valueAttribute = new ValueAttribute;
	    assert (valueAttribute);

	    //copy the object.
	    *valueAttribute = *(*iter);

	    //assign to the vector.
	    addValueAttribute (valueAttribute);
	}
	else
	{
	    addValueAttribute ( 0 );
	}
    }
}

/******************** SdpRtpMapAttribute class methods **************************/

SdpRtpMapAttribute::SdpRtpMapAttribute ()
{
    encodingParms = 0;     // no additional audio channel
}


void
SdpRtpMapAttribute::setPayloadType (int payload)
{
    payloadType = payload;
}

int
SdpRtpMapAttribute::getPayloadType ()
{
    return payloadType;
}

void
SdpRtpMapAttribute::setEncodingName (const char* name)
{
    strncpy(encodingName, name, encodingNameLen);
    encodingName[encodingNameLen-1] = '\0';
}

char*
SdpRtpMapAttribute::getEncodingName ()
{
    return encodingName;
}

void
SdpRtpMapAttribute::setClockRate (int rate)
{
    clockrate = rate;
}

int
SdpRtpMapAttribute::getClockRate ()
{
    return clockrate;
}

void
SdpRtpMapAttribute::setEncodingParms (int parms)
{
    encodingParms = parms;
}

int
SdpRtpMapAttribute::getEncodingParms ()
{
    return encodingParms;
}


///
void
SdpRtpMapAttribute::encode (ostrstream& s)
{
    cpLog (LOG_DEBUG_STACK, "a=rtpmap:%d %s/%d",
           payloadType,
           encodingName,
           clockrate);

    s << "a=rtpmap:" << payloadType
    << ' '
    << encodingName
    << '/'
    << clockrate;
    if (encodingParms >= 1)    // Number of channels for audio streams
    {                          //   This parameter may be omitted if it is "/1".
                               //   The constructor of this class omits it (set to 0) if it 
                               //     it is not set explicitly.
                               //   If the original line omits it, encodingParms will have
                               //     the default value 0 after decoding.
        cpLog (LOG_DEBUG_STACK, "/%d", encodingParms);
        s << '/' << encodingParms;
    }
    s << "\r\n";
}    // SdpRtpMapAttribute::encode


/********************** Value Attribute Class Methods *******************/

ValueAttribute::ValueAttribute ()
{
}


char*
ValueAttribute::getAttribute ()
{
    return attribute;
}

void
ValueAttribute::setAttribute (const char* attrib)
{
    strcpy(attribute, attrib);
}

char*
ValueAttribute::getValue ()
{
    return value;
}

void
ValueAttribute::setValue (const char* val)
{
    strcpy(value, val);
}


///
void
ValueAttribute::encode (ostrstream& s)
{
    s << "a=" << attribute << ':' << value << "\r\n";
}    // ValueAttribute::encode


/************* Media Attributes class methods ******************/

void
MediaAttributes::setAttribute (string& attrib)
{
    /** check for rtpmap here. The rest is taken care of by the
        parent class : SdpAttributes */
    /* a=rtpmap:...... */
    string::size_type pos;
    pos = attrib.find("rtpmap:");
    if (pos == 0)
    {
        //rtpmap found
        attrib.erase(0, strlen("rtpmap:"));
        split_t attribList(split(attrib, " "));
        if (attribList.size() < 2)
        {
            throw SdpExceptions(PARAM_NUMERR);
        }
        else
        {
            int payload = strtol(attribList[0].c_str(), (char**)NULL, 10);
            SdpRtpMapAttribute* rtpmapAttrib = new SdpRtpMapAttribute;
            assert(rtpmapAttrib);

            rtpmapAttrib->setPayloadType(payload);

            //split the second parameter
            string encodDetails = (attribList[1]);
            split_t encodList(split(encodDetails, "/"));

            rtpmapAttrib->setEncodingName(encodList[0].c_str());

            if (encodList.size() > 1)
            {
                int rate = strtol(encodList[1].c_str(), (char**)NULL, 10);
                rtpmapAttrib->setClockRate(rate);
            }
            if (encodList.size() > 2)
            {
                int encodParms = strtol(encodList[2].c_str(), (char**)NULL, 10);
                rtpmapAttrib->setEncodingParms(encodParms);
            }
            addmap(rtpmapAttrib);
        }
    }
    else
    {
        cpDebug(LOG_DEBUG_STACK, " %s :attribute is not an rtpmap", attrib.c_str());
        //call parent function.
        SdpAttributes::setAttribute(attrib);
    }
}

void
MediaAttributes::flushrtpmap ()
{
    vector < SdpRtpMapAttribute* > ::iterator iter;
    iter = rtpmap.begin();
    while (iter != rtpmap.end())
    {
        if (*iter)
        {
            delete (*iter);
        }
        ++iter;
    }
    rtpmap.clear();
}

MediaAttributes&
MediaAttributes::operator=(MediaAttributes& attrib)
{
    (this)->SdpAttributes::operator=(attrib);
    //copy objects to which rtpmap elements point to
    vector < SdpRtpMapAttribute* > * newrtpmap = attrib.getmap();
    assert(newrtpmap);

    if (newrtpmap->size() > 0)
    {
        cpDebug(LOG_DEBUG_STACK, "MediaAttributes: found rtpmap");
        vector < SdpRtpMapAttribute* > ::iterator iter;
        iter = newrtpmap->begin();
        if (rtpmap.size() > 0)
        {
            flushrtpmap();
        }
        while (iter != newrtpmap->end())
        {
            if (*iter)
            {
                //create an SdpRtpMapAttribute object and put in vector.
                SdpRtpMapAttribute* thisrtpmap = new SdpRtpMapAttribute();
                assert(thisrtpmap);
                *(thisrtpmap) = *(*iter);

                //assign to the vector.
                addmap(thisrtpmap);
            }
            ++iter;
        }
    }
    return *(this);
}

///
void
MediaAttributes::encode (ostrstream& s)
{
    vector < SdpRtpMapAttribute* > ::iterator iter = rtpmap.begin();
    while (iter != rtpmap.end())
    {
        (*iter)->encode (s);
        iter++;
    }
    SdpAttributes::encode (s);
}    // MediaAttributes::encode
