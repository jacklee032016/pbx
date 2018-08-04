#ifndef DEVICE_EVENT_HXX
#define DEVICE_EVENT_HXX

/*
 * $Id: DeviceEvent.hxx,v 1.2 2007/03/02 04:19:27 lizhijie Exp $
 */

#include "global.h"
#include <string>
#include "Sptr.hxx"
#include "SipProxyEvent.hxx"
#include "Fifo.h"

namespace Vocal
{

typedef enum
{
    DeviceEventUndefined = 0,
    DeviceEventShutdown,
    DeviceEventHookUp,			/* from on-hook to off-hook */
    DeviceEventHookDown,
    DeviceEventFlash,
    //added by lijie 
    DeviceEventIdle,
    DeviceEventBusy,
    DeviceEventDropTalk,
    DeviceEventDtmf0 = 100,
    DeviceEventDtmf1,
    DeviceEventDtmf2,
    DeviceEventDtmf3,
    DeviceEventDtmf4,
    DeviceEventDtmf5,
    DeviceEventDtmf6,
    DeviceEventDtmf7,
    DeviceEventDtmf8,
    DeviceEventDtmf9,
    DeviceEventDtmfStar,
    DeviceEventDtmfHash,
    DeviceEventCallUrl = 200
} DeviceEventType;

/** Object DeviceEvent
<pre>
<br> Usage of this Class </br>

   A place holder for events from a device

</pre>
*/
class DeviceEvent : public SipProxyEvent
{
    public:

        /** Create a device event given the associated fifo.
         */
        DeviceEvent(Sptr < Fifo < Sptr < SipProxyEvent > > > outputFifo );


        /** Virtual destructor
         */
        virtual ~DeviceEvent();


        /** Name of this class.
         */
        virtual const char* const name() const
      {
            return "DeviceEvent";
        }

        DeviceEventType type;
        int          id;
};
 
}

#endif // DEVICE_EVENT_HXX
