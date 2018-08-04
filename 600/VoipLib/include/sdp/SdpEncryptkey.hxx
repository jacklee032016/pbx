#ifndef SDPENCRYPTKEY_HXX_
#define SDPENCRYPTKEY_HXX_
/*
 * $Id: SdpEncryptkey.hxx,v 1.2 2007/03/01 20:04:27 lizhijie Exp $
 */


#include "global.h"
#include <strstream>
#include <string>
#include "SdpExterns.hxx"
#include "cpLog.h"
#include "SdpExceptions.hxx"

enum EncryptMethod
{
    EncryptMethodUnknown,
    EncryptMethodClear,
    EncryptMethodBase64,
    EncryptMethodURI,
    EncryptMethodPrompt
};

/** k=<method> or
 *  k=<method>:<encryption key>
 */

class SdpEncryptkey
{
    public:
        ///
        SdpEncryptkey()
        {
            encrypt_method = EncryptMethodUnknown;
            encrypt_key = "";
        }
        ///
        SdpEncryptkey(string& s);
        ///
        int getEncryptMethod()
        {
            return encrypt_method;
        }
        ///
        void setEncryptMethod(int method)
        {
            encrypt_method = method;
        }
        ///
        string getEncryptKey()
        {
            return encrypt_key;
        }
        ///
        void setEncryptKey (const string& key)
        {
            encrypt_key = key;
        }
        ///
        void encode (ostrstream& s);
        ///
        bool dump ();

    private:
        ////
        string encrypt_key;
        int encrypt_method;
};
#endif
