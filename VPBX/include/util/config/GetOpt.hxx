#if !defined(ASSIST_GETOPT_HXX)
#define ASSIST_GETOPT_HXX
/*
* $Id: GetOpt.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "NonCopyable.hxx"
#include "NameValue.hxx"

#include <string>
#include <vector>
#include <list>
#include <cstdio>
#ifdef ASSIST_USE_GETOPT_LONG
#include "getopt_long.h"
#else
#if defined(__svr4__)
#include <stdlib.h>
#else
  #if defined(__APPLE__)
    #include <unistd.h>
  #else
    #include <getopt.h>
  #endif
#endif
#endif


/** Vovida Open Communication Application Library.<br><br>
 */
namespace Assist 
{


/** 
 */
namespace Configuration
{


using std::string;
using std::vector;
using std::list;


/**
 */
class GetOpt : public Assist::NonCopyable
{
    public:
   

        /**
         */ 
        GetOpt();
        
        
        /**
         */
	virtual ~GetOpt();


        /**
         */
    	ReturnCode  add
        (
            const char      *   longOption,
	    char                optionChar  = '\0',
	    ValueType           valType     = NO_VALUE
        );


        /**
         */
    	ReturnCode  add
        (
            const string &      longOption,
	    char                optionChar  = '\0',
	    ValueType           valType     = NO_VALUE
        );

        /**
         */
        void        printError(bool);


        /**
         */
    	ReturnCode  parse(int argc, char ** argv);


        /**
         */
        void        addValue
        (
            const string    &   name, 
            const char      *   value = 0, 
            ValueType           type = NO_VALUE
        );


        /**
         */
        void        addValue
        (
            const string    &   name, 
            const Value     &   value
        );
        
        
        /**
         */
        const NameValueMap &    options() const;
        

        /**
         */
        int         remainingArgc() const;
        
    
        /**
         */
        char **     remainingArgv() const;

        
        /**
         */
        const list<string> &    remaining() const;


        /**
         */
        bool        get(const string & name);


        /**
         */
        bool        get(const string & name, string & value);
        

        /**
         */
        bool        get(const string & name, int & value);


    private:

        /**
         */
        const option &      find(const char *, char) const;
        
      	string	    	    	    	myOptString;
	vector<option>	    	    	myAllOpts;
	vector<option>	    	    	myLongOpts;
        int                             myPrintError;
    
        int                             myRemainingArgc;
        char **                         myRemainingArgv;
        list<string>                    myRemaining;
        
        NameValueMap                    myOptionMap;
};


} // namespace Configuration
} // namespace Assist


#endif // !defined(ASSIST_GETOPT_HXX)
