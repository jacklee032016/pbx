/* 
 * $Id: AssistCheckHardware.cxx,v 1.2 2007/03/21 18:12:39 lizhijie Exp $
 */

#include "global.h"
#include "AssistCheckHardware.hxx"
#include <typeinfo>

#include "pbx_globals.h"

using namespace Vocal;

AssistCheckHardware::AssistCheckHardware()
{
}


///
AssistCheckHardware::~AssistCheckHardware()
{}


///
const char* AssistCheckHardware::getType()
{
	const char *deviceName = UaConfiguration::instance()->getDeviceName().c_str();
	int fd = 0;
#if __WITH_SLIC__
	as_state_t  state ;
#endif

	if ( deviceName == "" )
		return "NULL";

    // open and close it to reset
	fd = open( deviceName, O_RDWR );
	close( fd );
	fd = open( deviceName, O_RDWR );
	if( fd <= 0 )
	{
		cpLog( LOG_EMERG, "Device '%s' Open failed. Error: '%s'.",  deviceName,  strerror(errno) );
#if 0
		cpLog( LOG_ERR, "Defaulting to NULL_HARDWARE" );
		return "NULL";
#else
		cpLog( LOG_EMERG, "Program will quit !!!" );
		cpLog( LOG_EMERG, "Please check your hardware and software configuration carefully and try again!" );
		cpLog( LOG_EMERG, "You may contact our support--support@assistcn.com" );
		exit(-1);
#endif
	}

#if __WITH_SLIC__
	state = as_device_check_state_on_startup( fd, deviceName);
	if(state == AS_DEVICE_STATE_ONHOOK )
		cpDebug(LOG_DEBUG, "Device '%s' is in the state of ONHOOK", deviceName);
	else
		cpDebug(LOG_DEBUG, "Device '%s' is in the state of ONHOOK", deviceName);
#endif
	close( fd );

#if __WITH_SLIC__
	cpDebug(LOG_DEBUG, "device is type Assist Codec/SLIC card" );
#else
	cpDebug(LOG_DEBUG, "device is type Assist PCM card" );
#endif

	return "ASSIST";

}

