/*
* $Log: SipUser.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:10  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipUser.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/
#include "global.h"
#include "SipUser.hxx"

    
using namespace Assist;

SipUser::SipUser()
{
}
    
    
SipUser::~SipUser()
{
}

SipUser::SipUser(const SipUser & src)
    :   BaseUser(src),
        user(src.user)
{
}

const SipUser &
SipUser::operator=(const SipUser & src)
{
    if ( this != &src )
    {
        user = src.user;
    }
    return ( *this );
}
    
SipUser::SipUser(const Data& data)
{
    user = data;
}
    
UserType
SipUser::getType() const
{
    return SIP_USER;
}
    
    
    
bool
SipUser::decode(const Data& data)
{
    //*(unreserved | escaped | user-unreserved )
    user = data;
    return true; // this should be based on parsing user.
}
    
    
Data
SipUser::encode() const
{
    return user;
}

    
Sptr<BaseUser> 
SipUser::duplicate() const
{
    return new SipUser(*this);
}

    
bool
SipUser::operator == (const SipUser& src) const
{
    return (user == src.user);
}
    
    
bool
SipUser::operator < (const SipUser& src) const
{
    return (user < src.user);
}
    

bool SipUser::areEqual(Sptr<BaseUser> baseUser) const
{
    if (baseUser != 0 && baseUser->getType() == SIP_USER)
    {
        Sptr<SipUser> newUser;
        newUser.dynamicCast(baseUser);
        return ( *(this) == *(newUser) );
    }
    else
    {
	return false;
    }
}
    
    
bool SipUser::isLessThan(Sptr<BaseUser> baseUser) const
{
    if (baseUser != 0 && baseUser->getType() == SIP_USER)
    {
        Sptr<SipUser> newUser;
        newUser.dynamicCast(baseUser);
        return ( *(this) < *(newUser) );
    }
    else
    {
	return false;
    }
}
