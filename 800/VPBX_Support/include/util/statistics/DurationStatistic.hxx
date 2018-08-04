#if !defined(DURATION_STATISTIC_DOT_HXX)
#define DURATION_STATISTIC_DOT_HXX
/*
* $Id: DurationStatistic.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/
#include "Statistic.hxx"
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


/** One or more duration statistics.<br><br>
 */
class DurationStatistic : public Assist::Statistics::Statistic
{
    public:


	/** Create the statistic given the associated database,
	 *  the label and the optional window width. The window width
	 *  default to 0 indicating no window, thus it remembers all
	 *  combined durations. 
	 */
	DurationStatistic(StatisticsDb  &   db, 
	    	    	  const Data 	&   name, 
			  u_int32_t 	    window = 0);


	/** Copy constructor.
	 */
	DurationStatistic(const DurationStatistic &);


	/** Assignment operator.
	 */
	const DurationStatistic & operator=(const DurationStatistic &);


	/** Virtual destructor.
	 */
	virtual ~DurationStatistic();

    	
	/** This will combine the two statistics, chaining them together.
	 *  If a window set and this addition exceeds the window size, the 
	 *  oldest list element will be removed from the list.
    	 */
	virtual void	combine(const Statistic &);


    	/** Create a copy of this statistic.
    	 */
	virtual Statistic * 	copy() const;


	/** Start measuring the duration. If this is a combined statistic,
	 *  it will be cleared first.
	 */
	void	start();


	/** Stop measuring the duration.
	 */
	void	stop();
	
	
	/** Return the next statistic or 0 if this is the last statistic.
	 */
	DurationStatistic * 	next() const;


    	/** Return the number of statistics.
	 */
    	u_int32_t   size() const;
	
	
	/** Return the window size or 0 if there is no window.
	 */
	u_int32_t   window() const;
	
	
	/** Returns the combined length of all durations.
	 */
	int64_t length() const;
	

	/** The length divided by the size.
	 */
	double	average() const;


	/** Write the statistic to an ostream.
	 */
        virtual ostream &   writeTo(ostream &) const;
	 

    private:
    	

    	/*  Copy constructor used by append. Copies only the first element
	 *  from a combination statistic, creating an individual statistic.
	 */
	DurationStatistic(const Statistic &, const Duration  &, u_int32_t);


	/* Append a list to the end.
	 */
	void	append(const DurationStatistic &);
	

	/* Pop from the front as needed.
	 */
	void	pop();
	

	/* Clears list.
	 */
	void	clear();
    

    	Duration   	    	    m_duration;
    	DurationStatistic   	*   m_next;
    	DurationStatistic   	*   m_last;
    	u_int32_t   	    	    m_window;
	u_int32_t   	    	    m_size;
	int64_t     	    	    m_length;
};


inline void	
DurationStatistic::start()
{
    clear();
    m_duration.start();
}


inline void	
DurationStatistic::stop()
{
    m_duration.stop();
    m_length = m_duration.length();
}



} // namespace Statistics
} // namespace Assist 


#endif // !defined(DURATION_STATISTIC_DOT_HXX)
