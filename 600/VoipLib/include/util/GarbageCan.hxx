#if !defined(VOCAL_GARBAGE_CAN_HXX)
#define VOCAL_GARBAGE_CAN_HXX
/*
 * $Id: GarbageCan.hxx,v 1.2 2007/03/01 20:04:29 lizhijie Exp $
 */

#include "Mutex.hxx"
#include <list>


/** Infrastructure common to VOCAL.<br><br>
 */
namespace Vocal 
{


class Application;
class Garbage;


using Vocal::Threads::Mutex;
//using Vocal::Application;
using std::list;


/** A container that will destroy the contents upon destruction.
 *  Currently only the Application class and is allowed  to create and 
 *  destroy a garbage can. This is the mechanism by which Singletons 
 *  are destroyed.
 */
class GarbageCan
{
    private:
    
        /** Constructor
         */
        GarbageCan();
        
        
        /** Destructor
         */
        ~GarbageCan();
   
   public:
   

        /** Anyone can add garbage to the garbage can. It will be deleted
         *  when the garbage can is deleted. This is thread safe.
         */
        static void add(Garbage *);

        
    private:

    
        /** Deletes all the items in the garbage can. This is thread safe.
         */        
        void        empty();

    
        /** Protects the garbage list from concurrent access.
         */
        static  Mutex               myMutex;
        
        
        /** The stuff that need to be deleted.
         */
        static  list<Garbage *>     myGarbage;
        

    // Only Applications can create a GarbageCan.
    //
    friend  class Application;
};


} // namespace Vocal


#endif // !defined(VOCAL_GARBAGE_CAN_HXX)
