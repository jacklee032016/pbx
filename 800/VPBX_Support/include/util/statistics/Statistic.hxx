#if !defined(STATISTIC_DOT_HXX)
#define STATISTIC_DOT_HXX
/*
* $Id: Statistic.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/
#include "Writer.hxx"
#include "StatisticsDb.hxx"
#include "Data.hxx"


/** Infrastructure common to ASSIST.<br><br>
 */
namespace Assist 
{


/** Infrastructure in ASSIST to measuring, recording and reporting
 *  statistics.<br><br>
 */
namespace Statistics
{


/** An abstract statistic class.<br><br> 
 *
 *  The statistic is tightly coupled with the StatisticsDb. The database 
 *  key associated with this statistic's label are allocated by the database. 
 *  The statistic also know how to record itself in the database.
 *
 *  The statistic is a type of composite class, where it may represent 
 *  a single measurement, or it may represent a combination of measurements.
 *  Typically a single measurement is recorded by the user, and then the
 *  database holds a combined statistic.
 */
class Statistic : public Assist::IO::Writer
{
    protected:


	/** Create the statistic given the associated database, and the 
	 *  label. Statistics with different labels will be assigned 
	 *  different keys.
	 */
	Statistic(StatisticsDb & db, const Data & label);


	/** Create the statistic given the associated database, and the 
	 *  previously allocated key. 
	 */
	Statistic(StatisticsDb & db, StatisticsDbKey);


	/** Copy constructor. All derived statistics should have full
	 *  copy semantics.
	 */
	Statistic(const Statistic &);


	/** Assignment operator. All derived statistics should have full
	 *  copy semantics.
	 */
	const Statistic & operator=(const Statistic &);


    public:
    

	/** Virtual destructor.
	 */
	virtual ~Statistic();

    	
	/** Record this individual statistic to the associated database.
	 */
	void	record();
	
	
	/** Database key accessor.
	 */
    	StatisticsDbKey     key() const;


	/** Label name accessor.
	 */
    	const Data &	name() const;
	

	/** Associate database accessor.
	 */
    	const StatisticsDb &	db() const;
	

	/** This will combine the two statistics. 
    	 */
	virtual void	combine(const Statistic &) = 0;


    	/** Create a copy of this statistic.
    	 */
	virtual Statistic * copy() const = 0;


    protected:
    

    	StatisticsDb *	    myDb;
	

    private:


	StatisticsDbKey     myKey;
};


} // namespace Statistics
} // namespace Assist 


#endif // !defined(STATISTIC_DOT_HXX)
