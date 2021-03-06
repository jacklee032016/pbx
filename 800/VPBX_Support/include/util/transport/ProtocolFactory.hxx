#if !defined(ASSIST_PROTOCOL_FACTORY_HXX)
#define ASSIST_PROTOCOL_FACTORY_HXX
/*
* $Id: ProtocolFactory.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "Singleton.hxx"
#include "Sptr.hxx"
#include "ProtocolCreator.hxx"
#include <string>
#include <map>


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


using Assist::Singleton;
using std::map;
using std::string;


/** 
 *
 */
class ProtocolFactory : public Singleton<ProtocolFactory>
{
    public:


    	/** Constructor.
	 */
    	ProtocolFactory();


    	/** Virtual destructor
	 */
    	virtual ~ProtocolFactory();
	

        /**
         */
        void    addCreator( Sptr<ProtocolCreator> );
        
        
        /**
         */
        Sptr<Protocol>      createProtocol(
            const string        &   ianaName,
            const string        &   proto
        ) const;
        
        /**
         */
        Sptr<Protocol>      createProtocol(
            const string        &   ianaName,
            const string        &   proto,
            const IPAddress     &   ipAddress
        ) const;
        

    private:
        
        struct ProtocolKey
        {
            ProtocolKey(ProtocolCreator & creator)
                :   ianaName(creator.ianaName()),
                    proto(creator.proto())
            {}

            ProtocolKey(const string & i, const string & p)
                :   ianaName(i),
                    proto(p)
            {}
                
            const string  &     ianaName;
            const string  &     proto;
            
            bool operator==(const ProtocolKey & src) const
            {
                return ( ianaName == src.ianaName && proto == src.proto );
            }

            bool operator<(const ProtocolKey & src) const
            {
                return ( ianaName < src.ianaName ? true : proto < src.proto );
            }
        };        

        typedef  map<ProtocolKey, Sptr<ProtocolCreator> >    CreatorMap;
        
        CreatorMap      myCreators;
};


} // namespace Transport
} // namespace Assist


#endif // !defined(ASSIST_PROTOCOL_FACTORY_HXX)
