#ifndef SipTextData_HXX
#define SipTextData_HXX
/*
 * $Id: SipTextData.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */


#include "global.h"

#include "Data.hxx"
#include "SipContentData.hxx"

namespace Vocal
{
    
/// data container for an object of type text in the body of a SIP message.
class SipTextData : public SipContentData
{
    public:
        /// Create one with default values
        SipTextData(const Data& textData, const Data& type = "text");
        ///
        SipTextData(const SipTextData& src);
        ///
        SipTextData& operator =(const SipTextData&);

	///
	~SipTextData();
    
        /** encode the body of the ContentData and return it.  
	    @param len   returns the length in bytes of the encoded body.
	*/
        Data encodeBody(int& len);

	/// virtual function to duplicate the header	
	virtual SipContentData* duplicate() const;

    private:
        Data myTextData;
        Data _type;
       
};
 
} // namespace Vocal

#endif
