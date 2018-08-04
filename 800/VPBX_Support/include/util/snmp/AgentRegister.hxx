#ifndef AgentRegister_H
#define AgentRegister_H
/*
* $Id: AgentRegister.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#ifdef __cplusplus

#include <unistd.h>
#include <sys/param.h>
#include <netinet/in.h>
#include "Sptr.hxx"
#include "SnmpCommon.h"
#include "UdpStack.hxx"
#include "ThreadIf.hxx"
#include "cpLog.h"
#endif /* __cplusplus */

/* common between c and c++ code: */
#define agentTrapPort 33602
#define agentCommPort 33604
#define  registerMulticastPort 33616
//#define  registerMulticastPort 12616
#define registerMulticastIP "230.1.2.3"
#ifdef __cplusplus

class AgentRegister: public ThreadIf
{
    public:
        AgentRegister(void *msg, int msgLEN);
        virtual ~AgentRegister();

    protected:
        virtual void thread();
    private:
		//This stack is used to receive multicast messages 
        Sptr < UdpStack > regUdpStack;
		// This stack is used to send unicast messages to the SNMPD
		// Modified by nismail@cisco.com
		Sptr <UdpStack > regTrUdpStack;
        int SockNum, MaxSockNum;
        NetworkAddress dest;
        char *txMessage;
        int txMsgLen;
        char rxMessage[2048];  /* for lack of a better size */
}
;

#endif /* __cplusplus */

#endif
