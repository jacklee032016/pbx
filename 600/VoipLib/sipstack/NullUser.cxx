/*
 * $Id: NullUser.cxx,v 1.2 2007/03/01 20:04:31 lizhijie Exp $
 */

#include "global.h"
#include "NullUser.hxx"

using namespace Vocal;
    
NullUser::NullUser()
{
}
    
    
NullUser::~NullUser()
{
}
    

bool NullUser::decode(const Data& data)
{
    return true;
}


Data NullUser::encode() const
{
    return "";
}


Sptr<BaseUser> 
NullUser::duplicate() const
{
    return new NullUser;
}


UserType 
NullUser::getType() const
{
    return NULL_USER;
}


bool 
NullUser::areEqual(Sptr<BaseUser> other) const
{
    if(other->getType() == NULL_USER)
    {
	return true;
    }
    else
    {
	return false;
    }
}


bool 
NullUser::isLessThan(Sptr<BaseUser> other) const
{
    if(other->getType() == NULL_USER)
    {
	return false;
    }
    else
    {
	return true;
    }
}
