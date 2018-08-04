/*
* $Id: testDnsResolver.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/


/*
 * testLoadBalance -- test prog to retrieve the DNS SRV records, sort   
 *              the records, and select one of the records based on
 *              the preference (prioirty) and weight
 *
 */

// #include <cstdlib> 
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#ifdef __linux__
#include <getopt.h>
#endif
#include "DnsResolver.hxx"


extern char *optarg;
extern int optind;

/* local function prototypes */
void usage(void);


/* Max number of name server to be checked: */
const int MAX_NS = 20;


char *progname;


void
usage( void )
{
    fprintf( stderr, "usage: %s domain\n", progname );
    exit(2);
}

void do_test0( DnsResolver& srv )
{
    // test 0:
    cout << "\nTest case 0:\n" << endl;

    // provide function to dynamically reorder the srv records
    // based on random selection given a range of the srv
    // records in the same priority
    if (srv.isReady())
       srv.reOrder();

}   // do_test0()

// single record:- passed
void do_test1( DnsResolver& srv )
{
   srv.clear();
   srv.addSrvRecord(2, 2, 5060, "vanilla.private.vovida.com");
   srv.sortByPreference();
   srv.reOrder();
}   // do_test1()

// unique records:
void do_test2( DnsResolver& srv )
{
   srv.clear();
   srv.addSrvRecord(2, 2, 5060, "vanilla.private.vovida.com");
   srv.addSrvRecord(3, 0, 5060, "chocolate.private.vovida.com");
   srv.addSrvRecord(1, 2, 5060, "strawberry.private.vovida.com");
   srv.sortByPreference();
   srv.reOrder();
}   // do_test2()

// 
void do_test3( DnsResolver& srv )
{
   cout << "\nTest Case 3:\n" << endl;
   srv.clear();
   srv.addSrvRecord(5, 20, 5060, "vanilla.private.vovida.com");
   srv.addSrvRecord(2, 256, 5060, "maple.private.vovida.com");
   srv.addSrvRecord(2, 128, 5060, "blueberry.private.vovida.com");
   srv.addSrvRecord(2, 64, 5060, "bubblegum.private.vovida.com");
   srv.addSrvRecord(2, 32, 5060, "mango.private.vovida.com");
   srv.addSrvRecord(2, 32, 5060, "neopolitan.private.vovida.com");
   srv.addSrvRecord(3, 5, 5060, "chocolate.private.vovida.com");
   srv.addSrvRecord(1, 5, 5060, "strawberry.private.vovida.com");
   srv.sortByPreference();
   srv.reOrder();
   srv.reOrder();
   srv.reOrder();
}  // do_test3()

// 
void do_test4( DnsResolver& srv )
{
   cout << "\nTest Case 4:\n" << endl;
   srv.clear();
   srv.addSrvRecord(1, 8, 5060, "vanilla.private.vovida.com");
   srv.addSrvRecord(1, 256, 5060, "coffee.private.vovida.com");
   srv.addSrvRecord(1, 128, 5060, "blueberry.private.vovida.com");
   srv.addSrvRecord(1, 64, 5060, "bubblegum.private.vovida.com");
   srv.addSrvRecord(1, 32, 5060, "mango.private.vovida.com");
   srv.addSrvRecord(1, 32, 5060, "neopolitan.private.vovida.com");
   srv.addSrvRecord(1, 16, 5060, "chocolate.private.vovida.com");
   srv.addSrvRecord(1, 8, 5060, "strawberry.private.vovida.com");
   srv.sortByPreference();
   for (int i = 0; i <= 9999; i++)
   {
       srv.reOrder();
       if ((i%50) == 0)
	   usleep(10);
   }
   srv.printTally();
}  // do_test4()

// main body
main(int argc, char* argv[])
{
    progname = *argv;

    if (argc < 2)
    {
        usage();
    }

	cpLogSetPriority( LOG_DEBUG );
	cpLogSetLabel("dns");
	// test A record retrieval: e.g. www.yahoo.com
	// cpLog(LOG_DEBUG, "Testing buildARecords()");
    // DnsResolver AResolver( argv[1], ns_t_a, NULL, NULL );

	// test SRV record retrieval: e.g. _sip._udp.cisco.com
	cpLog(LOG_DEBUG, "\n\nTesting buildSRVRecords()");

    const char* st = 0;
    const char* proto = 0;
    if (argc >= 4)
    {
        st = argv[2];
        proto = argv[3];
    } 
    DnsResolver srv( argv[1], ns_t_srv, st, proto );
 
    cpLog( LOG_DEBUG, "\n\nReorder the list again:");
    int num = srv.getNumDnsRecords();
    for (int j=0; j<3; j++)
    {  

        for (int i=0; i<num; i++)
        {
           Sptr<DnsRecord>* rec = srv.getRecord( i );
           cpLog( LOG_DEBUG, "Getting record #%d, target=%s",
                  i,(*rec)->getTarget() );
        }

        cpLog( LOG_DEBUG, "********* Reordering the SRV list");
        srv.reOrder();
       
    }

    // test 0:
    // do_test0(srv);

    // do_test1(srv);

    // do_test2(srv);
    //
    // do_test3(srv);
    //
    // do_test4(srv);
    exit( 0 );

}   // end of main()

