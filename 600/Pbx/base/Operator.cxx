/*
 * $Id: Operator.cxx,v 1.2 2007/03/02 04:19:27 lizhijie Exp $
 */

#include "global.h"
#include "Operator.hxx"
#include "SipVia.hxx"
#include "SipUrl.hxx"
#include "SipRecordRoute.hxx"
#include "SystemInfo.hxx"

using namespace Vocal;

Operator::Operator()
{
}


Operator::~Operator()
{
}


void
Operator::OpAddVia(SipVia& sipvia, int port) const
{
    sipvia.setHost(theSystem.gethostAddress());
    sipvia.setPort(port);
    sipvia.setprotoVersion("2.0");
}


void
Operator::OpAddRecordRoute( 
    SipRecordRoute  &   recordRoute,
    const SipUrl    &   recordRouteUrl,
    const int           port
) const
{
    Sptr < SipUrl > routeUrl = new SipUrl( recordRouteUrl );

    routeUrl->setHost(theSystem.gethostAddress());
    routeUrl->setPort(Data(port));
    routeUrl->setMaddrParam(theSystem.gethostAddress());

    recordRoute.setUrl( routeUrl );
}

