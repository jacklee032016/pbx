/*
 * $Id: SignalSet.cxx,v 1.2 2007/03/01 20:04:33 lizhijie Exp $
 */


#include "global.h"
#include "SignalSet.hxx"
#include <cassert>


using Vocal::Signals::SignalSet;


SignalSet::SignalSet()
{
    empty();
}


SignalSet::SignalSet(int signum)
{
    empty();
    insert(signum);
}


SignalSet::SignalSet(int * signum, size_t size)
{
    empty();
    insert(signum, size);
}


SignalSet::SignalSet(const vector<int> & signum)
{
    empty();
    insert(signum);
}


SignalSet::SignalSet(const SignalSet & src)
    :   signals(src.signals),
        mySignalNumbers(src.mySignalNumbers)
{
}



SignalSet::~SignalSet()
{
}



SignalSet &
SignalSet::operator=(const SignalSet & src)
{
    if ( this != &src )
    {
        signals = src.signals;
        mySignalNumbers = src.mySignalNumbers;
    }
    return ( *this );
}


void
SignalSet::fill()
{
    #ifndef WIN32
    int rc = sigfillset(&signals);
    assert( rc == 0 );

    mySignalNumbers.clear();

    // Note that this is ridiculously os dependent.
    //
    int signums[] =
    {
        SIGHUP,
        SIGINT,
        SIGQUIT,
        SIGILL,
        SIGTRAP,
        SIGABRT,
        SIGBUS,
        SIGFPE,
        SIGKILL,
        SIGUSR1,
        SIGSEGV,
        SIGUSR2,
        SIGPIPE,
        SIGALRM,
        SIGTERM,
        #ifdef __linux__
        SIGSTKFLT,
        #endif
        SIGCHLD,
        SIGCONT,
        SIGSTOP,
        SIGTSTP,
        SIGTTIN,
        SIGTTOU,
        SIGURG,
        SIGXCPU,
        SIGXFSZ,
        SIGVTALRM,
        SIGPROF,
        SIGWINCH,
        SIGIO,
        #if ! (defined(__FreeBSD__) || defined(__APPLE__))
        SIGPWR
        #endif

    };

    insert(signums, sizeof(signums)/sizeof(signums[0]));
    #endif
}


void
SignalSet::empty()
{
    #ifndef WIN32
    int rc = sigemptyset(&signals);
    assert( rc == 0 );

    mySignalNumbers.clear();
    #endif
}


void
SignalSet::insert(int signum)
{
    #ifndef WIN32
    if ( !contains(signum) )
    {
        int rc = sigaddset(&signals, signum);
        assert( rc == 0 );

        mySignalNumbers.push_back(signum);
    }
    #endif
}

void
SignalSet::insert(int * signum, size_t size)
{
    assert( signum );

    for ( size_t i = 0; i < size; i++ )
    {
        insert(signum[i]);
    }
}


void
SignalSet::insert(const vector < int > & signum)
{
    size_t size = signum.size();

    for ( size_t i = 0; i < size; i++ )
    {
        insert(signum[i]);
    }
}


void
SignalSet::erase(int signum)
{
    #ifndef WIN32
    int rc = sigdelset(&signals, signum);
    assert( rc == 0 );

    if ( contains(signum) )
    {
        // Note that this is O(n), n is the mySignalNumbers.size().
        // I.e n/2 to find + n/2 to erase. *sigh*
        //
        for ( vector < int > ::iterator it = mySignalNumbers.begin();
                it != mySignalNumbers.end();
                it++
            )
        {
            if ( *it == signum )
            {
                mySignalNumbers.erase(it);
                break;
            }
        }
    }
    #endif
}


void
SignalSet::erase(int * signum, size_t size)
{
    assert( signum );

    for ( size_t i = 0; i < size; i++ )
    {
        // Note this is O(n*m), where m is size.
        //
        erase(signum[i]);
    }
}


void
SignalSet::erase(const vector < int > & signum)
{
    size_t size = signum.size();

    for ( size_t i = 0; i < size; i++ )
    {
        // Note this is O(n*m), where m is size.
        //
        erase(signum[i]);
    }
}


bool
SignalSet::contains(int signum) const
{
    int rc = 0;
    #ifndef WIN32
    rc = sigismember(&signals, signum);
    assert ( rc > -1 );
    #endif
    return ( rc == 0 ? false : true );
}


bool
SignalSet::contains(int * signum, size_t size) const
{
    assert( signum );

    for ( size_t i = 0; i < size; i++ )
    {
        if ( !contains(signum[i]) )
        {
            return ( false );
        }
    }
    return ( true );
}


bool
SignalSet::contains(const vector < int > & signum) const
{
    size_t size = signum.size();

    for ( size_t i = 0; i < size; i++ )
    {
        if ( !contains(signum[i]) )
        {
            return ( false );
        }
    }
    return ( true );
}


const vector < int > &
SignalSet::signalNumbers() const
{
    return ( mySignalNumbers );
}


ostream &
SignalSet::writeTo(ostream & out) const
{
    #ifndef WIN32
    bool first = true;
    for ( vector < int > ::const_iterator it = mySignalNumbers.begin();
            it != mySignalNumbers.end();
            it++
        )
    {
        if ( first )
        {
            first = false;
        }
        else
        {
            out << ", ";
        }

        switch ( *it )
        {
            case SIGHUP:    out << "SIGHUP";	break;
            case SIGINT:    out << "SIGINT";	break;
            case SIGQUIT:   out << "SIGQUIT";	break;
            case SIGILL:    out << "SIGILL";	break;
            case SIGTRAP:   out << "SIGTRAP";	break;
            case SIGABRT:   out << "SIGABRT";	break;
            case SIGBUS:    out << "SIGBUS";	break;
            case SIGFPE:    out << "SIGFPE";	break;
            case SIGKILL:   out << "SIGKILL";	break;
            case SIGUSR1:   out << "SIGUSR1";	break;
            case SIGSEGV:   out << "SIGSEGV";	break;
            case SIGUSR2:   out << "SIGUSR2";	break;
            case SIGPIPE:   out << "SIGPIPE";	break;
            case SIGALRM:   out << "SIGALRM";	break;
            case SIGTERM:   out << "SIGTERM";	break;
            #ifdef __linux__
            case SIGSTKFLT: out << "SIGSTKFLT"; break;
            #endif
            case SIGCHLD:   out << "SIGCHLD";	break;
            case SIGCONT:   out << "SIGCONT";	break;
            case SIGSTOP:   out << "SIGSTOP";	break;
            case SIGTSTP:   out << "SIGTSTP";	break;
            case SIGTTIN:   out << "SIGTTIN";	break;
            case SIGTTOU:   out << "SIGTTOU";	break;
            case SIGURG:    out << "SIGURG";	break;
            case SIGXCPU:   out << "SIGXCPU";	break;
            case SIGXFSZ:   out << "SIGXFSZ";	break;
            case SIGVTALRM: out << "SIGVTALRM"; break;
            case SIGPROF:   out << "SIGPROF";	break;
            case SIGWINCH:  out << "SIGWINCH";	break;
            case SIGIO:     out << "SIGIO"; break;
            #if ! (defined(__FreeBSD__) || defined(__APPLE__))
            case SIGPWR:    out << "SIGPWR";	break;
            #endif
            default:
            out << "Unknown Signal";	break;	
		}
	}
    #endif // !WIN32
    return ( out );
}
