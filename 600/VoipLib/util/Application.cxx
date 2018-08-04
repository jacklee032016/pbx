/*
 * $Id: Application.cxx,v 1.2 2007/03/01 20:04:32 lizhijie Exp $
 */

#include "global.h"
#include "Application.hxx"
#include "VException.hxx"
#include <iostream>
#include <assert.h>

using Vocal::Application;
using Vocal::ApplicationCreator;
using Vocal::Runnable;
using Vocal::ReturnCode;


ApplicationCreator  *   Application::myCreator = 0;
Application  	    *   Application::myApplication = 0;


void
Application::creator(ApplicationCreator * creator_)
{
    myCreator = creator_;
}

Application &	    
Application::instance()
{
    if ( !myApplication )
    {
        if ( myCreator != 0 )
        {
    	    myApplication = myCreator->create();
        }
        else
        {
            throw "No ApplicationCreator";
        }
    }
    return ( *myApplication );
}

ReturnCode  	    
Application::main(int argc, char ** argv, char ** arge)
{
    // Create the application.
    //
    Application &   app = Application::instance();

    //
    // Could install new handler here.
    //
    
    // Initialize the application
    //
    ReturnCode	rc = SUCCESS;
    try
    {
        rc = app.init(argc, argv, arge);
    }
    catch ( VException & )
    {
    	cerr << "Unhandled VException from Application::init(). Exiting." << endl;
    	return ( 1 );
    }
    catch ( ... )
    {
    	cerr << "Unhandled exception from Application::init(). Exiting." << endl;
    	return ( 2 );
    }
    
    if ( rc != SUCCESS )
    {
    	return ( rc );
    }
    
    try
    {
    	rc = app.run();
    }
    catch ( VException & )
    {
    	cerr << "Unhandled VException from Application::run(). Exiting." << endl;
	return ( 1 );
    }
    catch ( ... )
    {
    	cerr << "Unhandled exception from Application::run(). Exiting." << endl;
    	return ( 2 );
    }

    // Cleanup the application
    //
    try
    {
    	app.uninit();
    }
    catch ( VException & )
    {
    	cerr << "Unhandled VException from Application::uninit(). Exiting." << endl;
	return ( 1 );
    }
    catch ( ... )
    {
    	cerr << "Unhandled exception from Application::uninit(). Exiting." << endl;
    	return ( 2 );
    }    

    // Delete the application
    //
    delete myApplication;
    myApplication = 0;
    
    return ( rc );    
}

	
Application::Application()
{
}


Application::~Application()
{
}


ReturnCode
Application::init(int argc, char ** argv, char ** arge)
{
    return ( SUCCESS );
}


void
Application::uninit()
{
}
