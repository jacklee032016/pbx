/*
 * $Id: RetransmitContents.cxx,v 1.2 2007/03/01 20:04:31 lizhijie Exp $
 */


#include "global.h"
#include "RetransmitContents.hxx"

using namespace Vocal;

RetransmitContents::RetransmitContents()
        : sipMsg(0),
        retransmitt(0),
        timeToGo(0)
{}


RetransmitContents::RetransmitContents(SipMsgContainer* msg, int count)
        : sipMsg(msg),
        retransmitt(count),
        timeToGo(0)
{
}


RetransmitContents::~RetransmitContents()
{}

RetransmitContents::RetransmitContents(const RetransmitContents& src )
        : sipMsg(src.sipMsg),
        retransmitt(src.retransmitt),
        timeToGo(src.timeToGo)
{}
RetransmitContents&
RetransmitContents::operator=( const RetransmitContents& src )
{
    if ( &src != this )
    {
        sipMsg = src.sipMsg;
        retransmitt = src.retransmitt;
        timeToGo = src.timeToGo;
    }
    return *this;
}

void RetransmitContents::setMsg(SipMsgContainer* msg)
{
    sipMsg = msg;
}

SipMsgContainer* RetransmitContents::getMsg() const
{
    return sipMsg;
}


void RetransmitContents::setCount(const int i)
{
    retransmitt = i;
}

int RetransmitContents::getCount() const
{
    return retransmitt;
}


int RetransmitContents::getTimeToGo() const
{
    return timeToGo;
}

void RetransmitContents::setTimeToGo(const int newtimeToGo)
{
    timeToGo = newtimeToGo;
}
