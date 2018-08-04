#ifndef SIPSNMPDETAILS_HXX
#define SIPSNMPDETAILS_HXX
/*
 * $Id: SipSnmpDetails.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */

#include <cassert>

#include "SipFtrSupportedTable.hxx"
#include "SipUriSupportedTable.hxx"
#include "SipFtrSupportedEntry.hxx"
#include "SipUriSupportedEntry.hxx"
#include "VThread.hxx"
#include "RWMutex.hxx"
#include "symbols.hxx"
#include "SnmpData.hxx"
#include "SipAgent.hxx"


namespace Vocal
{
    
/* Operation Status of the service     */

typedef enum {

    shutdwn = 0,
    reset,
    restart,
    stop,
    rts
} serviceOperStatus;

/* Operation Status of the application */
typedef enum {

    aup = 0,
    adown,
    atesting,
    aunknown,
    ahalted,
    arestarting

} appOperStatus;


///
class SipSnmpDetails
{

    public:
        ///
        friend class SipAgent;
        ///
        typedef map < int , snmpData > stackDataMap;
        ///
        SipSnmpDetails(SipAgent*, const Data& name);
        ///
        virtual ~SipSnmpDetails();
        ///
        serviceOperStatus getserviceOperStatus();
        ///
        void setServiceOperStatus(serviceOperStatus);
        ///
        void setAppOperStatus(appOperStatus);
        ///
        void setServiceStartTime();
        ///
        void setServiceLastChange();
        ///
        long getServiceLastChange();
        ///
        long getServiceStartTime();
        ///
        appOperStatus getAppOperStatus();
        ///
        int ftrSize();
        ///
        string sipFtrSupported(int);
        ///
        int uriSize();
        ///
        string sipUriSupported(int);
        ///
        string getSipOrganization();
        ///
        unsigned long getMaxTransactions();
        ///
        void setSipOrganization(string );
        ///
        unsigned long getSipRequestDfltExpires();
        ///
        void setSipRequestDfltExpires(unsigned int);
        ///
        Data getappName();
        ///
        void setstackdata(int index, const snmpData&);
        ///
        snmpData getstackdata(int index);
    protected:

        void getReadLock(void);
        void getWriteLock(void);
        void getUnLock(void);


    private:
        ///
        long starttime;
        ///
        long lastchange;
        ///
        serviceOperStatus currentStatus;
        ///
        appOperStatus appStatus;
        ///
        SipAgent* sAgent;
        ///

        stackDataMap stackMap;

        Vocal::Threads::RWMutex rwlock;

        Data appName;
};
 
} // namespace Vocal

#endif // AGENT_API_MIB_VARS_HXX
