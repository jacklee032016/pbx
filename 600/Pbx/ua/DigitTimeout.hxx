#ifndef DIGITTIMEOUT_HXX
#define DIGITTIMEOUT_HXX

/*$Id: DigitTimeout.hxx,v 1.2 2007/03/02 04:19:29 lizhijie Exp $*/

#include <VMutex.h>
#include <Sptr.hxx>
#include <SipProxyEvent.hxx>
#include <Fifo.h>
#include <VThread.hxx>

namespace Vocal
{

typedef enum {
    INTER_DIGIT_TIME_OUT = -1,
    INTER_DIGIT_OK
} InterDigitTimeoutType;


/* added by lizhijie, 2005.02.11 */
typedef struct 
{
	InterDigitTimeoutType  	type;
	FifoEventId 			eventId;
	int 					deviceId;
	bool 				isTimeout;
}PendingDigitTimerEvent;


class DigitTimeout
{
	public:
		static DigitTimeout* instance();

		static void instance(Sptr < Fifo < Sptr < SipProxyEvent > > > cpq,
                      int initTimeout , int interDigitTimeout );

		void setInitTimeout(int initTimeout = 16000)
		{
			initTimeoutVal = initTimeout;
		};

		void setInterDigitTimeout(int interDigitTimeout = 8000)
		{
			interDigitTimeoutVal = interDigitTimeout;
		};
		
#if 0		
/* changed as following, lizhijie ,2005.02.11 */
        bool isInterDigitTimeout()
        {
            return isTimeout;
        };
		void timerReset();
		void timerStart();
		void timerCancel( );
#endif
		void timerReset(Sptr <PendingDigitTimerEvent > pendingEvent);
		void timerStart(Sptr <PendingDigitTimerEvent > pendingEvent);
		void timerCancel(Sptr <PendingDigitTimerEvent > pendingEvent);

	private:
		DigitTimeout( Sptr < Fifo < Sptr < SipProxyEvent > > > cpq,
			int initTimeout = 16000,
                      int interDigitTimeout = 8000 );

		int initTimeoutVal;
		int interDigitTimeoutVal;
#if 0
/* changed as following, lizhijie ,2005.02.11 */
        Fifo < InterDigitTimeoutType > digitTimeoutFifo;
        FifoEventId id;
#endif
		Fifo < Sptr < PendingDigitTimerEvent > > digitTimeoutFifo;

		VMutex digitTimeoutMutex;
		VThread digitTimeoutThread;
		static void* digitTimeoutThreadWrapper(void* timeout);
		void digitTimeoutMain();
#if 0
	/* commentted by lizhijie */
        bool isTimeout;
#endif

		Sptr < Fifo < Sptr < SipProxyEvent > > > callProcessingQueue;

		static DigitTimeout* uaTimeout;

};
 
}


#endif
