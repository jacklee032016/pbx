#ifndef TRANSFERMSG_H
#define TRANSFERMSG_H
/*
 * $Id: TransferMsg.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */


#include "SipCommand.hxx"
#include "StatusMsg.hxx"

namespace Vocal
{
    
class BaseUrl;

///
class TransferMsg : public SipCommand
{
    public:
        enum TransferMsgForward { transferMsgForward };
        ///
        TransferMsg();
        ///
        TransferMsg(const StatusMsg&, Sptr <BaseUrl> transferTo);
        ///
        TransferMsg(const SipCommand&, Sptr <BaseUrl> transferTo );
        ///
        TransferMsg(const TransferMsg&, enum TransferMsgForward);

        

        ///
        //called by the sipMsg decode.
        TransferMsg( Data& data );
        ///
        TransferMsg& operator =(const TransferMsg&);
        ///
        TransferMsg(const TransferMsg &);

        ///
//        virtual Data encode() const;  // get details and create Message, and return.
        ///
        virtual Method getType() const;
    private:
        void setTransferDetails(const SipMsg&, int type, Sptr <BaseUrl> url);
};
 
} // namespace Vocal

#endif
