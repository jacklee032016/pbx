/*
* $Log: NullUser.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: NullUser.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "global.h"
#include "NullUser.hxx"

using namespace Assist;
    
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
