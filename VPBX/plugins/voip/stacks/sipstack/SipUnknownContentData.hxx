#ifndef SipUnknownContentData_HXX
#define SipUnknownContentData_HXX
/*
* $Log: SipUnknownContentData.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:06:38  lizhijie
* no message
*
* $Id: SipUnknownContentData.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "SipContentData.hxx"

namespace Assist
{
    
/// data container for content bodies in a SIP message of unknown type.
class SipUnknownContentData : public SipContentData
{
    public:
        /// Create one with default values
        SipUnknownContentData();
        /// Create one passing it an unknown value
        SipUnknownContentData(const Data& data);
        ///
        SipUnknownContentData( const SipUnknownContentData& rhs );

        ///
        SipUnknownContentData& operator=( const SipUnknownContentData& rhs );

        ///
        virtual ~SipUnknownContentData();

        /** encode the body of the ContentData and return it.  
	    @param len   returns the length in bytes of the encoded body.
	*/
        Data encodeBody(int& len);

	/// virtual function to duplicate the header
	virtual SipContentData* duplicate() const;
    private:
	Data myBody;
};
 
} 

#endif
