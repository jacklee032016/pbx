#ifndef SipContentDisposition_HXX
#define SipContentDisposition_HXX

/*
* $Log: SipContentDisposition.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:06:38  lizhijie
* no message
*
* $Id: SipContentDisposition.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"
#include "Data.hxx"
#include "SipHeader.hxx"
#include "VException.hxx"


namespace Assist
{

enum SipContentDispositionErrorType
{
    DECODE_CONTENTDISPOSITION_FAILED

    //may need to change this to be more specific
};

/// Exception handling class 
class SipContentDispositionParserException : public VException
{
    public:
        SipContentDispositionParserException( const string& msg,
                                              const string& file,
                                              const int line,
                                              const int error = 0 );
        SipContentDispositionParserException( const string& msg,
                                              const string& file,
                                              const int line,
                                              SipContentDispositionErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        SipContentDispositionErrorType getError() const
        {
            return value;
        }
        string getName() const ;
    private:
        SipContentDispositionErrorType value;

};

/// data container for ContentDisposition header
class SipContentDisposition : public SipHeader
{
    public:
        /// Create one with default values
        SipContentDisposition();

        // TODO - add more constructors that take the parameters needed to build it

        // TODO - add accessor functions to get at the sub components

        /*** Create by decoding the data string passed in. This is the decode
             or parse. This call should only be used inside the stack and is
             not part of the API */
        SipContentDisposition( const Data& newData );
        SipContentDisposition(const SipContentDisposition& src);
        SipContentDisposition& operator= ( const SipContentDisposition & src);
        bool operator== ( const SipContentDisposition & src) const;
        void setDispositionType(const Data& newdispositiontype)
        {
            dispositiontype = newdispositiontype ;
        }
        void setAttribute(const Data& newAttribute)
        {
            attribute = newAttribute;
        }
        void setValue(const Data& newvalue)
        {
            value = newvalue;
        }
        void setHandleParm(const Data& newhandleparm)
        {
            handleparm = newhandleparm;
        }
        Data getAttribute() const
        {
            return attribute;
        }
        Data getValue() const
        {
            return value;
        }
        Data getHandleParm() const
        {
            return handleparm;
        }
        Data getDispositionType() const
        {
            return dispositiontype ;
        }
       
        /*** return the encoded string version of this. This call should only be
             used inside the stack and is not part of the API */
        Data encode() const;



	/// method for copying sip headers of any type without knowing which type
	SipHeader* duplicate() const;
	/// compare two headers of (possibly) the same class
	virtual bool compareSipHeader(SipHeader* msg) const;
    private:
        Data attribute;
        Data value;
        Data handleparm;
        Data dispositiontype;
        void decode(const Data& data);
        void scanSipContentDisposition(const Data& data);
        void parseDispositionParm(const Data &tmepdata);
        void parseParms(const Data &tpdata);
        void parseFinParms(const Data &tpdata, const Data &tvalue);  
    
        friend class SipMsg;
        friend class SipCommand;
};
 
}

#endif
