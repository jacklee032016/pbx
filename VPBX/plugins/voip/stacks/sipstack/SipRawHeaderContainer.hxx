#ifndef SIP_RAW_HEADER_CONTAINER_HXX_
#define SIP_RAW_HEADER_CONTAINER_HXX_
/*
* $Log: SipRawHeaderContainer.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:06:38  lizhijie
* no message
*
* $Id: SipRawHeaderContainer.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include <set>
#include <vector>

#include "SipRawHeader.hxx"
#include "SipHeaderList.hxx"

namespace Assist
{


/**

   this object is the container which holds all of the headers of a
   SipMsg.  It is a vector of Headers, where the index of the vector
   is indexed by the SipHeaderType.
   <p>

   You should only need to fiddle with this if you are changing how
   SipMsg works.

   <p>
   
*/
class SipRawHeaderContainer : public std::vector < Sptr<SipRawHeader> >
{
    public:
	/// constructor
	SipRawHeaderContainer();
        /// destructor
        ~SipRawHeaderContainer();
        /// modify the copy constructor to deep copy by default
        SipRawHeaderContainer(const SipRawHeaderContainer& src);

        /// assignment operator
        const SipRawHeaderContainer& operator=(const SipRawHeaderContainer& src);
        /// equality operator -- slow but necessary
        bool operator==(const SipRawHeaderContainer& src) const;
        /// inequality operator -- slow but necessary
        bool operator!=(const SipRawHeaderContainer& src) const;

	/// insert a header
	void insertManyHeader(Sptr<SipRawHeader> header);
	/// insert a header
	void insertHeader(Sptr<SipRawHeader> header, int pos=-1);
	/// get a header by type
	Sptr<SipRawHeader> getHeader(SipHeaderType type, int index=0) const;
	/// replace a header with another
	void replaceHeader(SipHeaderType type, Sptr<SipRawHeader> header);
        /// get a header or create one
        Sptr<SipRawHeader> getHeaderOrCreate(SipHeaderType type, int index=0) const;

	/// decode a series of headers passed in in parseData
	bool decode(Data* headers);
	/// encode a series of headers passed in in parseData
	bool encode(Data* msg) const;

        /// mask out certain headers
        void mask(std::set <SipHeaderType>& mask );

        /// get the parsed header by modifying p (the smart pointer to the header)
        template <class HeaderType> void
        getParsedHeader( 
            Sptr<HeaderType>& p,
            SipHeaderType type, 
            int index = 0) const
        {
            Sptr<SipRawHeader> rawHeader = getHeaderOrCreate(type, index);

            rawHeader->parse();
    
            p.dynamicCast(rawHeader->header);
            assert((0 != p));
        }

        /// get a list of headers of a single type (in a SipHeaderList container)
        template <class HeaderType> void
        getHeaderList(SipHeaderList<HeaderType>* p, SipHeaderType type) const
        {
            Sptr<SipRawHeader> rawHeader = getHeader(type);
            *p = SipHeaderList<HeaderType>(rawHeader);
        }

        /// return the number of headers of a given type
        int getNumHeaders( SipHeaderType type ) const;

        /// remove the nth header (coalescing the list)
        void removeHeader ( SipHeaderType type, int index=-1 /* last element */ );

        /// set the number of items (coalescing the list)
        void setNumHeaders ( SipHeaderType type, int number );

        /// add a header at a given position
        void appendHeader ( SipHeaderType type, const Sptr<SipHeader> p, int pos=-1 );

        /// force all headers to be parsed
        void forceParse();
};



template <class HeaderType> HeaderType&
getParsed( 
    const SipRawHeaderContainer& container,
    SipHeaderType type, 
    int index = 0 )
{
    Sptr<HeaderType> p;
    container.getParsedHeader(p, type, index);
    return *p;
}
 
} 

#endif
