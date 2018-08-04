#ifndef UNKNOWNURL_HXX_
#define UNKNOWNURL_HXX_
/*
 * $Id: UnknownUrl.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */

#include "BaseUrl.hxx"
#include "BaseUser.hxx"

namespace Vocal
{
    
/// data container for URLs of unknown type 
class UnknownUrl : public BaseUrl
{
    public:
	/// Create one with default values
	UnknownUrl();

	/// Create one with default values
	explicit UnknownUrl(const Data& data);


	/// Create one with default values
	UnknownUrl(const UnknownUrl& url);
    
	///
	~UnknownUrl();

        ///
        const UnknownUrl& operator=(const UnknownUrl& url);

        ///
        bool operator==(const UnknownUrl& url) const;

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
        Data myUrl;
};

 
} // namespace Vocal

#endif
