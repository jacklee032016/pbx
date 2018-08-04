/*
 * $Id: SipMsgQueue.cxx,v 1.2 2007/03/01 20:04:31 lizhijie Exp $
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
