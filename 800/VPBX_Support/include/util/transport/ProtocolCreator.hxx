#if !defined(ASSIST_PROTOCOL_CREATOR_HXX)
#define ASSIST_PROTOCOL_CREATOR_HXX
/*
* $Id: ProtocolCreator.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "NonCopyable.hxx"
#include "Sptr.hxx"
#include "Protocol.hxx"
#include <string>


/** Infrastructure common to ASSIST.
 */
namespace Assist 
{


/** Infrastructure in ASSIST related to making network transport layer 
 *  connections.<br><br>
 */
namespace Transport 
{


class IPAddress;


using std::string;


/** 
 *
 */
class ProtocolCreator : public Assist::NonCopyable
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
} // namespace Assist


#endif // !defined(ASSIST_PROTOCOL_CREATOR_HXX)
