#ifndef PresUrl_HXX
#define PresUrl_HXX

/*
 * $Id: PresUrl.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */

#include "ImpBaseUrl.hxx"

namespace Vocal
{

/// data container for Presence Url header
class PresUrl : public ImpBaseUrl
{
    public:
	/// Create one with default values
	PresUrl();
	///
	PresUrl(const Data& data) throw(ImpBaseUrlParserException &) ;
	///
	PresUrl(const PresUrl& );
    
        ///
        Sptr<BaseUrl> duplicate() const ;
	///
	UrlType getType() const  { return PRES_URL; };
};
 
} // namespace Vocal

#endif
