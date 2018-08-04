#ifndef ResGwDevice_HXX
#define ResGwDevice_HXX

/*
 * $Id: ResGwDevice.hxx,v 1.2 2007/03/02 04:19:29 lizhijie Exp $
 */

#include <termios.h>

#include "Sptr.hxx"
#include "Fifo.h"
#include "SipProxyEvent.hxx"

#include "UaHardwareEvent.hxx"
#include "UaDeviceEvent.hxx"
#include "DigitCollector.hxx"
#ifdef HAS_RSVPLIB
#include "SuaRsvp.hxx"
#endif


namespace Vocal
{

class InviteMsg;
class AssistDevice;

typedef enum {
    HARDWARE_AVAILABLE = 0,
    HARDWARE_UNAVAILABLE,
    HARDWARE_CALLWAITING_ALLOWED
} HardwareStatusType;

typedef enum {
    DialToneEmulation,
    RingbackToneEmulation
} ToneEmulation;


/* added by lizhijie 2005.02.08 */
typedef enum
{
	pbx_unused = 0,
	pbx_ring,
	pbx_offhook,
	pbx_hanging,
	pbx_audio_stop
		
}pbx_stats;
/* end of add */

///
typedef list < Sptr<SipCallId> > CallWaitingIdList;

/* added for concurrent multi-call. lizhijie 2005.02.08 */
typedef list < Sptr<SipCallId> > CallIdList;

///
enum EntryState
{
    EntryStateTelephoneUI,
    EntryStateEnterUrl
};

typedef enum
{
	NEW_CALL = 0,
	DELETE_CALL,
	REPLACE_CALL,
	ACTION_UNKNOW = 99
	
}CallAction;

/** 
 * Abstract base class for all hardware.  Specifies the SIP gateway
 * hardware API.  Defines a basic level of service provided by all
 * supported hardware interfaces.
 */
class ResGwDevice
{
#if 0 //modified by lijie 2005-06-03
    public:

        ///
        ResGwDevice() {};

        /// Creates a hardware object to control a single port gateway.
        ResGwDevice( const char* deviceName,
                     Sptr < Fifo < Sptr < SipProxyEvent > > > inputQ,
                     Sptr < Fifo < Sptr < SipProxyEvent > > > outputQ );

        /// Creates a hardware object to control a multi port gateway.
        /** Right now, this feature is not used, and simply asserts.
	 *  Later, we may need to implement this functionality.
	 */
        ResGwDevice( const char* deviceName,
                     int numEndpoints,
                     Sptr < Fifo < Sptr < SipProxyEvent > > > inputQ,
                     Sptr < Fifo < Sptr < SipProxyEvent > > > outputQ[] )
        { assert( 0 ); };

        /// Destructor
        virtual ~ResGwDevice();

        /// Starts the main processing loop of the hardware
        virtual void* hardwareMain( void* parms ) = 0;

        /** Add device's socket to the fd; returns 0 if successful,
         ** errorcode otherwise 
         */
        virtual int addToFdSet( fd_set* fd ) = 0;

        /** Process all pending hardware events; returns 0 if successful,
         ** otherwise returns an errorcode.  
         */
        virtual int process( fd_set* fd ) = 0;

        /** Start audio channel on device; returns 0 if successful,
         ** errorcode otherwise 
         */
//        virtual int audioStart( const HardwareAudioRequest& request ) = 0;
        virtual int audioStart( const HardwareAudioRequest& request , int deviceId) = 0;

        /** Stops audio channel on device; returns 0 if successful,
         ** errorcode otherwise.
         */
//        virtual int audioStop() = 0;
        virtual int audioStop(int deviceId) = 0;

        /** suspend audio channel on device; returns 0 if successful,
         ** errorcode otherwise 
         */
//        virtual int audioSuspend() = 0;
        virtual int audioSuspend(int deviceId) = 0;

        /** resume audio channel on device; returns 0 if successful,
         ** errorcode otherwise 
         */
//        virtual int audioResume( const HardwareAudioRequest& request ) = 0;
        virtual int audioResume( const HardwareAudioRequest& request , int deviceId) = 0;

		virtual void processRTP() = 0;
		virtual void processPbxRTP() = 0;


