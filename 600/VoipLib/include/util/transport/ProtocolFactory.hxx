#if !defined(VOCAL_PROTOCOL_FACTORY_HXX)
#define VOCAL_PROTOCOL_FACTORY_HXX
/*
 * $Id: ProtocolFactory.hxx,v 1.2 2007/03/01 20:04:30 lizhijie Exp $
 */

#include "Singleton.hxx"
#include "Sptr.hxx"
#include "ProtocolCreator.hxx"
#include <string>
#include <map>


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


using Vocal::Singleton;
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
} // namespace Vocal


#endif // !defined(VOCAL_PROTOCOL_FACTORY_HXX)
