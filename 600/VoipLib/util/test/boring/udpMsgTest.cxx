/*
* $Id: udpMsgTest.cxx,v 1.2 2007/03/02 07:29:49 lizhijie Exp $
*/
#include <cstdlib>

#include <UdpStack.hxx>
#include <NetworkAddress.h>
#include <Sptr.hxx>
#include <support.hxx>

int
main( int argc, char* argv [] )
{
    if ( argc < 5 )
    {
        cerr << "Usage: "
        << argv[0]
        << " <remote_host>"
        << " <remote_port> "
        << " <local_port> "
        << " <msg_file_name>"
        << " [<#messages>]"
        << " [<interval(ms)>]"
        << endl;
        exit( -1 );
    }

    Sptr < UdpStack > udpStack = new UdpStack;
    int numOfMessages = 1;
    int interval = 2000000;

    // Parse all command line arguments
    //    udpStack->setDestination( argv[1], atoi( argv[2] ) );
    udpStack->setLocal( atoi( argv[3] ) );
    NetworkAddress dest( argv[1], atoi( argv[2] ) );

    std::ifstream inputFile( argv[4] );
    if ( !inputFile )
    {
        cerr << "Error: Cannot open input file " << argv[4] << endl;
        exit( -1 );
    }
    string s;
    char ch;
    while ( inputFile.get( ch ) )
    {
        s += ch;
    }
    if ( argc >= 6 )
    {
        numOfMessages = atoi( argv[5] );
    }
    if ( argc == 7 )
    {
        interval = atoi( argv[6] ) * 1000;
    }
    cout << "\t<<< UDP message test >>>\n" << endl;
    cout << "        Remote Host : " << argv[1] << endl;
    cout << "        Remote Port : " << atoi( argv[2] ) << endl;
    cout << "         Local Port : " << atoi( argv[3] ) << endl;
    cout << "         Input File : " << argv[4] << endl;
    cout << " Number of Messages : " << numOfMessages << endl;
    cout << "           Interval : " << interval << endl;
    cout << endl << "<<< Message to send begin >>>" << endl << s << endl;
    cout << "<<< Message to send end >>>" << endl << endl;

    //TODO Spawn the receiving thread that does something useful

    // Find the location of the Call-ID:
    string::size_type start = s.find( "Call-ID: " ) + 9;
    string::size_type end = s.find( "@", start );
    srandom((unsigned int)time(NULL));
    int tmpCallId;
    string t;

    // Send messages
    for ( int i = 0; i < numOfMessages; i++ )
    {
        cout << "\rSending " << i + 1;
        cout.flush();
        //TODO Modify the message
        tmpCallId = random();
        t = s;
        t.replace( start, end - start, itos( tmpCallId ) );
        udpStack->transmitTo( t.c_str(), t.length(), &dest );
        usleep( interval );
    }
    cout << endl << endl;
    //TODO Wait for the receiving thread to terminate
    exit( 0 );
}
