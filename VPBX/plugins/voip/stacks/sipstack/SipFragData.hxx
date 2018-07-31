#ifndef SipFragData_HXX
#define SipFragData_HXX
/*
* $Log: SipFragData.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:06:38  lizhijie
* no message
*
* $Id: SipFragData.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"

#include "Data.hxx"
#include "SipContentData.hxx"

namespace Assist
{
    
/// data container for FragData header
class SipFragData : public SipContentData
{
    public:
        /// Create one with default values
        SipFragData(const Data& fragData);
        ///
        SipFragData(const SipFragData& src);
        ///
        SipFragData& operator =(const SipFragData&);

	///
	~SipFragData();
    
        /** encode the body of the ContentData and return it.  
	    @param len   returns the length in bytes of the encoded body.
	*/
        Data encodeBody(int& len);

	/// virtual function to duplicate the header	
	virtual SipContentData* duplicate() const;

    private:
        Data myFragData;
       
};
 
}

#endif
