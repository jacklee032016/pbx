#ifndef ImUrl_HXX
#define ImUrl_HXX
/*
 * $Id: ImUrl.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */

#include "ImpBaseUrl.hxx"

namespace Vocal
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
 
}// namespace Vocal

#endif
