#ifndef SipCSeq_HXX
#define SipCSeq_HXX
/*
 * $Id: SipCSeq.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */

#include "global.h"
#include "symbols.hxx"
#include "Data.hxx"
#include "SipHeader.hxx"
#include "VException.hxx"

namespace Vocal
{


enum SipCSeqErrorType
{
    DECODE_CSeq_FAILED

    //may need to change this to be more specific
};

/// Exception handling class SipCSeqParserException
class SipCSeqParserException : public VException
{
    public:
        SipCSeqParserException( const string& msg,
                                const string& file,
                                const int line,
                                const int error = 0 );
        SipCSeqParserException( const string& msg,
                                const string& file,
                                const int line,
                                SipCSeqErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        const SipCSeqErrorType& getError() const
        {
            return value;
        }
        string getName() const ;
    private:
        SipCSeqErrorType value;
};



/// data container for CSeq header
class SipCSeq : public SipHeader
{
    public:
        SipCSeq(const SipCSeq& src);
        const SipCSeq& operator=(const SipCSeq& src);
        SipCSeq();
        /// Create one with default values
        SipCSeq( Method newMethod, const Data& cseqvalue);
        SipCSeq(const Data& newMethod, const Data& cseqvalue);
        SipCSeq( const Data& data );

        Data encode() const;

        int getNextCSeq() const;

        int getCSeq() const;

        void setCSeq(unsigned int cseq);

        void incrCSeq();

        Data getCSeqData() const
        {
            return cseq;
        }

        void setCSeqData(const Data& cseqData);

        void setMethod(const Data&);

        Data getMethod() const
        {
            return method;
        }
        void parse(const Data& data);
        void parseSipCSeq(const Data& data);
        void parseCSeq(const Data& data);
        void parseMethod(const Data& data);
        ///
        void scanSipCSeq(const Data& data);
        ///
        bool operator == (const SipCSeq& other) const;
        ///
        bool operator != (const SipCSeq& other) const
        { return !(*this == other);}
        ///
        bool operator < (const SipCSeq& other) const;

        ///
        bool operator > (const SipCSeq& other) const;
#if USE_HASH_MAP
        ///
        size_t hashfn() const;
#endif
	/// method for copying sip headers of any type without knowing which type
	SipHeader* duplicate() const;
	/// compare two headers of (possibly) the same class
	virtual bool compareSipHeader(SipHeader* msg) const;
    private:
        Data cseq;
        Data method;
        bool flag;  // TODO - this variable name is REALLY bad - CJ
        void decode(const Data& data);
        friend class SipMsg;
};
 
} // namespace Vocal

#endif
