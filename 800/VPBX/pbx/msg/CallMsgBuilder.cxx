/*
* $Id: CallMsgBuilder.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "CallMsgBuilder.hxx"
#include "EndPoint.hxx"
using namespace Assist;

/**
*  Virtual function : const char* _Describer();
*
*/
VIRTUAL_FUNCTION(Alerting, Describer)
{
	return "Alerting";     
}  

VIRTUAL_FUNCTION(Connect, Describer)
{
	return "Connect";  
}

VIRTUAL_FUNCTION(ConnectAck, Describer)
{
	return "ConnectAck";
}

VIRTUAL_FUNCTION(DisConnect, Describer)
{
	return "DisConnect";
}


VIRTUAL_FUNCTION(Information, Describer)
{
	return "Information";
}

VIRTUAL_FUNCTION(Proceeding, Describer)
{
	return "Proceeding";
}


VIRTUAL_FUNCTION(Release, Describer)
{
	return "Release";
}


VIRTUAL_FUNCTION(ReleaseComplete, Describer)
{
	return "ReleaseComplete";
}


VIRTUAL_FUNCTION(Setup, Describer)
{
	return "Setup";
}

VIRTUAL_FUNCTION(SetupAck, Describer)
{
	return "SetupAck";
}


