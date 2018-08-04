/*
 * $Id: CallInfo.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "global.h"
#include "CallInfo.hxx"

#include "CallState.hxx"
#include "PbxStateMachine.hxx"
#include "EndPoint.hxx"
#include "PbxAgent.hxx"


using namespace Assist;
//static int callinfos = 0;
CallInfo::CallInfo( )
	: PbxStateObject( PbxAgent::instance()->getScheduler() ),
	myCallId(0),
	mySrc(0),
	myDest(0)
{
	setState( findState(CALL_STATE_IDLE) );
	//++callinfos;
	//cout << "------new CallInfo: " << callinfos << endl;
}

/*
CallInfo::CallInfo(const CallInfo & rhs)
{
	copyRhsToThis(rhs);
}
*/

const CallInfo &CallInfo::operator=(const CallInfo & rhs)
{
	if ( this != &rhs )
	{
		copyRhsToThis(rhs);
	}

	return ( *this );
}


CallInfo::~CallInfo()
{
	//--callinfos;
	//cout << "------delete CallInfo: " << callinfos << endl;
	cpLog(LOG_DEBUG, "CallInfo is deconstructing.....");
}

Sptr <CallId> CallInfo::getCallId() const
{
	return myCallId;
}

void CallInfo::setCallId(Sptr <CallId> _callId)
{
	myCallId = _callId;
}

Sptr <EndPoint> CallInfo::getSrcEP() const
{
	return mySrc;
}

void CallInfo::setSrcEP(Sptr <EndPoint> _src)
{
	mySrc = _src;
}

Sptr <EndPoint> CallInfo::getDestEP() const
{
	return myDest;		
}

void CallInfo::setDestEP(Sptr <EndPoint> _dest)
{
	myDest = _dest;
}

void CallInfo::copyRhsToThis(const CallInfo & rhs)
{
//	myStateMachine = rhs.myStateMachine;
/*
	this->setState (rhs.getState() );
	this->setTimer( rhs.getTimer() );
*/	
}

