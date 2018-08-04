/*
* $Id: AddressFamily.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/
#include "global.h"
#include "AddressFamily.hxx"


using Assist::Transport::AddressFamily;


AddressFamily::AddressFamily(int addressFamily)
    :	addressFamily_(addressFamily)
{
}


AddressFamily::AddressFamily(const AddressFamily & src)
    :	addressFamily_(src.addressFamily_)
{
}


AddressFamily::~AddressFamily()
{
}


AddressFamily &
AddressFamily::operator=(const AddressFamily & src)
{
    if ( this != &src )
    {
    	addressFamily_ = src.addressFamily_;
    }
    return ( *this );
}

int
AddressFamily::operator()() const
{
    return ( addressFamily_ );
}


void	    	    
AddressFamily::operator()(int addressFamily)
{
    addressFamily_ = addressFamily;
}


bool	    	    
AddressFamily::operator==(const AddressFamily & rhs) const
{
    return ( addressFamily_ == rhs.addressFamily_ );
}

bool	    	    
AddressFamily::operator!=(const AddressFamily & rhs) const
{
    return ( addressFamily_ != rhs.addressFamily_ );
}


bool	    	    
AddressFamily::operator<(const AddressFamily & rhs) const
{
    return ( addressFamily_ < rhs.addressFamily_ );
}


bool	    	    
AddressFamily::operator<=(const AddressFamily & rhs) const
{
    return ( addressFamily_ <= rhs.addressFamily_ );
}


bool	    	    
AddressFamily::operator>(const AddressFamily & rhs) const
{
    return ( addressFamily_ > rhs.addressFamily_ );
}


bool	    	    
AddressFamily::operator>=(const AddressFamily & rhs) const
{
    return ( addressFamily_ >= rhs.addressFamily_ );
}


std::ostream &      
AddressFamily::writeTo(std::ostream & src) const
{
    switch ( addressFamily_ )
    {
    	case AF_INET:
	{
	    src << "AF_INET";
	    break;
	}

    	#if defined(__linux__)
	case AF_INET6:
	{
	    src << "AF_INET6";
	    break;
	}
	#endif // defined(__linux__)
	
	default:
	{
	    src << "Unknown AddressFamily";
	}
    }

    return ( src );
}
