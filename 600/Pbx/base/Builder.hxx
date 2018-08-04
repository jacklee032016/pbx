#ifndef BUILDER_HXX
#define BUILDER_HXX
/*
 * $Id: Builder.hxx,v 1.2 2007/03/02 04:19:27 lizhijie Exp $
 */

#include "Sptr.hxx"
#include "CallContainer.hxx"

namespace Vocal
{

class SipProxyEvent;
class SipTransceiver;
class Feature;

/** 
   Object Builder.
    <b> Usage of this Class </b>
    <P>
     Builder is a base class. It is passed Events by the WorkerThread
    for processing. It is a Feature container. Feature is a State container.
    State is an Operator container.

    It also contains a pointer to the proxy CallContainer.
    A builder object is used in constructing a HeartLessProxy/BasicProxy
    object.

 FooBuilder.hxx :<p>
<pre>

    class FooBuilder : public Vocal::Builder
    {
      public:
        FooBuilder();
        virtual ~FooBuilder();
        const char* const name() const;
    };
</pre>
 FooBuilder.cxx:
<pre>
    using namespace Vocal;

    FooBuilder::FooBuilder()
    {
      Sptr < FooFeature > FooFeature = new FooFeature;
      addFeature( FooFeature );
    }

    FooBuilder::~FooBuilder()
    {}

    const char* const
    FooBuilder::name() const
    {
     return "FooBuilder";
    }

</pre>
*/
class Builder
{
   public:


      /** Normal constructor initializes myCallContainer.
       */
      Builder();


      /** Virtual destructor
       */
      virtual ~Builder();


      /** First checks if the event is of SipEvent type. If so,
       *  it gets the CallInfo from the CallContainer and sets it in the
       *  SipEvent. It then passes the event to the Feature container
       *  process methods. After processing, it removes the call from 
       *  the CallContainer.
       */
      virtual void process( const Sptr < SipProxyEvent > event );


      /** Set the builder's callContainer
       */
      void setCallContainer(const Sptr < CallContainer > callContainer);

      void setSipStack(const Sptr < SipTransceiver > sipStack);
      

   protected:


      /** Method adds a feature to the container
       *  @param feature pointer to the feature to add
       *  @return void
       */
      void addFeature( const Sptr < Feature > feature );


      /** typedef list of Feature Sptr's
       */
      typedef list < Sptr < Feature > > FeatureContainer;


      /** FeatureContainer iterator. Yes, it's named poorly.
       */
      typedef FeatureContainer::iterator FeatureIter;
        

      /** Feature container object
       */
      FeatureContainer myFeatures;


      /** Pointer to the proxy CallContainer
       */
      Sptr < CallContainer > myCallContainer;

      ///
      Sptr < SipTransceiver > mySipStack;

   private:


      /** Suppress copying
       */
      Builder(const Builder &);

      /** Suppress copying
       */
      const Builder& operator=(const Builder &);
};
 
}

#endif // BUILDER_HXX
