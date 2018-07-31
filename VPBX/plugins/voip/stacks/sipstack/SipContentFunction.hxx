#ifndef SipContentFunction_HXX
#define SipContentFunction_HXX

/*
* $Log: SipContentFunction.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:06:38  lizhijie
* no message
*
* $Id: SipContentFunction.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"
#include "Data.hxx"
#include "VException.hxx"


namespace Assist
{


enum SipContentFunctionErrorType
{
    DECODE_CONTENTFUNCTION_FAILED,

    //may need to change this to be more specific
};

/// Exception handling class 
class SipContentFunctionParserException : public VException
{
    public:
        SipContentFunctionParserException( const string& msg,
                                           const string& file,
                                           const int line,
                                           const int error = 0 );
        SipContentFunctionParserException( const string& msg,
                                           const string& file,
                                           const int line,
                                           SipContentFunctionErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        SipContentFunctionErrorType getError() const
        {
            return value;
        }
        string getName() const ;
    private:
        SipContentFunctionErrorType value;

};

/// data container for ContentFunction header
class SipContentFunction
{
    public:
        /// Create one with default values
        SipContentFunction();

        // TODO - add more constructors that take the parameters needed to build it

        // TODO - add accessor functions to get at the sub components

        /*** Create by decoding the data string passed in. This is the decode
             or parse. This call should only be used inside the stack and is
             not part of the API */
        SipContentFunction( const Data& newData );
        SipContentFunction( SipContentFunction& src);
        SipContentFunction& operator=(const SipContentFunction& src);
        Data get();
        void set( const Data& newData);
        void scanSipContentFunction(const Data& data);
        /*** return the encoded string version of this. This call should only be
             used inside the stack and is not part of the API */
        Data encode();



    private:
        Data data;
        void decode(const Data& data);
};
 
}

#endif
