#ifndef SipOrganization_HXX
#define SipOrganization_HXX
/*
 * $Id: SipOrganization.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */

#include "global.h"

#include "Data.hxx"
#include "SipHeader.hxx"
#include "VException.hxx"

namespace Vocal
{


enum SipOrganizationErrorType
{
    DECODE_ORGANIZATION_FAILED

    //may need to change this to be more specific
};

/// Exception handling class
class SipOrganizationParserException : public VException
{
    public:
        SipOrganizationParserException( const string& msg,
                                        const string& file,
                                        const int line,
                                        const int error = 0 ):VException(msg, file, line,error){};
        SipOrganizationParserException( const string& msg,
                                        const string& file,
                                        const int line,
                                        SipOrganizationErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        SipOrganizationErrorType getError() const
        {
            return value;
        }
        string getName() const ;
    private:
        SipOrganizationErrorType value;
};




/// data container for Organization header
class SipOrganization : public SipHeader
{
    public:
        /// Create one with default values
        SipOrganization();

        // TODO - add more constructors that take the parameters needed to build it

        /*** Create by decoding the data string passed in. This is the decode
             or parse. This call should only be used inside the stack and is
             not part of the API */
        SipOrganization( const Data& src );

        SipOrganization( const SipOrganization& src );

        ///
        SipOrganization& operator = (const SipOrganization& src);
        ///
        bool operator == (const SipOrganization& src) const;

        ///
        void scanSipOrganization(const Data& cdata);
        ///
        Data get() const;

        ///
        void set( const Data& newData);

        /*** return the encoded string version of this. This call should only be
           used inside the stack and is not part of the API */
        Data encode() const;

	/// method for copying sip headers of any type without knowing which type
	SipHeader* duplicate() const;
	/// compare two headers of (possibly) the same class
	virtual bool compareSipHeader(SipHeader* msg) const;
    private:
        Data organization;
        void decode(const Data& data);
        friend class SipCommand;
};

 
} // namespace Vocal

#endif
