/*
 * $Id: OpAddDigit.cxx,v 1.4 2007/03/07 18:24:08 lizhijie Exp $
 */

#include "global.h"
#include "OpAddDigit.hxx"
#include "UaDeviceEvent.hxx"
#include "UaDevice.hxx"

using namespace Vocal;

OpAddDigit::OpAddDigit()
{}

OpAddDigit::~OpAddDigit()
{}

const char* const OpAddDigit::name() const
{
	return "OpAddDigit";
}

const Sptr <State> OpAddDigit::process( const Sptr <SipProxyEvent> event )
{
	cpDebug(LOG_DEBUG, "OpAddDigit operation" );

	Sptr <UaDeviceEvent> deviceEvent;
	deviceEvent.dynamicCast( event );
	if ( deviceEvent == 0 )
	{
		return 0;
	}

	char digit = ' ';
	switch ( deviceEvent->type )
	{
		case DeviceEventDtmfStar:
		{
			digit = '*';
		}
		break;
		
		case DeviceEventDtmfHash:
		{
			digit = '#';
		}
		break;
		
		case DeviceEventDtmf0:
		{
			digit = '0';
		}
		break;
		
		case DeviceEventDtmf1:
		{
			digit = '1';
		}
		break;
		
		case DeviceEventDtmf2:
		{
			digit = '2';
		}
		break;
		
		case DeviceEventDtmf3:
		{
			digit = '3';
		}
		break;
		
		case DeviceEventDtmf4:
		{
			digit = '4';
		}
		break;
		
		case DeviceEventDtmf5:
		{
			digit = '5';
		}
		break;
		
		case DeviceEventDtmf6:
		{
			digit = '6';
		}
		break;
		
		case DeviceEventDtmf7:
		{
			digit = '7';
		}
		break;
		
		case DeviceEventDtmf8:
		{
			digit = '8';
		}
		break;
		
		case DeviceEventDtmf9:
		{
			digit = '9';
		}
		break;
		
		// TODO add 'A', 'B', 'C' and 'D'
		case DeviceEventCallUrl:
		{
			cpDebug(LOG_DEBUG_STACK, "setting URL to %s", deviceEvent->text.logData());
			UaDevice::instance()->getDigitCollector( event->getDeviceId() )->setDialedUrl(deviceEvent->text.convertString());
		}
		break;
		
		default:
		{
			cpDebug(LOG_DEBUG, "Not a digit" );
		}
	}
	
	if ( digit != ' ' )
	{
		cpDebug(LOG_DEBUG, "Add Digit %c", digit );
		//cout << digit << flush;
		UaDevice::instance()->getDigitCollector( event->getDeviceId() )->addDigit( digit );
	}
	
	return 0;
}

