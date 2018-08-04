#ifndef COMMAND_LINE_H
#define COMMAND_LINE_H
/*
* $Id: CommandLine.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include <unistd.h>
#include <cstdio>

#if defined(__svr4__)
#include <stdio.h>
#endif

#if 0
#if defined(ASSIST_USE_GETOPT_LONG)
  #include "getopt_long.h"
#else
  #if defined(__linux__)
    #include <getopt.h>
  #endif
#endif
#endif

#include <cassert>
#include <map>
#include <string>
#include "global.h"
#include "Sptr.hxx"
#include "Data.hxx"

/**Singelton class, implements the prtocessing of command line arguments 
in Assist applications. Options available wth default values and mappings 
to query the values for the options
<pre>
  -Option  Query-String     What it means 
 ----------------------------------------------------------------
     'd'  "NO_DAEMON"       "do not fork into background" 
     'r'  "NO_RETRANSMIT"   "disable SIP retransmission"  
     'f'  "LOGFILE"         "specify log file" 
     'v'  "LOGLEVEL"        "specify log level" 
     'R'  "RETRANS_INITIAL" "initial retransmission time in ms" 
     'M'  "RETRANS_MAX"     "maximum retransmission time in ms" 
     'h'  "HEARTBEAT"       "disable multicast heartbeat" 
     's'  "USE_SYSLOG"      "use syslog for logging" 
     'n'  "NO_CONFFILE"     "do not read options from any configuration file 
     'P'  "PSERVER"         "default provisioning server" 
     'S'  "PSERVERBACKUP"   "backup provisioning server" 
     'V'  "VERSION"         "print version and exit" 

For example to check if -d option is given, call interface

    // you must first instantiate the CommandLine object by passing in
    // the argc and argv.

    CommandLine::instance(argc, argv, 
        "[-rd] [-f log-filename] [-v DEBUG_LEVEL] pserver:port redundantpserver:port sip-port");


    if(CommandLine::instance().getInt("NO_DEAMON") == 1)
    {
        //Not running in daemon mode
    }
</pre>
*/
class CommandLine
{
    public:

        ///
        static Sptr < CommandLine > instance (
            const int argc,
            const char** argv,
            const char* applicationUsage =
                " [-rd] [-f log-filename] [-v DEBUG_LEVEL] pserver:port redundantpserver:port sip-port"
        );


        ///
        static Sptr < CommandLine > instance();
        /**Returns the string value of a command line option
          * For example, to get value for option  -v LOG_DEBUG_STACK
          * Call CommandLine::instance().getString("LOGLEVEL")
          * to get value as "LOG_DEBUG_STACK"
          */
        string getString(const string& cmdLineOption);

        /**Returns the int value of a command line option
          * For example, to get value for option  -p 5060
          * Call CommandLine::instance().getInt("PORT")
          * to get value as 5060
          */
        int getInt(const string& cmdLineOption);
        ///
#ifdef WIN32_DEBUGGING
	void print(const int loglevel, const char* file = NULL, int line = 0);
#else
        void print(const int loglevel);
#endif
        ~CommandLine() {};
    private:

        CommandLine();

        /// print out usage information
        void usage(const char* cmd_name) const;

        //Set the default values for all the command line  options supported
        void setDefaultValues ( map <Data, Data>& map);
        ///
        void parseCommandLine( const int argc, 
                               const char**argv, 
                               map <Data, Data>& map );
        ///
        static Sptr < CommandLine > commandLine;
        static const pair < const char*, const char* > cmdLineOptionString[];
        map < Data, Data > myMap;
};

#endif
