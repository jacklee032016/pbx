/*
* $Id: Registration.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "global.h"
#include <cassert>

#include "Registration.hxx"
#include "StatusMsg.hxx"
#include "SipContact.hxx"
#include "SipRequestLine.hxx"
#include "Authenticate.hxx"
#include "PbxConfiguration.hxx"

using namespace Assist;

Registration::Registration()
        :  seqNum(0),
        status(0),       
        registerMsg()
{
	compart = false;
	expire = false;
}

Registration::Registration(bool isCompart)
{
	compart = isCompart;
	expire = false;
}

Registration::Registration(const RegisterMsg& srcMsg)
        :seqNum(0),
        status(0),
        registerMsg(srcMsg)
{
	compart = false;
	expire = false;
}


Registration::~Registration()
{
}


Registration::Registration(const Registration& src)
	: registerMsg(src.registerMsg)
{
    status = src.status;
    seqNum = src.seqNum;
    compart = src.compart;
    expire = src.expire;
}

RegisterMsg Registration::getNextRegistrationMsg()
{
	seqNum++;
	SipCSeq cseq = registerMsg.getCSeq();
	cseq.setCSeq(seqNum);
	registerMsg.setCSeq(cseq);
	return registerMsg;
}


RegisterMsg Registration::getNextRegistrationCancel()
{
	SipExpires expires;

	expires.setDelta(Data("0"));
	registerMsg.setExpires(expires);

	//seqNum++;
	SipCSeq cseq = registerMsg.getCSeq();
	cseq.setCSeq(seqNum);
	registerMsg.setCSeq(cseq);

	return registerMsg;
}


SipContact
Registration::findMyContact(const StatusMsg& msg) const
{
    Sptr< BaseUrl > myContactUrl = registerMsg.getContact(0).getUrl();

//    SipUrl myContactUrl = registerMsg.getContact(0).getUrl();
    SipContact myContact;
    int numContacts = msg.getNumContact();
    for ( int i = 0; i < numContacts; i++)
    {
        if ( msg.getContact(i).getUrl()->areEqual(myContactUrl) )
        {
            myContact = msg.getContact(i);
            break;
        }
    }

    return myContact;
}


int
Registration::updateRegistrationMsg(const StatusMsg& msg)
{
    const int DEFAULT_DELAY = 60000;   // 60 sec.
    int delay = 0;

    status = msg.getStatusLine().getStatusCode();

    //handling 200 status message
    //for now, just extract expires out of the message
    //may not be complete
    if ( 200 == status )
    {
        SipContact myContact = findMyContact(msg);
        Data expires = myContact.getExpires().getDelta();
        if ( expires == "" )
        {
            //normally we need to get Delta not Date, but
            //for now the parse is not working correctly
            //so we will use this work around for now and
            //fix it later.
            expires == msg.getExpires().getDelta();
            if ( expires != "" )
            {
                SipExpires sipexpires;
                sipexpires.setDelta(expires);
                registerMsg.setExpires(sipexpires);
            }
        }

        if ( expires != "" )
        {
            SipExpires sipexpires;
            sipexpires.setDelta(expires);
            registerMsg.setExpires(sipexpires);
        }

        delay = getDelay();

        return delay;
    }

    //handle 3xx and 4xx message

    //first thing to do is to get any potential contacts from these messages
    //for now, we simply use the first one in the list.  Eventually we
    //need to maintain a list of unsuccessfully contacted servers and make
    //sure not to contact them again in order to avoid the loop
    if ( status >= 300 && status <= 500 )
    {
        int numContact = msg.getNumContact();

        if ( numContact > 0 )
        {
            //later on, we need to pick the sipcontact that has not be
            //contacted before
            SipContact sipcontact = msg.getContact(0);
            // for now, we will just copy mos of the info in contact to
            // request line. Later, we will need to handle each of 3xx and
            // 4xx individually
            SipRequestLine requestLine = registerMsg.getRequestLine();
            requestLine.setUrl(sipcontact.getUrl());
            registerMsg.setRequestLine(requestLine);
        }

        if ( status != 401 && status != 407 )
        {
            delay = DEFAULT_DELAY;
            cpLog( LOG_ERR, "Register failed" );
            cpLog( LOG_ERR, "Will try again in 60 seconds" );
        }
    }

    // for 401 message, we need to extract the proxy authentication info and
    // add to the register message
    if ( status == 401 || status == 407 )
    {
       // Data user = PbxConfiguration::instance()->getUserName();
        Data user = msg.getTo().getUser().convertString();
        Data password = PbxConfiguration::instance()->getPassword();

	if (!authenticateMessage(msg, registerMsg, user, password))
	{
	    // i could not find auth information, so delay
            delay = DEFAULT_DELAY;
	}

	int cseq ( msg.getCSeq().getCSeqData().convertInt() );
	{
	    if( cseq > 1 )
	    {

		// This is not the first 401 that we received (we may
		// have supplied a bad username or password). Set the
		// default delay to allow the user time to figure it
		// out.

		cpLog( 
		    LOG_ERR, 
		    "Authentication may have failed, check configuration info"
		    );
		delay = DEFAULT_DELAY;
	    }
	}

        cpLog( LOG_WARNING, 
	       "Will try Registration again with authentication information" );

    }

    return delay;
}


void
Registration::setRegistrationMsg(const RegisterMsg& msg)
{
    registerMsg = msg;

    SipCSeq cseq = registerMsg.getCSeq();

    //always use the latest seqNum
    cseq.setCSeq(seqNum);
    registerMsg.setCSeq(cseq);
}


Registration&
Registration::operator = (const Registration& src)
{
    status = src.status;
    seqNum = src.seqNum;
    registerMsg = src.registerMsg;
    return *this;
}


bool
Registration::operator == (Registration& right) const
{
    // Cast away const until someone fixes RegisterMsg's equality
    // operator.
    //
    Registration &  left = const_cast<Registration &>(*this);
    
    return ( left.registerMsg == right.registerMsg );
}


int
Registration::getDelay()
{
    return 1000*registerMsg.getExpires().getDelta().convertInt();
}
