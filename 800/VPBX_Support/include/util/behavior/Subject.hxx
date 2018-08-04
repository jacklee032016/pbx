#if !defined(ASSIST_SUBJECT_HXX)
#define ASSIST_SUBJECT_HXX
/*
* $Id: Subject.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "Writer.hxx"
#include <vector>


/** Infrastructure common to ASSIST.
 */
namespace Assist 
{


/** Infrastructure in ASSIST related to "algorithms and the assignment of
 *  responsibilities between objects".<br><br>
 *
 *  See Design Patterns, ISBN 0-201-63361-2, Chapter 5.
 */
namespace Behavioral
{


using std::vector;


class Observer;


/** Defines a relationship between a Subject and an Observer such that when
 *  the Subject changes, the Observer will be "notified and updated 
 *  automatically".<br><br>
 *
 *  See Design Patterns, Gamma et al, page 293.
 *
 *  @see    Assist::Behavioral::Observer
 */
class Subject : public Assist::IO::Writer
{
    public:
    
    	/** Default constructor.
	 */
    	Subject();


    	/** Virtual destructor.
	 */
	virtual ~Subject();


    	/** Attach an observer to the subject. When the subject changes
	 *  the observer will be updated. The same observer can be
	 *  added multiple times, and will be updated multiple times.
	 */	
    	void	    	    attach(Observer &);


    	/** Detach an observer from the subject. If the observer was
	 *  attached multiple times, the first one will be removed.
	 */
	void	    	    detach(Observer &);


    	/** Notify all observer's of a change.
	 */
    	void	    	    notify();


    	/** Equality relational operator. The address of the object is
	 *  used for comparison, since each Subject is considered unique.
	 */
	bool	    	    operator==(const Subject &) const;


    	/** Write the Subject to an ostream.
	 */
	virtual ostream &   writeTo(ostream &) const;


    private:

    	vector<Observer *>  myObservers;
};


} // namespace Behavioral
} // namespace Assist


#endif // !defined(ASSIST_SUBJECT_HXX)
