/*
 * $Id: Feature.cxx,v 1.2 2007/03/02 04:19:27 lizhijie Exp $
 */

#include "global.h"
#include "Feature.hxx"
#include "SipProxyEvent.hxx"
#include "CallInfo.hxx"


using namespace Vocal;

Feature::Feature()
{
}


Feature::~Feature()
{
	myStates.clear();
}

void Feature::process(const Sptr < SipProxyEvent > event)
{
    assert ( event != 0 );

    for ( StateIter iter = myStates.begin(); iter != myStates.end(); iter++ )
    {
        if ( (*iter)->process(event) != PROXY_CONTINUE )
        {
           cpDebug(LOG_DEBUG, "Feature::process done");
            break;
        }
    }
    cpDebug(LOG_DEBUG, "Done with feature (%s)", name());

}

void Feature::addState(const Sptr < State > state)
{
    assert( state != 0);

    cpDebug(LOG_DEBUG, "Insert %s state", state->name() );

    myStates.push_back(state);

    assert( myStates.begin() != myStates.end() );
}

