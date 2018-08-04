/*
 * $Id: SdpEncryptkey.cxx,v 1.3 2007/06/05 11:33:25 lizhijie Exp $
 */

#include "global.h"
#include <cassert>
#include <strstream>
#include <string>
#include "SdpEncryptkey.hxx"
#include "support.hxx"

///
SdpEncryptkey::SdpEncryptkey(string& s)
{
    // k=<method>:<encryption key> or
    // k=<method> for "prompt" method

    // For "uri" method, the <encryption key> field may contains ":",
    // we can't use split() with ":"
    string tmp_string = s.substr(0, 3);
#if defined (__SUNPRO_CC) || (defined(__GNUC__) && (__GNUC__ > 2)) || defined(__INTEL_COMPILER)
    if (0 == (tmp_string.compare(0,3,"uri")))		// URI method
#elif defined(WIN32)
    if (0 == (tmp_string.compare("uri")))
#else 
    if (0 == (tmp_string.compare("uri",0,3)))
#endif 
    {
        encrypt_method = EncryptMethodURI;
        encrypt_key = s.substr(4);
    }
    else
    {
        split_t encryptkeyList(split(s, ":"));

        if (encryptkeyList.size() == 1)	// prompt method
        {
            if (encryptkeyList[0] == SdpEncryptkeyMethodPrompt)
            {
                encrypt_method = EncryptMethodPrompt;
                encrypt_key = "";
            }
            else
            {
                cpLog(LOG_ERR, "SdpEncryptkey: Undefined Method: %s", encryptkeyList[0].c_str());
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
                cpLog(LOG_ERR, "SdpEncryptkey: Undefined Method: %s", encryptkeyList[0].c_str());
                throw SdpExceptions(UNKNOWN_ENCRYPT_METHOD);
            }

        }
        else  // bad "k=" line
        {
            cpLog(LOG_ERR, "SdpEncryptkey: bad k line: %s", s.c_str());
            throw SdpExceptions(UNKNOWN_ENCRYPT_METHOD);
        }
    }
}

///
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

///
bool SdpEncryptkey::dump ()
{
	cpDebug(LOG_DEBUG, "ENCRYPTION KEY ------------");

	switch (encrypt_method)
	{
		case EncryptMethodClear:
		{
			cpDebug(LOG_DEBUG, "    Method\t\t%s", SdpEncryptkeyMethodClear);
			break;
		}
		case EncryptMethodBase64:
		{
			cpDebug(LOG_DEBUG, "    Method\t\t%s", SdpEncryptkeyMethodBase64);
			break;
		}
		case EncryptMethodURI:
		{
			cpDebug(LOG_DEBUG, "    Method\t\t%s", SdpEncryptkeyMethodURI);
			break;
		}
		case EncryptMethodPrompt:
		{
			cpDebug(LOG_DEBUG, "    Method\t\t%s", SdpEncryptkeyMethodPrompt);
			break;
		}
		default:
		{
			cpDebug(LOG_DEBUG, "    Method\t\t%s", "unknown");
		}
	}

	if (encrypt_key.size() > 0)
	{
		cpDebug(LOG_DEBUG, "    Key\t\t\t%s", encrypt_key.c_str());
	}

	return true;
}

