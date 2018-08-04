#if !defined(ASSIST_PROTOCOL_SERVICE_HXX)
#define ASSIST_PROTOCOL_SERVICE_HXX
/*
* $Id: ProtocolService.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "PollFifoService.hxx"
#include "PollFifo.hxx"
#include "Protocol.hxx"
#include "Sptr.hxx"
#include "Event.hxx"
#include <list>


namespace Assist {
    namespace Configuration {
        class Config;
    }
}


/** Vovida Open Communication Application Library.
 */
namespace Assist
{


/** 
 */
namespace Services
{


//using Assist::Services::PollFifoService;
//using Assist::Services::Event;
using Assist::Configuration::Config;
using Assist::Transport::Protocol;
using Assist::ReturnCode;
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
} // namespace Assist


#endif // !defined(ASSIST_PROTOCOL_SERVICE_HXX)
