#ifndef SipMsgQueue_HXX
#define SipMsgQueue_HXX

/*
* $Log: SipMsgQueue.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipMsgQueue.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "Sptr.hxx"
#include <deque>

namespace Assist
{
class SipMsg;
typedef std::deque < Sptr <SipMsg> > SipMsgQueue;
    
}

#endif
