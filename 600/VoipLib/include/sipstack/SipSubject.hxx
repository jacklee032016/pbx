#ifndef SipSubject_HXX
#define SipSubject_HXX
/*
 * $Id: SipSubject.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */

#include "global.h"

#include "Data.hxx"
#include "SipHeader.hxx"
#include "VException.hxx"

namespace Vocal
{
    
enum SipSubjectErrorType
{
    DECODE_SUBJECT_FAILED


    //may need to change this to be more specific
};

/// Exception handling class
class SipSubjectParserException : public VException
{
    public:
        SipSubjectParserException( const string& msg,
                                   const string& file,
                                   const int line,
                                   const int error = 0 );
        SipSubjectParserException( const string& msg,
                                   const string& file,
                                   const int line,
                                   SipSubjectErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        SipSubjectErrorType getError() const
        {
            return value;
        }
        string getName() const ;
    private:
        SipSubjectErrorType value;

};
/// data container for Subject header
class SipSubject : public SipHeader
{
    public:
        /// Create one with default values
        SipSubject();

        /*** Create by decoding the data string passed in. This is the decode
             or parse. This call should only be used inside the stack and is
             not part of the API */
        SipSubject( const Data& srcData );
        SipSubject( const SipSubject& src );
       ///
        void scanSipSubject(const Data& data);
        ///
        Data get() const;

        ///
        void set( const Data& newData);

        const SipSubject& operator=(const SipSubject& src);
        ///
        bool operator==(const SipSubject& src) const;
        /*** return the encoded string version of this. This call should only be
             used inside the stack and is not part of the API */
        Data encode() const;

	/// method for copying sip headers of any type without knowing which type
	SipHeader* duplicate() const;
	/// compare two headers of (possibly) the same class
	virtual bool compareSipHeader(SipHeader* msg) const;
    private:
        Data data;
        void decode(const Data& data);
        friend class SipCommand;
};

 
} // namespace Vocal

#endif
