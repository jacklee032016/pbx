#ifndef SipAgent_H
#define SipAgent_H

/*
* $Log: SipAgent.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:06:38  lizhijie
* no message
*
* $Id: SipAgent.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "SipFtrSupportedTable.hxx"
#include "SipUriSupportedTable.hxx"
#include "AgentApi.hxx"
#include "AgentApiMibVars.hxx"
#include <map>
#include "LockHelper.hxx"
#include "SnmpData.hxx"
#include "Mutex.hxx"
#include "symbols.hxx"
    
namespace Assist
{

class SipSnmpDetails;

class SipAgent: public AgentApi
{
    public:

        ///
        SipAgent(const Data&);
        ///
        virtual ~SipAgent();
        ///
        voReturnStatus updateSipCounter(AgentApiMibVarT inCounterName);
        //
        void setstackData(int, const snmpData& );
        //
        snmpData getstackData(int);
        ///
        void setName(const Data& data);
        ///
        int getSipCounter(AgentApiMibVarT counter);
        ///
        void updateStatusSnmpData(int , SnmpType );
        ///
        void updateCommandSnmpData(Method , SnmpType );

    protected:
        ///
        virtual voReturnStatus processMessage(ipcMessage *message, NetworkAddress *sender);
        ///

        Sptr<SipSnmpDetails> snmpdetails;

    private:
        
        /// this will have the size of the enum defined in AgentApiMibVars
        typedef map < unsigned long, unsigned long > Table;
        typedef Table::iterator TableIter;
        static Table SipCountersTable;
        Assist::Threads::Mutex itsMutex;
        Data name;
};
 
}

#endif
