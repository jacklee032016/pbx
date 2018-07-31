#ifndef __SIP_EVENT_HXX__
#define __SIP_EVENT_HXX__

#include <vector>

#include "SipMsgQueue.hxx"
#include "SipCallLeg.hxx"
#include "PbxEvent.hxx"

namespace Assist
{

typedef enum
{
	ERR_NODEFINE = -1,
	ERR_BUSY,
	ERR_DIAL,
	ERR_CONNECT,
	ERR_LINEDROP
	
}error_info_t;

class SipCommand;
class InviteMsg;
class SipMsg;
class SipTransceiver;

/** Object SipEvent
<pre>
<br> Usage of this Class </br>

    SipEvent is a SipProxyEvent. SipThread object creates a SipEvent
    on receiving a SipMsg. SipEvent gets posted to an output fifo.

</pre>
*/
class SipEvent : public PbxEvent
{
	public:

		/** Convenience typedef. */
		typedef vector < Sptr <SipMsg> > CallLegVector;

		/** Create the sip event given the associated fifo. */
		SipEvent( );

		virtual ~SipEvent();

		virtual const char* const name() const
		{
			return "SipEvent";
		}

		/** Set the sip message associated with this event. Updates the call leg.*/
		void setSipMsg(const Sptr <SipMsg> sipMsg);

		void setCallLeg();

		/** Returns the associated sip message.*/
		const Sptr <SipMsg> getSipMsg() const;

		/** Set the sip message queue associated with this event.
		*  It also updates the sip message. */
		void setSipMsgQueue( const Sptr <SipMsgQueue> sipRcv );

		/** Get the sip message queue associated with this event.*/
		const Sptr < SipMsgQueue > getSipMsgQueue() const;

		/** Returns the INVITE associated with the event, or 0 if none exists */
		const Sptr <InviteMsg> getInvite() const;

		/** Returns the SIP request associated with the event, or 0 if none exists */

		const Sptr <SipCommand> getCommand() const;

		/** Takes a Command and returns the corresponding command which is to 
		*  be cancelled, or 0 if none exists */
		const Sptr <SipCommand> getPendingCommand( Sptr <SipCommand> SipCommand ) const;

		///
		const Sptr <SipCallLeg> getSipCallLeg() const;

		/**    */
		void removeCall();

		void setSipStack(Sptr <SipTransceiver> sipStack);

		void setErrorInfo(error_info_t e){ myErrInfo = e; }
		error_info_t getErrorInfo() const { return myErrInfo; }
	private:

		/** Sip message associated with this event. */
		Sptr <SipMsg> 						mySipMsg;

		/** Sip message queue associate with this event. May be updated
		*  during the lifetime of an event.*/
		Sptr <SipMsgQueue> 					mySipMsgQueue;

		/** Sip call leg associated with this event. May be updated during
		*  this lifetime of an event. */
		Sptr <SipCallLeg> 						mySipCallLeg;

		Sptr <SipTransceiver>					mySipStack;

		error_info_t 							myErrInfo;
		
		SipEvent(const SipEvent &);

		const SipEvent & operator=(const SipEvent &);
};
 
}

#endif

