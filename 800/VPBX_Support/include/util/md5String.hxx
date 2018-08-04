/*
* $Id: md5String.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#ifndef MD5String_HXX
#define MD5String_HXX


#include "global.h"
#include <assert.h>
#include <string>
#include "vmd5.h"
#include "cpLog.h"

static const char* const md5StringVersion = "$Id: md5String.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $";


/// C++ wrapper for vmd5.[ch]
//  Usage:
//  #include "md5String.hxx"
//
//   md5String  md5Str;
//   string     key = "E944D9A6-88C509CE-0-2888184C@192.168.6.210";
//   string     digest = md5Str.getMd5String( key );
//
class md5String
{
    public:
        ///
        md5String()
        {
            MD5Init( &_context );
        }

        ///
        ~md5String()
        {}

        // given a string, compute the message digest

        const string& getMd5String( const string& buf )
        {

            unsigned char digest[16];

            MD5Update( &_context,
                       reinterpret_cast < unsigned const char* > (buf.c_str()),
                       buf.length() );

            MD5Final( digest, &_context );

            /// note: to make digest printable, need to encode digest
            ///       in base64 encoding
            _md5String = string(reinterpret_cast < char* > (digest), 16);
            return _md5String;
        }

        ///

    private:

        MD5Context _context;
        string _md5String;

        ///
        md5String( const md5String& rhs )
        {
            assert( 0 );
        }
        ///
        md5String& operator=( const md5String& rhs )
        {
            assert( 0 );
	    return (*this);
        }

};

#endif
