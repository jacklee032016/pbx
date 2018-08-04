#if !defined(ASSIST_PRIORITYLOG_HXX)
#define ASSIST_PRIORITYLOG_HXX
/*
* $Id: PriorityLog.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "Writer.hxx"
#include <string>
/* 
change this header as following, for the backward-warning
#include <strstream>
*/
#include <strstream>

/** Infrastructure common to ASSIST.
 */
namespace Assist 
{


/** Infrastructure in ASSIST related to logging.<br><br>
 */
namespace Logging
{


/** The ostream based log associated with a given priority.<br><br>
 *
 *  @see    Assist::Logging::Log
 *  @see    Assist::Logging::VLog
 *  @see    Assist::Logging::CpPriorityLog   
 */
class PriorityLog : public Assist::IO::Writer
{
    protected:


    	/** Create a prototype PriorityLog. See the Prototype pattern
	 *  in GOF's Design Pattern.
	 *
	 *  @param  priorityName    The name of the priority.
	 *  @param  priority	    The value of the priority.
	 *  @param  alwaysOn	    If set, the log cannot be disabled.
	 *  @param  defaultOn	    If set, the log will be created enabled.
	 */
	PriorityLog(
	    const char  	*   priorityName,
	    int 	    	    priority,
	    bool	    	    alwaysOn	= false,
	    bool	    	    defaultOn 	= true);
	

    	/** Copy constructor.
	 */
	PriorityLog(const PriorityLog &);


        /** Assignment
         */
        PriorityLog &	    operator=(const PriorityLog &);


    public:
    					

    	/** Virtual destructor.
	 */
	virtual ~PriorityLog();


    	/** Create a clone of this object. See the Prototype pattern from
	 *  Design Patterns.
	 */
    	virtual PriorityLog *	    clone() const = 0;
	

    	/** Get the native ostream.
	 */
		ostream &	    get(bool header = false);
	

    	/** Test to see if this object is enabled.
	 */
	    	bool	    	    isOn() const;


    	/** Enable this log.
	 */
		void	    	    on();


    	/** Disable this log.
	 */
		void	    	    off();


    	/** Callback that is called from end(), before writeLog() is called.
	 */
    	virtual void	    	    preWrite();


    	/** Callback that is called from end(), after writeLog() is called.
	 */
	virtual void	    	    postWrite();


        /** Set the file name for this log.
         */
                void                file(const char *);
            

        /** Set the file line for this log.
         */
                void                line(int);
                
                
    	/** Commit the contents of the native ostream to the log.
	 */
    	    	void	    	    end();
	

    	/** Abstract callback that is called from end() after preWrite and
	 *  before postWrite. The intent is that the user will override this
	 *  method and write the given log message to the log.
	 *
	 *  @param  priorityName    The name of the priority.
	 *  @param  priority	    The value of the priority.
	 *  @param  logMessage	    The actual message to log.
	 *  @param  file    	    The name of the file where the log is taking
	 *  	    	    	    place.
	 *  @param  line    	    The line number of the file where the log
	 *  	    	    	    is taking place.
	 */
	virtual void	    	    writeLog(
	    	    	    	    	const string	&   priorityName,
					int 	    	    priority,
					const char  	*   logMessage,
					const char  	*   file = 0, 
					int 	    	    line = 0) = 0;


        /** Another way to call end.
         */
        ostream & writeTo(ostream &) const;


    private:

    	    	ostrstream  	*   buffer_;
    	    	string	    	    priorityName_;
		int 	    	    priority_;
		bool	    	    alwaysOn_;
		bool	    	    on_;
                string              file_;
                int                 line_;
};


class setFile : public Assist::IO::Writer
{
    public:
        setFile(PriorityLog &, const char *);
        ostream & writeTo(ostream &) const;
};


class setLine : public Assist::IO::Writer
{
    public:
        setLine(PriorityLog &, int);
        ostream & writeTo(ostream &) const;
};


} // namespace Logging
} // namespace Assist


#endif // !defined(ASSIST_PRIORITYLOG_HXX)
