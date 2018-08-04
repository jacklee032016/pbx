#ifndef NULLURL_HXX_
#define NULLURL_HXX_
/*
* $Log: NullUrl.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: NullUrl.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "BaseUrl.hxx"
#include "BaseUser.hxx"

namespace Assist
{

/// data container for Url header
class NullUrl : public BaseUrl
{
    public:
	/// Create one with default values
	NullUrl();
    
	///
	~NullUrl();

	///
	virtual Data encode() const;
	///
	virtual Sptr<BaseUser> getUser() const;
	///
	virtual Data getUserValue() const;
	///
	virtual Data getUniqueKey() const;
	///
	virtual void setUser(Sptr <BaseUser> );
	///
	virtual bool setUserValue(const Data&, const Data& dummy = "");
	///
	virtual UrlType getType() const;
	///
	virtual Sptr<BaseUrl> duplicate() const;
	///
	virtual bool areEqual(Sptr<BaseUrl>) const;
	///
	virtual bool isLessThan(Sptr<BaseUrl>) const;
	/// comparison operator is used by others
	virtual bool areEqual(const BaseUrl& other) const;
    private:
};
 
}

#endif
