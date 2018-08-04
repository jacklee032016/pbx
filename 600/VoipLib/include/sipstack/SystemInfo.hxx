#ifndef SYSINFO_H
#define SYSINFO_H
/*
 * $Id: SystemInfo.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */

#include "global.h"
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Sptr.hxx"
#include "Data.hxx"

class NetworkAddress;    

namespace Vocal
{
    
class SystemInfo
{
    public:
        ///
        SystemInfo();
        ~SystemInfo();

        //Accessors methods
        const char* const getUrlToRegister()const
        {
            return registerUrl;
        }
        ///
        const char* const getRegisterDomain() const
        {
            return registerDomain;
        }
        ///
        const char* const getUserName() const
        {
            return userName;
        }
        ///
        const char* const getDisplayName() const
        {
            return displayName;
        }
        ///
        const char* const gethostAddress() ;//const;

	const char * const gethostAddress(const Data *destHost);

        ///
//        const char* const gethostAddress (char* buf, int len, int family) const;

        ///
        const char* const gethostName() const
        {
            return hostName;
        }
        ///
        const char* const getSessionName() const
        {
            return sessionName;
        };
        ///
        const char* const getFirstIPpart() const;

        ///Mutators
        void setUrlToRegister(const char* regUrl)
        {
            makeCopy(&registerUrl , regUrl);
        }
        ///
        void setRegisterDomain(const char* regDomain)
        {
            makeCopy(&registerDomain, regDomain);
        }
        ///
        void setUserName(const char* name)
        {
            makeCopy(&userName , name);
        }
        ///
        void setDisplayName(const char* name)
        {
            makeCopy(&displayName , name);
        }
        ///
        void setSessionName(const char* session)
        {
            makeCopy(&sessionName , session);
        }
        ///
        void setSystemPort(int port)
        {
            mySystemPort = port;
        }
        ///
        int getSystemPort()
        {
            return mySystemPort;
        }

        /// return true if the host and port refers to this element
        bool isMe(const NetworkAddress& na);

        friend ostream& operator<<(ostream& os, const SystemInfo& sysInfo);

    private:
        ///
        void breakIP();

        ///
        void makeCopy(char** dest, const char* src);
	void updateHost(const Data *destHost);


        char* userName;
        char* displayName;
        char* hostName;
        char* hostIP;
        char* sessionName;
        char* registerDomain;
        char* registerUrl;
        int mySystemPort;
	unsigned mySeq;

	VMutex systemInfoMutex;
};

///
extern SystemInfo theSystem;
///
extern void parseSystemInfo(char* tag, char* type, char* value);
inline ostream& operator<<(ostream& os, const SystemInfo& sysInfo)
{
    os.width(20);
    os << "User:" << sysInfo.getUserName() << endl;
    os.width(20);
    os << "Display name:" << sysInfo.getDisplayName() << endl;
    os.width(20);
    os << "Host name:" << sysInfo.gethostName() << endl;
    os.width(20);
//    os << "Host IP:" << sysInfo.gethostAddress() << endl;
    os.width(20);
    os << "Session :" << sysInfo.getSessionName() << endl;
    os.width(20);
    os << "Register domain:" << sysInfo.getRegisterDomain() << endl;
    os.width(20);
    os << "Register URL:" << sysInfo.getUrlToRegister() << endl;
    return os;
}

extern void parseSystemInfo(char* tag, char* type, char* value);
 
} // namespace Vocal
#endif
