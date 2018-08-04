/*
* $Log: NullUrl.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: NullUrl.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "global.h"
#include "Data.hxx"
#include "cpLog.h"
#include "NullUrl.hxx"
#include "NullUser.hxx"

using namespace Assist;
    
NullUrl::NullUrl()
{
}
    
    
NullUrl::~NullUrl()
{
}


Data 
NullUrl::encode() const
{
    return "";
}


Sptr<BaseUser> 
NullUrl::getUser() const
{
    return new NullUser;
}


Data 
NullUrl::getUserValue() const
{
    return "";
}

Data 
NullUrl::getUniqueKey() const
{
    return "null-url";
}

void 
NullUrl::setUser(Sptr <BaseUser> )
{
    // does nothing
}

bool
NullUrl::setUserValue(const Data&, const Data& dummy)
{
    // does nothing
    return true;
}

UrlType
NullUrl::getType() const
{
    return NULL_URL;
}

Sptr<BaseUrl> 
NullUrl::duplicate() const
{
    return new NullUrl;
}

bool 
NullUrl::areEqual(Sptr<BaseUrl> other) const
{
    if(other != 0)
    {        
        return (other->getType() == NULL_URL);
    }
    else
    {
        return false;
    }
}

bool
NullUrl::isLessThan(Sptr<BaseUrl> other) const
{
    // the Null URL is less than anything other than itself
    if (other != 0 && other->getType() == NULL_URL)
    {
	return false;
    }
    else
    {
	return true;
    }
}

bool 
NullUrl::areEqual(const BaseUrl& other) const
{
    return (other.getType() == NULL_URL);
}
