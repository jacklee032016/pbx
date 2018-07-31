#ifndef SIPOSP_H
#define SIPOSP_H

/*
* $Log: SipOsp.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:06:38  lizhijie
* no message
*
* $Id: SipOsp.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"

#include "SipContentData.hxx"
#include "Data.hxx"

namespace Assist
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

}

#endif
