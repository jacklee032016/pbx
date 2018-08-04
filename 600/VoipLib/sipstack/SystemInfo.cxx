/*
 * $Id: SystemInfo.cxx,v 1.4 2007/06/05 11:33:26 lizhijie Exp $
 */

#include <iostream>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <asm/types.h>
#include <linux/rtnetlink.h>
#include "global.h"
#include "cpLog.h"
#include "NetworkAddress.h"
#include "NetworkConfig.hxx"
#include "SystemInfo.hxx"
#include <net/if.h>
#include <fcntl.h>
#include <sys/ioctl.h>

using namespace Vocal;

SystemInfo Vocal::theSystem;

SystemInfo::SystemInfo()
: userName(0),
  displayName(0),
  hostName(0),
  hostIP(0),
  sessionName(0),
  registerDomain(0),
  registerUrl(0),
  mySeq(0)
{
    hostName = new char[NI_MAXHOST+1];
    hostIP = new char[NI_MAXHOST+1];

#ifdef WIN32
    WORD wVersionRequested;
    WSADATA wsaData;

    wVersionRequested = MAKEWORD( 2, 2 );

    err = WSAStartup( wVersionRequested, &wsaData );
    assert(err == 0);
#endif

    makeCopy(&sessionName , "Assist Session");
    makeCopy(&userName , "-");

    //initialize registerDomain.
    makeCopy(&registerDomain , "");
    makeCopy(&registerUrl , "");
    makeCopy(&displayName , "");

	updateHost(NULL);

#if 0
    err = gethostname(hostName, NI_MAXHOST);
    assert( err == 0 );

    // can not call cpLog becuase it may not be constructed yet

    struct addrinfo hints;
    struct addrinfo *res;
    struct sockaddr *sa;

    memset(&hints, 0, sizeof(hints));
    
    /* set-up hints structure */
    hints.ai_family = NetworkConfig::instance().getAddrFamily();

    err = getaddrinfo(hostName, NULL, &hints, &res);

    if (err) {
       cpLog(LOG_ERR, "Failed to resolve the host name, reason %s",
                  gai_strerror(err));
       assert(0);
    } else {
       sa = (struct sockaddr *) res->ai_addr;
       err = getnameinfo(sa, res->ai_addrlen, hostIP, 256, NULL, 0, NI_NUMERICHOST); 
       if (err) {
	  cpLog(LOG_ERR, "Failed to get the host name info, %s" ,hostIP);
          perror("getnameinfo");
          assert(0);
       }

       if (res->ai_family == PF_INET6) { // Enclose IP in ['s
		string tmp(hostIP);
		tmp.insert(0,"[");
		tmp+="]";
		const char* tmp2 = tmp.c_str();
                memcpy((void *)hostIP, (void *)tmp2, tmp.length());
       }
#endif

#if 0
	else if(res->ai_family == PF_INET)
	{
		struct ifreq ifLocal;
		int ret;
		int sd;

		sd=socket(PF_INET,SOCK_DGRAM,0);
		if(sd < 0)
		{	
			cpLog(LOG_ERR,"Sock open error");
			exit(-1);
		}

		strcpy(ifLocal.ifr_name,"ppp0");
		ret=ioctl(sd,SIOCGIFADDR,&ifLocal);
		if(ret)
		{
			strcpy(ifLocal.ifr_name,myInterface.c_str());
			ret=ioctl(sd,SIOCGIFADDR,&ifLocal);
			if(ret)
			{
				close(sd);
				cpLog(LOG_ERR,"No interface found");
				exit(-1);
			}
		}
		close(sd);
		char *p=ifLocal.ifr_addr.sa_data;
		p+=2;
		inet_ntop(res->ai_family,p,hostIP,NI_MAXHOST+1);
		cpDebug(LOG_DEBUG,"Got local host: %s",hostIP);

	}
    }

 
    freeaddrinfo(res);
#endif
/*    char buf[2048];
    int myErrno;

    int retval = NetworkAddress::getHostByName(hostName, &ent, buf, 2048, &myErrno);
    if (retval != NetworkAddress::getHostLookupOK)
    {
        ipName = string("0.0.0.0");
    }
    else
    {
        unsigned char* loc = (unsigned char*)ent.h_addr_list[0];

        char buf[1000];
        sprintf(buf, "%d.%d.%d.%d",
                int(*(loc + 0)),
                int(*(loc + 1)),
                int(*(loc + 2)),
                int(*(loc + 3)) );

        ipName = buf;
    }

    makeCopy(&hostIPAddress, ipName.c_str());
    breakIP();*/

    // can not call cpLog becuase it may not be constructed yet
}


/*
void
SystemInfo::breakIP()
{
    //get hostAddress and get first three octects in this.

    char buf[256];

    // can not call cpLog becuase it may not be constructed yet

    int i1, i2, i3, i4;
    sscanf(hostIPAddress, "%d.%d.%d.%d", &i1, &i2, &i3, &i4);

    sprintf(buf, "%d.%d.%d", i1, i2, i3);
    firstIPpart = new char[strlen(buf) + 1];
    strcpy(firstIPpart, buf);
}*/


