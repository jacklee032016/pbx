#ifndef SipMethod_HXX
#define SipMethod_HXX
/*
 * $Id: SipMethod.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */

#include "global.h"
#include "Data.hxx"
#include "SipMsg.hxx"

namespace Vocal
{

/// Convert SIP method (request) enum to Data string
Data methodToData(Method method);

/// Convert SIP method (request) Data string to enum
Method dataToMethod(Data data);
    
} // namespace Vocal

#endif
