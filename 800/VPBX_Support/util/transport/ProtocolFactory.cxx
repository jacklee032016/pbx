/*
* $Id: ProtocolFactory.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "global.h"
#include "ProtocolFactory.hxx"


using Assist::Transport::ProtocolFactory;
using Assist::Transport::ProtocolCreator;
using Assist::Transport::Protocol;
using Assist::Transport::IPAddress;
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
        

