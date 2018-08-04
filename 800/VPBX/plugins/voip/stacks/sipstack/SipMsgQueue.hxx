#ifndef SipMsgQueue_HXX
#define SipMsgQueue_HXX

/*
* $Log: SipMsgQueue.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:06:38  lizhijie
* no message
*
* $Id: SipMsgQueue.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "Sptr.hxx"
#include <deque>

namespace Assist
{
class SipMsg;
typedef std::deque < Sptr <SipMsg> > SipMsgQueue;
    
}

#endif
