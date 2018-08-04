#ifndef SipTransceiver_HXX
#define SipTransceiver_HXX
/*
 * $Id: SipTransceiver.hxx,v 1.4 2007/06/05 11:33:25 lizhijie Exp $
 */

#include <string>
#include "global.h"
#include "TransceiverSymbols.hxx"
#include "Fifo.h"

#include "SipTcpConnection.hxx"
#include "SipUdpConnection.hxx"

#include "SipTransactionDB.hxx"
#include "SipSentRequestDB.hxx"
#include "SipSentResponseDB.hxx"
#include "SipTransactionLevels.hxx"
#include "SipTransactionId.hxx"

#include "SipSnmpDetails.hxx"
#include "SipMsgQueue.hxx"
#include "SipMsg.hxx"
#include "SipCommand.hxx"
#include "StatusMsg.hxx"
#include "Sptr.hxx"
#include "SipDebuggingInterface.hxx"

namespace Vocal
{
    
///
typedef enum
{
   APP_CONTEXT_GENERIC,
   APP_CONTEXT_PROXY
}SipAppContext;

/**
   SipTransceiver is the main class for users the SIP stack.  It is the object
   which the caller uses to send and receive SIP messages.
*/
class SipTransceiver
{
    public:
	///
	SipTransceiver(Data s = 0, 
		       int sipPort = SIP_PORT, 
		       bool nat = false , 
		       SipAppContext aContext=APP_CONTEXT_GENERIC);

	///
	virtual ~SipTransceiver();

	/**
	 * Copy on write is "expensive", can we change this interface,
	 * specially since we know that most of these are in Sptr's
	 * already
	 */
	virtual void sendAsync(SipCommand& sipMessage);

	///Interface so that we do not have to copy message again in stack
	void sendAsync(Sptr<SipCommand> sipMessage, const Data& host="",
                       const Data& port="5060");

	///
	virtual void sendReply(StatusMsg& sipMessage);

	///Interface so that we do not have to copy message again in stack
	virtual void sendReply(Sptr<StatusMsg> sipMessage);

	/**
	 ** Return a deque of SipMsgs, basically containing the msg chain.
	 ** A timeOut out of -1 means infinite timeout.
	 */
	virtual Sptr <SipMsgQueue> receive(int timeOutMs = -1);

	///
	static void reTransOff();

	///
	static void reTransOn();

	///
	static void setRetransTime(int initial = retransmitTimeInitial ,
				   int max = retransmitTimeMax 
				   /* Default values in TranscevierSymbols.hxx*/ );

	/// ?????????
	void setRandomLosePercent(int percent);

	///
	void updateSnmpData(Sptr < SipMsg > sipMsg, SnmpType snmpType);

	///
	SipTransactionDB::CallLegVector getCallLegMsgs(Sptr < SipMsg >
						       sipmsg);
	///
	void printSize();

	///
	static SipAppContext myAppContext;

	///
	Data getLocalNamePort() const;

    private:
	///
	SipTransceiver(const SipTransceiver&);
	///
	SipTransceiver& operator =(const SipTransceiver& src);
  

	/* ---------------------------------------------------------------
           these will have to be shared iff there has to be thread pooling 
           --------------------------------------------------------------- */

	// we'll do the decoding of sip msgs in the sip thread's context
	// (encoding is in worker's context when it calls send)
	// for the time being this will be in udp, but later move it here
	Fifo <SipMsgContainer *> recvdMsgsFifo;

	Sptr < SipUdpConnection > udpConnection;

	Sptr <SipTcpConnection> tcpConnection;

	SipSentRequestDB sentRequestDB;

	SipSentResponseDB sentResponseDB;

        Sptr < SipDebuggingInterface > debugger;

	///
	void send(SipMsgContainer *msgPtr, const Data& host="",
                  const Data& port="5060");
  
	/// SNMP details data member vriables
	Sptr<SipAgent> sipAgent;

	Data myLocalNamePort;
  
	Data application;
  
	bool natOn;
  
};
 
} // namespace Vocal

#endif
