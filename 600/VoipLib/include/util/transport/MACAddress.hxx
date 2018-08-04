#if !defined(MACADDRESS_DOT_H)
#define MACADDRESS_DOT_H
/*
 * $Id: MACAddress.hxx,v 1.2 2007/03/01 20:04:30 lizhijie Exp $
 */


#include "vtypes.h"


/** Infrastructure common to VOCAL.
 */
namespace Vocal 
{


/** Infrastructure in VOCAL related to making network transport layer 
 *  connections.<br><br>
 */
namespace Transport 
{


class Socket;


/** To retrieve the MAC address.<br><br>
 *
 *  @see    Vocal::Transport::Socket
 */
class MACAddress
{

    public:


    	/** Default constructor
	 */    
    	MACAddress();
	
	
	/** Retrieve the MAC address of the associated Socket.
	 */
    	MACAddress(const Socket &);
	

    	/** Virtual destructor.
	 */    
	virtual     	    	    ~MACAddress();


    	/** High end word.
	 */    
    	u_int32_t   	    	    high();


    	/** Low end word.
	 */    
    	u_int32_t   	    	    low();
	
    private:

    	void	    	    	    getMACAddress(int);
    
    	u_int32_t   	    	    high_;
    	u_int32_t   	    	    low_;
};


} // namespace Transport
} // namespace Vocal


#endif // !defined(MACADDRESS_DOT_H)
