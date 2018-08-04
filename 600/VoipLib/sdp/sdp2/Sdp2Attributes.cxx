/*
 * $Id: Sdp2Attributes.cxx,v 1.3 2007/06/05 11:33:25 lizhijie Exp $
 */

#include "global.h"
#include <list>

#include "support.hxx"
#include "Sdp2Exceptions.hxx"
#include "Sdp2Attributes.hxx"


using Vocal::SDP::SdpAttributes;
using Vocal::SDP::SdpRtpMapAttribute;
using Vocal::SDP::ValueAttribute;
using Vocal::SDP::MediaAttributes;

/******************** SdpAttributes class methods ***********************/
bool SdpAttributes::isUserDefined (const char* str)
{
	if (strlen(str) < 2)
		return false;

	if ( (*str == 'X') && (*(str++) == '-') )
		return true;
	else
		return false;
}


/* Parse s and set individual values.
   we are sure we dont get the rtpmap Data here, since this is
   called from the Session part, i.e. rtpmap attribute is set
   only within the media attribute 
*/
void SdpAttributes::setAttribute (Data& s)
{
	/* the line can be of the form 
	a=<attribute> 			i.e. Property Attribute OR 
	a=<attribute>:<value> 		i.e. Value Attribute	OR 
	a=<userdefined attr>:<value>	i.e. User-defined Attribute
	*/

	cpDebug(LOG_DEBUG_STACK, "SdpAttributes: %s", s.logData());

	list<Data> attributeList;

	bool finished = false;

	while (!finished)
	{
		Data x = s.parse(":", &finished);
		if(finished)
		{
			x = s;
		}
		attributeList.push_back(x);
	}

	if (attributeList.size() < 1)
	{
		delete(&attributeList);
		cpLog(LOG_ERR, "Attribute: param count < 1 on attributeList");
		throw SdpExceptions(PARAM_NUMERR);
	}
	else if (attributeList.size() == 1)	// Property Attribute
	{
		Data x = *attributeList.begin();
		//	Data y = *(++attributeList.begin());
		if (x == "recvonly")
		{
			setrecvonly();
		}
		else if (x == "sendrecv")
		{
			setsendrecv();
		}
		else if (x == "sendonly")
		{
			setsendonly();
		}
	}
	else if ( attributeList.size() == 2 ) // Value or User-defined attribute
	{                                       
		Data x = *attributeList.begin();
		Data y = *(++attributeList.begin());


		cpDebug(LOG_DEBUG_STACK, "Attrib, value: %s, %s",
		x.logData(),
		y.logData());


		ValueAttribute* valueAttribute = new ValueAttribute;
		assert (valueAttribute);

		LocalScopeAllocator lAttr;
		LocalScopeAllocator lVal;

		valueAttribute->setAttribute (x.getData(lAttr));
		valueAttribute->setValue (y.getData(lVal));

		addValueAttribute (valueAttribute);
	}
	else if ( attributeList.size() > 2 ) // Assume a value attribute but the value Data has : in it
	{
		list<Data>::iterator iter;
		iter = attributeList.begin();

		Data initial = *iter;

		Data value = *(++iter);

		++iter;

		for ( ; iter != attributeList.end(); iter++)
		{
			value += ":";
			value += *iter;
		}

		cpDebug(LOG_DEBUG_STACK, "Attrib, value: %s, %s",
		initial.logData(),
		value.logData());

		ValueAttribute* valueAttribute = new ValueAttribute;
		assert (valueAttribute);

		LocalScopeAllocator lAttr;
		LocalScopeAllocator lVal;

		valueAttribute->setAttribute (initial.getData(lAttr));
		valueAttribute->setValue (value.getData(lVal));

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

void SdpAttributes::setrecvonly ()
{
	recvonly = true;
}

void SdpAttributes::setsendrecv ()
{
	sendrecv = true;
}

void SdpAttributes::setsendonly ()
{
	sendonly = true;
}


bool SdpAttributes::getrecvonly ()
{
	return recvonly;
}

bool SdpAttributes::getsendrecv ()
{
	return sendrecv;
}

bool SdpAttributes::getsendonly ()
{
	return sendonly;
}

void SdpAttributes::flushValueAttributes ()
{
	vector <ValueAttribute*> ::iterator iter;
	for( iter = valueAttributes.begin(); iter != valueAttributes.end() ; ++iter)
	{
		if (*iter)
		{
			delete (*iter);
		}
	}
	valueAttributes.clear();
}

const SdpAttributes& SdpAttributes::operator=(SdpAttributes& attrib)
{
	recvonly = attrib.getrecvonly();
	sendonly = attrib.getsendonly();
	sendrecv = attrib.getsendrecv();
	//copy user defined attributes from attrib to this.
	//copy individual members.
	copyValueAttributes(attrib);

	return *(this);
}


void SdpAttributes::encode (ostrstream& s)
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


void SdpAttributes::copyValueAttributes(const SdpAttributes& attrib)
{
	const vector < ValueAttribute* > * newValueAttributes = &attrib.valueAttributes;
	assert(newValueAttributes);

	flushValueAttributes();

	vector < ValueAttribute* > ::const_iterator iter;
	for(iter = newValueAttributes->begin();iter != newValueAttributes->end();++iter)
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


void SdpRtpMapAttribute::setPayloadType (int payload)
{
	payloadType = payload;
}

int SdpRtpMapAttribute::getPayloadType ()
{
	return payloadType;
}

void SdpRtpMapAttribute::setEncodingName (const char* name)
{
	strncpy(encodingName, name, encodingNameLen);
	encodingName[encodingNameLen-1] = '\0';
}

char* SdpRtpMapAttribute::getEncodingName ()
{
	return encodingName;
}

void SdpRtpMapAttribute::setClockRate (int rate)
{
	clockrate = rate;
}

int SdpRtpMapAttribute::getClockRate ()
{
	return clockrate;
}

void SdpRtpMapAttribute::setEncodingParms (int parms)
{
	encodingParms = parms;
}

int SdpRtpMapAttribute::getEncodingParms ()
{
	return encodingParms;
}

void SdpRtpMapAttribute::encode (ostrstream& s)
{
	cpDebug(LOG_DEBUG_STACK, "a=rtpmap:%d %s/%d", payloadType,  encodingName, clockrate);

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
		cpDebug(LOG_DEBUG_STACK, "/%d", encodingParms);
		s << '/' << encodingParms;
	}
	s << "\r\n";
}    // SdpRtpMapAttribute::encode

/********************** Value Attribute Class Methods *******************/
ValueAttribute::ValueAttribute ()
{
}

char* ValueAttribute::getAttribute ()
{
	return attribute;
}

void ValueAttribute::setAttribute (const char* attrib)
{
	strncpy(attribute, attrib, 255);
	attribute[255] = '\0'; // prevent runoff
}

char* ValueAttribute::getValue ()
{
	return value;
}

void ValueAttribute::setValue (const char* val)
{
	strncpy(value, val, 1255);
	value[1255] = '\0'; // prevent runoff
}

void ValueAttribute::encode (ostrstream& s)
{
	s << "a=" << attribute << ':' << value << "\r\n";
}    // ValueAttribute::encode


/************* Media Attributes class methods ******************/
void MediaAttributes::setAttribute (Data& attrib)
{
	/** check for rtpmap here. The rest is taken care of by the
	parent class : SdpAttributes */
	/* a=rtpmap:...... */
	int pos;
	Data before;
	pos = attrib.match("rtpmap:", &before, true);
	if (pos == FIRST || pos == FOUND)
	{
		//rtpmap found
		list<Data> attribList;

		bool finished = false;

		while (!finished)
		{
			Data x = attrib.parse(" ", &finished);
			if(finished)
			{
				x = attrib;
			}
			attribList.push_back(x);
		}


		if (attribList.size() < 2)
		{
			throw SdpExceptions(PARAM_NUMERR);
		}
		else
		{
			list<Data>::iterator i = attribList.begin();
			Data x0 = *i;
			Data x1 = *(++i);

			LocalScopeAllocator lPay;

			int payload = strtol(x0.getData(lPay), (char**)NULL, 10);
			SdpRtpMapAttribute* rtpmapAttrib = new SdpRtpMapAttribute;
			assert(rtpmapAttrib);

			rtpmapAttrib->setPayloadType(payload);

			//split the second parameter
			Data encodDetails = (x1);
			list<Data> encodList;

			finished = false;

			while (!finished)
			{
				Data x = encodDetails.parse("/", &finished);
				if(finished)
				{
					x = encodDetails;
				}
				encodList.push_back(x);
			}

			list<Data>::iterator encoder = encodList.begin();

			Data e0 = *(encoder);

			LocalScopeAllocator lEnc;

			rtpmapAttrib->setEncodingName(e0.getData(lEnc));

			if (encodList.size() > 1)
			{
				Data e1 = *(++encoder);
				LocalScopeAllocator lRate;
				int rate = strtol(e1.getData(lRate), (char**)NULL, 10);
				rtpmapAttrib->setClockRate(rate);
			}
			
			if (encodList.size() > 2)
			{
				Data e2 = *(++encoder);
				LocalScopeAllocator lRate;

				int encodParms = strtol(e2.getData(lRate), (char**)NULL, 10);
				rtpmapAttrib->setEncodingParms(encodParms);
			}
			addmap(rtpmapAttrib);
		}
	}
	else
	{
		cpDebug(LOG_DEBUG_STACK, " %s :attribute is not an rtpmap", attrib.logData());
		//call parent function.
		SdpAttributes::setAttribute(attrib);
	}
}

void MediaAttributes::flushrtpmap ()
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

MediaAttributes& MediaAttributes::operator=(MediaAttributes& attrib)
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
void MediaAttributes::encode (ostrstream& s)
{
	vector < SdpRtpMapAttribute* > ::iterator iter = rtpmap.begin();
	while (iter != rtpmap.end())
	{
		(*iter)->encode (s);
		iter++;
	}
	SdpAttributes::encode (s);
} 

