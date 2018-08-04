#ifndef SipMethod_HXX
#define SipMethod_HXX

/*
* $Log: SipMethod.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipMethod.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
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
