#ifndef _Sip_Transaction_DB__hxx
#define _Sip_Transaction_DB__hxx
/*
 * $Id: SipTransactionDB.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */

#include "SipTransactionId.hxx"
#include "SipTransHashTable.hxx"
#include "SipTransactionList.hxx"
#include "SipTransactionLevels.hxx"
#include "SipTransactionGC.hxx"
#include "SipMsgQueue.hxx"
#include "VMutex.h"

namespace Vocal
{
    
#define FILTER_RETRANS_COUNT 1
#define MAX_RETRANS_COUNT    7

class SipTransactionDB
{
public:
    SipTransactionDB();
    SipTransactionDB(int size);
    virtual ~SipTransactionDB();

private:
    SipTransactionDB(const SipTransactionDB&);
    SipTransactionDB& operator= (const SipTransactionDB&);
    bool operator==(const SipTransactionDB&) const;

public:
    /// entry point for transceiver functionality
/**
 * it will be passed the Sptr reference from worker thread, and will return
 * a pointer to a newly created Sip message container that needs to be passed
 * to the transport layer
 */
    virtual SipMsgContainer* processSend(const Sptr<SipMsg>& msg) = 0;

    /// entry point for filter functionality
/**
 * it will be given the Sip message container received from transport layer,
 * and either will return the transaction queue, or will return null and modify
 * the Sip message container w/ the old message to be retransmitted
 *
 * (if the message is droped then msg queue AND the reference to msg in the
 * container will both be 0) 
 */
    virtual SipMsgQueue* processRecv(SipMsgContainer* msgContainer) = 0;

  /**
   * this will return all the messages corresponding to the call leg of
   * the sip message in argument
   */
  typedef vector<Sptr <SipMsg> > CallLegVector;
  virtual CallLegVector getCallLegMsgs(Sptr <SipMsg>& sipMsg);

  Data getDetails();

protected:
/**
 * this is NOT private, 'coz want to lock the Bucket node from find/insert
 * during the whole operation using RWHelper. hence, it all has to be in
 * a single scope
 */
    SipTransHashTable table;

/**
 * cancel the retrans of the given message...
 */
    void cancel(SipMsgContainer* msg);

/**
 * get the top node and update state info specific to DB type
 * (needed here 'coz of getCallLegMsgs)
 */
    virtual SipTransLevel1Node* getTopNode(const SipTransactionId& id,
				   const Sptr<SipMsg>& msg) = 0;
};
 
} // namespace Vocal
#endif
