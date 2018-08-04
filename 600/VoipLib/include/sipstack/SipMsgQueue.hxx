#ifndef SipMsgQueue_HXX
#define SipMsgQueue_HXX

/*
 * $Id: SipMsgQueue.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */

#include "Sptr.hxx"
#include <deque>

namespace Vocal
{

class SipMsg;
typedef std::deque < Sptr < SipMsg > > SipMsgQueue;
    
} // namespace Vocal

#endif
