#ifndef SIPISUP_H
#define SIPISUP_H

/*
* $Log: SipIsup.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:06:38  lizhijie
* no message
*
* $Id: SipIsup.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "SipContentData.hxx"
#include "global.h"
#include "Data.hxx"

namespace Assist
{


///
class SipIsup : public SipContentData
{
    
    public:
	///
	
	SipIsup();
	
	
	SipIsup(const Data& data);
	
	
	SipIsup( const SipIsup& src );
	
	
	const SipIsup& operator=( const SipIsup& src );
	
	
	~SipIsup();
	
	void  setData( const Data& data );
	
	Data  getData();
	
	Data  encodeBody(int&);

	/// virtual function to duplicate the header	
	virtual SipContentData* duplicate() const;
    
    
private:
    
    Data isupData;
};
 
}

#endif
