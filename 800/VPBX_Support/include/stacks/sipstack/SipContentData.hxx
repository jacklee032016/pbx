#ifndef SipContentData_HXX
#define SipContentData_HXX

/*
* $Log: SipContentData.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipContentData.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "Data.hxx"
#include "SipContentType.hxx"
#include "SipContentLength.hxx"
#include "SipContentDisposition.hxx"

namespace Assist
{

/// data container for ContentLength header
class SipContentData
{
    public:
        /// Create one with default values
        SipContentData();
        ///
        Data encodeHeaders() const;

        /** encode the body of the ContentData and return it.  
	    @param len   returns the length in bytes of the encoded body.
	*/
        virtual Data encodeBody(int& len) = 0;
        ///
        virtual ~SipContentData();
        ///
        SipContentData( const SipContentData& rhs );
        ///
        SipContentData& operator=( const SipContentData& rhs );
        ///
        void setContentType(Sptr <SipContentType>);
        ///
        Sptr <SipContentType> getContentType();
    
        ///
        void setContentLength(Sptr <SipContentLength>);
        ///
        Sptr<SipContentLength> getContentLength();
    
        ///
        void setContentDisposition(Sptr <SipContentDisposition>);
        ///
        Sptr<SipContentDisposition> getContentDisposition();

	/// virtual function to duplicate the header
	virtual SipContentData* duplicate() const = 0;

        /// xxx need to implement equality operator?
        
    private:

        Sptr<SipContentType>  contentType;
        Sptr<SipContentLength> contentLength;
        Sptr<SipContentDisposition> contentDisposition;
       
};
 
}

#endif
