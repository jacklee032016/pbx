#ifndef SDP2ENCRYPTKEY_HXX_
#define SDP2ENCRYPTKEY_HXX_
/*
 * $Id: Sdp2Encryptkey.hxx,v 1.2 2007/03/01 20:04:27 lizhijie Exp $
 */

#include "global.h"
#include <strstream>

#include "Data.hxx"
#include "Sdp2Externs.hxx"
#include "cpLog.h"
#include "Sdp2Exceptions.hxx"


/** Infrastructure common to VOCAL.
 */
namespace Vocal 
{


/** Infrastructure in VOCAL related to logging.<br><br>
 */
namespace SDP
{


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
        SdpEncryptkey(Data& s);
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
        Data getEncryptKey()
        {
            return encrypt_key;
        }
        ///
        void setEncryptKey (const Data& key)
        {
            encrypt_key = key;
        }
        ///
        void encode (ostrstream& s);
        ///
        bool dump ();

    private:
        ////
        Data encrypt_key;
        int encrypt_method;
};


}


}

#endif
