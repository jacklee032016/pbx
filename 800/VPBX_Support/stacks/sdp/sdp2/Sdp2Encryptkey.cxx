/*
* $Log: Sdp2Encryptkey.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: Sdp2Encryptkey.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "global.h"
#include <cassert>
#include <strstream>
#include <list>
#include <deque>

#include "Data.hxx"
#include "Sdp2Encryptkey.hxx"
#include "support.hxx"


using Assist::SDP::SdpEncryptkey;


SdpEncryptkey::SdpEncryptkey(Data& s)
{
    // k=<method>:<encryption key> or
    // k=<method> for "prompt" method

    // For "uri" method, the <encryption key> field may contains ":",
    // we can't use split() with ":"
//    Data tmp_Data = s.substr(0, 3);

    Data tmp_Data = s.parse(":");

    if (tmp_Data == "uri")
    {
        encrypt_method = EncryptMethodURI;
        encrypt_key = s;
    }
    else
    {
        deque<Data> encryptkeyList;

	encryptkeyList.push_back(tmp_Data);//bug fix by tangr for 'k' description
	bool finished = false;
	
	while (!finished)
	{
	    Data x = s.parse(":", &finished);
	    if(finished)
	    {
		x = s;
	    }
	    encryptkeyList.push_back(x);
	}

        if (encryptkeyList.size() == 1)	// prompt method
        {
            if (encryptkeyList[0] == SdpEncryptkeyMethodPrompt)
            {
                encrypt_method = EncryptMethodPrompt;
                encrypt_key = "";
            }
            else
            {
                cpLog(LOG_ERR, "SdpEncryptkey: Undefined Method: %s", encryptkeyList[0].logData());
                cpLog(LOG_ERR, "SdpEncryptkey: or valid method but no required value");
                throw SdpExceptions(UNKNOWN_ENCRYPT_METHOD);
            }
        }
        else if (encryptkeyList.size() == 2) // clear, base64, or URI
        {
            if (encryptkeyList[0] == SdpEncryptkeyMethodClear)
            {
                encrypt_method = EncryptMethodClear;
                encrypt_key = encryptkeyList[1];
            }
            else if (encryptkeyList[0] == SdpEncryptkeyMethodBase64)
            {
                encrypt_method = EncryptMethodBase64;
                encrypt_key = encryptkeyList[1];
            }
            else if (encryptkeyList[0] == SdpEncryptkeyMethodPrompt)
            {
                // Prompt method should not have value
                // Log a warning and set empty value
                encrypt_method = EncryptMethodPrompt;
                encrypt_key = "";
                cpLog(LOG_WARNING, "SdpEncryptkey: Prompt method shouldn't have value");
            }
            else
            {
                cpLog(LOG_ERR, "SdpEncryptkey: Undefined Method: %s", encryptkeyList[0].logData());
                throw SdpExceptions(UNKNOWN_ENCRYPT_METHOD);
            }

        }
        else  // bad "k=" line
        {
            cpLog(LOG_ERR, "SdpEncryptkey: bad k line: %s", s.logData());
            throw SdpExceptions(UNKNOWN_ENCRYPT_METHOD);
        }
    }
}


void
SdpEncryptkey::encode (ostrstream& s)
{
    switch (encrypt_method)
    {
        case EncryptMethodClear:
	    s << "k=" << SdpEncryptkeyMethodClear << ':' << encrypt_key << "\r\n";
	    break;
        case EncryptMethodBase64:
	    s << "k=" << SdpEncryptkeyMethodBase64 << ':' << encrypt_key << "\r\n";
	    break;
        case EncryptMethodURI:
	    s << "k=" << SdpEncryptkeyMethodURI << ':' << encrypt_key << "\r\n";
	    break;
        case EncryptMethodPrompt:
	    s << "k=" << SdpEncryptkeyMethodPrompt << "\r\n";
	    break;
        default:     // Log an error for unknown method but don't terminate
	    cpLog(LOG_ERR, "SdpEncryptkey: unknown method: %d", encrypt_method);
    }
}    // SdpEncryptkey::encode


bool
SdpEncryptkey::dump ()
{
    cpDebug(LOG_DEBUG, "ENCRYPTION KEY ------------");

    switch (encrypt_method)
    {
        case EncryptMethodClear:
        cpDebug(LOG_DEBUG, "    Method\t\t%s", SdpEncryptkeyMethodClear);
        break;
        case EncryptMethodBase64:
        cpDebug(LOG_DEBUG, "    Method\t\t%s", SdpEncryptkeyMethodBase64);
        break;
        case EncryptMethodURI:
        cpDebug(LOG_DEBUG, "    Method\t\t%s", SdpEncryptkeyMethodURI);
        break;
        case EncryptMethodPrompt:
        cpDebug(LOG_DEBUG, "    Method\t\t%s", SdpEncryptkeyMethodPrompt);
        break;
        default:
        cpDebug(LOG_DEBUG, "    Method\t\t%s", "unknown");
    }
    if (encrypt_key.length() > 0)
        cpDebug(LOG_DEBUG, "    Key\t\t\t%s", encrypt_key.logData());

    return true;
}