        /* added by lizhjie 2005.02.08 */
//        virtual void addCallId( const Sptr<SipCallId> newCallId );
//		virtual Sptr<SipCallId> getPbxCallId(int deviceId )  = 0;
//		virtual void setPbxCallId( const Sptr<SipCallId> newCallId , int deviceId) = 0;
		virtual Sptr<SipCallId> getCallId(int deviceId )  = 0;
		virtual void setCallId( const Sptr<SipCallId> newCallId , int deviceId) = 0;
		virtual int getDeviceId( const SipCallId& callId ) = 0;
		virtual int getPbxDeviceId( const SipCallId& callId ) = 0;
		virtual int getFreePbxDevice( ) = 0;
		virtual AssistDevice*  getAssistDevice(int deviceId) = 0;
		virtual AssistDevice*  getAssistDeviceInfo(const char *deviceName) = 0;
		virtual AssistDevice*  getAssistDeviceInfoByPhoneNumber(const char *phonenumbre) = 0;
		virtual Sptr<DigitCollector> getDigitCollector(int deviceId) = 0;
//        virtual void removeCallId( const SipCallId& newCallId );
		virtual int getRtpPort(int deviceId) = 0;
		virtual void releaseRtpPort(int deviceId) = 0;
		virtual HardwareStatusType getHardwareStatus(int deviceId) = 0 ;
		virtual bool isMyHardware(const SipCallId& callid, int deviceId) = 0;
//		virtual AssistDevice* getMyDevice( int deviceId) = 0;

	/* end of added */

        ///
        //modified by lijie to allow call waiting feathure
        virtual void addCallWaitingId( const Sptr<SipCallId> newCallId, int deviceId) = 0;
	 virtual Sptr<SipCallId> getCallWaitingId(int deviceId )  = 0;
	
       ///
        virtual void removeCallWaitingId( const SipCallId& newCallId, int deviceId ) = 0;

        ///
        virtual bool isOnCallWaiting(const SipCallId& callid, int deviceId) = 0;
	//modified ends here

		/// Report hardware event by adding it to specified queue
		void reportEvent( DeviceEventType eventType , int deviceId);
		
        /// access functions to private data
        inline Sptr < Fifo < Sptr<SipProxyEvent> > > getQ () 
        { return myQ; };

        inline bool isOffhook () { return hookStateOffhook; };


        /// returns the file descriptor for the device
        inline int getFD() { return myFD; };

#ifdef HAS_RSVPLIB
        /// RSVP stuff ported from Session.hxx
        void setUseRsvp( bool flag ) { useRsvp = flag; };
        bool getUseRsvp() { return useRsvp; };
#endif
        void setUsePolicy( bool flag ) { usePolicy = flag; };
        bool getUsePolicy() { return usePolicy; };
        InviteMsg* ringInvite;

        bool originator;

#ifdef HAS_RSVPLIB
        SuaRsvp& getSuaRsvpSender() { return suaRsvpSender; };
        SuaRsvp& getSuaRsvpReserv() { return suaRsvpReserv; };
#endif

    protected:

#ifdef HAS_RSVPLIB
        /// RSVP stuff ported from Session.hxx
        bool useRsvp;
        SuaRsvp suaRsvpSender;
        SuaRsvp suaRsvpReserv;
#endif
        bool usePolicy;


        // ****************
        // for signal control
        // ****************

 //       virtual pbx_stats  getPbxStatus() = 0;

	  //added by lijie to query device status
	 virtual void provideQueryStatus(int deviceId) = 0;
        virtual void provideDialToneStart(int deviceId) = 0;
        virtual void provideDialToneStop(int deviceId) = 0;
        virtual void provideRingStart(int deviceId) = 0;
        virtual void provideRingStop(int deviceId) = 0;
        virtual void provideLocalRingbackStart(int deviceId) = 0;
        virtual void provideLocalRingbackStop(int deviceId) = 0;
        virtual void provideBusyToneStart(int deviceId) = 0;
        virtual void provideBusyToneStop(int deviceId) = 0;
        virtual void provideFastBusyToneStart(int deviceId) = 0;
        virtual void provideFastBusyToneStop(int deviceId) = 0;
        virtual void provideCallWaitingBeepStart(int deviceId) = 0;
        virtual void provideCallWaitingBeepStop(int deviceId) = 0;

        virtual void providePbxTimerEvent(Sptr<UaHardwareEvent> msg ) = 0;

        ///
        virtual void provideDtmf( DeviceSignalType signal,int deviceId ) = 0;

        ///
        virtual void provideCallInfo( string CallerId,
                                      string CalleeId,
                                      string ForwardReason ) = 0;

