#ifndef DnsResolver_H
#define DnsResolver_H
/*
* $Id: DnsResolver.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

//
//  Wrapper for DNS SRV (Server Selection) or A Record

#if defined(__sparc)|| defined(__SUNPRO_CC)
#if !defined(MAXHOSTNAMELEN)
#include <netdb.h>
#if !defined(MAXHOSTNAMELEN)
#define MAXHOSTNAMELEN 64
#endif // !defined(MAXHOSTNAMELEN)
#endif // !defined(MAXHOSTNAMELEN)
#endif // defined(__sparc)

#include <cstdlib>
#include <cstdio>
#include <stdio.h>
#ifdef __linux__
#include <getopt.h>
#endif
#include "sys/types.h"
#include "netinet/in.h"
#include "netdb.h"
#include "errno.h"
#include "arpa/nameser.h"

// need the following, else got:
// src/include/resolv.h:119: invalid exception specifications error
#undef __P
#define __P(args) args
#include <resolv.h>
#include <arpa/inet.h>

#include <vector>
#include <algorithm>

#include "Sptr.hxx"
#include "DnsRecord.hxx"
#include "cpLog.h"



#define PARM1SIZE 128
#define PARM2SIZE 1024


class DnsResolver
{
    typedef vector < Sptr < DnsRecord > >  VECTOR_DNS;

    private:
    ///
    bool     myReady;       // is this object ready to be used?
    int      myNumDns;      // # of Dns records matching the QName
	int      myCurIdx;      // current index to myVDns records
	int      myQType;       // qualified record type: srv or A record
    string   myQName;       // qualified service.protocol.domain name
    string   myTarget;      // domain name

    VECTOR_DNS     myVDns;

    public:
    /// constructor - for testing purposes
    DnsResolver( const char * target);

	/**
	 *  target - e.g. vovida.org
	 *  qType - ns_t_a (A record lookup) or ns_t_srv (SRV record)
	 *  serviceType - e.g. _sip, OK to be null
	 *  protocol - e.g. _udp, OK to be null
     *  (For SRV record type, a qualified name will be built using
     *  "serviceType"."protocol"."target" format)
	 */
	DnsResolver(const char * target,int qType, 
                const char * serviceType, const char * protocol);

    /// destructor
    virtual ~DnsResolver();

	/// 
	bool isReady() const  { return myReady; }

	///
	int getNumDnsRecords() const { return myNumDns; }

	/// do a ns lookup and store result, if any, in myVDns
	void dnsLookUp();

	/** do server selection base on the list of servers already
	    cached locally
	 */
	void reOrder();  
    
    /** return a reference to a DnsRecord given an index, does not
     *  affect/ update myCurIdx
     *  returns myVDns.end() if idx is out of bound
     */
    VECTOR_DNS::iterator getRecord( int idx );

	/** like an iterator, locate the next record in myVDns using
	 *  myCurIdx is an index to the vector of Dns records
	 */
	bool getNextRecord( DnsRecord& rec );

    /** select a DnsRecord based on the callID
     */
    bool selectRecord( const string& callId, DnsRecord& rec );

    /** given a DnsRecord, get the hostname for the domain in the record:
     *  (uses NetworkAddress)
     */
	int getHostByRecord( const DnsRecord& rec, struct hostent* hEnt,
					     char* buf, int buflen );

	/// remove DNS records pointed by myVDns
	void clear();

	/// display dns record error
	void nsError( int error, const char* domain );

	// for debugging only
	void addSrvRecord( int prio, int wt, int port, const string& name ) {
        Sptr < DnsRecord > rec = new DnsRecord;
	    assert (rec != 0);
		rec->setQtype( ns_t_srv );
	    rec->setPriority( prio );
	    rec->setWeight( wt );
	    rec->setPort( port );
	    rec->setTarget( name );
	    myVDns.push_back( rec );
	    myNumDns++;
	    myReady = true;   
	}
	
    /// sort list by priority (lowest number first)
    void sortByPreference( );

	/// clone a copy of the srv vector:
	bool cloneVSrv( VECTOR_DNS& localDns ) { return false; }

    /// 
    void print(int logLevel );

	///
	void printTally( );      // used for debugging only
	

    protected:
    /**  request NameServer to return records matching the given domain, 
	 *   called by dnsLookUp()
	 */
    bool findRecords( void );

    /// invoked by dnsLookUp()
    void loadBalance(void );

	///
	void buildRecords( ns_msg handle, ns_sect section, int responseLen);

	///
	void buildSRVRecords( ns_msg handle, ns_sect section, int responseLen,
                          int numRecords);

	///
	void buildARecords( ns_msg handle, ns_sect section, int responseLen,
                        int numRecords);

	/// sort range of same priority by a random weight factor
    void sortByWeight( int startIdx, int endIdx );

    const u_char * expandName(const u_char *cp, const u_char *msg,
                               char *name, int namelen);

	/// locate a set of records that shares the same priority value
    int findPriorityRange( int startIdx );

	/// calculate the sum of the weights within the given range
    int addWeights( int startIdx, int endIdx );

};

#endif //DnsResolver_H
