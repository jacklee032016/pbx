#ifndef CALL_INFO_HXX
#define CALL_INFO_HXX
/*
 * $Id: CallInfo.hxx,v 1.2 2007/03/02 04:19:27 lizhijie Exp $
 */

#include "VMutex.h"
#include "Sptr.hxx"
#include "SipCallLeg.hxx"
#include "SipCommand.hxx"
#include "Feature.hxx"
#include "StateMachnData.hxx"

namespace Vocal
{

class TimerEvent;
class Feature;
class State;

/** Object CallInfo
<pre>
 <br> Usage of this Class </br>

      CallInfo is a base class. It stores information about a call.
      A SipProxyEvent object has CallInfo. CallInfo data members are
      thread safe.

</pre>
*/
class CallInfo
{
    public:


        /** Normal constructor initializes its data members.
         */
        CallInfo();


        /**
         * Copy constructor
         * @param rhs the entry to copy
         */
        CallInfo(const CallInfo& rhs);


        /** Destructor method
         */
        virtual ~CallInfo();


        /**
         * Assignment Operator
         * @param rhs the entry to assign
         * @return *this
         */
        const CallInfo & operator=(const CallInfo& rhs);


        /** Feature mutator
         */
        void setFeature(const Sptr < Feature > nextFeature);
        
        
        /** Feature accessor
         */
        Sptr < Feature > getFeature() const;


        /** State mutator
         */
        void setState(const Sptr < State > nextState);


        /** State accessor
         */
        Sptr < State > getState() const;


        /** SipCommand mutator
         */
        void setSipCommand(const Sptr < SipCommand > sipCommand);


        /** SipCommand accessor
         */
        Sptr < SipCommand > getSipCommand() const;


        /** Timer mutator
         */
        void setTimer(const Sptr < TimerEvent > timerEvent);


        /** Timer accessor
         */
        Sptr < TimerEvent > getTimer() const;


        /** CallData mutator
         */
        void setCallStateData(Sptr < StateMachnData > callData);


        /** CallData accessor
         */
        Sptr < StateMachnData > getCallStateData() const;


        /** CallLeg accessor
         */
        Sptr < SipCallLeg > getSipCallLeg() const;


        /** Sets itsTimer member to 0.
         */
        void removeTimer();


        /** Call mutex lock on CallInfo
         */
        void lock();


        /** Call mutex unlock on CallInfo
         */
        void unlock();
 

    private:


        /** Copy method
         *  @param rhs builder to copy
         *  @return void
         */
        void copyRhsToThis(const CallInfo& rhs);


        /** 
         */
        VMutex myMutex;


        /** Pointer to the call's Feature
         */
        Sptr < Feature >    myFeature;


        /** Pointer to the call's State
         */
        Sptr < State >      myState;


        /** Pointer to the call's SipCommand
         */
        Sptr < SipCommand > mySipCommand;


        /** Pointer to the call's TimerEvent
         */
        Sptr < TimerEvent > myTimer;


        /** Pointer to the call's StateMachData
         */
        Sptr < StateMachnData > myCallData;
};
 
}

#endif // CALL_INFO_HXX
