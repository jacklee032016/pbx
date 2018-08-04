#if !defined(VOCAL_CONFIG_HXX)
#define VOCAL_CONFIG_HXX
/*
 * $Id: Config.hxx,v 1.2 2007/03/01 20:04:29 lizhijie Exp $
 */

#include "RWMutex.hxx"
#include "IPAddress.hxx"
#include "GetOpt.hxx"
#include "NameValue.hxx"
#include "Writer.hxx"
#include <string>
#include <list>


/** Vovida Open Communication Application Library.<br><br>
 */
namespace Vocal 
{


/** 
 */
namespace Configuration
{


using Vocal::IO::Writer;
using Vocal::Transport::IPAddress;
using Vocal::Threads::RWMutex;
using Vocal::ReturnCode;
//using Vocal::Configuration::GetOpt;
//using Vocal::Configuration::NameValueMap;


/** 
 */
class Config : public Vocal::IO::Writer
{
    public:


    	/** Creates the configuration information. The defaults are
	 *  set here.
	 */
    	Config();


    	/** Destroys the configuration information.
    	 */
    	virtual ~Config();


    	/** Loads the configuration information from the command line.
	 *  The command line may in turn load information from 
	 *  a configuration and/or a provisioning server.
	 */
	ReturnCode	    load(int, char **);

    
    	/** If set, it is the configuration file used to configure the
	 *  application.
	 */
	string	    	    configFile() const;
	
	
        /** Get the new pid. Useful if you have been Daemon-ed.
         */
        virtual void        newPid();
        
        
    	/** Accessor for process id. A good thing to know.
	 */
	pid_t	    	    pid() const;
	
	
	/** Flag indicates whether we are running as a daemon or not.
	 */
	bool	    	    asDaemon() const;


        /** Set to true if when the application runs as a daemon, it
         *  is to redirect stdout and stderr to a file.
         */
        bool                daemonRedirectOutput() const;


    	/** Flag indicates at which log level to run.
	 */
	int 	    	    logLevel() const;
	

	/** Flag indicates if we are being run under the debugger.
	 */
	bool	    	    underDebugger() const;
	

        /** Provides the name of the application.
         */
        string              applicationName() const;
        
        
        /** The config file loads is xml.
         */
        bool                xmlConfig() const;


        /** Returns a copy of the map of all options set.
         */
        const NameValueMap  options() const;
        
        
    	/** If we need to dump it to a log.
	 */
        virtual ostream &   writeTo(ostream &) const;


    protected:


        /** Provide the usage to the user. Mutex is locked on entry.
         */
        virtual void            usage() const;
        

        /** Provide the error message when the config file parsing fails.
         *  Mutex is locked on entry.
         */
        virtual void            loadCfgFileErrorMsg(ReturnCode) const;
        
        
        /** The user has the ability to modify the options before
         *  it is used to parse the command line. The mutex is write locked on
         *  entry.
         */
        virtual ReturnCode      preParse();
        
        
        /** The user has the ability to query the options after
         *  it has been used to parse the command line. The mutex is write
         *  locked on entry.
         */
        virtual ReturnCode      postParse();
        

    	/** Parse the command line. The mutex is write locked on entry.
	 */
    	virtual ReturnCode  	parseCmdLine(int argc, char ** argv);
	

	/** Load configuration from a file. The mutex is write locked on entry.
	 */
	virtual ReturnCode  	loadCfgFile();
	
	
        string  cfgStr;
        string  nodaemonStr;
        string  loglevelStr;
        string  debuggerStr;
        string  xmlcfgStr;

        char    cfgChar;
        char    nodaemonChar;
        char    loglevelChar;
        char    debuggerChar;
        char    xmlcfgChar;


    	/** If set, it is the configuration file used to configure the
	 *  application.
	 */
	string	    	    myConfigFile;
        	
	
	/** If set, it is the ip addresses of the provisioning servers 
	 *  used to configure the application.
	 */
	list<IPAddress>     myPservers;
	

    	/** It's a good thing to know.
	 */
	pid_t	    	    myPid;
	
	
	/** Flag indicates whether we are running as a daemon or not.
	 */
	bool	    	    myAsDaemon;


        /** Set to true when the application runs as a daemon, it
         *  is to redirect stdout and stderr to a file.        
         */
        bool                myDaemonRedirectOutput;
        
        
    	/** Flag indicates at which log level to run.
	 */
	int 	    	    myLogLevel;
	
	
	/** Flag indicates if we are being run under the debugger.
	 */
	bool	    	    myUnderDebugger;
	

        /** Application name
         */
        string              myApplicationName;


        /** Should we load an xml cfg file?
         */
        bool                myXmlCfg;

        /** My options
         */
        GetOpt              myOptions;
                
        
    	/** Provide safe mutlithreaded access.
	 */
	mutable RWMutex	    myMutex;

};


} // namespace Configuration
} // namespace Vocal


#endif // !defined(VOCAL_CONFIG_HXX)
