#if !defined(ASSIST_SINGLETON_HXX)
#define ASSIST_SINGLETON_HXX

/*
* $Id: Singleton.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "NonCopyable.hxx"
#include "GarbageCan.hxx"
#include "Garbage.hxx"


/** Infrastructure common to ASSIST.<br><br>
 */
namespace Assist 
{


/** Singleton pattern. See Gang of Four.
 */
template <class Object>
class Singleton : public Garbage, public NonCopyable
{
    protected:
    
        /** Protected constructor to force specialization.
         */
        Singleton();
        
        
        /** Protected virtual destructor. This should only be deleted
         *  via Garbage's virtual destructor.
         */
        virtual ~Singleton();


    public:


        /**  Create an instance of this class.
         */
        static Object &         instance();

        
    protected:


        /** Initialize the instance.
         */    
        static void             init();

        
    private:


        /** The instance.
         */    
        static  Object      *   myInstance;
        static  unsigned int    myCount;
};


template <class Object>
Object  *   Singleton<Object>::myInstance = 0;


template <class Object>
unsigned int    Singleton<Object>::myCount = 0;


template <class Object>
Singleton<Object>::Singleton()
{
    ++myCount;
    
    if ( myCount > 1 )
    {
        throw ( "There can be only one.");
    }
}


template <class Object>
Singleton<Object>::~Singleton()
{
}


template <class Object>
Object &
Singleton<Object>::instance()
{
    init();
    return ( *myInstance );
}


template <class Object>
void
Singleton<Object>::init()
{
    if ( myInstance == 0 )
    {
        myInstance = new Object;

        GarbageCan::add(myInstance);
    }
}


} // namespace Assist


#endif // !defined(ASSIST_SINGLETON_HXX)
