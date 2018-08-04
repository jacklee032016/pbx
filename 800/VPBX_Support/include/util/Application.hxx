#if !defined(ASSIST_APPLICATION_HXX)
#define ASSIST_APPLICATION_HXX
/*
* $Id: Application.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "Runnable.hxx"
#include "AssistCommon.hxx"
#include "GarbageCan.hxx"


/** Infrastructure common to ASSIST.<br><br>
 */
namespace Assist 
{


//using Assist::GarbageCan;

class Application;

class ApplicationCreator
{
    public:
        virtual Application * create() = 0;
};

/** A singleton class which should be the entry point for a process.<br><br>
 *  
 *  @see    Assist::Runnable
 */
class Application : public Assist::Runnable
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


} // namespace Assist


#include "deprecated/DeprecatedApplication.hxx"


#endif // !defined(ASSIST_APPLICATION_HXX)
