/*
 * $Id: Builder.cxx,v 1.2 2007/03/02 04:19:27 lizhijie Exp $
 */

#include "global.h"
#include "Builder.hxx"
#include "SipEvent.hxx"
#include "SipTransceiver.hxx"

using namespace Vocal;


Builder::Builder()
    :   myCallContainer(0)
{
}


Builder::~Builder()
{
    myFeatures.clear();
}


void
Builder::process(const Sptr < SipProxyEvent > nextEvent)
{
    assert ( nextEvent != 0 );
    assert ( myCallContainer != 0 );

    Sptr < SipEvent > sipEvent;
    sipEvent.dynamicCast(nextEvent);
    if ( sipEvent != 0)
    {
        Sptr < CallInfo > callInfo 
            = myCallContainer->getCall(sipEvent->getSipCallLeg());

        sipEvent->setCallInfo(callInfo, myCallContainer);
    }

    // Locking is done in the operators
    //
    for (   FeatureIter iter = myFeatures.begin(); 
            iter != myFeatures.end(); 
            iter++
        )
    {
        (*iter)->process(nextEvent);
    }

    if ( sipEvent != 0 )
    {
        myCallContainer->removeCall(sipEvent->getSipCallLeg());
    }
    cpDebug(LOG_DEBUG, "Done with Builder " );
}


void
Builder::setCallContainer(const Sptr < CallContainer > callContainer)
{
    myCallContainer = callContainer;
}

void
Builder::setSipStack(const Sptr < SipTransceiver > sipStack)
{
   mySipStack = sipStack;
}


void
Builder::addFeature(const Sptr < Feature > feature)
{
    assert( feature != 0);

    cpDebug(LOG_DEBUG, "Insert %s feature", feature->name() );

    myFeatures.push_back(feature);

    assert( myFeatures.begin() != myFeatures.end() );
}


