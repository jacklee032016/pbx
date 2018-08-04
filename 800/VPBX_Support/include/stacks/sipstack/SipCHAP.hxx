#ifndef SIPCHAP_H
#define SIPCHAP_H
/*
* $Log: SipCHAP.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipCHAP.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
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
