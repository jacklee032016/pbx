/* ====================================================================
 * The Vovida Software License, Version 1.0 
 * 
 * Copyright (c) 2000 Vovida Networks, Inc.  All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 
 * 3. The names "VOCAL", "Vovida Open Communication Application Library",
 *    and "Vovida Open Communication Application Library (VOCAL)" must
 *    not be used to endorse or promote products derived from this
 *    software without prior written permission. For written
 *    permission, please contact vocal@vovida.org.
 *
 * 4. Products derived from this software may not be called "VOCAL", nor
 *    may "VOCAL" appear in their name, without prior written
 *    permission of Vovida Networks, Inc.
 * 
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, TITLE AND
 * NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL VOVIDA
 * NETWORKS, INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT DAMAGES
 * IN EXCESS OF $1,000, NOR FOR ANY INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 * 
 * ====================================================================
 * 
 * This software consists of voluntary contributions made by Vovida
 * Networks, Inc. and many individuals on behalf of Vovida Networks,
 * Inc.  For more information on Vovida Networks, Inc., please see
 * <http://www.vovida.org/>.
 *
 */

static const char* const UaCommandLine_cxx_Version =
    "$Id: UaCommandLine.cxx,v 1.2 2007/03/03 18:39:28 lizhijie Exp $";

#include "global.h"
#if defined(WIN32) || defined(linux)
#if defined(WIN32)
#define __GNU_LIBRARY__
#define __STDC__ 1
#endif // defined(WIN32)
#include "getopt.h"

#else
#include "getopt_long.h"
#endif

#include <cstdio>
#include "parse3tuple.h"
#include "UaCommandLine.hxx"
#include "UaConfiguration.hxx"
#include "cpLog.h"

using namespace Vocal;

Sptr<UaCommandLine> UaCommandLine::commandLine = 0;


UaCommandLine::UaCommandLine()
{
}


const pair<const char*,const char*> UaCommandLine::cmdLineOptionString[] =
{
    pair < const char*, const char* > ( "daemon", "0" ),
    pair < const char*, const char* > ( "cfgfile", "ua.cfg" ),
    pair < const char*, const char* > ( "quicknet", "0" ),
    pair < const char*, const char* > ( "retransmit", "0" ),
    pair < const char*, const char* > ( "loglevel", "4"),
    pair < const char*, const char* > ( "no_heartbeat", "0" ),
    pair < const char*, const char* > ( "voicemail", "0" ),
    pair < const char*, const char* > ( "soundcard", "0" ),
    pair < const char*, const char* > ( "retransinitial", "500" ),
    pair < const char*, const char* > ( "retransmax", "4000" ),
};


void
UaCommandLine::instance( const int argc,
                         const char** argv,
                         const char* applicationUsage )
{
    assert (commandLine == 0 );

    commandLine = new UaCommandLine();
    assert( commandLine != 0 );

    commandLine->setDefaultValues();
    commandLine->parseCommandLine( argc, argv, applicationUsage );
}


void
UaCommandLine::setDefaultValues()
{
    for ( unsigned int i = 0;
          i < sizeof(cmdLineOptionString) / sizeof(cmdLineOptionString[0]);
          i++ )
    {
        cmdLineOptionTable.insert(pair < string, string > (
            UaCommandLine::cmdLineOptionString[i].first,
            UaCommandLine::cmdLineOptionString[i].second ) );
    }
}


static const char* LogLevelStrings [] =
{
    "LOG_EMERG" ,        /* system is unusable */
    "LOG_ALERT" ,        /* action must be taken immediately */
    "LOG_CRIT" ,        /* critical conditions */
    "LOG_ERR" ,        /* error conditions */
    "LOG_WARNING",        /* warning conditions */
    "LOG_NOTICE" ,        /* normal but significant condition */
    "LOG_INFO" ,        /* informational */
    "LOG_DEBUG" ,        /* debug-level messages */
    "LOG_DEBUG_STACK"    /* stack debug-level messages */
};


