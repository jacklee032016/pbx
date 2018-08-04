#ifndef SIPOSP_H
#define SIPOSP_H
/*
 * $Id: SipOsp.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */

#include "global.h"

#include "SipContentData.hxx"
#include "Data.hxx"


namespace Vocal
{


    
///
class SipOsp : public SipContentData
{
    public:
        ///
        SipOsp();
        SipOsp(const SipOsp& src);
        ///
        const SipOsp& operator=( const SipOsp& src );
        ///
        ~SipOsp();
        ///
        SipOsp( const Data& data);
        ///
        void setData( const Data& data );
        ///
        Data getData();
        /** encode the body of the ContentData and return it.  
	    @param len   returns the length in bytes of the encoded body.
	*/
        Data encodeBody(int& len);
	/// virtual function to duplicate the header	
	virtual SipContentData* duplicate() const;
      
    private:
        Data ospData;
    
};

} // namespace Vocal

#endif
