#ifndef FEATURE_HXX
#define FEATURE_HXX

/*
 * $Id: Feature.hxx,v 1.2 2007/03/02 04:19:27 lizhijie Exp $
 */
 
#include "Sptr.hxx"
#include "cpLog.h"

#include "State.hxx"
#include "SipProxyEvent.hxx"

namespace Vocal 
{

/** Object Feature
<pre>
<br> Usage of this Class </br>

  Feature is a state container.

FooFeature.hxx:

  class FooFeature : public Vocal::Feature
  {
    public:
         FooFeature();
         virtual ~FooFeature();
         const char* const name() const;
  };

FooFeature.cxx:

  using namespace Vocal;

  FooFeature::FooFeature()
  {
    Sptr < State > mrshlState = new FooState;
    addState( mrshlState );
  }


  FooFeature::~FooFeature()
  {}


  const char* const
  FooFeature::name() const
  {
    return "FooFeature";
  }

</pre>
*/
class Feature
{
    public:


        /** Default constructor
         */
        Feature();


        /** Virtual destructor
         */
        virtual ~Feature();


        /** The extender of this class must provide the name of this
         *  class via this abstract method.
         */
        virtual const char* const name() const = 0;


        /** When an event arrives for this feature, each state machine 
         *  associated with this feature process the event, in the order 
         *  the state machines were added to this feature.
         */
        virtual void process(const Sptr < SipProxyEvent > event);


        /** Logging helper method
         */
        void doCpLog() const
        {
            cpLog( LOG_DEBUG, "Feature: %s", name() );
        }


        /** Add a state machine to the feature.
         */
        void addState(const Sptr < State > state);


    protected:


        /** List of states.
         */
        typedef list < Sptr < State > > StateContainer;


        /** Iterator for the states list. Yes, it's poorly named.
         */
        typedef StateContainer::iterator StateIter;


        /** Contains state machines to handle events.
         */
        StateContainer myStates;


    private:
        

        /** Suppress copying
         */
        Feature(const Feature &);


        /** Suppress copying
         */
        const Feature & operator=(const Feature &);
};
 
}

#endif // FEATURE_HXX
