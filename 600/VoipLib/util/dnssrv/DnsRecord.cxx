/*
 * $Id: DnsRecord.cxx,v 1.2 2007/03/01 20:04:32 lizhijie Exp $
 */

/*
 * DnsRecord.cxx
 *
 */

// #include <cstdlib> 
//#include <stdlib.h>
//#include <stdio.h>
//#include <unistd.h>
// #ifdef __linux__
// #include <getopt.h>
// #endif
#include "DnsRecord.hxx"


DnsRecord::DnsRecord( const DnsRecord& rhs )
{
    cpDebug(LOG_DEBUG, "DnsRecord copy constructor invoked");
    myQtype = rhs.myQtype;
    myPriority = rhs.myPriority;
    myWeight = rhs.myWeight;
    myPort = rhs.myPort;
    myTarget = rhs.myTarget;
    myAddress = rhs.myAddress;
}

DnsRecord& DnsRecord::operator=( const DnsRecord& rhs )
{
    cpDebug(LOG_DEBUG, "DnsRecord assignment operator invoked");
    myQtype = rhs.myQtype;
    myPriority = rhs.myPriority;
    myWeight = rhs.myWeight;
    myPort = rhs.myPort;
    myTarget = rhs.myTarget;
    myAddress = rhs.myAddress;
    return *this;
}

