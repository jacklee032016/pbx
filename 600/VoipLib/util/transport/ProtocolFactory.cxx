/*
 * $Id: ProtocolFactory.cxx,v 1.2 2007/03/01 20:04:33 lizhijie Exp $
 */


#include "global.h"
#include "ProtocolFactory.hxx"


using Vocal::Transport::ProtocolFactory;
using Vocal::Transport::ProtocolCreator;
using Vocal::Transport::Protocol;
using Vocal::Transport::IPAddress;
using std::map;
using std::string;


ProtocolFactory::ProtocolFactory()
{
}


ProtocolFactory::~ProtocolFactory()
{
}


void    
ProtocolFactory::addCreator(Sptr<ProtocolCreator> creator)
{
    myCreators[ProtocolKey(*creator)] = creator;
}
        
        
Sptr<Protocol>      
ProtocolFactory::createProtocol
(
    const string        &   ianaName,
    const string        &   proto
) const
{
    CreatorMap::const_iterator i = myCreators.find(ProtocolKey(ianaName, proto));
    if ( i != myCreators.end() )
    {
        return ( i->second->create() );
    }
    return ( 0 );
}
        

Sptr<Protocol>      
ProtocolFactory::createProtocol(
    const string        &   ianaName,
    const string        &   proto,
    const IPAddress     &   ipAddress
) const
{
    CreatorMap::const_iterator i = myCreators.find(ProtocolKey(ianaName, proto));
    if ( i != myCreators.end() )
    {
        return ( i->second->create(ipAddress) );
    }
    return ( 0 );
}
        

