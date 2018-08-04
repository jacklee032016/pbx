#if !defined(DURATION_DOT_HXX)
#define DURATION_DOT_HXX
/*
 * $Id: Duration.hxx,v 1.2 2007/03/01 20:04:30 lizhijie Exp $
 */

#include "TickCount.hxx"


/** Infrastructure common to VOCAL.<br><br>
 */
namespace Vocal 
{


/** Infrastructure in VOCAL to measuring, recording and reporting
 *  statistics.<br><br>
 */
namespace Statistics
{


/** A period of time between two tick counts.<br><br>
 *
 *  @see    Vocal::Statistics::TickCount
 */
class Duration
{
    public:

	TickCount 	startTick;
	TickCount 	stopTick;

	/** Start measuring the duration.
	 */
	void	    start();


	/** Stop measuring the duration.
	 */
	void	    stop();


	/** The length of the duration in ticks, as a 64 bit integer.
	 */
	int64_t	    length() const;
};


inline void Duration::start()
{
    startTick.set();
}


inline void Duration::stop()
{
    stopTick.set();
}


inline int64_t	Duration::length() const
{
    return ( stopTick - startTick );
}


} // namespace Statistics
} // namespace Vocal 


#endif // !defined(DURATION_DOT_HXX)
