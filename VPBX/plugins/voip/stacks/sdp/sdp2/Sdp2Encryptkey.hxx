#ifndef SDP2ENCRYPTKEY_HXX_
#define SDP2ENCRYPTKEY_HXX_
/*
* $Log: Sdp2Encryptkey.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:05:46  lizhijie
* no message
*
* $Id: Sdp2Encryptkey.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"
#include <strstream>

#include "Data.hxx"
#include "Sdp2Externs.hxx"
#include "cpLog.h"
#include "Sdp2Exceptions.hxx"


/** Infrastructure common to VOCAL.
 */
namespace Assist 
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
