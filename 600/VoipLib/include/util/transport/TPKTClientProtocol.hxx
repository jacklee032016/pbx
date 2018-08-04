#if !defined(VOCAL_TPKT_CLIENT_PROTOCOL_HXX)
#define VOCAL_TPKT_CLIENT_PROTOCOL_HXX
/*
 * $Id: TPKTClientProtocol.hxx,v 1.2 2007/03/01 20:04:30 lizhijie Exp $
 */


#include "TransportCommon.hxx"
#include "Protocol.hxx"
#include "Sptr.hxx"
#include "SystemException.hxx"
#include "SystemStatus.hxx"
#include "ConnectionBrokenException.hxx"
#include <vector>


/** Infrastructure common to VOCAL.
 */
namespace Vocal 
{


/** Infrastructure in VOCAL related to making network transport layer 
 *  connections.<br><br>
 */
namespace Transport 
{


class TPKTClientSocket;


/** The TPKTClientProtocol will receive bytes from the wire and
 *  notify the user, via the onTPKTArrived, when a complete TPKT
 *  has been received, even if multiple TCP packets were sent.<br><br>
 *
 *  @see    Vocal::Transport::Protocol
 *  @see    Vocal::Transport::TPKTClientSocket
 *  @see    Vocal::SystemException
 *  @see    Vocal::SystemStatus
 *  @see    Vocal::ConnectionBrokenException
 */
class TPKTClientProtocol : public Vocal::Transport::Protocol
{
    public:


	/** Create with an optional name
	 */
    	TPKTClientProtocol(const char * name = 0);


	/** Virtual destructor
	 */
    	virtual ~TPKTClientProtocol();


	/** Get the TPKTClientSocket associated with this protocol
	 */
    	virtual TPKTClientSocket &     	    
	    	getTPKTClientSocket() = 0;
		

	/** Abstract callback that notifies the user of the 
	 *  availability of a complete TPKT.
	 */
    	virtual void	    	    	    
	    	onTPKTArrived(vector<u_int8_t> & message) = 0;


    	/** Override's Protocol's onIncomingAvailable. This will read
	 *  the bytes from the file descriptor until a complete TPKT
	 *  is available, at which point it will call onTPKTArrived.
	 */
	virtual void	    	    	    
	    	onIncomingAvailable()
		throw ( Vocal::SystemException, 
		    	Vocal::SystemStatus, 
			Vocal::Transport::ConnectionBrokenException );

    private:
    	    	Sptr< vector<u_int8_t> >    pkt_;
		u_int16_t   	    	    pktPosition_;
		u_int16_t   	    	    pktLength_;
};


} // namespace Transport
} // namespace Vocal


#endif // !defined(VOCAL_TPKT_CLIENT_PROTOCOL_HXX)
