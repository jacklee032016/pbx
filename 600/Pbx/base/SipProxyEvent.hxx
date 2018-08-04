#ifndef SIP_PROXY_EVENT_HXX
#define SIP_PROXY_EVENT_HXX
/*
 * $Id: SipProxyEvent.hxx,v 1.2 2007/03/02 04:19:27 lizhijie Exp $
 */

#include "Sptr.hxx"
#include "Fifo.h"


namespace Vocal
{

class SipProxyEvent;
class SipTransceiver;
class CallContainer;
class CallInfo;


#define  AS_DEVICE_NOT_DEFINED		-1
/** Object SipProxyEvent
<pre>
<br> Usage of this Class </br>

     It has an output fifo and callinfo.
     A event can post itself to its output fifo

   FooDosomthing()
   {

      Sptr < Fifo < Sptr < SipProxyEvent > > > myOutputFifo;
      Sptr < SipEvent > nextEvent = new SipEvent(myOutputFifo);
      if ( nextEvent != 0 )
       {
         nextEvent->setSipReceive(sipRcv);
         nextEvent->setSipStack(mySipStack);
         if(myCallLegHistory) nextEvent->setCallLeg();
         myOutputFifo->add(nextEvent);
      }
    }

</pre>
*/
/* added by wuaiwu, 2005/10/11 */
typedef enum
{
	ERR_NODEFINE = -1,
	ERR_BUSY,
	ERR_DIAL,
	ERR_CONNECT,
	ERR_LINEDROP
	
}error_info_t;
class SipProxyEvent
{
   public:
      ///
      SipProxyEvent();

      /// Create given an associated fifo.
      SipProxyEvent(Sptr < Fifo < Sptr < SipProxyEvent > > > fifo);

      ///
      virtual ~SipProxyEvent();

      /** Post the given event to the fifo associated with this event.
       *  This only really makes sense if this event is newEvent.
       */
      virtual void postEvent(const Sptr < SipProxyEvent > newEvent);

      /// Post the given event to the given fifo.
      virtual void postEvent( const Sptr < SipProxyEvent > newEvent, 
                              const Sptr < Fifo < Sptr < SipProxyEvent > > > newFifo) const;

      /// Set the call info and the call container associated with this event.
      void setCallInfo(const Sptr < CallInfo > callInfo, 
                       const Sptr < CallContainer > container );

      /// Access the associated call info. May be zero if not set.
      Sptr < CallInfo > getCallInfo() const;

      /// Remove the call info and call container associated with this event.
      void removeCallInfo();

      /** Accessor to the fifo associate with this event.
       *  May be zero if not set.
       */
      Sptr < Fifo < Sptr < SipProxyEvent > > > getFifo() const;

      /** Accessor to the call container for this event. 
       *  May be zero if not set.
       */
      Sptr < CallContainer > getCallContainer() const;

      ///
      void setSipStack( const Sptr < SipTransceiver > sipStack );

      ///
      const Sptr < SipTransceiver > getSipStack();

/* added by lizhijie 2005.02.10 */
	int getDeviceId( ) const;
	void setDeviceId( int _deviceId );
/* end of added */
		
      // The name of the extending class.
      virtual const char* const name() const = 0;

      /* added by wuaiwu, 2005/10/11 */
	void setErrorInfo(error_info_t e){ errinfo = e; }
	error_info_t getErrorInfo() const { return errinfo; }
     /* end of added */
   protected:
      /// The fifo associated with this event.
      Sptr < Fifo < Sptr < SipProxyEvent > > > myFifo;

      /** The FifoEventId associated with this event. This only makes
       *  sense for events that were posted to a fifo with a delay.
       */
      FifoEventId myId;

      /// The call info associated with this event.
      Sptr < CallInfo > myCallInfo;

      /// The call container associated with this event.
      Sptr < CallContainer > myCallContainer;

      /// Sip stack associated with this event.
      Sptr < SipTransceiver > mySipStack;

	/* added by lizhijie, 2005.02.10 */
	int myDeviceId;

	/// 
	error_info_t errinfo;
   private:

      // Suppress copying
      SipProxyEvent(const SipProxyEvent &);
        
      // Suppress copying
      const SipProxyEvent & operator=(const SipProxyEvent &);
};
 
}

#endif // SIP_PROXY_EVENT_HXX
