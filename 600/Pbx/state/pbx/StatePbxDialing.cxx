/*
$Id: StatePbxDialing.cxx,v 1.1.1.1 2006/11/29 09:50:50 lizhijie Exp $
 */

#include "global.h"
#include "StatePbxDialing.hxx"
#include "OpPbxDialOK.hxx"


#include "OpStartAudioDuplex.hxx"
#include "OpAck.hxx"
#include "OpTerminateCall.hxx"
#include "OpEndCall.hxx"
#include "OpReInvite.hxx"
#include "OpConfTargetOk.hxx"
#include "OpAddPbxDigit.hxx"
#include "OpPbxDialError.hxx"
#include "OpTerminateCall.hxx"
#include "OpStartDialTone.hxx"
#include "OpStopDialTone.hxx"
#include "OpStopPbxDialing.hxx"
#include "UaConfiguration.hxx"

using namespace Vocal;
/* copy from StateInCall */
StatePbxDialing::StatePbxDialing()
{
//modified by lijie 2005-06-03
#if 0
	addEntryOperator( new OpStartAudioDuplex );

	addOperator( new OpAck );
	addOperator( new OpConfTargetOk );
	addOperator( new OpAddDigit );  /* added from StateDialing */
//	addOperator( new OpFwdDigit );

	addOperator( new OpPbxDialError ); /* added for pbx */
	addOperator( new OpPbxDialOK ); /* added for pbx */
	
	addOperator( new OpTerminateCall );
	addOperator( new OpEndCall );
	addOperator( new OpReInvite );
	addOperator( new OpStartCallWaiting );

#if 0
	if ( UaConfiguration::instance()->getXferMode() != XferOff )
	{
		addOperator( new OpSecondCall );
		addOperator( new OpRecvdXfer );
	}
#endif

//	addExitOperator( new OpStopAudio );
#else
	addEntryOperator( new OpStartAudioDuplex );
	addEntryOperator( new OpStartDialTone );

	//addOperator( new OpAck );
    	addOperator( new OpConfTargetOk );
	addOperator( new OpAddPbxDigit );  
	addOperator( new OpStopDialTone ); 
	addOperator( new OpPbxDialError ); 
	addOperator( new OpPbxDialOK ); 	
	addOperator( new OpReInvite );
	addOperator( new OpTerminateCall );
	addOperator( new OpEndCall );

	addExitOperator( new OpStopPbxDialing );

#endif
}


StatePbxDialing::~StatePbxDialing()
{
}


const char* const  StatePbxDialing::name() const
{
	return "StatePbxDialing";
}