void
SystemInfo::makeCopy(char** dest, const char* src)
{
    delete [] *dest;
    *dest = new char[strlen(src) + 1];
    strcpy(*dest, src);
}

SystemInfo::~SystemInfo()
{
    delete [] userName;
    delete [] displayName;
    delete [] hostName;
    delete [] hostIP;
    delete [] sessionName;
    delete [] registerDomain;
    delete [] registerUrl;
}

void
Vocal::parseSystemInfo(char* tag, char* type, char* value)
{
    if (strcmp(tag, "REGISTER") == 0)
    {
        //check for url and domain.
        if (strcmp(type, "url") == 0)
        {
            Vocal::theSystem.setUrlToRegister(value);
        }
        else if (strcmp(type, "domain") == 0)
        {
            if (strcmp(value, "DEFAULT") != 0)
            {
                theSystem.setRegisterDomain(value);
            }
        }
    }
    else if (strcmp(tag, "FROM") == 0)
    {
        //handle the username, and displayname.
        if (strcmp(type, "user") == 0)
        {
            theSystem.setUserName(value);
        }
        else if (strcmp(type, "display") == 0)
        {
            if (strcmp(value, "DEFAULT") != 0)
            {
                theSystem.setDisplayName(value);
            }
        }
    }
    else if (strcmp(tag, "TO") == 0)
    {
        //currently do nothing.
    }

    else if (strcmp(tag, "SESSION") == 0)
    {
        //handle the session name.
        if (strcmp(type, "name") == 0)
        {
            theSystem.setSessionName(value);
        }
    }
}
/*
const char* const 
SystemInfo::getFirstIPpart() const
{ //return the first three octects of the IP address
    cpDebug(LOG_DEBUG_STACK, "FirstIPPart: %s", firstIPpart);
    return firstIPpart;
}*/

bool
SystemInfo::isMe(const NetworkAddress& na)
{
    NetworkAddress me(hostName, mySystemPort);
    return (na == me);
}

void
SystemInfo::updateHost(const Data *destHost)
{
	if(destHost)
	{
		const int RTBUFSIZE=1024;
		int ret;
		int fd;
		int val;
		struct nlmsghdr *msg;
		struct rtmsg *rtm;
		struct rtattr *rta;
		struct in_addr *addr;	
		char data[RTBUFSIZE*sizeof(char)];

		memset(data,0,RTBUFSIZE*sizeof(char));
		
		//nlmsghdr
		msg=(struct nlmsghdr *)data;	
		msg->nlmsg_type=RTM_GETROUTE;	
		msg->nlmsg_flags=NLM_F_REQUEST;	
		msg->nlmsg_seq=++mySeq;
		msg->nlmsg_pid=getpid();

		//rtm
		rtm=(struct rtmsg *)(NLMSG_DATA(msg));
		rtm->rtm_family=AF_INET;
		rtm->rtm_type=RTN_UNICAST;
		
		//rta
		rta=RTM_RTA(rtm);
		rta->rta_type=RTA_DST;
		addr=(struct in_addr *)RTA_DATA(rta);	
		
		//resolve name
		struct hostent *dHostent;	
		dHostent=gethostbyname(destHost->c_str());
		if(!dHostent)	
		{
			cpLog(LOG_ERR,"gethostbyname error");
			return;
		}
		if(dHostent->h_addrtype!=AF_INET)
			return;
		if(*dHostent->h_addr_list)
		{
			memcpy(addr,*dHostent->h_addr_list,dHostent->h_length);
		}
		else
		{
			cpLog(LOG_ERR,"gethostbyname error");
			return;
		}

		//fill length caculated
		rta->rta_len=RTA_LENGTH(sizeof(struct in_addr));
		msg->nlmsg_len=NLMSG_LENGTH(sizeof(struct rtmsg)+rta->rta_len);

		if((fd = socket(PF_NETLINK, SOCK_RAW,NETLINK_ROUTE))<0)	
		{
			cpLog(LOG_ERR,"netlink socket");
			return;
		}
		val=fcntl(fd,F_GETFL,0);
		fcntl(fd,F_SETFL,val|O_NONBLOCK);
		ret=write(fd,msg,msg->nlmsg_len);	
		if(ret < 0)	
		{		
			cpLog(LOG_ERR,"netlink socket send error");
			close(fd);
			return ;	
		}
		ret=read(fd,msg,RTBUFSIZE);
		if(ret < 0)		
		{
			cpLog(LOG_ERR,"netlink socket recv error");
			close(fd);
			return ;		
		}
		close(fd);
		if(msg->nlmsg_seq!=mySeq||msg->nlmsg_pid!=(unsigned)getpid())
		{
			cpLog(LOG_ERR," netlink socket data error");
			return;
		}

		if(msg->nlmsg_type==NLMSG_ERROR)return;
		if(NLMSG_OK(msg,(unsigned)ret))
		{
			int rtalen;
			int rtabuflen;
			
			rtm=(struct rtmsg *)NLMSG_DATA(msg);
			rta=RTM_RTA(rtm);
			rtabuflen=RTM_PAYLOAD(msg);	
//			cpDebug(LOG_DEBUG,"/// got %s: nlmsghdr[len %xH,type %xH,flags %dH] rtmsg[family %d,type %d] ///","ok",
//				msg->nlmsg_len,msg->nlmsg_type,msg->nlmsg_flags,rtm->rtm_family,rtm->rtm_type);

			while(RTA_OK(rta,rtabuflen))
			{
				if(rta->rta_type==RTA_PREFSRC)
				{
					struct in_addr *p;
					
					rtalen=RTA_PAYLOAD(rta);
					p=(struct in_addr *)RTA_DATA(rta);
					strncpy(hostIP,inet_ntoa(*p),NI_MAXHOST);
//					cpDebug(LOG_DEBUG,"/// got local address: %s ///",hostIP);
					return;
				}
				rta=RTA_NEXT(rta,rtabuflen);
			}
		}
	}
	else
	{
		struct addrinfo hints;
		struct addrinfo *res;
		struct sockaddr *sa;
		int err;

		err = gethostname(hostName, NI_MAXHOST);
		assert( err == 0 );

		memset(&hints, 0, sizeof(hints));
		hints.ai_family = NetworkConfig::instance().getAddrFamily();
		err = getaddrinfo(hostName, NULL, &hints, &res);
		if (err) 
		{
			cpLog(LOG_ERR, "Failed to resolve the host name, reason %s",
			gai_strerror(err));
			assert(0);
		} 
		else
		{
			sa = (struct sockaddr *) res->ai_addr;
			err = getnameinfo(sa, res->ai_addrlen, hostIP, 256, NULL, 0, NI_NUMERICHOST); 
			if (err) 
			{
				cpLog(LOG_ERR, "Failed to get the host name info, %s" ,hostIP);
				perror("getnameinfo");
				assert(0);
			}
		}
		if (res->ai_family == PF_INET6) 
		{ // Enclose IP in ['s
			string tmp(hostIP);
			tmp.insert(0,"[");
			tmp+="]";
			const char* tmp2 = tmp.c_str();
			memcpy((void *)hostIP, (void *)tmp2, tmp.length());
	       }
		 freeaddrinfo(res);
	}
}

