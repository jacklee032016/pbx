
/*
* $Id: voip.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "cpLog.h"
#include "PbxAgent.hxx"

#include "VoipDeviceMgr.hxx"

extern "C" int init(const char* plugName, void *param);

using namespace Assist;

int init(const char* plugName, void *param)
{
	assert(param!=0);
	PbxAgent *agent = (PbxAgent *)param;

	assert(agent!=0);
	Sptr <GatewayMgr> gatewayMgr = agent->getGateway();
	assert(gatewayMgr!=0);
	cpLog(LOG_DEBUG, "< Dynamic Load VOIP in VPBX plugin successfully >" );
	DeviceMgr *voipMgr = new VoipDeviceMgr(agent);

	gatewayMgr->registerDeviceMgr(plugName, voipMgr);
	
	return 0;
}

