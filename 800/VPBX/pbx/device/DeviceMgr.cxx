/*
* $Id: DeviceMgr.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"
#include "cpLog.h"
#include "vpbx_globals.h"

#include "DeviceMgr.hxx"
#include "PbxAgent.hxx"
#include "PbxConfiguration.hxx"
#include "CallTimerEvent.hxx"
#include "CtrlDevice.hxx"
#include "EndPoint.hxx"
#include "PbxHwEvent.hxx"

using namespace Assist;

DeviceMgr::DeviceMgr(PbxAgent *agent)
        : PbxManager(agent, "DeviceMgr" ),
        myUsePolicy(true),
        mySupportSecondDial(false)
{
	myGatewayMgr = agent->getGateway();
	myDeviceCount = 0;
	
#if 0
	callWaitingIdList = new CallWaitingIdList;
#endif

}

DeviceMgr::~DeviceMgr()
{
	cpLog(LOG_DEBUG, "Destorying.....");
//	destory();
}

void DeviceMgr::processAudio()
{
	Sptr <CtrlDevice> dev = NULL;
	int index = 0;
	
//	cpLog(LOG_DEBUG, "processAudio");
#if 0
	for (int i = 0; i < myDeviceCount; ++i)
	{
		myDeviceList[i]->processAudio();
	}
#endif 

	while( (dev=getNextDevice(index++))  != 0)
	{
		dev->processAudio();
	}
}

void DeviceMgr::addDevice( CtrlDevice *newCtrlDevice) 
{
	Sptr <CtrlDevice> ctrl = newCtrlDevice;
	cpLog(LOG_DEBUG, "Add CtrlDevice");
	assert( ctrl != 0 );
	myDeviceList.push_back( ctrl );
	myDeviceCount = myDeviceList.size();
	cpLog(LOG_DEBUG, "Add CtrlDevice");
}

Sptr <CtrlDevice> DeviceMgr::getNextDevice(const Sptr <CtrlDevice> dev)
{
#if 0//WITH_DEBUG
	Sptr <CtrlDevice> _dev;
	cpLog(LOG_DEBUG, "get next for %s", (dev!=0)?dev->getName():"NULL");
#endif

	DeviceList::iterator iter = myDeviceList.begin();
	if(dev==NULL)
	{
#if 0//WITH_DEBUG	
		_dev = *iter;
		cpLog(LOG_DEBUG, "CtrlDevice %s returned" , _dev->getName()  );
#endif		
		return *iter;
	}
	
	while ( iter != myDeviceList.end() )
	{
	if ( (*iter)->getIndex() == dev->getIndex() )
		{
			iter++;
			if( iter!= myDeviceList.end() )
			{
#if 0//WITH_DEBUG			
				_dev = *iter;
				cpLog(LOG_DEBUG, "CtrlDevice %s return", _dev->getName() );
#endif
				return *iter;
			}
			else
				return NULL;
		}
		iter++;
	}

	return NULL;	
}

Sptr <CtrlDevice> DeviceMgr::getNextDevice(int index)
{
	return ((unsigned int)index < myDeviceList.size())?  myDeviceList[index] : NULL ;
}
Sptr<CtrlDevice> DeviceMgr::getDevice(int deviceIndex  ) 
{
	Sptr <CtrlDevice> dev = NULL;

	while( (dev = getNextDevice(dev)) != 0)
	{
		if ( dev->getIndex() == deviceIndex )
		{
			return dev;
		}
	}

	return NULL;	
}

void DeviceMgr::removeDevice( const Sptr <CtrlDevice> ctrlDev )
{
	cpLog( LOG_DEBUG, "try to remove AudioChannel %s", ctrlDev->getName() );

	DeviceList::iterator iter = myDeviceList.begin();

	while ( iter != myDeviceList.end() )
	{
		if ( (*iter)->getIndex() == ctrlDev->getIndex() )
		{
			cpLog( LOG_DEBUG, "AudioChannel %s removed", ctrlDev->getName() );
			myDeviceList.erase(iter);
			break;
		}
		iter++;
	}

	return ;
}

#if 0
bool DeviceMgr::isOnCallWaiting(const CallId &callid)
{
    bool ret = false;

    CallWaitingIdList::iterator iter = callWaitingIdList->begin();

    while ( iter != callWaitingIdList->end() )
    {
        if ( *(*iter) == callid )
        {
            ret = true;
            break;
        }
        iter++;
    }
    return ret;
}
#endif

/* only called by DeviceMgrThread */
void DeviceMgr::myProcessEvent( Sptr<PbxEvent> event )
{
	cpLog( LOG_DEBUG, "processSessionMsg %s", event->name() );

	Sptr<PbxHwEvent> hwEvent;
	hwEvent.dynamicCast( event );
	if( hwEvent == 0)
		return;
	
	myProcessHwEvent( hwEvent);
}

void DeviceMgr::activate()
{
	Sptr <CtrlDevice> dev = NULL;
	while( (dev=getNextDevice(dev)) != 0 )
	{
#if 0	
		Sptr <IsdnCtrlDevice> isdn = NULL;
		isdn.dynamicCast(dev);
		assert(isdn!= 0);
		isdn->activate();
#endif		
	}
}

#if WITH_DEBUG
const char *DeviceMgr::debugInfo()
{
	char *tmp, *t2;
	int length = 0;
	Sptr <CtrlDevice> ctrlDev = NULL;
	
	tmp = (char *)malloc(4096);
	assert( tmp != 0);
	memset(tmp, 0, sizeof(tmp) );

	length += sprintf(tmp+length, "\r\nDeviceManager-%d, %s (Access Code : %s)", getIndex(), getName() ,accessCode );
	while( (ctrlDev = getNextDevice(ctrlDev)) != 0)
	{
		t2 =(char *) ctrlDev->debugInfo();
		length += sprintf(tmp + length, "%s", t2);
		free(t2);
	}
	
	return tmp;
}
#endif

Sptr <GatewayMgr>	DeviceMgr::getGatewayMgr()
{
	return myGatewayMgr;
}

const char *DeviceMgr::getAccessCode()
{
	return accessCode;
}

Sptr <EndPoint> DeviceMgr::findEpbyNumber(const string& num)
{
	DeviceList::iterator iter = myDeviceList.begin();	
	DeviceList::iterator itend = myDeviceList.end();	
	while (iter != itend)
	{
//		if (num == (*iter)->getNextEP(0)->getPhoneNumber())
			return (*iter)->getNextEP(0);
	}

	return NULL;
}

int DeviceMgr::startup()
{
	Sptr <CtrlDevice>	ctrl = NULL;
	int ret = -1;

	while( (ctrl= getNextDevice(ctrl))!=NULL)
	{
		cpLog(LOG_DEBUG, "%s startup...", ctrl->getName());
		ret = ctrl->startup();
		if(ret!=0)
			return ret;
	}

	return ret;
}

void DeviceMgr::setAccessCode(const char *_accessCode)
{
	if(_accessCode==0)
	{
		cpLog(LOG_DEBUG, "No Access Code for %s" ,getName() );
		return ;
	}
	memset(accessCode, 0, MAX_ACCESS_CODE_LENGTH);
	sprintf(accessCode, "%s", _accessCode);
}

Sptr <EndPoint> DeviceMgr::findFreeEP()
{
	return 0;
}

void DeviceMgr::setName(const char *name)
{
	sprintf(myName, "%s", name);
}

DialMethodsType DeviceMgr::getDialMethodType()
{
	return myDialMethodType;
}

