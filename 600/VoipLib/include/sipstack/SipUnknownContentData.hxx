#ifndef SipUnknownContentData_HXX
#define SipUnknownContentData_HXX
/*
 * $Id: SipUnknownContentData.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */

#include "SipContentData.hxx"

namespace Vocal
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
 
} // namespace Vocal

#endif
