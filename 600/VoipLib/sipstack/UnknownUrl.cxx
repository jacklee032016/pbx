/*
 * $Id: UnknownUrl.cxx,v 1.2 2007/03/01 20:04:32 lizhijie Exp $
 */

#include "global.h"
#include "Data.hxx"
#include "cpLog.h"
#include "UnknownUrl.hxx"
#include "NullUser.hxx"

    
using namespace Vocal;

UnknownUrl::UnknownUrl()
{
}


UnknownUrl::UnknownUrl(const Data& data)
    :myUrl(data)
{
}


UnknownUrl::UnknownUrl(const UnknownUrl& url)
    :myUrl(url.myUrl)
{
}
    
    
UnknownUrl::~UnknownUrl()
{
}


const UnknownUrl&
UnknownUrl::operator=(const UnknownUrl& url)
{
    if(&url != this)
    {
	myUrl = url.myUrl;
    }
    return *this;
}


bool
UnknownUrl::operator==(const UnknownUrl& url) const
{
    return myUrl == url.myUrl;
}


Data 
UnknownUrl::encode() const
{
    return myUrl;
}


Sptr<BaseUser> 
UnknownUrl::getUser() const
{
    return new NullUser;
}


Data 
UnknownUrl::getUserValue() const
{
    return "";
}

Data 
UnknownUrl::getUniqueKey() const
{
    return myUrl;
}

void 
UnknownUrl::setUser(Sptr <BaseUser> )
{
    // does nothing
}

bool
UnknownUrl::setUserValue(const Data&, const Data& dummy)
{
    // does nothing
    return true;
}

UrlType
UnknownUrl::getType() const
{
    return UNKNOWN_URL;
}

Sptr<BaseUrl> 
UnknownUrl::duplicate() const
{
    return new UnknownUrl;
}

bool 
UnknownUrl::areEqual(Sptr<BaseUrl> other) const
{

    return 
	(other != 0) &&  
	(other->getType() == UNKNOWN_URL) && 
	(myUrl == other->getUniqueKey()) ;
}

bool
UnknownUrl::isLessThan(Sptr<BaseUrl> other) const
{
    if(other == 0)
    {
	return false;
    }

    // the Null URL is less than anything other than itself
    if (other->getType() == NULL_URL)
    {
	return false;
    }
    else
    {
	return myUrl < other->getUniqueKey();
    }
}

bool 
UnknownUrl::areEqual(const BaseUrl& other) const
{
    return ((other.getType() == UNKNOWN_URL) &&
	    (myUrl == other.getUniqueKey()));
}
