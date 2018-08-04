#ifndef SIPCHAP_H
#define SIPCHAP_H
/*
* $Log: SipCHAP.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:06:38  lizhijie
* no message
*
* $Id: SipCHAP.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "Data.hxx"

namespace Assist
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
 
}

#endif
