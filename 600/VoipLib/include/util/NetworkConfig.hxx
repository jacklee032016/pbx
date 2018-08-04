#ifndef NETWORKCONFIG_HXX_
#define NETWORKCONFIG_HXX_
/*
 * $Id: NetworkConfig.hxx,v 1.2 2007/03/01 20:04:29 lizhijie Exp $
 */

#include <netdb.h>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <stdio.h>

#include "global.h"
#include "vtypes.h"

/** Singleton class, detects the IPv4 and IPv6 protocol support
 *  on the running host. 
 *  Relies on DNS query to decide what IP ptotocol is supporetd.
 *  In case of dual stack, chooses IPv6 as default address family, 
 *  that can be  over-witten by calling setAddrFamily()
 */
class NetworkConfig
{
   public:
      ///Sets the address family to be PF_INET or PF_INET6
      void setAddrFamily(int addrFamily) { myAddrFamily = addrFamily; };            
      ///Returns the IP address family supported
      const int getAddrFamily() const { return myAddrFamily; };

      ///
      static NetworkConfig& instance();

      ///Returns true, if the host suppprts both IPv4 and IPv6
      bool isDualStack() const { return dualStack; };

   private:
      ///
      NetworkConfig();
      ///
      int init();
      ///
      int myAddrFamily;
      ///
      bool dualStack;
      ///
      static NetworkConfig* myInstance;
};

// NETWORKCONFIG_HXX_
#endif
