#ifndef SipPriority_HXX
#define SipPriority_HXX
/*
* $Log: SipPriority.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:06:38  lizhijie
* no message
*
* $Id: SipPriority.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"

#include "Data.hxx"
#include "SipHeader.hxx"
#include "VException.hxx"

namespace Assist
{


enum SipPriorityErrorType
{
    DECODE_FAILED_PRIORITY

    //may need to change this to be more specific
};

/// Exception handling class
class SipPriorityParserException : public VException
{
    public:
        SipPriorityParserException( const string& msg,
                                    const string& file,
                                    const int line,
                                    const int error = 0 );
        SipPriorityParserException( const string& msg,
                                    const string& file,
                                    const int line,
                                    SipPriorityErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        SipPriorityErrorType getError() const
        {
            return value;
        }
        string getName() const ;
    private:
        SipPriorityErrorType value;

};




/// data container for Priority header
class SipPriority : public SipHeader
{
    public:
        /// Create one with default values
        SipPriority();

        // TODO - add more constructors that take the parameters needed to build it

        /*** Create by decoding the data string passed in. This is the decode
             or parse. This call should only be used inside the stack and is
             not part of the API */
        SipPriority( const Data& srcData );

        ///
        SipPriority( const SipPriority& src );

        ///
        SipPriority& operator = (const SipPriority& src);

  ///
  bool operator == (const SipPriority& src) const;
        void scanPriority(const Data & scandata);
        ///
        Data get() const;

        ///
        void set( const Data& newData);

        /*** return the encoded string version of this. This call should only be
             used inside the stack and is not part of the API */
        Data encode() const;
        Data prioritydata;

	/// method for copying sip headers of any type without knowing which type
	SipHeader* duplicate() const;
	/// compare two headers of (possibly) the same class
	virtual bool compareSipHeader(SipHeader* msg) const;
    private:
        void decode(const Data& data);
        Data data;
        friend class SipCommand;
};

}

#endif
