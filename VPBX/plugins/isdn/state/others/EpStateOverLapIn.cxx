/*
 * $Id: EpStateOverLapIn.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */
#include "global.h"
#include "EpStateOverlapIn.hxx"

#include "OpConfComplete.hxx"
#include "OpConferencing.hxx"
#include "OpXferHold.hxx"
#include "OpConfTargetOk.hxx"
#include "OpConfError.hxx"
#include "OpReDirect.hxx"

using namespace Assist;

EpStateOverlapIn::EpStateOverlapIn()
{
#if 0
	addEntryOperator( new OpXferHold );

	addOperator( new OpConferencing );
	addOperator( new OpReDirect );
	addOperator( new OpConfTargetOk );
	addOperator( new OpConfComplete );
	addOperator( new OpConfError );
#endif	
}

EpStateOverlapIn::~EpStateOverlapIn()
{
}

const char* const EpStateOverlapIn::name() const
{
	return "EpStateOverlapIn";
}

