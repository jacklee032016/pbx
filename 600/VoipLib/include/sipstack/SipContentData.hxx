#ifndef SipContentData_HXX
#define SipContentData_HXX
/*
 * $Id: SipContentData.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */

#include "Data.hxx"
#include "SipContentType.hxx"
#include "SipContentLength.hxx"
#include "SipContentDisposition.hxx"

namespace Vocal
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
 
} // namespace Vocal

#endif
