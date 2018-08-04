/*
* $Id: AgentApi.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "global.h"
#include <unistd.h>
#include <sys/param.h>
#include <netinet/in.h>
#include "AgentApi.hxx"
#include "UdpStack.hxx"
#include "cpLog.h"
#include "NetworkConfig.hxx"


AgentApi::AgentApi(ServerType serType /*Default Argument*/, string appName /*Default Argument*/ ) : ThreadIf()
{
    char hostname[MAXHOSTNAMELEN];
    myServerType = serType;
    myApplName = appName;

    memset(hostname, 0, sizeof(hostname));
    NetworkAddress na;
    agentIpStr = na.getIpName().c_str();

}


///
AgentApi::~AgentApi()
{
    // close ports, deallocate memory
    cpLog( LOG_DEBUG, "entered agentApi destructor");
    shutdown();
    join();
}

/**
 * send a trap message to the trap agentTrapPort on the local machine
 * @param trapType enum representing trap type defined in mib
 * @param parameter text string to include in message
 * @return voReturnStatus
 */
voReturnStatus
AgentApi::sendTrap(int trapType, string parameter)
{
    try
    {
        NetworkAddress dest(agentIpStr, agentTrapPort);
        memset(&trapMessage, 0, sizeof(trapMessage));
        trapMessage.action = (actionT)Trap;
        trapMessage.mibVariable = (AgentApiMibVarT)trapType;
        if (parameter.length() < PARM1SIZE )
        {
            strcpy(trapMessage.parm1, parameter.c_str());
        }
        else
        {
            return voFailure;
        }
        udpStack->transmitTo((char *)&trapMessage, sizeof(trapMessage), &dest);
        return voSuccess;
    }
#ifdef PtW32CatchAll
    PtW32CatchAll
#else
    catch ( ... ) 
#endif
    {
        return voFailure;
    }
}

/**
 * send an integer reaponse to another agentapi 
 * @param val integer value
 * @param sender destination of message
 */
voReturnStatus
AgentApi::sendResponse(int val, NetworkAddress *sender)
{
    try
    {
        message.action = (actionT)Response;
	memset(message.parm2, 0, sizeof(message.parm2));
	memcpy(message.parm2, &val,  sizeof(val));
	if (sender) 
	    cpLog( LOG_DEBUG, "send integer response: %d to %s:%d",
		   val, sender->getHostName().c_str(), sender->getPort());
        udpStack->transmitTo((char *)&message, sizeof(message), sender);
        return voSuccess;
    }
#ifdef PtW32CatchAll
    PtW32CatchAll
#else
    catch ( ... ) 
#endif
    {
        return voFailure;
    }
    return voSuccess;
}

/**
 * send a ulong reaponse to another agentapi 
 * @param val ulong value
 * @param sender destination of message
 */
voReturnStatus
AgentApi::sendResponse(unsigned long val, NetworkAddress *sender)
{
    try
    {
        message.action = (actionT)Response;
	memset(message.parm2, 0, sizeof(message.parm2));
	memcpy(message.parm2, &val, sizeof(val));
        cpLog( LOG_DEBUG, "send ulong response: %d to %s:%d",
               val, sender->getHostName().c_str(), sender->getPort());
        udpStack->transmitTo((char *)&message, sizeof(message), sender);
        return voSuccess;
    }
#ifdef PtW32CatchAll
    PtW32CatchAll
#else
    catch ( ... ) 
#endif
    {
        return voFailure;
    }
    return voSuccess;
}

/**
 * send a string reaponse to another agentapi 
 * @param parameter string value
 * @param sender destination of message
 */
voReturnStatus
AgentApi::sendResponse(string parameter, NetworkAddress *sender)
{
    try
    {
        message.action = (actionT)Response;
        if (parameter.length() < PARM2SIZE )
        {
            strcpy(message.parm2, parameter.c_str());
        }
        else
        {
            return voFailure;
        }
        cpLog( LOG_DEBUG, "send string response: %s to %s:%d",
               parameter.c_str(), sender->getHostName().c_str(), sender->getPort());
        udpStack->transmitTo((char *)&message, sizeof(message), sender);
        return voSuccess;
    }
#ifdef PtW32CatchAll
    PtW32CatchAll
#else
    catch ( ... ) 
#endif
    {
        return voFailure;
    }
}

