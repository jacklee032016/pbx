#ifndef SubscribeManager_HXX
#define SubscribeManager_HXX

/*
 * $Id: SubscribeManager.hxx,v 1.2 2007/03/02 04:19:29 lizhijie Exp $
 */

#include <vector>

#include <Fifo.h>
#include "VThread.hxx"
#include <VMutex.h>
#include <SubscribeMsg.hxx>
#include <NotifyMsg.hxx>
#include <SipTransceiver.hxx>

namespace Vocal
{

class SubscribeManager
{
    public:
        SubscribeManager( Sptr < SipTransceiver > sipstack );
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

        Sptr < Fifo < Sptr < SubscribeMsg > > > myQ;
        VMutex subscribeMutex;

        Sptr < SubscribeMsg > subscribeMsg;
        int delay;
        FifoEventId msgId;

        Sptr < SipTransceiver > sipStack;
};
 
}


#endif
