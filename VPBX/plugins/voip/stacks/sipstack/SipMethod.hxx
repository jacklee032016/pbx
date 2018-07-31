#ifndef SipMethod_HXX
#define SipMethod_HXX

/*
* $Log: SipMethod.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:06:38  lizhijie
* no message
*
* $Id: SipMethod.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"
#include "Data.hxx"
#include "SipMsg.hxx"

namespace Assist
{

/// Convert SIP method (request) enum to Data string
Data methodToData(Method method);

/// Convert SIP method (request) Data string to enum
Method dataToMethod(Data data);
    
}

#endif
