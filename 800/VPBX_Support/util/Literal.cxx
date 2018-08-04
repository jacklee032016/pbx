/*
* $Id: Literal.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "Literal.hxx"

Literal::Literal() : Data()
{}


Literal::Literal( const char* str, int length )
    : Data (str, length)
{}


Literal::Literal( const char* str )
    : Data (str)
{}


Literal::Literal( const string& str)
    : Data (str)
{}


/*
Literal::Literal( const mstring& mstr)
    : Data (mstr)
{}
*/

Literal::Literal( int value)
    : Data (value)
{}


Literal::Literal( const Data& data )
    : Data (data)
{}


bool
Literal::operator==( const Data& data) const
{
//    return static_cast < LiteralImplType* > (dataImpl__)->
//	operator==(*(static_cast < LiteralImplType* > (data.dataImpl__)));
    return isEqualNoCase(*this, data);
}

bool
Literal::operator==( const char* data) const
{
//    return static_cast < LiteralImplType* > (dataImpl__)->
//	operator==(*(static_cast < LiteralImplType* > (data.dataImpl__)));
    return isEqualNoCase(*this, data);
}

bool
Literal::operator!=(const Data& data) const
{
//    return static_cast < LiteralImplType* > (dataImpl__)->operator!=(*(static_cast < LiteralImplType* > (data.dataImpl__)));
    return !isEqualNoCase(*this, data);
}

bool
Literal::operator!=(const char* data) const
{
//    return static_cast < LiteralImplType* > (dataImpl__)->operator!=(*(static_cast < LiteralImplType* > (data.dataImpl__)));
    return !isEqualNoCase(*this, data);
}


bool
operator==(const char* s, const Literal& data)
{
//    return static_cast < LiteralImplType* > (data.dataImpl__)->operator==(s);
    return isEqualNoCase(data,s);
}

bool
operator==(const Data& s, const Literal& data)
{
//    return static_cast < LiteralImplType* > (data.dataImpl__)->operator==(s);
    return isEqualNoCase(data,s);
}


bool
operator!=(const char* s, const Literal& data)
{
//    return static_cast < LiteralImplType* > (data.dataImpl__)->operator!=(s);
    return !isEqualNoCase(data,s);
}

bool
operator!=(const Data& s, const Literal& data)
{
//    return static_cast < LiteralImplType* > (data.dataImpl__)->operator!=(s);
    return !isEqualNoCase(data,s);
}


Literal::~Literal()
{}

