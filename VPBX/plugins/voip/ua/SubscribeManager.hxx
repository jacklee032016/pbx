#ifndef SubscribeManager_HXX
#define SubscribeManager_HXX

#include <vector>

#include <Fifo.h>
#include "VThread.hxx"
#include <VMutex.h>
#include <SubscribeMsg.hxx>
#include <NotifyMsg.hxx>
#include <SipTransceiver.hxx>

namespace Assist
{

class SubscribeManager
{
	public:
		SubscribeManager( Sptr <SipTransceiver> sipstack );
		~SubscribeManager();

		void subscribeMain();

		void startSubscribe();

		//this function return false if the input StatusMsg is not
		//a response to a register message; otherwise, true is returned
		bool handleSubscribeResponse(const StatusMsg& msg);

		// this function handles the corresponding notify message
		bool handleNotify( const NotifyMsg& msg );

	private:
		void updateSubscribeMsg(const StatusMsg& statusMsg );

		Sptr < Fifo < Sptr <SubscribeMsg> > > 		myQ;

		VMutex 									subscribeMutex;

		Sptr <SubscribeMsg> 						subscribeMsg;	/* current Subscribe msg in queue */
		int 										delay;			/* delay for this msg */
		FifoEventId 								msgId;			/* FIFO id for this msg */

		Sptr <SipTransceiver> 						mySipStack;
};
 
}

#endif

