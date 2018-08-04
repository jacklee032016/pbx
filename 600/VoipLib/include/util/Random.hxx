#if !defined(VOCAL_RANDOM_HXX)
#define VOCAL_RANDOM_HXX
/*
 * $Id: Random.hxx,v 1.2 2007/03/01 20:04:29 lizhijie Exp $
 */


#include "global.h"


/** Infrastructure common to VOCAL.
 */
namespace Vocal 
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


} // namespace Vocal


#endif // !defined(VOCAL_RANDOM_HXX)
