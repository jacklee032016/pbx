#ifndef __CONSOLE_OPERATOR_HXX__
#define __CONSOLE_OPERATOR_HXX__

/*
 * $Id: ConsoleOperator.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "EpOperator.hxx"

#include "ConsoleEndPoint.hxx"

namespace Assist
{

class ConsoleCtrlDevice;

/* common super class for all Console Operations */
class ConsoleOperator : public EpOperator
{
	public:
		ConsoleOperator(const string& name);
		virtual ~ConsoleOperator();

	protected:
		Sptr <ConsoleEndPoint>  getMasterEndPoint(Sptr <PbxEvent> event);
		Sptr <ConsoleEndPoint>  getSlaveEndPoint(Sptr <PbxEvent> event);

		/* only audiochannel of master endpoint is used */
		void setAudioChannel(Sptr <PbxEvent> event, console_audio_state_t );
	private:
		ConsoleOperator(const ConsoleOperator& rhs);
		ConsoleOperator& operator=(const ConsoleOperator& rhs);

		Sptr <ConsoleCtrlDevice>  getCtrlDevice(Sptr <PbxEvent> event);

	protected:
		
};
 
}

#endif

