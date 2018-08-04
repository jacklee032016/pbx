#if !defined(VOCAL_OBSERVER_HXX)
#define VOCAL_OBSERVER_HXX
/*
 * $Id: Observer.hxx,v 1.2 2007/03/01 20:04:29 lizhijie Exp $
 */


#include "Writer.hxx"
#include "NonCopyable.hxx"


/** Infrastructure common to VOCAL.
 */
namespace Vocal 
{


/** Infrastructure in VOCAL related to "algorithms and the assignment of
 *  responsibilities between objects".<br><br>
 *
 *  See Design Patterns, ISBN 0-201-63361-2, Chapter 5.
 */
namespace Behavioral
{


class Subject;


/** Defines a relationship between an Observer and a Subject such that when
 *  the Subject changes, the Observer will be "notified and updated 
 *  automatically".<br><br> 
 *
 *  See Design Patterns, Gamma et al, page 293. This is meant as an 
 *  interface class only. The user should extend this class as needed.<br><br>
 *
 *  For simplicity, Observers are not copyable, and each instance
 *  is considered unique.
 *
 *  @see    Vocal::Behavioral::Subject
 */
class Observer : public Vocal::IO::Writer, public Vocal::NonCopyable
{
    public:
    
    	Observer();
	virtual ~Observer();
	

    	/** The user will extend the interface using update().
	 *  When attached to a Subject and the Subject changes state,
	 *  this method will be called by the Subject, allowing the
	 *  Observer to synchronize with the Subject.
	 */
	virtual void	    update(Subject &);
	

    	/** Since each Observer is unique the memory address of the
	 *  Observer can be used to determine it's equality.
	 */
    	bool	    	    operator==(const Observer &) const;


    	/** Since each Observer is unique the memory address of the
	 *  Observer can be used to determine it's relation to other
	 *  Observers.
	 */
    	bool	    	    operator<(const Observer &) const;
};


} // namespace Behavioral
} // namespace Vocal


#endif // !defined(VOCAL_OBSERVER_HXX)
