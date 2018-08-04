/*
* $Id: AgentRegister.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "global.h"
#include <unistd.h>
#include <sys/param.h>
#include <netinet/in.h>
#include "AgentRegister.hxx"
#include "UdpStack.hxx"
#include "cpLog.h"
#include <errno.h>
#include "NetworkConfig.hxx"


/**
 * constructor.  This will send a register message to the snmpd from this agent.
 * @param msg the register message to send
 * @param msgLEN size of msg in bytes
 */
AgentRegister::AgentRegister(void *msg, int msgLEN) : ThreadIf()
{
    const int maxHostNameLen = 128;
    char hostname[maxHostNameLen];
    txMsgLen = msgLEN;
    txMessage = (char *)calloc(txMsgLen, sizeof(char));
    memcpy(txMessage, msg, txMsgLen);
    memset(hostname, 0, sizeof(hostname));
    if (gethostname(hostname, maxHostNameLen ) < 0)
    {
        cpLog(LOG_ERR, "gethostname failed: %s", strerror(errno));
        throw "big fat mess";
    }
    NetworkAddress mcAddr;
    if((NetworkConfig::instance().isDualStack()) ||
       (NetworkConfig::instance().getAddrFamily() == PF_INET))
    {
        mcAddr.setHostName(registerMulticastIP);
        mcAddr.setPort(registerMulticastPort );
    }
    else 
    {
        mcAddr.setHostName("ff14::1");
        mcAddr.setPort(registerMulticastPort );
    }
    dest.setHostName(hostname);
    dest.setPort(agentTrapPort);

    regUdpStack = new UdpStack(&mcAddr, registerMulticastPort, registerMulticastPort, sendrecv, false, true);
    if (regUdpStack == 0)
    {
        cpLog(LOG_ERR, "can't register udp multicast port %d", registerMulticastPort);
        throw"another bad thing happened";
    }

	NetworkAddress iface(hostname);
    regUdpStack->joinMulticastGroup(mcAddr, &iface, 0);

	// A new stack has been created to do the transmission. Contact nismail@cisco.com
	regTrUdpStack = new UdpStack((const NetworkAddress *)&dest, -1, -1, sendonly, false, false);
    if (regTrUdpStack == 0)
    {
        cpLog(LOG_ERR, "can't create a register transmit UDP stack");
        throw"another bad thing happened";
    }

    // Transmission stack is connected as we always send to the same address. contact nismail@cisco.com
    //regTrUdpStack->connectPorts();


	// This is modified to send on the regTrUdpStack instead of the regUdpStack. Contact
	// nismail@cisco.com
    regTrUdpStack->transmitTo(txMessage, txMsgLen, &dest);
}

AgentRegister::~AgentRegister()
{
    if (txMessage) free (txMessage);
    shutdown();
    join();
}

/**
 * periodically send the register message to the snmpd
 */
void
AgentRegister::thread()
{
    int bytesRead = 0;
    NetworkAddress sender;

    while (true)
    {
        try
        {
            memset((void *)&rxMessage, 0, sizeof(rxMessage));
            bytesRead = regUdpStack->receiveTimeout((char *) & rxMessage, sizeof(rxMessage), &sender, 1);

            // There are 3 cases that need to be take care of.
            // 1. Timeout - send the message to the trapport (0)
            // 2. received response - send message to the trapport (>0)
            // 3. an error occured ..... just report an error (-1)
            // If the message we get is either a 0 or greater than that then send a message with
            // the port number and type, name of app, to the snmpd at the port 33602 (trapport ) on which the
            // snmpd will be listening ....
            // Also I will have to construct the message in the constructor.

            if ( bytesRead >= 0)
            {
		// Send on the transmission stack. Contact nismail@cisco.com
                regTrUdpStack->transmitTo(txMessage, txMsgLen, &dest);

            }
            if ( bytesRead == -1)
            {
                cpLog( LOG_ERR, "Error in receiving on port %d", regUdpStack->getRxPort());
            }
        }
#ifdef PtW32CatchAll
        PtW32CatchAll
#else
        catch ( ... )
#endif
        {
            cpLog( LOG_ERR, "Exception from udpstack for AgentRegister" );
        }
        if ( isShutdown() == true )
        {
            return ;
        }
    }
}
