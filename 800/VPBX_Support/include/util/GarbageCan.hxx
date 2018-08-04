#if !defined(ASSIST_GARBAGE_CAN_HXX)
#define ASSIST_GARBAGE_CAN_HXX
/*
* $Id: GarbageCan.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "Mutex.hxx"
#include <list>


/** Infrastructure common to ASSIST.<br><br>
 */
namespace Assist 
{


class Application;
class Garbage;


using Assist::Threads::Mutex;
//using Assist::Application;
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


} // namespace Assist


#endif // !defined(ASSIST_GARBAGE_CAN_HXX)
