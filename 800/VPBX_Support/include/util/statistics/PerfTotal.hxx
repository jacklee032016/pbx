#if !defined(PERF_TOTAL_DOT_HXX)
#define PERF_TOTAL_DOT_HXX
/*
* $Id: PerfTotal.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/
#include "TotalStatistic.hxx"


/** Infrastructure common to ASSIST.<br><br>
 */
namespace Assist 
{


/** Infrastructure in ASSIST to measuring, recording and reporting
 *  statistics.<br><br>
 */
namespace Statistics
{


/** A TotalStatistics that specifically uses the PerformanceDb.<br><br>
 */
class PerfTotal : public Assist::Statistics::TotalStatistic
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
} // namespace Assist 


#endif // !defined(PERF_TOTAL_DOT_HXX)
