/*
* $Log: SipUdpConnection.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:10  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipUdpConnection.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "global.h"
#include "SipUdpConnection.hxx"
#include "SipUdp_impl.hxx"

using namespace Assist;

SipUdpConnection::SipUdpConnection(Fifo <SipMsgContainer *> *fifo, int port)
{
    impl_ = new SipUdp_impl(fifo, port);
}

SipUdpConnection::~SipUdpConnection()
{
    delete impl_;
}


void
SipUdpConnection::send(SipMsgContainer* msg, const Data& host,
                       const Data& port)
{
    impl_->send(msg, host, port);
}

void SipUdpConnection::reTransOn()
{
    SipUdp_impl::reTransOn();
}

void SipUdpConnection::reTransOff()
{
    SipUdp_impl::reTransOff();
}

void SipUdpConnection::setRetransTime(int initial, int max)
{
    SipUdp_impl::setRetransTime(initial, max);
}

void SipUdpConnection::setRandomLosePercent(int percent)
{
    impl_->setRandomLosePercent(percent);
}


void SipUdpConnection::printSize() const
{
    impl_->printSize();
}


Data SipUdpConnection::getDetails() const
{
    return impl_->getDetails();
}
