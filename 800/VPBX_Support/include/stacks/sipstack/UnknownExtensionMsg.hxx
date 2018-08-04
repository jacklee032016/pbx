#ifndef UNKNOWNEXTENSIONMSG_H
#define UNKNOWNEXTENSIONMSG_H
/*
* $Log: UnknownExtensionMsg.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: UnknownExtensionMsg.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "SipCommand.hxx"
#include "BaseUrl.hxx"
#include "Sptr.hxx"

namespace Assist
{
    
///
class UnknownExtensionMsg : public SipCommand
{
    public:
        ///
        enum UnknownExtensionMsgForward { unknownExtensionMsgForward };
        ///
        UnknownExtensionMsg();

        //The listenPort is required to build the INVITE msg, in the Via field.
        UnknownExtensionMsg(Sptr<BaseUrl> url, int listenPort = 5060, int rtpPort = 3456);

        /// this should bot be called by ayone but he SipMsg class
        UnknownExtensionMsg( Data& data );
        ///
        UnknownExtensionMsg& operator =(const UnknownExtensionMsg&);
        ///
        bool operator ==(const UnknownExtensionMsg& src); 
        ///
        UnknownExtensionMsg(const UnknownExtensionMsg&);

        ///
        UnknownExtensionMsg(const UnknownExtensionMsg&, enum UnknownExtensionMsgForward);

        ///
//        virtual Data encode() const;  // get details and create Message, and return.
        ///
        virtual Method getType() const;

    private:
        void setUnknownExtensionMsgDetails(Sptr <BaseUrl> url, int listenPort, int rtpPort);
};
 
} // namespace Assist

#endif
