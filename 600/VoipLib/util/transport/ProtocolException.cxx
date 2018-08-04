/*
 * $Id: ProtocolException.cxx,v 1.2 2007/03/01 20:04:33 lizhijie Exp $
 */


#include "global.h"
#include "ProtocolException.hxx"


using Vocal::Transport::ProtocolException;
using Vocal::Transport::Protocol;


ProtocolException::ProtocolException(  
    const string	    & 	msg,
    Type    	    	    	type,
    Protocol	    	    &	protocol
)
    :	VException(msg, __FILE__, __LINE__, (int)type),
    	myType(type),
	myProtocol(protocol)
{
}
    

string      	    	    
ProtocolException::getName() const
{
    return ( "ProtocolException" );
}


ProtocolException::Type	    	    	    
ProtocolException::getType() const
{
    return ( myType );
}


Protocol &   	    	    	    
ProtocolException::getProtocol() const
{
    return ( myProtocol );
}
