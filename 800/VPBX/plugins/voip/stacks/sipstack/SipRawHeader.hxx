#ifndef SIP_RAW_HEADER_HXX_
#define SIP_RAW_HEADER_HXX_
/*
* $Log: SipRawHeader.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:06:38  lizhijie
* no message
*
* $Id: SipRawHeader.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "SipHeader.hxx"
#include <set>
#include "Sptr.hxx"

namespace Assist
{


/** this is the container inside of SipMsg which holds a single
 * header, both in raw form (without parsing), and if the parsing has
 * been done, as a SipHeader object.
*/

class SipRawHeader
{
    public:
	SipRawHeader();
	SipRawHeader(const SipRawHeader& src);

	Data headerName;
	Data headerNameOriginal;
	Data headerValue;
	SipHeaderType headerType;
	mutable Sptr<SipHeader> header;
	mutable Sptr<SipRawHeader> next;

        /// deep copy from src to this 
        void deepCopy(const SipRawHeader& src);
        /// duplicate this (deep copying) and return a ptr to the copy
        SipRawHeader* duplicate() const;
        /// compare whether these two are equal (deep comparison)
	bool operator==(const SipRawHeader& src) const;
        /// compare whether these two are not equal (deep comparison)
	bool operator!=(const SipRawHeader& src) const;

	/// decode the header
	bool decode(const Data& headerData);
	/// encode this header (deep encoding)
	bool encode(Data* msg) const;

        /// parse whatever is currently in this header
        bool parse() const;
        /** parse whatever is currently in this header and all other
            linked headers */
        bool parseDeep() const;
        /// shallow comparison only (the operator== is deep)
        bool shallowEquals(const SipRawHeader& src) const;
        /** shallow comparison only against a theoretical NUL ptr the
            NUL ptr stands in for an empty SipRawHeader of the proper
            type, which encodes to no text
        */
        bool shallowEqualsNul() const;
}; 

inline SipRawHeader::SipRawHeader()
    : headerName(),
      headerNameOriginal(),
      headerValue(),
      headerType(SIP_UNKNOWN_HDR),
      header()
{
}

inline SipRawHeader::SipRawHeader(const SipRawHeader& src)
    : headerName(src.headerName),
      headerNameOriginal(src.headerNameOriginal),
      headerValue(src.headerValue),
      headerType(src.headerType),
      header(src.header)
{
}
 
} 

#endif
