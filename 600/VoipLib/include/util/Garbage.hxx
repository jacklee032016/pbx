#if !defined(VOCAL_GARBAGE_HXX)
#define VOCAL_GARBAGE_HXX
/*
 * $Id: Garbage.hxx,v 1.2 2007/03/01 20:04:29 lizhijie Exp $
 */

#include "VocalCommon.hxx"


#include <iostream>


/** Infrastructure common to VOCAL.<br><br>
 */
namespace Vocal 
{


/** Asssures that a public virtual destructor is available, while allowing 
 *  this to be an abstract class.
 */
class Garbage
{
    public:


        /** Public abstract virtual destructor.
         */    
        virtual ~Garbage() = 0;
};


} // namespace Vocal


#endif // !defined(VOCAL_GARBAGE_HXX)
