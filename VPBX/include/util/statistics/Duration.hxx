#if !defined(DURATION_DOT_HXX)
#define DURATION_DOT_HXX
/*
* $Id: Duration.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "TickCount.hxx"


/** Infrastructure common to ASSIST.<br><br>
 */
namespace Assist 
{


/** Infrastructure in ASSIST to measuring, recording and reporting
 *  statistics.<br><br>
 */
namespace Statistics
{


/** A period of time between two tick counts.<br><br>
 *
 *  @see    Assist::Statistics::TickCount
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
} // namespace Assist 


#endif // !defined(DURATION_DOT_HXX)
