#if !defined(PERF_DURATION_DOT_HXX)
#define PERF_DURATION_DOT_HXX
/*
* $Id: PerfDuration.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "DurationStatistic.hxx"


/** Infrastructure common to ASSIST.<br><br>
 */
namespace Assist 
{


/** Infrastructure in ASSIST to measuring, recording and reporting
 *  statistics.<br><br>
 */
namespace Statistics
{


/** One or more duration statistics using the PerformanceDb.<br><br>
 */
class PerfDuration : public Assist::Statistics::DurationStatistic
{
    public:


	/** Create the statistic given the associated database,
	 *  the label and the optional window width. The window width
	 *  default to 0 indicating no window, thus it remembers all
	 *  combined durations. 
	 */
	PerfDuration(const Data & name, u_int32_t window = 0);


	/** Copy constructor.
	 */
	PerfDuration(const PerfDuration &);


	/** Assignment operator.
	 */
	const PerfDuration & operator=(const PerfDuration &);


	/** Virtual destructor.
	 */
	virtual ~PerfDuration();

    	
    	/** Create a copy of this statistic.
    	 */
	virtual Statistic * 	copy() const;
};


} // namespace Statistics
} // namespace Assist 


#endif // !defined(PERF_DURATION_DOT_HXX)
