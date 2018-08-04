#ifndef DnsRecord_HXX
#define DnsRecord_HXX

/*
 * $Id: DnsRecord.hxx,v 1.2 2007/03/01 20:04:30 lizhijie Exp $
 */


#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string>
#include "cpLog.h"


/** simple 5-tuple DNS SRV/A  record:
 *  priority and weight do no apply to A record
 */
class DnsRecord {

  public:
	/// constructor:
	DnsRecord(): myQtype(0), myPriority(0), myWeight(0),
                 myPort(0), myTarget(""), myAddress("")
	{
#if 0
		// the following was used for debugging
		myTimesSelected[0] = myTimesSelected[1] =      
		myTimesSelected[2] = myTimesSelected[3] = 0;
#endif
     }

    // copy constructor
    DnsRecord( const DnsRecord& rhs );

    // assignment operator
    DnsRecord& operator=( const DnsRecord& rhs );

    /// destructor:
	virtual ~DnsRecord() 
	{
        cpLog( LOG_DEBUG, "~DnsRecord() invoked" ); 
	}

    /// accessors:
    int getQtype()    const { return myQtype; }
    ///
	int getPriority() const { return myPriority; }
    ///
    int getWeight()   const { return myWeight; }
    ///
	int getPort()     const { return myPort; }
    ///
    const char*  getTarget() const { return myTarget.c_str(); }	
    ///
	const char* getAddress() const { return myAddress.c_str(); }
	 
	/// mutators:
    void setQtype( int qt )      { myQtype = qt; }
    ///
	void setPriority( int prio ) { myPriority = prio; }
    ///
	void setWeight( int wt ) { myWeight = wt; }
    ///
	void setPort( int prt ) { myPort = prt; }
    ///
	void setTarget( const string& name ) { myTarget.assign( name ); }
    ///
	void setAddress( const string& addr ){ myAddress.assign( addr ); }

    /// to display content of this record:
	void printRecord( ) {
	    cpLog( LOG_DEBUG,
		"t=%3d   p=%3d    w=%4d  port=%5d  target=%s addr=%s\n",
	    myQtype, myPriority, myWeight, myPort, myTarget.c_str(),
		myAddress.c_str());
	}

	/// increment myTimesSelected[ place ]
	void incrTimesSelected( int idx ) 
	{
#if 0
			myTimesSelected[idx]++;
#endif
    }

	/// clear myTimesSelected[]
	void clearTimesSelected( ) 
	{ 
#if 0
	    myTimesSelected[0] = myTimesSelected[1] =
		myTimesSelected[2] = myTimesSelected[3] = 0;
#endif
	}
    
    /// to verifiy the load balancing algorithm works for SRV records
	void printTally( ) {
#if 0
	    int sum = myTimesSelected[1]+myTimesSelected[2]+myTimesSelected[3];
	    cpLog( LOG_DEBUG,
		"p=%3d    w=%4d   1st=%4d   2nd=%4d   3rd=%4d   sum=%4d   t=%s\n",
	    myPriority, myWeight, myTimesSelected[1], myTimesSelected[2],
		myTimesSelected[3], sum, myTarget.c_str());
#endif
	}

  private:
    /// equality operator - suppressed
    bool operator==( const DnsRecord& rhs );

	int myQtype;                // SRV or A record type
    int myPriority;             // appl. to SRV record only
    int myWeight;               // appl. to SRV record only
	int myPort;                 // appl. to SRV record only
    string myTarget;            // server or host name: "xxx.yy.com"
	string myAddress;           // "a.b.c.d" notation

#if 0
	/** for debug only:
	 *  used by printTally()
	 */
	int myTimesSelected[4];     // # myTimes this record gets picked first,
	                          // second or third (0 not used)
#endif
};

#endif
