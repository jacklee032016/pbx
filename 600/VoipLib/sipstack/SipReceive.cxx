/*
 * $Id: SipReceive.cxx,v 1.2 2007/03/01 20:04:31 lizhijie Exp $
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
