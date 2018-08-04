/*
* $Id: ProtocolException.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "global.h"
#include "ProtocolException.hxx"


using Assist::Transport::ProtocolException;
using Assist::Transport::Protocol;


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
