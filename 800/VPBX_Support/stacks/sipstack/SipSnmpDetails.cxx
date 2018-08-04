/*
* $Log: SipSnmpDetails.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:10  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipSnmpDetails.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include <iostream>
#include <sys/time.h>

#include "SipSnmpDetails.hxx"
#include "SnmpData.hxx"
#include "Data.hxx"

using namespace Assist;

SipSnmpDetails::SipSnmpDetails(SipAgent* sipagent, const Data& name)
    : stackMap()
{
    currentStatus = rts;
    appStatus = aunknown;

    appName = name;
    //do all the initial settings over here
    setServiceOperStatus(restart);
    setAppOperStatus(aup);
    setServiceStartTime();
    setServiceLastChange();
   

        getWriteLock();
        for (int i = agentApiMibVarFirstEntry; i < agentApiMibVarLastEntry; i++)
        {

            if ( (AgentApiMibVarT)i == sipProtocolVersion)

            {  //done


                snmpData vc(false, false);
                vc.setData("SIP/2.0");
                setstackdata( i , vc);

            }
            else if ( (AgentApiMibVarT)i == sipServiceOperStatus)
            {  //done

                snmpData vc(false, false);
                vc.setData(Data(getserviceOperStatus()));
                setstackdata( i , vc);

            }
            else if ( (AgentApiMibVarT)i == sipServiceAdminStatus)
            {
                snmpData vc(false, false);
                vc.setData(Data(getAppOperStatus()));
                setstackdata( i , vc);

            }
            else if ( (AgentApiMibVarT)i == sipServiceStartTime)
            {

                snmpData vc(false, false);
                vc.setData(Data(int(getServiceStartTime())));
                setstackdata( i, vc);

            }
            else if ( (AgentApiMibVarT)i == sipServiceLastChange)

            {
                snmpData vc(false, false);
                vc.setData(Data(int(getServiceLastChange())));
                setstackdata( i, vc);

            }
            else if ( (AgentApiMibVarT)i == sipTransport)

            {
                snmpData vc(false, false) ;
                vc.setData("Not Implemented");
                setstackdata( i, vc);

            }
            else if ( (AgentApiMibVarT)i == sipPortStatus)

            {
                snmpData vc(false, false);
                vc.setData("Not Implemented");
                setstackdata( i , vc);

            }
            else if ( (AgentApiMibVarT)i == sipOrganization)

            {
                snmpData vc(false, false);
                vc.setData(getSipOrganization());
                setstackdata( i, vc);

            }
            else if ( (AgentApiMibVarT)i == sipMaxTransactions)

            {
                snmpData vc(false, false);
                vc.setData(Data(getMaxTransactions()));
                setstackdata( i, vc);

            }
            else if ( (AgentApiMibVarT)i == sipRequestDfltExpires)

            {
                snmpData vc(false, false);
                vc.setData(Data(getSipRequestDfltExpires()));
                setstackdata( i , vc);

            }
            else if ( (AgentApiMibVarT)i == sipHideOperation)

            {
                snmpData vc(false, false);
                vc.setData("Not Implemented");
                setstackdata( i , vc);

            }
            else if ( (AgentApiMibVarT)i == sipUserLocationServerAddr)

            {
                snmpData vc(false, false);
                vc.setData("Not implemented");
                setstackdata( i , vc);
            }

            else
                //  default:
            {
                snmpData vc(false, false);
                setstackdata( i , vc);

            }
        }
        getUnLock();
        sAgent = sipagent;

	//   } //non empty name
}



SipSnmpDetails::~SipSnmpDetails()
{}



serviceOperStatus
SipSnmpDetails::getserviceOperStatus()
{
    return currentStatus;
}


void
SipSnmpDetails::setServiceOperStatus(serviceOperStatus a)
{
    currentStatus = a;
}


void
SipSnmpDetails::setServiceStartTime()
{
    time_t curtime;
    struct tm *loctime;
    curtime = time (NULL);
    struct tm res;
#if defined(__APPLE__)
    loctime = localtime (&curtime);
#else
    loctime = localtime_r (&curtime, &res);
#endif
    starttime = mktime(loctime);
}


void
SipSnmpDetails::setServiceLastChange()
{
    time_t curtime;
    struct tm *loctime;
    curtime = time (NULL);
    struct tm res;
#if defined(__APPLE__)
    loctime = localtime (&curtime);
#else
    loctime = localtime_r (&curtime, &res);
#endif
    lastchange = mktime(loctime);
}


long
SipSnmpDetails::getServiceLastChange()
{
    return lastchange;
}


long
SipSnmpDetails::getServiceStartTime()
{
    return starttime;
}


appOperStatus
SipSnmpDetails::getAppOperStatus()
{
    return appStatus;
}


void
SipSnmpDetails::setAppOperStatus(appOperStatus a)
{
    appStatus = a;
}


int
SipSnmpDetails::ftrSize()
{
    sipFtrSupportedTable ftrTable;
    return ftrTable.getSize();
}


string
SipSnmpDetails::sipFtrSupported(int index)
{
    sipFtrSupportedTable ftrTable;
    return ftrTable.getEntry(index).getSnmpAdminString();
}


int
SipSnmpDetails::uriSize()
{
    SipUriSupportedTable uriTable;
    return uriTable.getSize();
}


string
SipSnmpDetails::sipUriSupported(int index)
{
    SipUriSupportedTable uriTable;
    return uriTable.getEntry(index).getSnmpAdminString();
}


string
SipSnmpDetails::getSipOrganization()
{
string s;
    return s;
}


unsigned long
SipSnmpDetails::getMaxTransactions()
{
    return 1;
}

void
SipSnmpDetails::setSipOrganization(string s)
{
}


unsigned long
SipSnmpDetails::getSipRequestDfltExpires()
{
    return 32;
}


void
SipSnmpDetails::setSipRequestDfltExpires(unsigned int expires)
{}

/*
   ( This function should be read/write)
    
    uint  SipSnmpDetails::getSipRequestDfltExpires() { return 1;}
    
    void  SipSnmpDetails::setHideOperation(HideOper ) {}
    HideOper  SipSnmpDetails::getHideOperation(){ return h;}
    
    void  SipSnmpDetails::UserLocationServAddr(string) { location =string; }
    string  SipSnmpDetails::getUserLocationServAddr() {return location; }
    


voReturnStatus
SipSnmpDetails::updateSipCounter(AgentApiMibVarT inCounterName)
{
    voReturnStatus x = sAgent->updateSipCounter(inCounterName);
    return x;
}


*/


Data SipSnmpDetails::getappName()
{
    return appName;
}


void SipSnmpDetails::getReadLock(void)
{
    rwlock.readlock();
}


void SipSnmpDetails::getWriteLock(void)
{
    rwlock.writelock();
}


void SipSnmpDetails::getUnLock(void)
{
    rwlock.unlock();
}


void SipSnmpDetails::setstackdata(int index, const snmpData& data)
{
    stackMap[index] = data;

}
snmpData SipSnmpDetails::getstackdata(int index)
{
    snmpData stdata(false, false);
    getReadLock();
    stackDataMap::iterator i = stackMap.find(index);
    if (i != stackMap.end())
    {
        stdata = i->second;


    }
    getUnLock();
    return stdata ;
}
