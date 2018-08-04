/*
 * $Id: StateConferencing.cxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "global.h"
#include "StateConferencing.hxx"

#include "OpConfComplete.hxx"
#include "OpConferencing.hxx"
#include "OpXferHold.hxx"
#include "OpConfTargetOk.hxx"
#include "OpConfError.hxx"
#include "OpReDirect.hxx"

using namespace Vocal;

StateConferencing::StateConferencing()
{
    addEntryOperator( new OpXferHold );

    addOperator( new OpConferencing );
    addOperator( new OpReDirect );
    addOperator( new OpConfTargetOk );
    addOperator( new OpConfComplete );
    addOperator( new OpConfError );
}


StateConferencing::~StateConferencing()
{
}


const char* const
StateConferencing::name() const
{
    return "StateConferencing";
}
