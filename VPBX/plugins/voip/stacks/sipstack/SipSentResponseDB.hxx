#ifndef _Sip_Sent_Response__hxx
#define _Sip_Sent_Response__hxx
/*
* $Log: SipSentResponseDB.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:06:38  lizhijie
* no message
*
* $Id: SipSentResponseDB.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "SipTransactionDB.hxx"

namespace Assist
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
 
}

#endif
