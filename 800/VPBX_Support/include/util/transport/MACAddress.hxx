#if !defined(MACADDRESS_DOT_H)
#define MACADDRESS_DOT_H
/*
* $Id: MACAddress.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "vtypes.h"


/** Infrastructure common to ASSIST.
 */
namespace Assist 
{


/** Infrastructure in ASSIST related to making network transport layer 
 *  connections.<br><br>
 */
namespace Transport 
{


class Socket;


/** To retrieve the MAC address.<br><br>
 *
 *  @see    Assist::Transport::Socket
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
} // namespace Assist


#endif // !defined(MACADDRESS_DOT_H)
