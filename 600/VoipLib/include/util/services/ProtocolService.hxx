#if !defined(VOCAL_PROTOCOL_SERVICE_HXX)
#define VOCAL_PROTOCOL_SERVICE_HXX
/*
 * $Id: ProtocolService.hxx,v 1.2 2007/03/01 20:04:30 lizhijie Exp $
 */


#include "PollFifoService.hxx"
#include "PollFifo.hxx"
#include "Protocol.hxx"
#include "Sptr.hxx"
#include "Event.hxx"
#include <list>


namespace Vocal {
    namespace Configuration {
        class Config;
    }
}


/** Vovida Open Communication Application Library.
 */
namespace Vocal
{


/** 
 */
namespace Services
{


//using Vocal::Services::PollFifoService;
//using Vocal::Services::Event;
using Vocal::Configuration::Config;
using Vocal::Transport::Protocol;
using Vocal::ReturnCode;
using std::list;


/** 
 */
class ProtocolService : public PollFifoService
{
    public:


	/** 
	 */
    	ProtocolService(
	    const Config	    &	config,
            const char              *   name
	);


	/** Virtual destuctor.
	 */
	virtual ~ProtocolService();


    protected:
    
    	/** Starts the service.
	 */
	ReturnCode  onStart();


    	/** Stops the service. The service may be started again.
	 */
	ReturnCode  onStop();


    	/** Shutdowns the service. The service will exit.
	 */
	ReturnCode  onShutdown();


        /**
         */
        virtual void    createProtocols();    
        

    	/** Clean up allocated memory.
    	 */
    	virtual void    clear();


	/** 
    	 */
    	list< Sptr<Protocol> >     myProtocols;


    	/** The configuration information for the application.
	 */
	const Config    	&   myConfig;
};


} // namespace Services
} // namespace Vocal


#endif // !defined(VOCAL_PROTOCOL_SERVICE_HXX)