/**
 * send a reaponse to another agentapi 
 * @param inData data to send in parameter.  must be smaller than PARM2SIZE
 * @param sender destination of message
 */
voReturnStatus
AgentApi::sendResponse(void *inData, NetworkAddress *sender)
{
    try
    {
        message.action = (actionT)Response;
	memset(message.parm2, 0, sizeof(message.parm2));
        memcpy(message.parm2, inData, sizeof(message.parm2));
        cpLog( LOG_DEBUG, "send generic response to %s:%d",
               sender->getHostName().c_str(), sender->getPort());
        udpStack->transmitTo((char *)&message, sizeof(message), sender);
        return voSuccess;
    }
#ifdef PtW32CatchAll
    PtW32CatchAll
#else
    catch ( ... ) 
#endif
    {
        return voFailure;
    }
}

voReturnStatus
sendRequest(string indexName, ServerType serType)
{
    cpLog( LOG_ERR, "Using function not yet implemented 2" );
    return voFailure;
}

voReturnStatus
sendRequest(string indexName, int setValue)
{
    cpLog( LOG_ERR, "Using function not yet implemented 2" );
    return voFailure;
}

/**
 * first it registers on ports based on the type of agent it is, then 
 * infinate loop that waits for messages from other agentApi's and processes them.
 */
void
AgentApi::thread()
{
    int bytesRead = 0;
    NetworkAddress sender;
    NetworkAddress dest;
    if((NetworkConfig::instance().isDualStack()) ||
       (NetworkConfig::instance().getAddrFamily() == PF_INET))
    {
        dest.setHostName(registerMulticastIP);
        dest.setPort(registerMulticastPort );
    }
    else
    {
        dest.setHostName("ff14::1");
        dest.setPort(registerMulticastPort );
    }

    memset(&message1, 0, sizeof(message1));
    myLock.lock();

    switch (myServerType)
    {
        case SERVER_Agent:
        // register to receive the messages on the well known agent port
        // then send out the multicast message.
#ifdef __linux__
	    udpStack = new UdpStack(0, agentTrapPort, agentTrapPort, sendrecv, false);
#else
	    udpStack = new UdpStack(0, -1 , -1 , sendrecv, false);
#endif	
	    message1.action = (actionT)Register_Req;
	    udpStack->transmitTo((char *)&message1, sizeof(message1), &dest);
	    break;

        case SERVER_NetMgnt:
	    udpStack = new UdpStack(0, MANAGERTRAPPORT , MANAGERTRAPPORT, sendrecv, false);
	    break;

        default :

	    udpStack = new UdpStack(0, -1 , -1 , sendrecv, false);

	    memset(&message, 0, sizeof(message));
	    message1.action = (actionT)Register;
	    int *pi = (int *)message1.parm1;
	    int tmpPort = udpStack->getRxPort();
	    memcpy(&pi[0], &myServerType, sizeof(int));
	    memcpy(&pi[1], &tmpPort, sizeof(int));
	    if (sizeof(message1.parm2) < myApplName.length()) 
	    {
		memcpy(message1.parm2, (char*)myApplName.c_str(), 
		       sizeof(message1.parm2));
	    } 
	    else 
	    {
		memcpy(message1.parm2, (char*)myApplName.c_str(), 
		       myApplName.length());
	    }
	    agentRegister = new AgentRegister((void *) & message1, 
					      sizeof(message1));
	    agentRegister->run();
	    break;
    }

    myLock.unlock();

    while (true)
    {
        try
        {
            // Poll every second 

            memset((void *)&message, 0, sizeof(ipcMessage));
            bytesRead = udpStack->receiveTimeout((char *) & message, 
						 sizeof(ipcMessage), 
						 &sender, 
						 0, 100);
            if ( bytesRead )
            {
                cpLog( LOG_DEBUG, "bytes read=%d", bytesRead);
                processMessage(&message, &sender);
            }
        }
#ifdef PtW32CatchAll
    PtW32CatchAll
#else
    catch ( ... ) 
#endif
        {
            cpLog( LOG_ERR, "Exception from udpstack for AgentApiRx" );
        }

        if ( isShutdown() == true )
        {
            return ;
        }
    }
}
