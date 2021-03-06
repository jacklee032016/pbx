#ifndef SIP_HEADER_LIST_HXX_
#define SIP_HEADER_LIST_HXX_
/*
 * $Id: SipHeaderList.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */

#include "SipRawHeader.hxx"
#include "Sptr.hxx"

namespace Vocal
{


/** this class contains a list of headers. */
template <class Header>
class SipHeaderList
{
    public:
	class iterator {
	    public:
		iterator() : myIterator()
		{}
		iterator(Sptr<SipRawHeader> src) : myIterator(src)
		{}
		iterator(const iterator& src) : myIterator(src.myIterator)
		{}
		~iterator()
		{}
		const iterator& operator=(const iterator& src)
		{
		    if(this != &src)
		    {
			myIterator = src.myIterator;
		    }
		    return *this;
		}
		const iterator& operator++() 
		{
		    if(myIterator != 0)
		    {
			myIterator = myIterator->next;
		    }
		    return *this;
		}
		iterator operator++(int) 
		{
		    iterator tmp = *this;
		    if(myIterator != 0)
		    {
			myIterator = myIterator->next;
		    }
		    return tmp;
		}
		Sptr <Header> operator*()
		{
		    myIterator->parse();
		    Sptr<Header> x;
		    x.dynamicCast(myIterator->header);
		    return x;
		}

		bool operator==(const iterator& orig) const
		{
		    return (myIterator == orig.myIterator);
		}

		bool operator!=(const iterator& orig) const
		{
		    return (myIterator != orig.myIterator);
		}

	    private:
		Sptr <SipRawHeader> myIterator;
	};


	/// default constructor
	SipHeaderList();

	/// constructor from RawHeader
	SipHeaderList(Sptr<SipRawHeader> orig);
	/// copy constructor
	SipHeaderList(const SipHeaderList& src);
	/// assignment operator
	const SipHeaderList& operator= (const SipHeaderList& src);

	iterator begin() const
	{
	    return myHeader;
	}

	iterator end() const
	{
	    return iterator(0);
	}

	bool empty() const
	{
	    return (myHeader == 0);
	}


    private:

	Sptr<SipRawHeader> myHeader;
};


template <class Header>
inline
SipHeaderList<Header>::SipHeaderList()
{
}

template <class Header>
inline
SipHeaderList<Header>::SipHeaderList(Sptr<SipRawHeader> orig)
    : myHeader(orig)
{
}


template <class Header>
inline
SipHeaderList<Header>::SipHeaderList (const SipHeaderList& src)
    : myHeader(src.myHeader)
{
}


template <class Header>
inline 
const SipHeaderList<Header>& 
SipHeaderList<Header>::operator= (const SipHeaderList& src)
{
    if(&src != this)
    {
	myHeader = src.myHeader;
    }
    return *this;
}
 
} // namespace Vocal

#endif
