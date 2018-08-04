/*
 * $Id: GetOpt.cxx,v 1.2 2007/03/01 20:04:32 lizhijie Exp $
 */

#include "global.h"
#include "GetOpt.hxx"
#include "ParsePair.hxx"
#include <cstring>
#include <cassert>
#include <cstdlib>


using Vocal::Configuration::GetOpt;
using Vocal::Configuration::ValueType;
using Vocal::Configuration::Value;
using Vocal::Configuration::NameValueMap;
using Vocal::ReturnCode;
using Vocal::SUCCESS;
using std::list;
using std::string;


GetOpt::GetOpt()
    :	myOptString(""),
        myPrintError(0),
        myRemainingArgc(0),
        myRemainingArgv(0)
{
    option  	nullOption = { 0, 0, 0, 0 };

    myLongOpts.insert(myLongOpts.begin(), nullOption);
    myAllOpts.insert(myAllOpts.begin(), nullOption);
}


GetOpt::~GetOpt()
{
    size_t  size =  myLongOpts.size();
    
    for ( size_t i = 0; i < size; i++ )
    {
    	delete [] const_cast<char *>(myLongOpts[i].name);
	myLongOpts[i].name = 0;
    }
}


ReturnCode
GetOpt::add(
    const char *            longOption,
    char 	    	    optionChar,
    ValueType               valType
)
{
    // Make sure we don't already have this option.
    //
    const option & opt = find(longOption, optionChar);

    if ( opt.name != 0 || opt.val != 0 )
    {
        return ( !SUCCESS );
    }

    bool    haveOption = false;

    size_t  size = 0;

    if ( longOption != 0 )
    {    
        size = strlen(longOption);
    }

    // Build the long options
    //
    option  newOption = { 0, 0, 0, 0 };
    
    if ( size != 0 )
    {
        // If we have a long option, copy the name.
        //
    	char *	newName = new char[size+1]; 
    	strncpy(newName, longOption, size+1);
    	newOption.name =  newName;
        haveOption = true;
    }
            
    // If we have a option character, add it to the option string
    // and add it to the new option.
    //
    if ( optionChar != '\0' )
    {    
        myOptString += optionChar;
        newOption.val = optionChar;
        haveOption = true;
    }
    
    if ( haveOption != true )
    {
        return ( !SUCCESS );
    }

    newOption.has_arg = static_cast<int>(valType);
        
    switch ( valType )
    {
    	case VALUE_PRESENT:
	{
            if ( optionChar )
            {
	        myOptString += ':';
            }
	    break;
	}

	case VALUE_OPTIONAL:
	{
            if ( optionChar )
            {
	        myOptString += "::";
            }
	    break;
	}
	
	default:
	{
	    break;
	}
    }

    if ( size != 0 )
    {
        vector<option>::iterator i = myLongOpts.end();
        myLongOpts.insert(--i, newOption);
    }

    vector<option>::iterator i = myAllOpts.end();
    myAllOpts.insert(--i, newOption);

    return ( SUCCESS );
}


ReturnCode  
GetOpt::add(
    const string &      longOption,
    char                optionChar,
    ValueType           valType
)
{
    return ( add(longOption.c_str(), optionChar, valType) );
}


void	    	    
GetOpt::printError(bool error)
{
    myPrintError = ( error ? 1 : 0 );
}
	

