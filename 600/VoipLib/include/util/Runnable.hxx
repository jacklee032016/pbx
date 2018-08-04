#if !defined(VOCAL_RUNNABLE_HXX)
#define VOCAL_RUNNABLE_HXX
/*
 * $Id: Runnable.hxx,v 1.2 2007/03/01 20:04:29 lizhijie Exp $
 */

#include "VocalCommon.hxx"


/** Infrastructure common to VOCAL.
 */
namespace Vocal 
{


/** An interface representing a unit of exection.<br><br>
 *
 *  Used by both Applications and Threads
 *
 *  @see    Vocal::Application
 *  @see    Vocal::Threads::Thread
 *  @see    Vocal::ReturnCode
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


} // namespace Vocal


#include "deprecated/DeprecatedRunnable.hxx"


#endif // !defined(VOCAL_RUNNABLE_HXX)
