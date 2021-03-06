#ifndef SessionTimer_HXX
#define SessionTimer_HXX
/*
 * $Id: SessionTimer.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */

   
#include "global.h"
#include <map>
#include "Data.hxx"
#include "Sptr.hxx"
#include "TimerEntry.hxx"
#include "InviteMsg.hxx"
#include "SipCallLeg.hxx"
#include "Fifo.h"
#include "SipTransceiver.hxx"

namespace Vocal
{

enum VSessionContext 
{
    VS_NONE=0,
    VS_SEND,
    VS_RECV
};

typedef struct VSessionData_t
{
    long myDelta;
    long myStartTime;
    TimerEntryId          myTimerId;
    VSessionContext       mySessionContext;
    Sptr<InviteMsg> myInviteMsg;

    VSessionData_t(): myDelta(0), myStartTime(0) {};
} VSessionData;

typedef void (*VSessionCallBack)(SipCallLeg arg);


/// Singelton clas to support SessionTimers
class SessionTimer
{
public:
    /// Create one with default values
    static SessionTimer& instance();
    ///
    static SessionTimer& instance(Sptr<SipTransceiver> tranceiver);

    ///
    static  void destroy();

    ///
    void registerCallBack(VSessionCallBack func) { myCallbackFunc = func; };

    ///Chjeck if timer already exists for a given call-leg
    bool timerExists(const InviteMsg& iMsg);

    /**Start a timer for Invite message with given sessionInterval
     * To be called when not a refresher
     */
    void startTimerFor(const InviteMsg& iMsg, long  sessionInterval, VSessionContext ct=VS_RECV);

    ///Interface to start the SessionTimer by refresher when received status msg
    void startTimerFor(const StatusMsg& sMsg);
    ///
    void processResponse(StatusMsg& sMsg);
private:
    ///
    void startTimerFor(const InviteMsg& iMsg, Sptr<VSessionData> sData);

    ///
    void sendInvite(Sptr<VSessionData> sData);
    ///
    SessionTimer(Sptr<SipTransceiver> tranceiver);
    ///
    static void* processThreadWrapper(void *p);
    ///
    virtual ~SessionTimer() ;

    static SessionTimer* mInstance;
    typedef Fifo<Sptr<VSessionData> > SessionDataFifo;
    SessionDataFifo mSessionDataFifo;
    typedef map<SipCallLeg, Sptr<VSessionData> > SessionDataMap;
    SessionDataMap mSessionDataMap;
    VThread mProcessThread;
    Sptr<SipTransceiver> mTransceiver;
    VSessionCallBack myCallbackFunc;
    bool mShutdown;
};
 
} // namespace Vocal

#endif
