#if !defined(ASSIST_RUNNABLE_HXX)
#define ASSIST_RUNNABLE_HXX

/*
* $Id: Runnable.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "AssistCommon.hxx"


/** Infrastructure common to ASSIST.
 */
namespace Assist 
{


/** An interface representing a unit of exection.<br><br>
 *
 *  Used by both Applications and Threads
 *
 *  @see    Assist::Application
 *  @see    Assist::Threads::Thread
 *  @see    Assist::ReturnCode
 */
class Runnable
{
    public:
    
    	Runnable();
	virtual     	    	    ~Runnable();
	
    	/** Entry point for the executable code.
	 */
	virtual ReturnCode  	    run() = 0;
};


} // namespace Assist


#include "deprecated/DeprecatedRunnable.hxx"


#endif // !defined(ASSIST_RUNNABLE_HXX)
