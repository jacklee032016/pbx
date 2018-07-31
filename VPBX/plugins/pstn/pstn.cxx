/*
* $Id: pstn.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "cpLog.h"
#include "PbxAgent.hxx"

#include "PstnDeviceMgr.hxx"

extern "C" int init(const char *plugName, void *param);

using namespace Assist;

int init(const char *plugName, void *param)
{
	assert(param!=0);
	PbxAgent *agent = (PbxAgent *)param;

	assert(agent!=0);
	Sptr <GatewayMgr> gatewayMgr = agent->getGateway();
	assert(gatewayMgr!=0);
	cpLog(LOG_DEBUG, "Dynamic Load VPBX PSTN plugin successfully" );
	DeviceMgr *pstnMgr = new PstnDeviceMgr(agent);

	gatewayMgr->registerDeviceMgr(plugName, pstnMgr);
	
	return 0;
}

