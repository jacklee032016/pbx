#if !defined(TOTAL_STATISTIC_DOT_HXX)
#define TOTAL_STATISTIC_DOT_HXX
/*
 * $Id: TotalStatistic.hxx,v 1.2 2007/03/01 20:04:30 lizhijie Exp $
 */

#include "Statistic.hxx"


/** Infrastructure common to VOCAL.<br><br>
 */
namespace Vocal 
{


/** Infrastructure in VOCAL to measuring, recording and reporting
 *  statistics.<br><br>
 */
namespace Statistics
{


/** Keeps a running total.<br><br>
 */
class TotalStatistic : public Vocal::Statistics::Statistic
{
    public:


	/** Create the statistic given the associated database, and the 
	 *  label. Statistics with different labels will be assigned 
	 *  different keys.
	 */
	TotalStatistic(StatisticsDb & db, const Data & label);


	/** Copy constructor. 
	 */
	TotalStatistic(const TotalStatistic &);


	/** Assignment operator. 
	 */
	const TotalStatistic & operator=(const TotalStatistic &);


	/** Virtual destructor.
	 */
	virtual ~TotalStatistic();

    	
	/** This will combine the two statistics, such that the total
	 *  will be the sum.
    	 */
	virtual void	combine(const Statistic &);


    	/** Create a copy of this statistic.
    	 */
	virtual Statistic * 	copy() const;


    	/** Set the total to the given value. This is useful for 
	 *  individual statistics.
	 */
	void	set(u_int32_t);


	/** Set's individual statistic's total to one, and then records
	 *  the statistic, such that the combined statistic in the database
	 *  is increased by one. Prefix operator.
	 */
        const TotalStatistic &    operator++();
	 

	/** Set's individual statistic's total to one, and then records
	 *  the statistic, such that the combined statistic in the database
	 *  is increased by one. Postfix operator.
	 */
        const TotalStatistic &    operator++(int);


	/** Set's individual statistic's total to the given value, and then 
	 *  records the statistic, such that the combined statistic in the 
	 *  database is increased by given value.
	 */
        const TotalStatistic &    operator+=(u_int32_t);


    	/** Returns the total. More meaningful for combined statistics.
	 */
	u_int32_t   get() const;


	/** Write the statistic to an ostream.
	 */
        virtual ostream & writeTo(ostream &) const;


    private:
    

    	u_int32_t   	    m_total;
};


} // namespace Statistics
} // namespace Vocal 


#endif // !defined(TOTAL_STATISTIC_DOT_HXX)
