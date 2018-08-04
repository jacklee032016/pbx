#ifndef BaseUrl_HXX
#define BaseUrl_HXX

/*
* $Log: BaseUrl.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: BaseUrl.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/
    
#include "global.h"
#include "Data.hxx"
#include "Sptr.hxx"
#include "BaseUser.hxx"


namespace Assist
{
    
/// URL Types    
typedef enum
{
    SIP_URL,
    TEL_URL,
    IM_URL,
    PRES_URL,
    UNKNOWN_URL,
    NULL_URL
} UrlType;
 

/// class for Url types
class BaseUrl
{
    public:
	/// Create one with default values
	BaseUrl();
	///
	virtual ~BaseUrl();
	//parser functions
	static Sptr<BaseUrl> decode(const Data& data);
	///
	virtual Data encode() const = 0;
	///
	virtual Sptr<BaseUser> getUser() const = 0;
	///
	virtual Data getUserValue() const = 0;
	///
	virtual Data getUniqueKey() const = 0;
	///
	virtual void setUser(Sptr <BaseUser> ) = 0;
	///
	virtual bool setUserValue(const Data&, const Data& dummy = "") = 0;
	///
	virtual UrlType getType() const = 0;
        ///
        virtual bool isLooseRouterPresent() const { return false; }
	///
	virtual Sptr<BaseUrl> duplicate() const = 0;
	///
	virtual bool areEqual(Sptr<BaseUrl>) const = 0;
	///
	virtual bool isLessThan(Sptr<BaseUrl>) const = 0;
	/// comparison operator is used by others
	virtual bool areEqual(const BaseUrl& other) const = 0;

        virtual ostream& dump(ostream& s) const;
        
#if 0
	/// friend operator for equality
	friend bool operator==(const BaseUrl& left, const BaseUrl& right);
#endif
};


/** this method duplicates a URL.  if the second parameter is true, it
 * returns a NullUrl instead of NULL if the original is NULL or
 * NullUrl.  if the second parameter is false (default), it returns
 * NULL if the original is NULL or NullUrl.
 */

Sptr<BaseUrl> duplicateUrl(const Sptr<BaseUrl> original, 
			   bool returnNullUrl = false);

ostream&
operator<<(ostream& s, const Sptr<BaseUrl>& url);


// end of vocal namespace
}

#if !defined(__SUNPRO_CC)
#include <hash_map.h>
#if ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1) )
namespace __gnu_cxx
#else
namespace std
#endif
{
    struct hash< Sptr < Assist::BaseUrl > >
    {
        size_t operator() ( const Sptr < Assist::BaseUrl >& src ) const
        {
            return __stl_hash_string(src->encode().logData());
        }
    };
// end of either __gnu_cxx or std namespace
}

// end for __linux__
#endif

// end for BaseUrl_HXX
#endif
