/*
* $Log: BaseUser.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: BaseUser.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "global.h"
#include "BaseUser.hxx"

using namespace Assist;
    
BaseUser::BaseUser()
{
}
    
    
BaseUser::~BaseUser()
{
}
    

bool 
Assist::operator<(const Sptr<BaseUser>& left, const Sptr<BaseUser>& right)
{
    if (left == 0 && right == 0) return false;
    else if (left == 0) return true;
    else return left->isLessThan(right);
}
