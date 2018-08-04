/*
* $Log: SipMsgQueue.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipMsgQueue.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "global.h"
#include "SipMsg.hxx"
#include "SipMsgQueue.hxx"
#include "cpLog.h"

/*
Sptr < SipMsgQueue > deepCopyMsgQueue(Sptr < SipMsgQueue > queue)
{
    Sptr < SipMsgQueue > newQueue = 0;

    if (queue != 0)
    {
        newQueue = new SipMsgQueue;

        SipMsgQueue::const_iterator i;

        for (i = queue->begin(); i != queue->end() ; ++i)
        {
            Sptr < SipMsg > msg = *i;
            if (msg != 0)
            {
                newQueue->push_back(copyPtrtoSptr(&(*msg)));
            }
            else
            {
                newQueue->push_back(0);
            }
        }
    }
    return newQueue;
}
*/

