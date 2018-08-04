#ifndef __ISDNEVENT_HXX__
#define __ISDNEVENT_HXX__

#include "PbxEvent.hxx"

#include "IsdnMsg.hxx"

namespace Assist
{

class IsdnCtrlDevice;


/*
* Message class used by DeviceMgr thread send to WorkerThread(GatewayMgr)
*/
class IsdnEvent : public PbxEvent
{
	public:
		IsdnEvent();

		/// Create given an associated fifo.
		IsdnEvent(Sptr < Fifo < Sptr <PbxEvent> > > fifo);

		virtual ~IsdnEvent();

		// The name of the extending class.
		virtual const char* const name() const 
		{
			return "ISDN-MSG";
		};

		
		void setIsdnMsg( Sptr <IsdnMsg> _msg);
		Sptr <IsdnMsg> getIsdnMsg();

		Sptr <IsdnCtrlDevice> getCtrlDevice();

	protected:

	private:
		Sptr <IsdnMsg>			myIsdnMsg;
		IsdnEvent(const IsdnEvent &_pbxMsg);
		const IsdnEvent & operator=(const IsdnEvent &);
};
 
}

#endif