//for multi-homed host
const char * const SystemInfo::gethostAddress( const Data *destHost)
{
	struct addrinfo hints;

	hints.ai_family = NetworkConfig::instance().getAddrFamily();
	if(hints.ai_family != PF_INET)
		return hostIP;
	
	systemInfoMutex.lock();
	updateHost(destHost);
	systemInfoMutex.unlock();
	return hostIP;
}

const char* const SystemInfo::gethostAddress () //const
{
	struct addrinfo hints;

	hints.ai_family = NetworkConfig::instance().getAddrFamily();
	if(hints.ai_family != PF_INET)
		return hostIP;
	systemInfoMutex.lock();
	updateHost(NULL);
	systemInfoMutex.unlock();
	return hostIP;

#if 0
	struct ifreq ifLocal;
	int ret;
	int sd;

	sd=socket(PF_INET,SOCK_DGRAM,0);
	if(sd < 0)
	{	
		cpLog(LOG_ERR,"Sock open error");
		return hostIP;
	}

	strcpy(ifLocal.ifr_name,"ppp0");
	ret=ioctl(sd,SIOCGIFADDR,&ifLocal);
	if(ret)
	{
		strcpy(ifLocal.ifr_name,myInterface.c_str());
		ret=ioctl(sd,SIOCGIFADDR,&ifLocal);
		if(ret)
		{
			close(sd);
			cpLog(LOG_ERR,"Try Interfaces error");
			return hostIP;
		}
	}
	close(sd);
	char *p=ifLocal.ifr_addr.sa_data;
	p+=2;
	inet_ntop(hints.ai_family,p,hostIP,NI_MAXHOST+1);
	return hostIP;
#endif
}

///
#if 0
const char* const 
SystemInfo::gethostAddress (char* buf, int len, int family) const
{
    struct addrinfo hints;
    struct addrinfo *res;
    struct sockaddr *sa;

    assert(buf);
    memset(&hints, 0, sizeof(hints));
    
    /* set-up hints structure */
    hints.ai_family = family;

    int err = getaddrinfo(hostName, NULL, &hints, &res);

    if (err) {
       cpLog(LOG_ERR, "Failed to resolve the host name, reason %s",
                  gai_strerror(err));
       buf[0] = 0; 
    } else {
       sa = (struct sockaddr *) res->ai_addr;
       err = getnameinfo(sa, res->ai_addrlen, buf, len, NULL, 0, NI_NUMERICHOST); 
       if (err) {
	  cpLog(LOG_ERR, "Failed to get the host name info for %s, reason %s" ,hostName, gai_strerror(err));
          buf[0] = 0;
       }
    }
    freeaddrinfo(res);
    return buf;
}
#endif

