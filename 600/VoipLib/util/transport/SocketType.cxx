/*
 * $Id: SocketType.cxx,v 1.2 2007/03/01 20:04:33 lizhijie Exp $
 */

#include "global.h"
#include "SocketType.hxx"


using Vocal::Transport::SocketType;
using std::string;


const string     SocketType::tcp("tcp");
const string     SocketType::udp("udp");
const string     SocketType::unknown("unknown");


SocketType::SocketType(int type)
    :	type_(type)
{
}


SocketType::SocketType(const SocketType & s)
    :	type_(s.type_)
{
}


SocketType::~SocketType()
{
}


SocketType &     
SocketType::operator=(const SocketType & src)
{
    if ( this != &src )
    {
    	type_ = src.type_;
    }
    return ( *this );
}


int  	    	    
SocketType::operator()()
{
    return ( type_ );
}


void	    	    
SocketType::operator()(int type)
{
    type_ = type;
}


bool	    	    
SocketType::operator==(const SocketType & rhs) const
{
    return ( type_ == rhs.type_ );
}


bool	    	    
SocketType::operator!=(const SocketType & rhs) const
{
    return ( type_ != rhs.type_ );
}


bool	    	    
SocketType::operator<(const SocketType & rhs) const
{
    return ( type_ < rhs.type_ );
}


bool	    	    
SocketType::operator<=(const SocketType & rhs) const
{
    return ( type_ <= rhs.type_ );
}


bool	    	    
SocketType::operator>(const SocketType & rhs) const
{
    return ( type_ > rhs.type_ );
}


bool	    	    
SocketType::operator>=(const SocketType & rhs) const
{
    return ( type_ >= rhs.type_ );
}


const string &      
SocketType::name(int type)
{
    switch ( type )
    {
        case SOCK_STREAM:   return tcp;
        case SOCK_DGRAM:    return udp;
        default:            break;
    }
    return ( unknown );
}


ostream &      
SocketType::writeTo(ostream & out) const
{
    return ( out << name(type_) );
}
