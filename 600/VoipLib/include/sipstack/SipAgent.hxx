#ifndef SipAgent_H
#define SipAgent_H
/*
 * $Id: SipAgent.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
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
    
namespace Vocal
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
        Vocal::Threads::Mutex itsMutex;
        Data name;
};
 
} // namespace Vocal

#endif //SipAgent_H
