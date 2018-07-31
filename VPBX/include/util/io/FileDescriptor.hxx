#if !defined(FILEDESCRIPTOR_DOT_H)
#define FILEDESCRIPTOR_DOT_H
/*
* $Id: FileDescriptor.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "Writer.hxx"
#include "SystemException.hxx"
#include "SystemStatus.hxx"
#include "vtypes.h"
#include <string>
#include <vector>

/** Infrastructure common to ASSIST.
 */
namespace Assist 
{




/** Infrastructure in ASSIST related to Input/Output.<br><br>
 */
namespace IO
{


/** Type for the native file descriptor, for portability 
 *  and readability.<br><br>
 */
typedef     int     file_descriptor_t;


/** A wrapper class for the operating systems file descriptor.<br><br>
 *  
 *  @see    	Assist::IO::file_descriptor_t
 *  @see    	Assist::IO::Writer
 *  @see    	Assist::SystemException
 */
class FileDescriptor : public Assist::IO::Writer
{
    public:

	/** The value of a file descriptor when it is unusable. This
	 *  value may vary across operating systems.
	 */
    	static const file_descriptor_t	    INVALID;


	/** The default constructor creates an INVALID file descriptor.
	 *  This can also be used to attach to pre-existing file descriptors.
	 */
    	FileDescriptor(file_descriptor_t = INVALID);
	

	/** The destructor will close() the file descriptor if it is valid.
	 *
	 *  @exception Assist::SystemException
	 */
	virtual ~FileDescriptor();


	/** Mutator given a new native file descriptor.
	 */
    	void	setFD(file_descriptor_t);


	/** Accessor to native file descriptor.
	 */
    	file_descriptor_t   	getFD() const;
	

	/** When set, this file descriptor will block when used with
	 *  a system call that could potentially block, e.g select().
	 *
	 *  @exception Assist::SystemException
	 */
	void	setBlocking()
	    	   throw ( Assist::SystemException );


	/** When set, this file descriptor will not block when used with
	 *  a system call that could potentially block, e.g select(),
	 *  potentially returning a status code like EAGAIN.
	 */
	void	setNonblocking()
	    	    throw ( Assist::SystemException );
	

    	/** Writes the string to this file descriptor.
	 *  <br><br>
    	 *  
	 *  Returns the number of bytes written. If a SystemStatus was 
	 *  thrown, the descriptor was set non-blocking and it would 
	 *  have blocked (status EAGAIN) or the read was interrupted by 
	 *  a signal (status EINTR). On a SystemException the descriptor 
	 *  encountered some system error.<br><br>
	 *
	 *  See write(2).
	 */
    	int 	write(const string &)
		    throw ( Assist::SystemException, Assist::SystemStatus );


    	/** See write(const string &).
	 */
    	int 	write(const char *)
		    throw ( Assist::SystemException, Assist::SystemStatus );


    	/** Binary version of write. See write(const string &).
	 */
    	int 	write(const vector<u_int8_t> &)
		    throw ( Assist::SystemException, Assist::SystemStatus );


    	/** Binary version of write. See write(const string &).
	 */
    	int 	write(const u_int8_t *, size_t)
		    throw ( Assist::SystemException, Assist::SystemStatus );
				

    	/** This will read a string from the descriptor.
	 *  Attempts to read bytes for the descriptor up to the size
	 *  of the given string.<br><br>
    	 *
	 *  Returns the number of bytes read, which may be less than the
	 *  size of the string. If a SystemStatus was thrown, the 
	 *  descriptor was set non-blocking and it would have blocked 
	 *  (status EAGAIN) or the read was interrupted by a signal (status
	 *  EINTR). On a SystemException the descriptor encountered some 
	 *  system error.<br><br>
	 *
	 *  See read(2).
	 */
    	int 	read(string &)
    	    	    throw ( Assist::SystemException, Assist::SystemStatus );


    	/** Text version of read. See read(string &).
	 */
    	int 	read(char *, size_t)
    	    	    throw ( Assist::SystemException, Assist::SystemStatus );


    	/** Binary version of read See read(string &).
	 */
    	int 	read(vector<u_int8_t> &)
		    throw ( Assist::SystemException, Assist::SystemStatus );


    	/** Binary version of read. See read(string &).
	 */
    	int 	read(u_int8_t *, size_t)
    	    	    throw ( Assist::SystemException, Assist::SystemStatus );


	/** Return the resource back to the operating system. The file 
	 *  descriptor is set INVALID.
	 */
	void	close()
	    	    throw ( Assist::SystemException );


	/** Equality relational operator.
	 */
	bool	operator==(const FileDescriptor &) const;


	/** Inequality relational operator.
	 */
	bool	operator!=(const FileDescriptor &) const;


	/** Less than relational operator.
	 */
	bool	operator<(const FileDescriptor &) const;


	/** Less than or equal relational operator.
	 */
	bool	operator<=(const FileDescriptor &) const;


	/** Greater than relational operator.
	 */
	bool	operator>(const FileDescriptor &) const;


	/** Greater than or equal relational operator.
	 */
	bool	operator>=(const FileDescriptor &) const;


	/** Write to an ostream.
	 */
	virtual ostream &       writeTo(ostream &) const;
	

    protected:

	/** The native file descriptor is available to classes extending
	 *  FileDescriptor.
	 */
    	file_descriptor_t   	fd_;


    private:


    	int 	writeToFD(void *, int length)
    	    	    throw ( Assist::SystemException, Assist::SystemStatus );
				

    	int 	readFromFD(void *, int length)
    	    	    throw ( Assist::SystemException, Assist::SystemStatus );

	/** Copying is suppressed.
	 */
    	FileDescriptor(const FileDescriptor &);


	/** Copying is suppressed.
	 */
	FileDescriptor &    	operator=(const FileDescriptor &);
};


} // namespace Transport
} // namespace Assist


#endif // !defined(FILEDESCRIPTOR_DOT_H)
