#ifndef SipEncryption_HXX
#define SipEncryption_HXX
/*
 * $Id: SipEncryption.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */

#include "Data.hxx"
#include "symbols.hxx"
#include "SipHeader.hxx"
#include "VException.hxx"


namespace Vocal
{

enum SipEncryptionErrorType
{
    DECODE_ENCRYPTION_FAILED

    //may need to change this to be more specific
};

/// Exception handling class SipEncryptionParserException
class SipEncryptionParserException : public VException
{
    public:
        ///
        SipEncryptionParserException( const string& msg,
                                      const string& file,
                                      const int line,
                                      const int error = 0 );
        ///
        SipEncryptionParserException( const string& msg,
                                      const string& file,
                                      const int line,
                                      SipEncryptionErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        ///
        SipEncryptionErrorType getError() const
        {
            return value;
        }
        ///
        string getName() const ;
    private:
        SipEncryptionErrorType value;

};


/// data container for Encryption header
class SipEncryption : public SipHeader
{
    public:
        /// Create one with default values
        SipEncryption();

        // TODO - add more constructors that take the parameters needed to build it

        // TODO - add accessor functions to get at the sub components

        /*** Create by decoding the data string passed in. This is the decode
        or parse. This call should only be used inside the stack and is
        not part of the API */
        SipEncryption( const Data& srcData );
        ///
        SipEncryption( const SipEncryption& src );

        ///
        bool operator ==( const SipEncryption& src ) const;
        ///
        const SipEncryption& operator=( const SipEncryption& src );

        ///
        Data getScheme();
        ///
        void setScheme( const Data& newScheme);

        ///
        Data getToken();
        ///
        Data getTokenValue();
        ///
        void setToken( const Data& newToken);
        ///
        void scanEncryption(const Data & scandata);
        ///
        void setTokenValue( const Data& newTokenvalue);

        /*** return the encoded string version of this. This call should only be
        used inside the stack and is not part of the API */
        ///
        Data encode() const;

	/// method for copying sip headers of any type without knowing which type
	SipHeader* duplicate() const;
	/// compare two headers of (possibly) the same class
	virtual bool compareSipHeader(SipHeader* msg) const;
    private:
        Data scheme;
        Data token;
        Data tokenvalue;
        void decode(const Data& data);
        friend class SipMsg;
};
 
} // namespace Vocal

#endif
