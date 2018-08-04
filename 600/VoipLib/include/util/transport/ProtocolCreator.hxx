#if !defined(VOCAL_PROTOCOL_CREATOR_HXX)
#define VOCAL_PROTOCOL_CREATOR_HXX

/*
 * $Id: ProtocolCreator.hxx,v 1.2 2007/03/01 20:04:30 lizhijie Exp $
 */

#include "NonCopyable.hxx"
#include "Sptr.hxx"
#include "Protocol.hxx"
#include <string>


/** Infrastructure common to VOCAL.
 */
namespace Vocal 
{


/** Infrastructure in VOCAL related to making network transport layer 
 *  connections.<br><br>
 */
namespace Transport 
{


class IPAddress;


using std::string;


/** 
 *
 */
class ProtocolCreator : public Vocal::NonCopyable
{
    public:


    	/** Constructor.
	 */
    	ProtocolCreator() {}


    	/** Virtual destructor
	 */
    	virtual ~ProtocolCreator() {}
	

        /**
         */
        virtual const string &  ianaName() const = 0;
        

        /**
         */
        virtual const string &  proto() const = 0;
        

        /**
         */
        virtual Sptr<Protocol>  create() const = 0;


        /**
         */
        virtual Sptr<Protocol>  create(const IPAddress &) const = 0;
};


} // namespace Transport
} // namespace Vocal


#endif // !defined(VOCAL_PROTOCOL_CREATOR_HXX)
