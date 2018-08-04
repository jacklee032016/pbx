#ifndef WRITER_DOT_H
#define WRITER_DOT_H
/*
* $Id: Writer.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "global.h"
#include <iostream>

using std::ostream;


/** Infrastructure common to ASSIST.<br><br>
 */
namespace Assist 
{


/** Infrastructure in ASSIST related to Input/Output.<br><br>
 */
namespace IO
{


/** A interface for writing an object to an ostream.<br><br>
 */
class Writer
{
    public:

        Writer();
        virtual ~Writer();

    	/** Overloading this method allows the extending class to
	 *  be inserted onto an ostream.
	 */
        virtual ostream & writeTo(ostream &) const = 0;
};


} // namespace IO
} // namespace Assist


/** The Writer ostream insertion operator.
 */
extern ostream & operator<<(ostream &, const Assist::IO::Writer &);


#endif // !defined(WRITER_DOT_H)
