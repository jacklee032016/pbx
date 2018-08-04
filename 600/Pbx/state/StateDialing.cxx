/*
 * $Id: StateDialing.cxx,v 1.4 2007/03/21 18:12:39 lizhijie Exp $
 */

#include "global.h"
#include "StateDialing.hxx"

#include "OpStartDialTone.hxx"
#include "OpAddDigit.hxx"
#include "OpStopDialTone.hxx"
#include "OpInviteUrl.hxx"
#include "OpDialError.hxx"
#include "OpOnHook.hxx"

using namespace Vocal;

StateDialing::StateDialing()
{
	addEntryOperator( new OpStartDialTone );

	addOperator( new OpAddDigit );
	addOperator( new OpStopDialTone );
	addOperator( new OpInviteUrl );
	addOperator( new OpDialError );
	addOperator( new OpOnHook );
}

StateDialing::~StateDialing()
{
}

const char*  const StateDialing::name() const
{
	return "StateDialing";
}

