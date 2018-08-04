#ifndef SIP_PARAMETER_LIST_HXX
#define SIP_PARAMETER_LIST_HXX
/*
 * $Id: SipParameterList.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */

#include <map>

#include "global.h"
#include "SipParserMode.hxx"
#include "Data.hxx"
#include "Sptr.hxx"
#include "Mutex.hxx"
#include "Lock.hxx"
#include "VException.hxx"

/// Exception handling class

namespace Vocal
{


enum SipParameterListErrorType
{
    PARAMETER_LIST_DECODE_FAILED
};

///
class SipParameterListParserException : public VException
{
    public:
        ///
        SipParameterListParserException( const string& msg,
                                         const string& file,
                                         const int line,
                                         const int error = 0 );
    
        ///
        SipParameterListParserException( const string& msg,
                                         const string& file,
                                         const int line,
                                         SipParameterListErrorType i)
            : VException( msg, file, line, static_cast < int > (i))
            {
                value = i;
            }

        ///
        SipParameterListErrorType getError() const
            {
                return value;
            }

        ///
        string getName() const ;
    private:
        SipParameterListErrorType value;

};

/** data container for Parameter Lists.  at the moment, we assume that
    there can be at most one parameter of any given pname, as that
    makes life easiest for the moment.  i don't think this is a
    guaranteed assumption, but it makes life better, and it seems to
    match up to NIST's approach, so we'll take it.  */

class SipParameterList : public std::map <Data, Data>
{
    public:
	/// Create one with default values
        SipParameterList(char delimiter=';');
	///
	SipParameterList(const Data& data, char delimiter=';') throw(SipParameterListParserException &) ;
	///
	SipParameterList(const SipParameterList& src );
    
	///
	~SipParameterList();

	/** return the encoded string version of this. This call
            should only be used inside the stack and is not part of
            the API */
	Data encode() const ;

	///parser functions
	void decode(Data data, char delimiter=';', bool eatWhitespace=true);

        ///
        Data getValue(const Data& key) const;

        ///
        void setValue(const Data& key, const Data& value);

        /// clear the value for key 
        void clearValue(const Data& key);

        ///
        const SipParameterList&  operator=(const SipParameterList& src)
        {
            if(this != &src)
            {
                Threads::Lock lock1(src.myMutex);
                Threads::Lock lock2(myMutex);
                std::map<Data, Data>::operator=(src);
                myDelimiter = src.myDelimiter; 
            }
            return *this;
        }

    private:
        char     myDelimiter;
        mutable Threads::Mutex   myMutex;

};

 
} // namespace Vocal

#endif
