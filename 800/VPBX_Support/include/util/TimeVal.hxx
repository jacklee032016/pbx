#if !defined(ASSIST_TIME_VAL_HXX)
#define ASSIST_TIME_VAL_HXX

/*
* $Id: TimeVal.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "Writer.hxx"
#include <sys/time.h>
#include <unistd.h>


/** Infrastructure common to ASSIST.
 */
namespace Assist 
{


/** Infrastructure common to ASSIST to manipulate the time.<br><br><br>
 */
namespace TimeAndDate
{


#if !defined(ASSIST_MILLISECONDS_T)
#define ASSIST_MILLISECONDS_T

/** 1/1000th of a second.<br><br>
 */
typedef int milliseconds_t;

#endif // !defined(ASSIST_MILLISECONDS_T)


/** A wrapper for the operating system's timeval structure.<br><br>
 */
class TimeVal : public timeval, public Assist::IO::Writer
{
    public:

    	/** Default constructor.
	 */
    	TimeVal();


    	/** Millisecond constructor.
	 */
        TimeVal(milliseconds_t);


        /** Seconds, microseconds constructor.
         */
        TimeVal(long sec, long usec);
        
        
    	/** Copy constructor.
	 */
        TimeVal(const timeval &);


    	/** Virtual desctructor.
	 */
        virtual ~TimeVal();


    	/** Assignment operator.
	 */
        TimeVal &	operator=(const timeval &);


    	/** Set this timeval with the current time. Wrapper for gettimeofday.
	 */
        const TimeVal & now();


    	/** Return the number of milliseconds.
	 */
        milliseconds_t	milliseconds() const;


    	/** Addition operator.
	 */
        TimeVal operator+(const timeval &) const;


    	/** Addition operator, millisecond version.
	 */
        TimeVal operator+(milliseconds_t) const;


    	/** Addition and assignment operator.
	 */
        TimeVal & operator+=(const timeval &);


    	/** Addition and assignment operator, millisecond version.
	 */
        TimeVal & operator+=(milliseconds_t);


    	/** Subtraction operator.
	 */
        TimeVal operator-(const timeval &) const;


    	/** Subtraction operator, millisecond version.
	 */
        TimeVal operator-(milliseconds_t) const;


    	/** Subtraction and assignment operator.
	 */
        TimeVal & operator-=(const timeval &);


    	/** Subtraction and assignment operator, millisecond version.
	 */
        TimeVal & operator-=(milliseconds_t);


    	/** Equality operator.
	 */
        bool	operator==(const timeval &) const;


    	/** Less than relational operator.
	 */
        bool	operator< (const timeval &) const;


    	/** Less than relational operator.
	 */
        bool	operator< (const TimeVal &) const;

    	/** Clear the timeval struct to 0.
	 */
        void	clear();


    	/** Copy the given timeval struct into this.
	 */
        void	copy(const timeval &);


    	/**  The purpose of normalize is to ensure that the tv_usec field
    	 *  is in [0,100000).
	 */
        void	normalize();


    	/** Write a TimeVal to an ostream.
	 */
        ostream & writeTo(ostream &) const;
};


} // namespace TimeAndDate
} // namespace Assist


#endif // !defined(ASSIST_TIME_VAL_HXX)
