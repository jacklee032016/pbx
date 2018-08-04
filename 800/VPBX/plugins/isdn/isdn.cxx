/*
* $Id: isdn.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "cpLog.h"
#include "PbxAgent.hxx"

#include "IsdnDeviceMgr.hxx"

extern "C" int init(const char *plugName, void *param);

using namespace Assist;

int init( const char *plugName, void *param)
{
	assert(param!=0);
	PbxAgent *agent = (PbxAgent *)param;

	assert(agent!=0);
	Sptr <GatewayMgr> gatewayMgr = agent->getGateway();
	assert(gatewayMgr!=0);
	cpLog(LOG_DEBUG, "Dynamic Load VPBX ISDN plugin successfully" );
	DeviceMgr *isdnMgr = new IsdnDeviceMgr(agent);

	gatewayMgr->registerDeviceMgr(plugName, isdnMgr );

#if 0
	if( isdnMgr->startup()!= 0)
	{
		cpLog(LOG_ERR, "ISDN plugin startup failed!!!");
	}	
#endif
//	sleep(10);
	cpLog(LOG_DEBUG, "ISDN plugin Load successfully" );

	return 0;//isdnMgr;
}

