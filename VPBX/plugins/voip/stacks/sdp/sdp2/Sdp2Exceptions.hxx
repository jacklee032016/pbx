#ifndef SDP2EXCEPTIONS_HXX_
#define SDP2EXCEPTIONS_HXX_
/*
* $Log: Sdp2Exceptions.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:05:46  lizhijie
* no message
*
* $Id: Sdp2Exceptions.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

/** Infrastructure common to VOCAL.
 */
namespace Assist 
{


/** Infrastructure in VOCAL related to logging.<br><br>
 */
namespace SDP
{


//TODO use VException base class
//#include "VException.hxx"

enum SDPErrorType
{
    UNKNOWN_NETTYPE,
    UNKNOWN_ADDRTYPE,
    PARAM_NUMERR,
    UNDEF_MODIFIER,
    UNKNOWN_FIELD,
    UNKNOWN_ENCRYPT_METHOD,
    FAILED
};


///class to handle exceptions of SDPMedia.
class SdpExceptions
{
    public:
        SdpExceptions(SDPErrorType i)
        {
            value = i;
        }
        SDPErrorType value;
};


}

}

#endif
