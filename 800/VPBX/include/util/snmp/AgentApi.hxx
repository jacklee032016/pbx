#ifndef AgentApi_H
#define AgentApi_H
/*
* $Id: AgentApi.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
/*
  This file is used by both the server (RS/FS...) C++ code and the snmp agent process
  (snmpd ) C code.  The snmpd uses only the type definitions.
*/

#if defined(__sparc)|| defined(__SUNPRO_CC)
#if !defined(MAXHOSTNAMELEN)
#include <netdb.h>
#if !defined(MAXHOSTNAMELEN)
#define MAXHOSTNAMELEN 64
#endif /* !defined(MAXHOSTNAMELEN) */
#endif /* !defined(MAXHOSTNAMELEN) */
#endif /* defined(__sparc) */


#ifdef __cplusplus
#include "Sptr.hxx"
#include "SnmpCommon.h"
#include "UdpStack.hxx"
#include "ThreadIf.hxx"
#include "cpLog.h"
#include "VMutex.h"

#endif /* __cplusplus */
#include "AgentRegister.hxx"
#include "AgentApiMibVars.hxx"

#define MANAGERTRAPPORT 33603
#define NWMUISOCKET 33606

/* global inter process communications timeouts in uSec */
/* put in file later */
#define RSAGENTIPCTIMEOUT 1000000

/* return value for statistics not yet implemented */
#define VO_NA_INT -1
#define VO_NA_STRING "Not Implemented"

typedef enum
{
    Get = 1,
    Set,
    Trap,
    Response,
    Register,
    Register_Req
} actionT;

#define PARM1SIZE 128
#define PARM2SIZE 1024
typedef struct
{
    actionT action;
    AgentApiMibVarT mibVariable;
    unsigned long transactionNumber;
    char parm1[PARM1SIZE];
    char parm2[PARM2SIZE];
}
ipcMessage;

#ifdef __cplusplus

static VMutex myLock;

class AgentApi: public ThreadIf
    {
        public:
            AgentApi(ServerType inSrvType = SERVER_Unknown, string appName = "unknown");
            virtual ~AgentApi();

            virtual voReturnStatus processMessage(ipcMessage *message, NetworkAddress *sender) = 0;

            // The following are generally used by the server processes
            voReturnStatus sendTrap(int trapType, string parameter);
            ///
            voReturnStatus sendResponse(int val, NetworkAddress *sender);
            voReturnStatus sendResponse(unsigned long val, NetworkAddress *sender);
            voReturnStatus sendResponse(string parameter, NetworkAddress *sender);
            voReturnStatus sendResponse(void *inData, NetworkAddress *sender);

            //  The following are generally used by the agent processes
            voReturnStatus sendRequest(string indexName, string setValue);
            voReturnStatus sendRequest(string indexName, int setValue);

        protected:
            ///
            virtual void thread();

        private:
            ///
            Sptr < AgentRegister > agentRegister;
            Sptr < UdpStack > udpStack;
            int SockNum, MaxSockNum;
            ipcMessage trapMessage;
            ipcMessage message;
            ipcMessage message1;
            string agentIpStr;
            ServerType myServerType;
            string myApplName;
    };

#endif /* __cplusplus */
#endif /* AgentApi_H */
