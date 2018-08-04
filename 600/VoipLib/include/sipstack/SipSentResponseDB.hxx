#ifndef _Sip_Sent_Response__hxx
#define _Sip_Sent_Response__hxx
/*
 * $Id: SipSentResponseDB.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */

#include "SipTransactionDB.hxx"

namespace Vocal
{
    
class SipSentResponseDB : public SipTransactionDB
{
public:
    SipSentResponseDB();
    SipSentResponseDB(int size);
    virtual ~SipSentResponseDB();

private:
    SipSentResponseDB(const SipSentResponseDB&);
    SipSentResponseDB& operator= (const SipSentResponseDB&);
    bool operator==(const SipSentResponseDB&);

public:
    virtual SipMsgContainer* processSend(const Sptr<SipMsg>& msg);

    virtual SipMsgQueue* processRecv(SipMsgContainer* msgContainer);

protected:
    virtual SipTransLevel1Node* getTopNode(const SipTransactionId& id,
				   const Sptr<SipMsg>& msg);
};
 
} // namespace Vocal

#endif
