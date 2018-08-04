#ifndef NULLURL_HXX_
#define NULLURL_HXX_
/*
 * $Id: NullUrl.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */

#include "BaseUrl.hxx"
#include "BaseUser.hxx"

namespace Vocal
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
 
} // namespace Vocal

#endif
