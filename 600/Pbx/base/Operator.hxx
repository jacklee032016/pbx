#ifndef OPERATOR_HXX
#define OPERATOR_HXX
/*
 * $Id: Operator.hxx,v 1.2 2007/03/02 04:19:27 lizhijie Exp $
 */

#include "Sptr.hxx"
#include "SipProxyEvent.hxx"
#include "State.hxx"
#include "SipUrl.hxx"

namespace Vocal
{

/* We have circular dependancies, so forward declare.
 */
class State;
class SipUrl;
class SipRecordRoute;
class SipVia;


/**Object Operator
<pre>
<br> Usage of this Class </br>

    Operator is a base class.  The Operator::process() is pure virtual.
    Event handler is implemented by the derived class of the operator

</pre>
*/
class Operator
{
    public:


        /** Default constructor.
         */
        Operator();


        /** Virtual destructor
         */
        virtual ~Operator();


        /** The extender of this class must provide the name of this
         *  class via this abstract method.
         */
        virtual const char* const name() const = 0;


        /** Process the incoming event. 
         */
        virtual const Sptr < State > process(const Sptr < SipProxyEvent > event)
	{ 
	    return ( 0 );
	}
        
        
        /** Logging helper method
         */
        virtual void doCpLog() const
        {
            cpLog( LOG_DEBUG, "Operator: %s", name() );
        }


        /** Add the local host address, port and version 2.0 to the 
         *  given via list. 
         *
         *  Don't know that this is the correct place for this method.
         */
        void OpAddVia(SipVia& sipvia, int port = 5060) const;


        /** Takes an empty recordRoute, an existing sipUrl, and optionally
         *  a port. It sets the host and port of the sipUrl to that of the
         *  caller and sets the recordRoute's url to the new sipUrl.
         *
         *  Don't know that this is the correct place for this method.
         */
        void OpAddRecordRoute( SipRecordRoute       &   recordRoute,
                               const Vocal::SipUrl  &   recordRouteUrl,
                               const int            port = 5060) const;


    private:


        /** Suppress copying
         */
        Operator(const Operator& rhs);


        /** Suppress copying
         */
        Operator& operator=(const Operator& rhs);
};
 
}

#endif // OPERATOR_HXX
