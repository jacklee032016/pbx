/*
* $Log: SessionTimer.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SessionTimer.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "global.h"
#include "SessionTimer.hxx"
#include "VTime.hxx"

using namespace Assist;

SessionTimer* SessionTimer::mInstance=0;
    
SessionTimer& 
SessionTimer::instance()
{
    assert(mInstance);
    return (*mInstance);
}

SessionTimer& 
SessionTimer::instance(Sptr<SipTransceiver> tranceiver)
{
    assert(mInstance == 0);
    if(mInstance == 0)
    {
        mInstance = new SessionTimer(tranceiver);
    }
    return (*mInstance);
}

SessionTimer::SessionTimer(Sptr<SipTransceiver> tranceiver)
   : mTransceiver(tranceiver),
     mShutdown(false)
{
    mProcessThread.spawn(processThreadWrapper, this);
}


void 
SessionTimer::destroy()
{
    delete SessionTimer::mInstance;
    SessionTimer::mInstance = 0;
}
SessionTimer::~SessionTimer()
{
    mShutdown = true;
    mSessionDataFifo.addDelayMs(0,0);
}

void 
SessionTimer::processResponse(StatusMsg& sMsg)
{
    int status = sMsg.getStatusLine().getStatusCode();
    if(status == 200)
    {
        SipCallLeg cLeg = sMsg.computeCallLeg();
        cpDebug(LOG_DEBUG,"SessionTimer processing response for callLeg (%s)", cLeg.encode().logData() );
        //LOcate the session data
        assert(mSessionDataMap.count(cLeg));
        int delta = sMsg.getSessionExpires().getDelta().convertInt();
        Sptr<VSessionData> sData = mSessionDataMap[cLeg];
        if(sData->mySessionContext == VS_SEND)
        {
            if(sData->myDelta != delta)
            {
                cpDebug(LOG_DEBUG,"Timer (%d)s value in INVITE does not match the one (%d)s in Status, adjusting.. for callLeg (%s)", sData->myDelta, delta, cLeg.encode().logData() );
                //Set the new session interval received in the response
                mSessionDataFifo.cancel(sData->myTimerId);
                sData->myDelta = delta;
                int ts = sData->myDelta /2;
                sData->myTimerId = mSessionDataFifo.addDelayMs(sData,ts * 1000);
            }
        }
        VTime tm;
        sData->myStartTime = tm.getSeconds();
    }
    else 
    {
        cpDebug(LOG_INFO, "Unexpected status message (%d) in SessionTimer", status);
    }
}
   
bool 
SessionTimer::timerExists(const InviteMsg& iMsg)
{
    SipCallLeg cLeg = iMsg.computeCallLeg();
    return((mSessionDataMap.count(cLeg)) ? true : false);
}

void
SessionTimer::startTimerFor(const InviteMsg& iMsg, Sptr<VSessionData> sData)
{
    SipCallLeg cLeg = iMsg.computeCallLeg();
    if(mSessionDataMap.count(cLeg))
    {
        cpDebug(LOG_DEBUG,"Timer exists for (%s) for callLeg (%s) for (%d)s", (sData->mySessionContext == VS_SEND)? "SENDER": "RECEIVER", cLeg.encode().logData(), sData->myDelta);
         return;
    }
    cpDebug(LOG_DEBUG,"Adding timer for (%s) for callLeg (%s) for (%d)s", (sData->mySessionContext == VS_SEND)? "SENDER": "RECEIVER", cLeg.encode().logData(), sData->myDelta);
    VTime tm;
    sData->myStartTime = tm.getSeconds();
    //Make a copy of Inviter message that will be reused when sending 
    //Re-Invite
    sData->myInviteMsg = new InviteMsg(iMsg);
    mSessionDataMap[cLeg] = sData;
    int ts = sData->myDelta /2;
    sData->myTimerId = mSessionDataFifo.addDelayMs(sData,ts * 1000);
}

void 
SessionTimer::startTimerFor(const InviteMsg& iMsg, long sessionInterval, VSessionContext ct)
{
    Sptr<VSessionData>  sData = new VSessionData;
    sData->mySessionContext = ct;
    sData->myDelta = sessionInterval;
    startTimerFor(iMsg, sData);
}

void 
SessionTimer::startTimerFor(const StatusMsg& sMsg)
{
    InviteMsg iMsg(sMsg);
    if(!sMsg.getSessionExpires().isEmpty())
    {
        iMsg.setSessionExpires(sMsg.getSessionExpires());
    }
    if(!sMsg.getMinSE().isEmpty())
    {
        iMsg.setMinSE(sMsg.getMinSE());
    }
    Sptr<VSessionData>  sData = new VSessionData;
    sData->mySessionContext = VS_SEND;
    sData->myDelta = sMsg.getSessionExpires().getDelta().convertInt();
    startTimerFor(iMsg, sData);

}


void* SessionTimer::processThreadWrapper(void *p)
{
    SessionTimer* self = static_cast<SessionTimer*>(p);
    while(1)
    {
        Sptr<VSessionData> sData = self->mSessionDataFifo.getNext();
        if(self->mShutdown)
        {
            break;
        }
        VTime tm;
        if((long)tm.getSeconds() >= (sData->myStartTime + sData->myDelta))
        {
            SipCallLeg cLeg = sData->myInviteMsg->computeCallLeg();
            cpDebug(LOG_INFO, "Session expired for call-leg (%s)", cLeg.encode().logData()); 
            (self->myCallbackFunc)(cLeg);
        }
        else
        {
            //Send the re-INVITE and schedule the timer again
            //if refresher 
            if(sData->mySessionContext == VS_SEND)
            {
                self->sendInvite(sData);
            }
            int ts = sData->myDelta /2;
            sData->myTimerId = self->mSessionDataFifo.addDelayMs(sData,ts * 1000);
        }
    }
    return( (void*)0);
}

void
SessionTimer::sendInvite(Sptr<VSessionData> sData)
{
   cpDebug(LOG_DEBUG, "Sending re-invite for call-leg (%s)", sData->myInviteMsg->computeCallLeg().encode().logData());
   SipCSeq cSeq = sData->myInviteMsg->getCSeq();
   cSeq.incrCSeq();
   sData->myInviteMsg->setCSeq(cSeq); 
   mTransceiver->sendAsync(sData->myInviteMsg);
}

