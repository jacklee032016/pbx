#ifndef SipTextData_HXX
#define SipTextData_HXX
/*
* $Log: SipTextData.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:06:38  lizhijie
* no message
*
* $Id: SipTextData.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"

#include "Data.hxx"
#include "SipContentData.hxx"

namespace Assist
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
 
}

#endif
