/*
* $Log: SipReceive.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipReceive.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "SipMsg.hxx"
#include "SipReceive.hxx"
#include "cpLog.h"

SipReceive::SipReceive()
{}



SipReceive::~SipReceive()
{}


Sptr < SipReceive > SipReceive::getPrevMsg()
{
    return prevMsg;
}


Sptr < SipMsg > SipReceive::getCurMsg()
{
    return curMsg;
}

void SipReceive::setPrevMsg(Sptr < SipReceive > prev)
{
    prevMsg = prev;
}

void SipReceive::setCurMsg(Sptr < SipMsg > cur)
{
    curMsg = cur;
}
