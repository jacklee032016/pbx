#ifndef STATE_MACHN_DATA_HXX
#define STATE_MACHN_DATA_HXX
/*
 * $Id: StateMachnData.hxx,v 1.2 2007/03/02 04:19:27 lizhijie Exp $
 */

#include "SipTo.hxx"
#include "SipFrom.hxx"
#include "Data.hxx"

namespace Vocal
{

/** Object StateMachnData
<pre>
<br> Usage of this Class </br>

  StateMachnData is a base class.
  It contains data that is active only during the length of a call.
  Applications must derive from this class and store their data
  in the derived class.

</pre>
 */
class StateMachnData
{
    public:


        /** Default constructor
         */
        StateMachnData();


        /** Virtual destructor
         */
        virtual ~StateMachnData();


        /** Returns name of class - derive
         */
        virtual const char* name() const = 0;


    private:
        
        
        /** Suppress copying
         */
        StateMachnData(const StateMachnData &);
        

        /** Suppress copying
         */
        const StateMachnData & operator=(const StateMachnData &);
};
 
}

#endif // STATE_MACHN_DATA_HXX
