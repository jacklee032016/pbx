/*
 * $Id: ConsoleOperator.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "ConsoleCtrlDevice.hxx"
#include "ConsoleOperator.hxx"

using namespace Assist;

ConsoleOperator::ConsoleOperator(const string& name)
	:EpOperator(name)
{
}

ConsoleOperator::~ConsoleOperator()
{}

Sptr <ConsoleCtrlDevice>  ConsoleOperator::getCtrlDevice(Sptr <PbxEvent> event)
{
	Sptr <EndPoint> ep = event->getEndPoint();
	assert(ep != 0);
	
	Sptr <ConsoleCtrlDevice> ctrl = NULL;
	ctrl.dynamicCast( ep->getCtrlDevice());
	assert(ctrl!=0);

	return ctrl;
}

Sptr <ConsoleEndPoint>  ConsoleOperator::getMasterEndPoint(Sptr <PbxEvent> event)
{
	Sptr <ConsoleEndPoint> master = getCtrlDevice(event)->getMasterEndPoint();
	assert( master != 0);

	return master;
}

Sptr <ConsoleEndPoint>  ConsoleOperator::getSlaveEndPoint(Sptr <PbxEvent> event)
{
	Sptr <ConsoleEndPoint> slave = getMasterEndPoint(event)->getSlave();
	assert(slave != 0);

	return slave;
}

void ConsoleOperator::setAudioChannel(Sptr <PbxEvent> event, console_audio_state_t audioState)
{
	Sptr <ConsoleAudioChannel> masterAudio = getCtrlDevice(event)->getMasterAudioChannel();
	assert(masterAudio != 0);
	
	masterAudio->setAudioState( audioState);
}

