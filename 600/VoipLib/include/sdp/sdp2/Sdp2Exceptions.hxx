#ifndef SDP2EXCEPTIONS_HXX_
#define SDP2EXCEPTIONS_HXX_

/*
 * $Id: Sdp2Exceptions.hxx,v 1.2 2007/03/01 20:04:27 lizhijie Exp $
 */


/** Infrastructure common to VOCAL.
 */
namespace Vocal 
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
