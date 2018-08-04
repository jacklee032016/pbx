#ifndef _Sip_Sent_Request__hxx
#define _Sip_Sent_Request__hxx
/*
 * $Id: SipSentRequestDB.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */

#include "SipTransactionDB.hxx"

namespace Vocal
{
    
class SipSentRequestDB : public SipTransactionDB
{
public:
    SipSentRequestDB();
    SipSentRequestDB(int size);
    virtual ~SipSentRequestDB();

private:
    SipSentRequestDB(const SipSentRequestDB&);
    SipSentRequestDB& operator= (const SipSentRequestDB&);
    bool operator==(const SipSentRequestDB&);

public:
    virtual SipMsgContainer* processSend(const Sptr<SipMsg>& msg);

    virtual SipMsgQueue* processRecv(SipMsgContainer* msgContainer);

protected:
    virtual SipTransLevel1Node* getTopNode(const SipTransactionId& id,
				   const Sptr<SipMsg>& msg);
};
 
} // namespace Vocal

#endif
