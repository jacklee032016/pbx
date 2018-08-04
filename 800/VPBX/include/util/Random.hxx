#if !defined(ASSIST_RANDOM_HXX)
#define ASSIST_RANDOM_HXX
/*
* $Id: Random.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"


/** Infrastructure common to ASSIST.
 */
namespace Assist 
{


/** A simple wrapper for the system srand() and rand().<br><br>
 */
class Random
{
    public:

        /** Constructor
         */
        Random();
        
        
        /** Virtual destructor
         */
        virtual ~Random();


        /** Get a random number.
         */
        virtual int     operator()();
        
        
        /** Get a random number.
         */
        virtual int     get();
        

        /** Initializes the random number generator.
         */
        static void     init(unsigned int seed = 0);
        
        
        /** Returns the seed used.
         */
        static unsigned int    seed();
        

    private:
    
        
        /** Indicates whether the random number generator has been initialized.
         */
        static bool             myInit;
        

        /** Number used to seed the generator.
         */
        static unsigned int     mySeed;
};


} // namespace Assist


#endif // !defined(ASSIST_RANDOM_HXX)
