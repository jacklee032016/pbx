#ifndef _Sip_Sent_Request__hxx
#define _Sip_Sent_Request__hxx
/*
* $Log: SipSentRequestDB.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipSentRequestDB.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "SipTransactionDB.hxx"

namespace Assist
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
 
}

#endif
