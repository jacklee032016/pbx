#if !defined(SCOPED_DURATION_DOT_HXX)
#define SCOPED_DURATION_DOT_HXX
/*
 * $Id: ScopedDuration.hxx,v 1.2 2007/03/01 20:04:30 lizhijie Exp $
 */

#include "Duration.hxx"


/** Infrastructure common to VOCAL.<br><br>
 */
namespace Vocal 
{


/** Infrastructure in VOCAL to measuring, recording and reporting
 *  statistics.<br><br>
 */
namespace Statistics
{


/** A decorator class for durations that start at the time this 
 *  class enters scope, and stop when this class leaves scope.<br><br>
 *
 *  @see    Vocal::Statistics::Duration
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
} // namespace Vocal 


#endif // !defined(SCOPED_DURATION_DOT_HXX)