        /** Trigger the device to send the specified signal; returns 0 if
         ** successful, errorcode otherwise.
         */
        int provideSignal( DeviceSignalType signal , int deviceId);

        /// Process a msg received from an session
        void processSessionMsg( Sptr < SipProxyEvent > event );


        /// file descriptor for hardware device
        int myFD;

        /// input queue to allow the session to control hardware signals
        Sptr < Fifo < Sptr< SipProxyEvent > > > myQ;

		// output queue to notify endpoint of occurence of hardware events
		Sptr < Fifo < Sptr< SipProxyEvent > > > sessionQ;

        /// true if last reported hook state was offhook
        bool hookStateOffhook;

	//added by lijie
#if 0	
        /// Call Id on call waiting list
        Sptr<CallWaitingIdList> callWaitingIdList;

	/* add for multi-call, lizhijie 2005.02.08 */
	Sptr<CallIdList>	callIdList;
#endif
	
#ifndef WIN32
        /// needed for proper terminal control
        int stdinFD;

        struct termios initialTerm;
#endif

        /// this bool is set to true if the user is entering text
        EntryState myEntryState;

        /** this Data contains a partial URL entered by a user if they
            have entered 'u'
          */
        Data myTextEntry;
#else
    public:

        ///
        ResGwDevice() {};

        /// Creates a hardware object to control a single port gateway.
        ResGwDevice( const char* deviceName,
                     Sptr < Fifo < Sptr < SipProxyEvent > > > inputQ,
                     Sptr < Fifo < Sptr < SipProxyEvent > > > outputQ );

        /// Creates a hardware object to control a multi port gateway.
        /** Right now, this feature is not used, and simply asserts.
	 *  Later, we may need to implement this functionality.
	 */
        ResGwDevice( const char* deviceName,
                     int numEndpoints,
                     Sptr < Fifo < Sptr < SipProxyEvent > > > inputQ,
                     Sptr < Fifo < Sptr < SipProxyEvent > > > outputQ[] )
        { assert( 0 ); };

        /// Destructor
        virtual ~ResGwDevice();

        /// Starts the main processing loop of the hardware
        virtual void* hardwareMain( void* parms ) = 0;

        /** Add device's socket to the fd; returns 0 if successful,
         ** errorcode otherwise 
         */
        virtual int addToFdSet( fd_set* fd ) = 0;

        /** Process all pending hardware events; returns 0 if successful,
         ** otherwise returns an errorcode.  
         */
        virtual int process( fd_set* fd ) = 0;

        /** Start audio channel on device; returns 0 if successful,
         ** errorcode otherwise 
         */
	//        virtual int audioStart( const HardwareAudioRequest& request ) = 0;
        virtual int audioStart( const HardwareAudioRequest& request , int deviceId) = 0;

        /** Stops audio channel on device; returns 0 if successful,
         ** errorcode otherwise.
         */
	//        virtual int audioStop() = 0;
        virtual int audioStop(int deviceId) = 0;

        /** suspend audio channel on device; returns 0 if successful,
         ** errorcode otherwise 
         */
	//        virtual int audioSuspend() = 0;
        virtual int audioSuspend(int deviceId) = 0;

        /** resume audio channel on device; returns 0 if successful,
         ** errorcode otherwise 
         */
	//        virtual int audioResume( const HardwareAudioRequest& request ) = 0;
        virtual int audioResume( const HardwareAudioRequest& request , int deviceId) = 0;

	virtual void processRTP() = 0;
	virtual void processPbxRTP() = 0;


	virtual Sptr<SipCallId> getCallId(int deviceId )  = 0;
	virtual void setCallId( const Sptr<SipCallId> newCallId , int deviceId) = 0;

	virtual int getDeviceId( const SipCallId& callId ) = 0;
	virtual int getPbxDeviceId( const SipCallId& callId ) = 0;
	virtual int getFreePbxDevice( ) = 0;

	virtual AssistDevice*  getAssistDevice(int deviceId) = 0;
	virtual AssistDevice*  getAssistDeviceInfo(const char *deviceName) = 0;
	virtual AssistDevice*  getAssistDeviceByPhoneNumber(const char *phonenumber, const SipCallId& callid) = 0;
	virtual Sptr<DigitCollector> getDigitCollector(int deviceId) = 0;
	virtual string  getPhoneNumber(int deviceId) = 0;
	virtual string  getDeviceName(int deviceId) = 0;

