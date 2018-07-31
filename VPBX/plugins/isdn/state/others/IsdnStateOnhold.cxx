/*
 * $Id: IsdnStateOnhold.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "global.h"
#include "IsdnStateOnhold.hxx"

#include "OpRevert.hxx"
#include "OpHoldTargetOk.hxx"
#include "OpReDirect.hxx"
#include "OpHoldTargetErr.hxx"
#include "OpEndCall.hxx"
#include "OpReRing.hxx"

using namespace Assist;

IsdnStateOnhold::IsdnStateOnhold()
{
	addOperator( new OpRevert );
	addOperator( new OpHoldTargetOk );
	addOperator( new OpReDirect );
	addOperator( new OpEndCall );
	addOperator( new OpReRing );
	addOperator( new OpHoldTargetErr );
}

IsdnStateOnhold::~IsdnStateOnhold()
{
}

const char* const IsdnStateOnhold::name() const
{
	return "IsdnStateOnhold";
}
