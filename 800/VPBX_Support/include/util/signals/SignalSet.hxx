#if !defined(ASSIST_SIGNAL_SET_HXX)
#define ASSIST_SIGNAL_SET_HXX
/*
* $Id: SignalSet.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "global.h"
#include <signal.h>
#include <vector>
#include "Writer.hxx"


/** Infrastructure common to ASSIST.
 */
namespace Assist 
{


/** Infrastructure common to ASSIST to handle signals.
 */
namespace Signals
{


using std::vector;


/** A light-weight wrapper for the sigset_t structure.<br><br>
 *  
 *  @see    Assist::Signals::SignalAction
 *  @see    Assist::Signals::SignalHandler
 */
class SignalSet : public Assist::IO::Writer
{
    public:

	/** Default constructor.
	 */
        SignalSet();


	/** Create a signal set with the given signal.
	 */
        SignalSet(int signum);


	/** Create a signal set with the array of signals.
	 */
        SignalSet(int *, size_t);


	/** Crate a signal set with the vector of signals.
	 */
        SignalSet(const vector<int> &);


	/** Copy constructor.
	 */
        SignalSet(const SignalSet &);


	/** Virtual destructor.
	 */
        virtual ~SignalSet();


	/** Equality relational operator.
	 */
        SignalSet & operator=(const SignalSet &);


	/** All signals are set.
	 */
        void	fill();


	/** No signals set.
	 */
        void	empty();


	/** Add the given signal to the set.
	 */
        void	insert(int);


	/** Add the given array of signals to the set.
	 */
        void	insert(int *, size_t);


	/** Add the given vector of signals to the set.
	 */
        void	insert(const vector<int> &);


	/** Remove a signal from the set.
	 */
        void	erase(int);


	/** Remove an array of signals from the set.
	 */
        void	erase(int *, size_t);


	/** Remove a vector of signals from the set.
	 */
        void	erase(const vector<int> &);


	/** Returns true if the given signal is in the set.
	 */
        bool	contains(int) const;


	/** Returns true if all the signals in the array are in the set.
	 */
        bool	contains(int *, size_t) const;


	/** Returne true if all the signals in the vector are in the set.
	 */
        bool	contains(const vector<int> &) const;


	/** Returns the vector of signal set.
	 */
        const vector<int> & signalNumbers() const;


	/** The native signal set.
	 */
        sigset_t signals;


	/** Write the SignalSet to an ostream.
	 */
        virtual ostream & writeTo(ostream &) const;


    private:

        vector<int>     mySignalNumbers;
};


} // namespace Signals
} // namespace Assist


#include "DeprecatedSignalSet.hxx"


#endif // !defined(ASSIST_SIGNAL_SET_HXX)
