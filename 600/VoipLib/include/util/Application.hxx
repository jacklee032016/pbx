#if !defined(VOCAL_APPLICATION_HXX)
#define VOCAL_APPLICATION_HXX

/*
 * $Id: Application.hxx,v 1.2 2007/03/01 20:04:29 lizhijie Exp $
 */

#include "Runnable.hxx"
#include "VocalCommon.hxx"
#include "GarbageCan.hxx"


/** Infrastructure common to VOCAL.<br><br>
 */
namespace Vocal 
{


//using Vocal::GarbageCan;

class Application;

class ApplicationCreator
{
    public:
        virtual Application * create() = 0;
};

/** A singleton class which should be the entry point for a process.<br><br>
 *  
 *  @see    Vocal::Runnable
 */
class Application : public Vocal::Runnable
{
    public:

        /** Sets the function object to create the application
         */
        static void                 creator(ApplicationCreator *);
        
    	/** To get the singleton. Will create the Application the first
	 *  time called.
    	 */
    	static	Application &	    instance();


    	/** Should be called from the program's main(). It could
	 *  possibly look like:<br><br>
	 *  <code>
	 *  int main(int argc, char ** argv, char ** arge)<br>
	 *  {<ul>
	 *  	return ( Application::main(argc, argv, arge) );
	 *  </ul>
	 *  }</code>
	 *
	 *  This method creates the instance of the Application, 
	 *  initializes it, runs it, uninitializes it and destroys the
	 *  instance.
    	 */
	static	ReturnCode  	    main(int 	    argc, 
	    	    	    	    	 char 	**  argv, 
					 char 	**  arge);
	
    protected:

    	/** Default constructor. Avoid creating objects in the constructor.
	 *  Instead, create them dynamically in init and destroy them in
	 *  uninit.
    	 */
    	Application();
	
    public:

    	/** Virtual destructor
    	 */
	virtual ~Application();

    protected:

    	/** Initialize the application. The command line arguments are
	 *  passed unaltered.
    	 */
    	virtual ReturnCode  	    init(int 	    argc, 
	    	    	    	    	 char 	**  argv, 
					 char 	**  arge);
					 

    	/** Uninitialize the application.
    	 */
	virtual void		    uninit();

    private:

        static ApplicationCreator   *   myCreator;
    	static Application  	    *   myApplication;
        
        GarbageCan                      myGarbageCan;
};


} // namespace Vocal


#include "deprecated/DeprecatedApplication.hxx"


#endif // !defined(VOCAL_APPLICATION_HXX)