void
UaCommandLine::parseCommandLine( const int argc,
                                 const char** argv,
                                 const char* applicationUsage )
{
    int c = 0;
    while( ( c = getopt( argc, const_cast<char**>( argv ), "R:M:df:hHqsmrv:" ) ) 
	   != EOF )
    {
        switch( c )
        {
        case 'd':  // daemon flag

            cmdLineOptionTable["daemon"] = "1";
            break;

        case 'f':  // cfgfile
            cmdLineOptionTable["cfgfile"] = optarg;
            break;
            
        case 'h':  // heartbeat
            cmdLineOptionTable["no_heartbeat"] = "1";
            break;

        case 'H': // help
            cerr << "Usage: " << argv[0] << " " << applicationUsage << endl;
            exit( 0 );
            break;

        case 'q':  // quicknet
            // check to make sure that the -m and -s option has NOT been set
            if( cmdLineOptionTable["voicemail"] == "1"  ||
                cmdLineOptionTable["soundcard"] == "1" )
            {
                cpLog(LOG_ERR,"Invalid options: [q|s|m] are mutually exclusive.");
                exit( -1 );
            }
            else
            {
                cmdLineOptionTable["quicknet"] = "1";
            }
            break;

        case 's':  // soundcard
            // check to make sure that the -q and -m  option has NOT been set
            if( cmdLineOptionTable["quicknet"] == "1"  ||
                cmdLineOptionTable["voicemail"] == "1" )
            {
                cpLog( LOG_ERR,"Invalid options: [q|s|m] are mutually exclusive.");
                exit( -1 );
            }
            else
            {
                cmdLineOptionTable["soundcard"] = "1";
            }
            break;

        case 'm':  // voicemail
            // check to make sure that the -q and -s option has NOT been set
            if( cmdLineOptionTable["quicknet"] == "1"  ||
                cmdLineOptionTable["soundcard"] == "1" )
            {
                cpLog( LOG_ERR,"Invalid options: [q|s|m] are mutually exclusive.");
                exit( -1 );
            }
            else
            {
                cmdLineOptionTable["voicemail"] = "1";
            }
            break;

        case 'r':
            // retransmit
            cmdLineOptionTable["retransmit"] = "1";
            break;

        case 'v':
            // log level
            if ( optarg == 0 )
                cmdLineOptionTable["loglevel"] = "7";
            else
            {
                unsigned int loglevel = strConst2i( optarg,
                                                    LogLevelStrings,
                                                    LOG_DEBUG + 1 );

                if ( loglevel <= LOG_DEBUG_STACK )
                {
                    char s[] = { '0' + loglevel, 0 };
                    cmdLineOptionTable["loglevel"] = s;
                }
                else
                {
                    cpLog( LOG_ERR, "Invalid option: %s", optarg);
                    exit( -1 );
                }
            }
            break;

        case 'R':  // get initial retransmission time
        {
            int initial = atoi(optarg);
            cpDebug(LOG_INFO, "setting initial retransmission time to %d ms",
                   initial);
            cmdLineOptionTable["retransinitial"] = optarg;
            break;
        }
        case 'M':  // get initial retransmission time
        {
            int max = atoi(optarg);

            cpDebug(LOG_INFO, "setting maximum retransmission time to %d ms",
                   max);
            cmdLineOptionTable["retransmax"] = optarg;
            break;
        }
        case '?':  // getopt_long already printed an error message
            break;

        default:
            cpLog( LOG_ERR, "getopt() returned 0%o", c );
        }
    }
    if ( ( argc - optind ) > 0 )
    {
        cpLog( LOG_ERR, "Unknown option: %s", argv[ optind ] );
        exit( -1 );
    }
}


Sptr < UaCommandLine >
UaCommandLine::instance()
{
    assert( commandLine != 0 );

    return commandLine;
}


const string&
UaCommandLine::getStringOpt(const string& cmdOption)
{
    static string nullstring = "";

    TableIter i = cmdLineOptionTable.find(cmdOption);

    if ( i != cmdLineOptionTable.end() )
        return i->second;

    return nullstring;
}


int
UaCommandLine::getIntOpt(const string& cmdOption)
{
    TableIter i = cmdLineOptionTable.find(cmdOption);

    int ret = 0;

    if ( i != cmdLineOptionTable.end() )
        ret = atoi(i->second.c_str());

    return ret;
}


bool
UaCommandLine::getBoolOpt(const string& cmdOption)
{
    TableIter i = cmdLineOptionTable.find(cmdOption);

    bool ret = false;

    if ( i != cmdLineOptionTable.end() )
        ret = ( i->second == "1" );

    return ret;
}

