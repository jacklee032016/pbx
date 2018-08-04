/*
 * $Id: Daemon.cxx,v 1.2 2007/03/01 20:04:32 lizhijie Exp $
 */


#include "global.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <stdlib.h>
#include "cpLog.h"
#include "Daemon.hxx"
#include <assert.h>
#include <stdio.h>
#include <errno.h>

int Daemon(bool stdout_to_file)
{
#if !defined(__vxworks) && !defined(WIN32)

    pid_t pid;

    // start as daemon as default, but let environment variable override
    char* envValString = getenv( "NO_DAEMON" );
    if ( envValString )
    {
        if ( atoi( envValString ) != 0 )
        {
            cerr << "NO_DAEMON environment variable set" << endl;
            return 0 ;
        }
    }

    if ( ( pid = fork() ) < 0 )
    {
        cerr << "first fork failed!" << endl;
        return -1 ;
    }
    else if ( pid != 0 )
    {
        exit ( 0 );
    }

    setsid();

    if ( ( pid = fork() ) < 0 )
    {
        cerr << "second fork failed!" << endl;
        return -1 ;
    }
    else if ( pid != 0 )
    {
	// output this to the appropriate place ?

        exit( 0 );
    }

    setsid();

    umask( 0 );

    close( 0 );
    int fd;
    fd = open( "/dev/null", O_RDONLY );

    if ( fd == -1 )
    {
        cerr << "reopen stdin failed!" << endl;
        return -1;
    }

    close( 1 );
    if ( stdout_to_file )
    {
        fd = open( "stdout.log", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR );
    }
    else
    {
        fd = open( "/dev/null", O_WRONLY );
    }

    if ( fd == -1 )
    {
        perror("reopen stdout failed");
        return -1;
    }

    close( 2 );
    
    if ( stdout_to_file )
    {
        fd = dup( 1 );
    }
    else
    {
        fd = open( "/dev/null", O_WRONLY );
    }

    if ( fd == -1 )
    {
        perror("reopen stderr failed");
	cout << "reopen stderr failed: " << strerror(errno) << endl;
        return -1;
    }
    return 0;

#else
    cerr << "Daemon not defined in vxworks" << endl;
    assert(0);

    return 0;
#endif
}
