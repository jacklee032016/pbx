#ifndef SIPBASIC_H
#define SIPBASIC_H
/*
* $Log: SipBasic.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:06:38  lizhijie
* no message
*
* $Id: SipBasic.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
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
