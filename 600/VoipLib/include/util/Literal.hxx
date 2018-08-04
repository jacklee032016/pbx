#ifndef LITERAL_HXX_
#define LITERAL_HXX_
/*
 * $Id: Literal.hxx,v 1.2 2007/03/01 20:04:29 lizhijie Exp $
 */


#include "Data.hxx"

class Literal: public Data
{
    public:
        /// Default constructor
        Literal( );
        /** constructor for C style strings
	 ** @param str      null-terminated (C style) character array
	 */
        Literal( const char* str );
        /** constructor for character arrays with length 
	 **  @param buffer character array 
	 **  @param len    size of buffer
	 */
        Literal( const char* buffer, int len );
        /// copy constructor
        Literal( const Data& data );
        /** constructor for C++ strings
	 */
        Literal( const string& str);
        /** constructor for mstring (a specialization of C++ strings)
	 */
        //Literal( const mstring& mstr);
        /// constructor that converts an int to a Literal
        Literal( const int value);

        /// destructor
        ~Literal();


        /** OVERLOADED for caseinsensitive equality operator
	 ** @param data   Literal to compare to
	 */
        bool operator==( const Data& data ) const;
        bool operator==( const char* data ) const;

        /// OVERLOADED for caseinsensitive inequality operator
        bool operator!=( const Data& data ) const;
        bool operator!=( const char* data ) const;

        /// friend to compare a c-style string to a data (avoids conversion)
        friend bool operator==( const char* data, const Literal& d );
        friend bool operator==( const Data& data, const Literal& d );

        /// friend to compare a c-style string to a data (avoids conversion)
        friend bool operator!=( const char* str, const Literal& d );
        friend bool operator!=( const Data& str, const Literal& d );

};


#endif
