#if !defined(PERFORMANCE_DB_DOT_HXX)
#define PERFORMANCE_DB_DOT_HXX
/*
* $Id: PerformanceDb.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "StatisticsDb.hxx"


/** Infrastructure common to ASSIST.<br><br>
 */
namespace Assist 
{


/** Infrastructure in ASSIST to measuring, recording and reporting
 *  statistics.<br><br>
 */
namespace Statistics
{


class PerfDuration;
class PerfTotal;


/** Database dedicated to gather performance statistics.<br><br>
 */
class PerformanceDb : public Assist::Statistics::StatisticsDb
{
    protected:


	/** Create the database.<br><br> Only used by instance().
	 */
	PerformanceDb();


    public:
    

	/** Create the singleton database.<br><br>
	 */
    	static PerformanceDb &	instance();


	virtual ~PerformanceDb();


	/** Finds the combined PerfDuration statistic in the database
         *  that matches the type (db key) of the passed in statistic. 
         *
         *  Returns 0 if not found.
	 */
	virtual PerfDuration * find(const PerfDuration &) const;


	/** Finds the combined PerfTotal statistic in the database
         *  that matches the type (db key) of the passed in statistic. 
         *
         *  Returns 0 if not found.
	 */
	virtual PerfTotal * find(const PerfTotal &) const;


    private:


    	static PerformanceDb *	m_instance;
};


} // namespace Statistics
} // namespace Assist 


#endif // !defined(PERFORMANCE_DB_DOT_HXX)
