#ifndef SipFragData_HXX
#define SipFragData_HXX
/*
 * $Id: SipFragData.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */

#include "global.h"

#include "Data.hxx"
#include "SipContentData.hxx"

namespace Vocal
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
 
} // namespace Vocal

#endif
