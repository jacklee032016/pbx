#if !defined(STATISTICS_DB_DOT_HXX)
#define STATISTICS_DB_DOT_HXX
/*
 * $Id: StatisticsDb.hxx,v 1.2 2007/03/01 20:04:30 lizhijie Exp $
 */


#include <map>
#include <vector>

#include "Writer.hxx"
#include "Data.hxx"
#include "RWMutex.hxx"

/** Infrastructure common to VOCAL.<br><br>
 */
namespace Vocal 
{


/** Infrastructure in VOCAL to measuring, recording and reporting
 *  statistics.<br><br>
 */
namespace Statistics
{


using Vocal::Threads::RWMutex;


class Statistic;


/** Using an unsigned int for simplicity.<br><br>
 */
typedef     u_int32_t	    StatisticsDbKey;


/** Never used as a database key.<br><br>
 *
 * All database keys will be greater than this value.
 */
extern const StatisticsDbKey  INVALID_STAT_DB_KEY;


/** A statistics database.<br><br>
 */
class StatisticsDb : public Vocal::IO::Writer
{
    public:


	/** Create the statistics database, given the optional name 
	 *  of the database (defaults to "StatisticsDb").
	 */
	StatisticsDb(const char * name = 0);


	/** Destroy the statistics database.
	 */
	virtual ~StatisticsDb();

    	
    	/** Return the key given the label. If this is a new label,
	 *  a key will be allocated. Keys are not consistent across
	 *  different instances of the StatisticsDb.
	 */
    	StatisticsDbKey     key(const Data & label);


    	/** Return the label name of the given key. If the key doesn't exist,
	 *  the label for INVALID_STAT_DB_KEY is returned.
	 */
    	const Data &	keyName(StatisticsDbKey) const;


    	/** Return the next key. All keys will be less than this value.
	 */
    	StatisticsDbKey     nextKey() const;


	/** Adds the statistic into the database. The statistic must be created 
	 *  using this database.
	 */
	virtual void	record(const Statistic &);


	/** Finds the statistic in the database. Returns 0 if not found.
	 */
	virtual Statistic * find(const Statistic &) const;


	/** Erases the individual statistic from the database. 
	 */
	virtual void	erase(const Statistic &);


    	/** Clear the entire database.
	 */
	virtual void	clear();

    	
    	/** Dump the database to an ostream.
	 */
        virtual ostream &   writeTo(ostream &) const;


	/** Returns the name of the database.
	 */
    	const Data &	name() const;	


    	/** Returns true if the databases are the same.
	 *  Since databases cannot be copied, this is an identity check.
	 */
	bool operator==(const StatisticsDb &) const;


    	/** The databases are ordered by their addresses.
	 */
	bool operator<(const StatisticsDb &) const;


    private:

    	/** Mutlithreaded protection.
	 */
	mutable RWMutex     myLock;
	 
	    
    	/** Name of the database.
	 */
	Data	myName;
    

    	/** The next key to be allocated.
	 */
	StatisticsDbKey	myNextKey;


    	/** 
	 */
    	typedef std::map<Data, StatisticsDbKey>  StatisticRegistrations;
	

    	/**
	 */
	StatisticRegistrations    myStatRegistrations;
	

    	/**
	 */
    	typedef std::vector<Data>  StatisticNames;
	

    	/**
	 */
	StatisticNames    myStatNames;


    	/**
    	 */
    	typedef std::map<StatisticsDbKey, Statistic *>  Database;
	
	
	/**
	 */
	Database    m_db;
    

    	/** Suppress copying
	 */
	StatisticsDb(const StatisticsDb &);


    	/** Suppress copying
	 */
	const StatisticsDb & operator=(const StatisticsDb &);
};


} // namespace Statistics
} // namespace Vocal 


#endif // !defined(STATISTICS_DB_DOT_HXX)
