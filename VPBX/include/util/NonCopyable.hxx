#if !defined(NONCOPYABLE_DOT_H)
#define NONCOPYABLE_DOT_H
/*
* $Id: NonCopyable.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "AssistCommon.hxx"


/** Infrastructure common to ASSIST.<br><br>
 */
namespace Assist 
{


/** Interface to suppress copying.<br><br>
 */
class NonCopyable
{
    protected:
        
        /** Protected constructor forces derivation.
         */
        NonCopyable() {}
        

        /** Virtual destructor, just in case someone does something 
         *  silly like trying to destroy a NonCopyable pointer.
         */
        virtual ~NonCopyable() {}

        
    private:
    
        /** Supresses copying. Do not create an implementation.
         */
        NonCopyable(const NonCopyable &);


        /** Supresses copying. Do not create an implementation.
         */
        const NonCopyable & operator=(const NonCopyable &);
};


} // namespace Assist


#endif // !defined(NONCOPYABLE_DOT_H)
