/*
* $Id: CommandLineTest.cxx,v 1.2 2007/03/02 07:29:49 lizhijie Exp $
*/
#include "CommandLine.hxx"
#include <iostream>
using namespace std;

int main(const int argc, const char** argv)
{
    CommandLine::instance( argc, argv);

    cout << "pshostname: " << CommandLine::instance( ) -> getString( "pshostname" ) << endl;
    ;
    cout << "psremoteport: " << CommandLine::instance( ) -> getInt( "psremoteport" ) << endl;
    ;
    cout << "sipport: " << CommandLine::instance( ) -> getInt( "sipport" ) << endl;
    ;
    cout << "retransmit: " << CommandLine::instance( ) -> getInt( "retransmit" ) << endl;
    ;
    cout << "daemon: " << CommandLine::instance( ) -> getInt( "daemon" ) << endl;
    ;
    cout << "debuglevel: " << CommandLine::instance( ) -> getString( "debuglevel" ) << endl;
    ;

    return 0;
}
