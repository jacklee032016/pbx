#ifndef _SDPEXCEPTIONS_H
#define _SDPEXCEPTIONS_H
/*
 * $Id: SdpExceptions.hxx,v 1.2 2007/03/01 20:04:27 lizhijie Exp $
 */


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

#endif
