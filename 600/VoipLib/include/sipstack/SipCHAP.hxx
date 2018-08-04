#ifndef SIPCHAP_H
#define SIPCHAP_H

/*
 * $Id: SipCHAP.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */

#include "Data.hxx"

namespace Vocal
{

///
class SipCHAP
{
    public:
        ///
        static Data form_CHAP(const Data& id,
                            const Data& pwd,
                            const Data& challenge);
};
 
} // namespace Vocal

#endif
