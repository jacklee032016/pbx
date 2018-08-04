#if !defined(ASSIST_SIGNAL_ACTION_HXX)
#define ASSIST_SIGNAL_ACTION_HXX
/*
* $Id: SignalAction.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "SignalSet.hxx"
#include "Writer.hxx"
#include "NonCopyable.hxx"
#include <string>


/** Infrastructure common to ASSIST.
 */
namespace Assist 
{


/** Infrastructure common to ASSIST to handle signals.
 */
namespace Signals
{


using std::string;


/** An interface class used to respond to a given signal.<br><br>
 *
 *  @see    Assist::Process::SignalHandler
 *  @see    Assist::Process::SignalSet
 */
class SignalAction : public Assist::IO::Writer, public Assist::NonCopyable
{
    public:

	/** Construct the action that will respond to the signals in the
	 *  given SignalSet.
	 */
        SignalAction(const SignalSet &, const char * name = 0);


	/** Virtual destructor.
	 */
        virtual ~SignalAction();


	/** Get the set of signals to be responded to.
	 */
        const SignalSet & signalSet() const;


	/** Function that will be called by the SignalHandler 
         *  when a signal in the SignalSet is thrown.
	 */
        void onSignal(int signum, int error, int code);


	/** The name of the class.
	 */
        const string & name() const;


	/** Write the SignalAction to an ostream.
	 */
        ostream & writeTo(ostream &) const;


        /** Returns true if a signal has been caught
         */
        bool signalCaught() const;


	/** The action is reset such that signalCaught will return false.
	 */
	void reset();


    protected:

	/** User callback called by onSignal.
	 */
        virtual void	handleSignal(int signum, int error, int code);
    
    
    private:

        SignalSet   mySignalSet;
        bool        mySignalCaught;
        string      myName;
};


} // namespace Signals
} // namespace Assist


#include "DeprecatedSignalAction.hxx"


#endif // !defined(ASSIST_SIGNAL_ACTION_HXX)
