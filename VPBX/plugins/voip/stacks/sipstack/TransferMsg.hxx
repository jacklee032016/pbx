#ifndef TRANSFERMSG_H
#define TRANSFERMSG_H
/*
* $Log: TransferMsg.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:06:38  lizhijie
* no message
*
* $Id: TransferMsg.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "SipCommand.hxx"
#include "StatusMsg.hxx"

namespace Assist
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
 
} // namespace Assist

#endif
