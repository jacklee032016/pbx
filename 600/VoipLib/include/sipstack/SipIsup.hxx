#ifndef SIPISUP_H
#define SIPISUP_H
/*
 * $Id: SipIsup.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */

#include "SipContentData.hxx"
#include "global.h"
#include "Data.hxx"

namespace Vocal
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
 
} // namespace Vocal

#endif
