/*
* $Id: CommonMsg.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "CommonMsg.hxx"
#include "CallId.hxx"
#include "EndPoint.hxx"
using namespace Assist;

static char* MethodName[] = 
{
	"Setup",
	"SetupAck",
	"Information",
	"Proceeding",
	"Altering",
	"Connect",
	"ConnectAck",
	"DisConnect",
	"Release",
	"ReleaseComplete",
	"MsgUnKnow"
};

CommonMsg::CommonMsg()
{
	peerSignalType = dtmf_signal_out_band;
}

CommonMsg::CommonMsg(call_method_t type)
{
	method = type;
	if (type >= CALL_SETUP && type <= CALL_RELEASE_COMPLETE)
		strMethod = MethodName[type];
	else 
	{
		strMethod = "MsgUnKnow";
		method = CALL_UNKNOWN;
	}

	peerSignalType = dtmf_signal_out_band;
}
CommonMsg::~CommonMsg()
{
}

CommonMsg::CommonMsg(const CommonMsg& rmsg)
{
	copyValue(rmsg);
}

const CommonMsg& CommonMsg::operator=(const CommonMsg& rmsg)
{
	if (this != &rmsg)
		copyValue(rmsg);
	return *this;
}

const char* CommonMsg::getFrom() const
{
	return strFrom.c_str();
}
void CommonMsg::setFrom(const char* newFrom)
{
	strFrom = newFrom;
}

const char* CommonMsg::getTo() const
{
	return strTo.c_str();
}

void CommonMsg::setTo(const char* newTo)
{
	strTo = newTo;
}

dtmf_signal_type_t CommonMsg::getPeerSignalType()
{
	return peerSignalType;
} 

void CommonMsg::setSignalType(dtmf_signal_type_t _type)
{ 
	peerSignalType=_type;
}
	
#if 0
void CommonMsg::setFromEpId(int Id)
{
	eidFrom = Id;
}

void CommonMsg::setFromAuId(int Id)
{
	aidFrom = Id;
}

int CommonMsg::getFromEpId() const
{
	return eidFrom;
}

int CommonMsg::getFromAuId() const
{
	return aidFrom;
}

void CommonMsg::setEndPoint(Sptr <EndPoint> ep)
{
	epFrom = ep;
}
Sptr<EndPoint> CommonMsg::getEndPoint() const
{
	return epFrom;
}

void CommonMsg::setToEpId(int Id)
{
	eidTo = Id;
}
void CommonMsg::setToAuId(int Id)
{
	aidTo = Id;
}
	
int CommonMsg::getToEpId() const
{
	return eidTo;
}

int CommonMsg::getToAuId()const
{
	return aidTo;
}
void CommonMsg::setCallId(Sptr <CallId> Id)
{
	callId = Id;
}


const CallId& CommonMsg::getCallId() const
{
	return *callId;
}
#endif 

const string& CommonMsg::getMethodName() const
{
	return strMethod;
}
call_method_t CommonMsg::getMethod() const 
{
	return method;
}


void CommonMsg::copyValue(const CommonMsg& rmsg)
{
	strFrom = rmsg.strFrom;
	strTo = rmsg.strTo;
	strMethod = rmsg.strMethod;
	method = rmsg.method;
#if 0	
	epFrom = rmsg.epFrom;
	eidTo = rmsg.eidTo;
	callId = new CallId(*(rmsg.callId));
#endif	
}

