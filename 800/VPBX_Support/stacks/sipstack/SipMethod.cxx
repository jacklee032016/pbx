/*
* $Log: SipMethod.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipMethod.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "global.h"
#include "SipMethod.hxx"
#include <cassert>

using namespace Assist;


Data 
Assist::methodToData(Method method)
{
    Data data;

    switch (method)
    {
    case SIP_INVITE :
        data = INVITE_METHOD;
        break;
    case SIP_CANCEL:
        data = CANCEL_METHOD;
        break;
    case SIP_ACK:
        data = ACK_METHOD;
        break;
    case SIP_BYE:
        data = BYE_METHOD;
        break;
    case SIP_REGISTER:
        data = REGISTER_METHOD;
        break;
    case SIP_OPTIONS:
        data = OPTIONS_METHOD;
        break;
    case SIP_INFO:
        data = INFO_METHOD;
        break;
    // TODO: remove case for TRANSFER (deprecated)
    case SIP_TRANSFER:
        data = TRANSFER_METHOD;
        break;
    case SIP_REFER:
        data = REFER_METHOD;
        break;
    case SIP_SUBSCRIBE:
        data = SUBSCRIBE_METHOD;
        break;
    case SIP_NOTIFY:
        data = NOTIFY_METHOD;
        break;
    case SIP_MESSAGE:
        data = MESSAGE_METHOD;
        break;
    default:
        assert(0);
        break;
    }
    return data;
}


Method 
Assist::dataToMethod(Data data)
{
    Method type = SIP_UNKNOWN;

    if (data == INVITE_METHOD)
    {
	type = SIP_INVITE;
    }
    else if (data == CANCEL_METHOD)
    {
	type = SIP_CANCEL;
    }
     else if (data == REFER_METHOD)
    {
	type = SIP_REFER;
    }
    else if (data == ACK_METHOD)
    {
	type = SIP_ACK;
    }
    else if (data == BYE_METHOD)
    {
	type = SIP_BYE;
    }
    else if (data == INFO_METHOD)
    {
	type = SIP_INFO;
    }
    else if (data == OPTIONS_METHOD)
    {
	type = SIP_OPTIONS;
    }
    else if (data == REGISTER_METHOD)
    {
	type = SIP_REGISTER;
    }
#if 0
    // TODO: remove case for TRANSFER (deprecated)
    else if (data == TRANSFER_METHOD)
    {
        type = SIP_TRANSFER;
    }
#endif
    else if (data == SUBSCRIBE_METHOD)
    {
	type = SIP_SUBSCRIBE;
    }
    else if (data == NOTIFY_METHOD)
    {
	type = SIP_NOTIFY;
    }
    else if (data == "SIP/2.0")
    {
        type = SIP_STATUS;
    }
    else if (data == MESSAGE_METHOD)
    {
        type = SIP_MESSAGE;
    }
    
    return type;
}
