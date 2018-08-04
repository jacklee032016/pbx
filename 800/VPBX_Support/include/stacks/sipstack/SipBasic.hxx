#ifndef SIPBASIC_H
#define SIPBASIC_H
/*
* $Log: SipBasic.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipBasic.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "vmd5.h"
#include "Data.hxx"

namespace Assist
{

class SipBasic
{
    public:
        SipBasic();
        ~SipBasic();
        Data formSIPBasic(const Data& user, const Data& pwd ) const;
};
 
} 

#endif
