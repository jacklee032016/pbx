#if !defined(ADDRESS_FAMILY_DOT_H)
#define ADDRESS_FAMILY_DOT_H
/*
 * $Id: AddressFamily.hxx,v 1.2 2007/03/01 20:04:30 lizhijie Exp $
 */


#if defined(__FreeBSD__) || defined(__APPLE__)
#include <sys/types.h>
#endif

#include <sys/socket.h>
#include "Writer.hxx"

/** Infrastructure common to VOCAL.
 */
namespace Vocal 
{

/** Infrastructure in VOCAL related to making network transport layer 
 *  connections.<br><br>
 */
namespace Transport 
{


/** Abstracts AF_* (or PF_*) as defined in sys/socket.h.<br><br>
 *
 *  @see    Vocal::Transport::Socket
 *  @see    Vocal::IO::Writer
 */
class AddressFamily : public Vocal::IO::Writer
{
    public:
    	
    	/** Construct by AF_* defined in sys/socket.h. Default constructs
	 *  to AF_INET.
	 */
    	AddressFamily(int addressFamily = AF_INET);

    	/** Copy constructor.
	 */
    	AddressFamily(const AddressFamily &);
	
    	/** Destructor.
	 */
	virtual ~AddressFamily();

    	/** Assignment operator.
	 */
    	AddressFamily &     operator=(const AddressFamily &);
	
    	/** Accessor.
	 */
	int  	    	    operator()() const;
	
    	/** Mutator.
	 */
    	void	    	    operator()(int);

    	/** Equality relational operator.
	 */
	bool	    	    operator==(const AddressFamily &) const;

    	/** Inequality relational operator.
	 */
	bool	    	    operator!=(const AddressFamily &) const;

    	/** Less than relational operator.
	 */
	bool	    	    operator<(const AddressFamily &) const;

    	/** Less or equal to than relational operator.
	 */
	bool	    	    operator<=(const AddressFamily &) const;

    	/** Greater than relational operator.
	 */
	bool	    	    operator>(const AddressFamily &) const;

    	/** Greater than or equal to relational operator.
	 */
	bool	    	    operator>=(const AddressFamily &) const;
	
    	/** Write to ostream.
	 */
	ostream &           writeTo(ostream &) const;
	
    private:

    	int  	    	    addressFamily_;
};


} // namespace Transport
} // namespace Vocal


#endif // !defined(ADDRESS_FAMILY_DOT_H)
