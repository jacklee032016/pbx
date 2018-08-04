#if !defined(ASSIST_GARBAGE_HXX)
#define ASSIST_GARBAGE_HXX
/*
* $Id: Garbage.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "AssistCommon.hxx"


#include <iostream>


/** Infrastructure common to ASSIST.<br><br>
 */
namespace Assist 
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


} // namespace Assist


#endif // !defined(ASSIST_GARBAGE_HXX)
