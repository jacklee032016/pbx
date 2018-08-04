#ifndef SIPBASIC_H
#define SIPBASIC_H
/*
 * $Id: SipBasic.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */


#include "vmd5.h"
#include "Data.hxx"

namespace Vocal
{

class SipBasic
{
    public:
        SipBasic();
        ~SipBasic();
        Data formSIPBasic(const Data& user, const Data& pwd ) const;
};
 
} // namespace Vocal

#endif
