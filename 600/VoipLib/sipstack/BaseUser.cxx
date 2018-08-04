/*
 * $Id: BaseUser.cxx,v 1.3 2007/03/03 18:38:55 lizhijie Exp $
 */

#include "global.h"
#include "BaseUser.hxx"

using namespace Vocal;
    
BaseUser::BaseUser()
{
}
    
    
BaseUser::~BaseUser()
{
}
    

bool 
Vocal::operator<(const Sptr<BaseUser>& left, const Sptr<BaseUser>& right)
{
    if (left == 0 && right == 0) return false;
    else if (left == 0) return true;
    else return left->isLessThan(right);
}


