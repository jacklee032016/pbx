#ifndef WRITER_DOT_H
#define WRITER_DOT_H
/*
 * $Id: Writer.hxx,v 1.2 2007/03/01 20:04:30 lizhijie Exp $
 */


#include "global.h"
#include <iostream>

using std::ostream;


/** Infrastructure common to VOCAL.<br><br>
 */
namespace Vocal 
{


/** Infrastructure in VOCAL related to Input/Output.<br><br>
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
} // namespace Vocal


/** The Writer ostream insertion operator.
 */
extern ostream & operator<<(ostream &, const Vocal::IO::Writer &);


#endif // !defined(WRITER_DOT_H)