	virtual string  getAllTransferUrl(int deviceId) = 0;
	virtual string  getTimeOutTransferUrl(int deviceId) = 0;
	virtual string  getBusyTransferUrl(int deviceId) = 0;
	virtual bool getCallWaitingOn(int deviceId) = 0;


	virtual int getRtpPort(int deviceId) = 0;
	virtual void releaseRtpPort(int deviceId) = 0;
	virtual HardwareStatusType getHardwareStatus(int deviceId) = 0 ;
	virtual bool isMyHardware(const SipCallId& callid, int deviceId) = 0;

	virtual void setAudioChannel(int destDeviceId, int sourceDeviceId) = 0; 

        virtual void addCallWaitingId( const Sptr<SipCallId> newCallId, int deviceId) = 0;
	 virtual Sptr<SipCallId> getCallWaitingId(int deviceId )  = 0;
	
        virtual void removeCallWaitingId( const SipCallId& newCallId, int deviceId ) = 0;

        virtual bool isOnCallWaiting(const SipCallId& callid, int deviceId) = 0;

	 virtual int getCallCount(int deviceId ) = 0;
	 virtual void setCallCount(int count, int deviceId) = 0;

	 virtual void incCallCount(int deviceId ) = 0;
	 virtual void decCallCount(int deviceId) = 0;

	//report all devices' info
        //virtual void reportDeviceInfo() = 0;
        
        virtual void  reportDeviceInfo(int deviceId, CallAction action, const string& wrstr = "Default") = 0;
	void reportEvent( DeviceEventType eventType , int deviceId);
		
        inline Sptr < Fifo < Sptr<SipProxyEvent> > > getQ () 
        { return myQ; };
		

#ifdef HAS_RSVPLIB
        /// RSVP stuff ported from Session.hxx
        void setUseRsvp( bool flag ) { useRsvp = flag; };
        bool getUseRsvp() { return useRsvp; };
#endif
        void setUsePolicy( bool flag ) { usePolicy = flag; };
        bool getUsePolicy() { return usePolicy; };
 
#ifdef HAS_RSVPLIB
        SuaRsvp& getSuaRsvpSender() { return suaRsvpSender; };
        SuaRsvp& getSuaRsvpReserv() { return suaRsvpReserv; };
#endif

    protected:

#ifdef HAS_RSVPLIB
        /// RSVP stuff ported from Session.hxx
        bool useRsvp;
        SuaRsvp suaRsvpSender;
        SuaRsvp suaRsvpReserv;
#endif
        bool usePolicy;

	 virtual void provideQueryStatus(int deviceId) = 0;
        virtual void provideDialToneStart(int deviceId) = 0;
        virtual void provideDialToneStop(int deviceId) = 0;
        virtual void provideRingStart(int deviceId) = 0;
        virtual void provideRingStop(int deviceId) = 0;
        virtual void provideLocalRingbackStart(int deviceId) = 0;
        virtual void provideLocalRingbackStop(int deviceId) = 0;
	 virtual void provideRemoteRingbackStart(int deviceId) = 0;
        virtual void provideRemoteRingbackStop(int deviceId) = 0;	
        virtual void provideBusyToneStart(int deviceId) = 0;
        virtual void provideBusyToneStop(int deviceId) = 0;
        virtual void provideFastBusyToneStart(int deviceId) = 0;
        virtual void provideFastBusyToneStop(int deviceId) = 0;
        virtual void provideCallWaitingBeepStart(int deviceId) = 0;
        virtual void provideCallWaitingBeepStop(int deviceId) = 0;

        virtual void providePbxTimerEvent(Sptr<UaHardwareEvent> msg ) = 0;
	 virtual void provideDtmf( DeviceSignalType signal,int deviceId ) = 0;

        virtual void provideCallInfo( string CallerId,
                                      string CalleeId,
                                      string ForwardReason ) = 0;

	virtual void provideLocalErrorTone(int deviceId, DeviceSignalType tsignal) = 0;
        /** Trigger the device to send the specified signal; returns 0 if
         ** successful, errorcode otherwise.
         */
        int provideSignal( DeviceSignalType signal , int deviceId);

        /// Process a msg received from an session
        void processSessionMsg( Sptr < SipProxyEvent > event );


        /// input queue to allow the session to control hardware signals
        Sptr < Fifo < Sptr< SipProxyEvent > > > myQ;

	// output queue to notify endpoint of occurence of hardware events
	Sptr < Fifo < Sptr< SipProxyEvent > > > sessionQ;
#endif	

};
 
}

#endif
