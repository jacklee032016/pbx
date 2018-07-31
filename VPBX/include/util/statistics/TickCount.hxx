#if !defined(TICKCOUNT_DOT_HXX)
#define TICKCOUNT_DOT_HXX
/*
* $Id: TickCount.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "vtypes.h"
#include "VThread.hxx"
#include <sys/times.h>
#include <unistd.h>


/** Infrastructure common to ASSIST.<br><br>
 */
namespace Assist 
{


/** Infrastructure in ASSIST to measuring, recording and reporting
 *  statistics.<br><br>
 */
namespace Statistics
{


/** Represents the number of clock ticks that have elapsed since the
 *  system has been up.<br> <br>
 *
 *  Should act similar to times(2), with a higher
 *  resolution.
 */
class TickCount
{
    public:

	/** Low order portion of tick count.
	 */
	u_int32_t	 	low;


	/** High order portion of tick count.
	 */
	u_int32_t	 	high;


	/** Once calibrated, the 
	 */
	static  int64_t	    	ticksPerSec;


	/** Initialize the ticks to 0.
	 */
	TickCount();


	/** The tick count will be set to the current number of ticks
	 *	that have elasped since the system has started.
	 */
	void    	    	set();


	/** Return the number of ticks as a 64 bit number.
	 */
	int64_t 	    	get() const;


	/** Return the difference between two TickCounts as a 64 bit int.
	 */
	int64_t 	    	operator-(const TickCount & src) const;


	/** Find the number of ticksPerSecond.
	 */
	static  void    	calibrate();
};


inline TickCount::TickCount()
    :   low(0), high(0)
{
}


inline void TickCount::set()
{

#if defined(__GNUG__)
    #if defined(ASSIST_USING_PENTIUM)
    	#define USE_ASM_TICKS 1
    #endif
#endif

#if ( USE_ASM_TICKS == 1 )

    // This code only works on intel 586 / 686 processors 
    //  (pentium, pentium pro, pentium ii, pentium iii) 
    //
    // This assembly code gets the value of the Intel CPU tick register
    // and deposits it in the TickCount tc.
    //

    #define rdtsc(tc) \
	 __asm__ __volatile__("rdtsc" : "=a" ((tc).low), "=d" ((tc).high))      


    rdtsc(*this);

#elif defined( WIN32 )

    low = GetTickCount();

#elif defined(__APPLE__)
    struct tms	vTp;
    low = times(&vTp);
#else

    low = times(0);

#endif

}
    

inline int64_t TickCount::get() const
{
    return ( (static_cast<int64_t>(high) << 32) + low );
}
    

inline int64_t
TickCount::operator-(const TickCount & src) const
{
    return  ( get() - src.get() );
}


inline void    	TickCount::calibrate()
{
    if ( ticksPerSec == 0 )
    {
    	TickCount start, stop;
	
	start.set();
	
	// There's got to be a better way.
	//
	sleep(1);

	stop.set();

	ticksPerSec = stop - start;
    }
}


} // namespace Statistics
} // namespace Assist 


#endif // !defined(TICKCOUNT_DOT_HXX)
