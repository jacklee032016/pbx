#if !defined(PERF_TOTAL_DOT_HXX)
#define PERF_TOTAL_DOT_HXX
/*
 * $Id: PerfTotal.hxx,v 1.2 2007/03/01 20:04:30 lizhijie Exp $
 */

#include "TotalStatistic.hxx"


/** Infrastructure common to VOCAL.<br><br>
 */
namespace Vocal 
{


/** Infrastructure in VOCAL to measuring, recording and reporting
 *  statistics.<br><br>
 */
namespace Statistics
{


/** A TotalStatistics that specifically uses the PerformanceDb.<br><br>
 */
class PerfTotal : public Vocal::Statistics::TotalStatistic
{
    public:


	/** Create the statistic the label. Statistics with different 
	 *  labels will be assigned different keys.
	 */
	PerfTotal(const Data & label);


	/** Copy constructor. 
	 */
	PerfTotal(const PerfTotal &);


	/** Assignment operator. 
	 */
	const PerfTotal & operator=(const PerfTotal &);


	/** Virtual destructor.
	 */
	virtual ~PerfTotal();

    	
    	/** Create a copy of this statistic.
    	 */
	virtual Statistic * 	copy() const;
};


} // namespace Statistics
} // namespace Vocal 


#endif // !defined(PERF_TOTAL_DOT_HXX)
