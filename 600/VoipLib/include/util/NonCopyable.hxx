#if !defined(NONCOPYABLE_DOT_H)
#define NONCOPYABLE_DOT_H
/*
 * $Id: NonCopyable.hxx,v 1.2 2007/03/01 20:04:29 lizhijie Exp $
 */

#include "VocalCommon.hxx"


/** Infrastructure common to VOCAL.<br><br>
 */
namespace Vocal 
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


} // namespace Vocal


#endif // !defined(NONCOPYABLE_DOT_H)
