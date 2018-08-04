#ifndef _SDPEXCEPTIONS_H
#define _SDPEXCEPTIONS_H
/*
* $Log: SdpExceptions.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:05:22  lizhijie
* no message
*
* $Id: SdpExceptions.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
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