ReturnCode   	    
GetOpt::parse(int argc, char **  argv)
{
    ReturnCode  rc = SUCCESS;
    
    opterr = myPrintError;
    
    int c = 0;

    int optionIndex = -1;
        	 
    while   ( 	( c = getopt_long(argc, argv, myOptString.c_str(), 
    	    	    	    &myLongOpts[0], &optionIndex) )
    	    	!= -1 
	    )
    {
        if ( c == ':' || c == '?' )
        {
            rc = !SUCCESS;
            continue;
        }

	const option  & opt =   c == 0 
                                ? myLongOpts[optionIndex] 
                                : find(0, c);

        if ( opt.name == 0 && opt.val == 0 )
        {
            rc = !SUCCESS;
            continue;
        }

        string  name;
        if ( opt.name )
        {
            name = opt.name;
        }
        else
        {
            name = opt.val;
        }

        addValue(name, optarg, static_cast<ValueType>(opt.has_arg));
    }

    myRemainingArgc = argc - optind;
    myRemainingArgv = ( myRemainingArgc ? &argv[optind] : 0 );
    
    myRemaining.clear();
    
    argv = myRemainingArgv;
    
    for ( argc = myRemainingArgc; argc > 0; --argc )
    {
        string  arg = *argv;
        myRemaining.push_back(arg);
        ++argv;
    }
    
    return ( rc );
}


void
GetOpt::addValue
(
    const string    &   name, 
    const char      *   value,
    ValueType           type
)
{
    Value & val = myOptionMap[name];
    ++val.count;
    
    switch ( type )
    {
	case VALUE_OPTIONAL:
	{
            if ( value )
            {
                val.type = VALUE_PRESENT;
                val.value.push_back(string(value));
            }
	    break;
	}

	case VALUE_PRESENT:
	{
	    assert(value);
            val.type = VALUE_PRESENT;
            val.value.push_back(string(value));
	    break;
	}

	default:
        {
            val.type = NO_VALUE;
	    break;
        }
    }
}


void
GetOpt::addValue
(
    const string    &   name, 
    const Value     &   value
)
{
    Value & val = myOptionMap[name];
    if ( val.count == 0 )
    {
        val = value;
        return;
    }
    
    val.count += value.count;
    
    if  (   value.type == VALUE_PRESENT 
        ||  value.type == VALUE_OPTIONAL
        )
    {
        val.type = VALUE_PRESENT;
        
        for (   list<string>::const_iterator i = value.value.begin();
                i != value.value.end();
                ++i
            )
        {
            val.value.push_back(*i);
        }
    }
}


const NameValueMap &   
GetOpt::options() const
{
    return ( myOptionMap );
}


int         
GetOpt::remainingArgc() const
{
    return ( myRemainingArgc );
}


char **     
GetOpt::remainingArgv() const
{
    return ( myRemainingArgv );
}


const list<string> &    
GetOpt::remaining() const
{
    return ( myRemaining );
}


bool        
GetOpt::get(const string & name)
{
    NameValueMap::const_iterator i;

    if ( (i = myOptionMap.find(name)) == myOptionMap.end() )
    {
        return ( false );
    }

    return ( true );
}


bool        
GetOpt::get(const string & name, string & value)
{
    value = "";
    
    NameValueMap::const_iterator i;

    if ( (i = myOptionMap.find(name)) == myOptionMap.end() )
    {
        return ( false );
    }

    if ( i->second.type == VALUE_PRESENT )
    {
        value = i->second.value.back();
    }
    return ( true );
}


bool        
GetOpt::get(const string & name, int & value)
{
    string  p;
    
    if ( get(name, p) )
    {
        value = atoi(p.c_str());
        return ( true );
    }

    return ( false );
}


const option &      
GetOpt::find(const char * longOpt, char c) const
{
    size_t  size = myAllOpts.size();

    for ( size_t i = 0; i < size; ++i )
    {
        if  ( c != 0 && c == myAllOpts[i].val )
        {
            return ( myAllOpts[i] );
        }
        if ( longOpt != 0 && myAllOpts[i].name != 0 )
        {
            size_t  n = strlen(longOpt);
            size_t  m = strlen(myAllOpts[i].name);
            
            if ( n == m && strncmp(myAllOpts[i].name, longOpt, n) == 0 )
            {
                return ( myAllOpts[i] );
            }
        }
    }

    // Not found, so return null last option
    //
    return ( myAllOpts[size-1] );
}

