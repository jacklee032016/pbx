#if !defined(SCOPED_DURATION_DOT_HXX)
#define SCOPED_DURATION_DOT_HXX
/*
* $Id: ScopedDuration.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "Duration.hxx"


/** Infrastructure common to ASSIST.<br><br>
 */
namespace Assist 
{


/** Infrastructure in ASSIST to measuring, recording and reporting
 *  statistics.<br><br>
 */
namespace Statistics
{


/** A decorator class for durations that start at the time this 
 *  class enters scope, and stop when this class leaves scope.<br><br>
 *
 *  @see    Assist::Statistics::Duration
 */
class ScopedDuration
{
    public:
    	

	/** The decorated duration.
	 */
	Duration &  	duration;


	/** Start the duration.
	 */
	ScopedDuration(Duration & duration);
	
	
	/** Stop the duration.
	 */
	~ScopedDuration();
};


inline ScopedDuration::ScopedDuration(Duration & duration_)
    :	duration(duration_)
{
    duration.start();
}


ScopedDuration::~ScopedDuration()
{
    duration.stop();
}


} // namespace Statistics
} // namespace Assist 


#endif // !defined(SCOPED_DURATION_DOT_HXX)
