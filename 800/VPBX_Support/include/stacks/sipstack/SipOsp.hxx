#ifndef SIPOSP_H
#define SIPOSP_H

/*
* $Log: SipOsp.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipOsp.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
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
