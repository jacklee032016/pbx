/*
 * $Id: Config.cxx,v 1.2 2007/03/01 20:04:32 lizhijie Exp $
 */


#include "global.h"
#include "Config.hxx"
#include "ParsePair.hxx"
#include "VLog.hxx"
#include "Lock.hxx"
#include <unistd.h>
#include <iostream>


using Vocal::Configuration::Config;
using Vocal::Transport::IPAddress;
using Vocal::Threads::ReadLock;
using Vocal::Threads::WriteLock;
using Vocal::Configuration::GetOpt;
using Vocal::Configuration::ParsePair;
using Vocal::Configuration::NameValueMap;
using Vocal::Configuration::NO_VALUE;
using Vocal::Configuration::VALUE_PRESENT;
using Vocal::Configuration::VALUE_OPTIONAL;
using Vocal::Logging::VLog;
using Vocal::ReturnCode;
using Vocal::SUCCESS;
using namespace std;


Config::Config()
    :	cfgStr("config"),
        nodaemonStr("nodaemon"),
        loglevelStr("loglvl"),
        debuggerStr("debugger"),
        xmlcfgStr("xmlcfg"),
        cfgChar('f'),
        nodaemonChar('n'),
        loglevelChar('l'),
        debuggerChar('D'),
        xmlcfgChar('x'),
        myConfigFile(""),
    	myPservers(),
	myPid(getpid()),
	myAsDaemon(true),
        myDaemonRedirectOutput(true),
	myLogLevel(LOG_ERR),
	myUnderDebugger(false),
        myApplicationName(""),
        myXmlCfg(false),
	myMutex()
{
}


Config::~Config()
{
}


ReturnCode
Config::load(int argc, char ** argv)
{
    WriteLock   lock(myMutex);
    
    string  appName = argv[0];
    
    size_t pos = appName.rfind('/');
    if ( pos != string::npos )
    {
        myApplicationName = appName.substr(pos+1);
    }
    else
    {
        myApplicationName = appName;
    }

    ReturnCode  rc = SUCCESS;

    if ( ( rc = preParse() ) != SUCCESS )
    {
        return ( rc );
    }

    // Read the command line
    //    
    if ( (rc = parseCmdLine(argc, argv)) != SUCCESS )
    {
        usage();
        return ( rc );
    }

    // Load the config file
    //
    if ( (rc = loadCfgFile()) != SUCCESS )
    {
        loadCfgFileErrorMsg(rc);
        return ( rc );
    }

    rc = postParse();
        
    return ( rc );
}


string	    	    
Config::configFile() const
{
    ReadLock    lock(myMutex);
    return ( myConfigFile );
}


void    
Config::newPid()
{
    WriteLock    lock(myMutex);
    myPid = getpid();
}


pid_t	    	    
Config::pid() const
{
    ReadLock    lock(myMutex);
    return ( myPid );
}
	
	
bool	    	    
Config::asDaemon() const
{
    ReadLock    lock(myMutex);
    return ( myAsDaemon );
}


bool                
Config::daemonRedirectOutput() const
{
    ReadLock    lock(myMutex);
    return ( myDaemonRedirectOutput );
}


int 	    	    
Config::logLevel() const
{
    ReadLock    lock(myMutex);
    return ( myLogLevel );
}


bool	    	    
Config::underDebugger() const
{
    ReadLock    lock(myMutex);
    return ( myUnderDebugger );
}


string
Config::applicationName() const
{
    ReadLock    lock(myMutex);
    return ( myApplicationName );
}


bool                
Config::xmlConfig() const
{
    ReadLock    lock(myMutex);
    return ( myXmlCfg );
}


const NameValueMap 
Config::options() const
{
    ReadLock    lock(myMutex);
    return ( myOptions.options() );
}


ostream & 
Config::writeTo(ostream & out) const
{
    ReadLock    lock(myMutex);

    out << myApplicationName << ": " << __DATE__ << ", " << __TIME__ 
        << "\n  pid:                   " << myPid
	<< "\n  log level:             " << VLog::logName(myLogLevel)
    	<< "\n  as daemon:             " << ( myAsDaemon ? "yes" : "no" )
        << "\n  config file:           " << myConfigFile;
    
    if ( myConfigFile.size() && myXmlCfg )
    {
        out << " as XML";
    }
    
    out << "\n  under debugger:        " << ( myUnderDebugger ? "yes" : "no" );
    
    return ( out );
}


void        
Config::usage() const
{
}
        

void        
Config::loadCfgFileErrorMsg(ReturnCode rc) const
{
    cerr << "Could not load config file: " << myConfigFile
         << ", error = " << rc << endl;
}


ReturnCode      
Config::preParse()
{
   ReturnCode rc = SUCCESS;
   
    if  (   ( rc = myOptions.add(cfgStr, cfgChar, VALUE_OPTIONAL) ) 
                != SUCCESS
                
        ||  ( rc = myOptions.add(nodaemonStr, nodaemonChar) ) 
                != SUCCESS
                
        ||  ( rc = myOptions.add(loglevelStr, loglevelChar, VALUE_PRESENT) ) 
                != SUCCESS
                
        ||  ( rc = myOptions.add(debuggerStr, debuggerChar) ) 
                != SUCCESS
                
        ||  ( rc = myOptions.add(xmlcfgStr, xmlcfgChar) ) 
                != SUCCESS
        )
    {
        // Using conditional for early exit.
    }
    return ( rc );
}
        
        
ReturnCode      
Config::postParse()
{
    // Load nodaemon value.
    //
    if ( myOptions.get(nodaemonStr) )
    {
        myAsDaemon = false;
    }
    
    // Load log level value.
    //
    string log_level;
    if ( myOptions.get(loglevelStr, log_level) && log_level.size() )
    {
        myLogLevel = VLog::logValue(log_level);
    }

    // Load underdebugger value.
    //
    if ( myOptions.get(debuggerStr) )
    {
        myUnderDebugger = true;
    }

    return ( SUCCESS );
}
	

ReturnCode  	
Config::parseCmdLine(int argc, char ** argv)
{
    ReturnCode  rc = SUCCESS;

    if ( ( rc = myOptions.parse(argc, argv) ) != SUCCESS )
    {
        return ( rc );
    }

    // Load config file value.
    //
    if  ( myOptions.get(cfgStr, myConfigFile) && myConfigFile.size() == 0 )
    {
        myConfigFile = myApplicationName + ".conf";
    }

    // Load xmlcfg value.
    //
    if ( myOptions.get(xmlcfgStr)  )
    {
        myXmlCfg = true;
    }

    return ( rc );
}


ReturnCode  	
Config::loadCfgFile()
{
    if ( myConfigFile.size() == 0 )
    {
        // No config file to load. That's ok.
        //
        return ( SUCCESS );
    }

    ParsePair   cfgFile;

    cfgFile.tagName(myApplicationName);
    
    ReturnCode  rc = cfgFile.parse(
                        myXmlCfg ? ParsePair::CFG_XML_FILE 
                                 : ParsePair::CFG_FILE,
                        myConfigFile);
    
    if ( rc == SUCCESS )
    {
        const NameValueMap    & pairs = cfgFile.pairs();

        NameValueMap::const_iterator i;

        for ( i = pairs.begin(); i != pairs.end(); ++i )
        {
            myOptions.addValue(i->first, i->second);
        }    
    }    
    
    return ( rc );
}


