/*
 * $Id: SipUdpConnection.cxx,v 1.3 2007/06/05 11:33:26 lizhijie Exp $
 */

#include "global.h"
#include "SipUdpConnection.hxx"
#include "SipUdp_impl.hxx"

using namespace Vocal;

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
