#if !defined(PIPE_DOT_H)
#define PIPE_DOT_H

/*
* $Id: Pipe.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "Writer.hxx"
#include "FileDescriptor.hxx"


/** Infrastructure common to ASSIST.
 */
namespace Assist 
{


class SystemException;


/** Infrastructure in ASSIST related to Input/Output.<br><br>
 */
namespace IO
{



/** A wrapper class for the operating systems pipe.<br><br>
 *  
 *  @see    	Assist::IO::FileDescriptor
 *  @see    	Assist::IO::Writer
 *  @see    	Assist::SystemException
 */
class Pipe : public Assist::IO::Writer
{
    public:

	/** The default constructor creates a pipe. If is file descriptor
	 *  array parameter is zero, then a new pipe will be created. 
	 *  Otherwise the given parameter will be used to initialize the pipe.
	 *
	 *  @param  descriptors     An optional array of two file descriptors, 
	 *  	    	    	    created using the pipe(2) system call.
	 * 
	 *  @exception	Assist::SystemException	
	 *  	Thrown if a pipe could not be created.
	 */
    	Pipe(const file_descriptor_t * descriptors = 0)
	    throw ( Assist::SystemException );


	/** Creates a pipe given a pair of file descriptors.
	 */
    	Pipe(
	    const FileDescriptor    & 	readFd,
	    const FileDescriptor    &	writeFd
	);
	

	/** The destructor will close() the file descriptors if valid.
	 *
	 *  @exception Assist::SystemException
	 *  	Thrown if the closure of the pipe failed.
	 */
	virtual ~Pipe()
	    throw ( Assist::SystemException );


	/** Accessor to FileDescriptors.
	 *
	 *  @param index    Index 0 indicated the file descriptor for reading,
	 *  	    	    and an index of 1 (or non zero) indicates the
	 *  	    	    file descriptor for writing.
	 */
    	FileDescriptor &    operator[](size_t index);
	

    	/** Accessor to file descriptor used for reading.
	 */
	FileDescriptor &    readFD();


    	/** Accessor to file descriptor used for reading.
	 */
	const FileDescriptor &    readFD() const;
	

	/** Accessor to file descriptor used for writing.
	 */
	FileDescriptor &    writeFD();
	

	/** Accessor to file descriptor used for writing.
	 */
	const FileDescriptor &    writeFD() const;

	
	/** Equality relational operator.
	 */
	bool	operator==(const Pipe &) const;


	/** Less than relational operator.
	 */
	bool	operator<(const Pipe &) const;


	/** Write to an ostream.
	 */
	virtual ostream &       writeTo(ostream &) const;
	

    private:

    	FileDescriptor	    readFD_;
	FileDescriptor	    writeFD_;


	/** Copying is suppressed.
	 */
    	Pipe(const Pipe &);


	/** Copying is suppressed.
	 */
	Pipe &	operator=(const Pipe &);
};


} // namespace Transport
} // namespace Assist


#endif // !defined(PIPE_DOT_H)
