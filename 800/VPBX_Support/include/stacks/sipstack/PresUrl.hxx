#ifndef PresUrl_HXX
#define PresUrl_HXX
/*
* $Log: PresUrl.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: PresUrl.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "ImpBaseUrl.hxx"

namespace Assist
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
 
}

#endif
