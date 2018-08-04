#ifndef CALL_CONTAINER_HXX
#define CALL_CONTAINER_HXX
/*
 * $Id: CallContainer.hxx,v 1.2 2007/03/02 04:19:27 lizhijie Exp $
 */

#include "CallInfo.hxx"

namespace Vocal
{

/** Object CallContainer

<pre>
<br> Usage of this Class </br>

    CallContainer is a base class mainly used by the UserAgent and
    feature servers for CallInfo objects.

    Sptr < CallContainer >  myCallContainer = new CallContainer ;
    Sptr < Builder >  myBuilder;

    myBuilder->setCallContainer(myCallContainer);

    Check HeartLessProxy.cxx for more details on the usage.
</pre>
*/
class CallContainer
{
    public:


        /** Default constructor
         */
        CallContainer();


        /** Destructor method
         */
        virtual ~CallContainer();


        /** checks for the passed call leg in the CallContainer.
	 *  It creates a new CallInfo object if the call leg is not found.
	 *  @param newOrExistingCall pointer to the call leg to search for.
	 *  @return Sptr< CallInfo > pointer to the CallInfo object
	 */ 
        virtual Sptr < CallInfo > 
        getCall(const Sptr < SipCallLeg > newOrExistingCall);


        /** findCall checks if the call leg is present in the CallContainer.
	 *  @param newOrExistingCall pointer to the call leg to search for.
	 *  @return Sptr< CallInfo > pointer to the CallInfo object if found, otherwise 0
	 */
        virtual Sptr < CallInfo > 
        findCall(const Sptr < SipCallLeg > newOrExistingCall);


        /** addCall adds a CallInfo object to the CallContainer
	 *  @param newOrExistingCall pointer to the call leg Key
	 *  @param theCallInfo pointer to the CallInfo object
	 *  @return void
	 */
        virtual Sptr < CallInfo > 
        addCall(const Sptr < SipCallLeg > newOrExistingCall,
                const Sptr < CallInfo > theCallInfo);


        /** remove call removes a CallInfo object from the CallContainer
	 *  @param existingCall pointer to the call leg Key
	 *  @return void
	 */
        virtual void removeCall(const Sptr < SipCallLeg > existingCall);


    protected:


        /** typedef map of CallInfo pointers, using a SipCallLeg key
         */
        typedef map < Sptr<SipCallLeg>, Sptr < CallInfo > > Table;


        /** Table iterator 
         */
        typedef Table::iterator TableIter;


        /** used to lock access to the CallContainer data.
         */
        mutable VMutex myMutex;


        /** CallContainer map object
         */
        Table myCallInfo;
        

    private:
    

        /** Suppress copying
         */
        CallContainer(const CallContainer &);


        /** Suppress copying
         */
        const CallContainer & operator=(const CallContainer &);
};
 
}

#endif // CALL_CONTAINER_HXX
