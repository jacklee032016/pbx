#ifndef REGISTERMSG_H
#define REGISTERMSG_H

/*
* $Log: RegisterMsg.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: RegisterMsg.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "SipCommand.hxx"

namespace Assist
{
/**
    Implements REGISTER message.
    <p>
    <b>Example</b>
    <p>
    <pre>
    RegisterMsg registerMsg;

    // Set Request line
    Data reqUrlString;
    SipRequestLine& reqLine = registerMsg.getMutableRequestLine();
    reqUrlString = Data( string("sip:") + toAddress );
    Sptr< SipUrl > reqUrl = new SipUrl( reqUrlString );
    reqLine.setUrl( reqUrl );

    //Set from
    SipFrom sipfrom = registerMsg.getFrom();
    sipfrom.setDisplayName("xyx");
    sipfrom.setUser("xyx");
    sipfrom.setHost(hostAddress);
    sipfrom.setPort(localSipPort );
    registerMsg.setFrom(sipfrom );

    // Set To header
    const Data regToUrlStr = reqUrlString;
    SipUrl regToUrl( regToUrlStr );
    SipTo sipto = registerMsg.getTo();
    sipto.setDisplayName("xyx');
    sipto.setUser("xyx");
    sipto.setHost(regToUrl.getHost() );
    sipto.setPortData(regToUrl.getPort() );
    registerMsg.setTo(sipto );

    // Set Via header
    SipVia sipvia = registerMsg.getVia();
    sipvia.setPort( localSipPort);
    registerMsg.removeVia();
    registerMsg.setVia( sipvia );

    //Add contact
    SipContact myContact;
    Sptr< SipUrl > contactUrl = new SipUrl;
    contactUrl->setUserValue("xyx");
    contactUrl->setPort( localPort );
    myContact.setUrl( contactUrl );
    registerMsg.setNumContact( 0 );
    registerMsg.setContact(myContact);

    SipExpires sipExpires;
    SipExpires.setDelta(3000);
    registerMsg.setExpires( sipExpires );
    //See interface RegistrationManager::addRegistration() for more details
    </pre>
*/
class RegisterMsg : public SipCommand
{
    public:
        //
        enum RegisterMsgForward { registerMsgForward };

        RegisterMsg();
        ///
        virtual ~RegisterMsg(){};
        ///
        RegisterMsg(const Data& data);
        ///
        RegisterMsg(const RegisterMsg&);

        ///
        RegisterMsg(const RegisterMsg& msg, enum RegisterMsgForward);
        ///
        bool operator ==(const RegisterMsg& src);  

        ///
        RegisterMsg& operator =(const RegisterMsg&);
        ///
        virtual Method getType() const;

    private:
        void setRegisterDetails();
};
 
}

#endif
