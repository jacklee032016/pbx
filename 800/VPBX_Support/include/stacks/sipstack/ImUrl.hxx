#ifndef ImUrl_HXX
#define ImUrl_HXX

/*
* $Log: ImUrl.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: ImUrl.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "ImpBaseUrl.hxx"

namespace Assist
{

/// data container for URLs of type im:
class ImUrl : public ImpBaseUrl
{
    public:
	/// Create one with default values
	ImUrl();
	///
	explicit ImUrl(const Data& data) throw(ImpBaseUrlParserException &) ;
	///
	ImUrl(const ImUrl& );
    
        ///
        Sptr<BaseUrl> duplicate() const ;
	///
	UrlType getType() const  { return IM_URL; };
};
 
}

#endif
