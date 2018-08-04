#ifndef SipReplaces_HXX
#define SipReplaces_HXX
/*
 * $Id: SipReplaces.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */

#include "global.h"
#include "Data.hxx"
#include "cpLog.h"
#include "Sptr.hxx"
#include "SipHeader.hxx"
#include "SipParameterList.hxx"
#include "VException.hxx"

namespace Vocal
{

/// Exception handling class SipReplacesParserException
class SipReplacesParserException : public VException
{
    public:
        SipReplacesParserException( const string& msg,
                                    const string& file,
                                    const int line) : VException( msg, file, line){}
        string getName() const { return "SipReplacesParserException";}
};

/// data container for Replaces header
class SipReplaces : public SipHeader
{
    public:

        SipReplaces();
        SipReplaces( const Data& data );
        SipReplaces(const SipReplaces&);
        SipReplaces& operator= (const SipReplaces&);
        bool operator== (const SipReplaces& srcReplaces) const;

        const Data& getCallId() const;
        const Data getFromTag() const;
        const Data getToTag() const;

        Data encode() const;

	/// method for copying sip headers of any type without knowing which type
	SipHeader* duplicate() const;

	/// compare two headers of (possibly) the same class
	virtual bool compareSipHeader(SipHeader* msg) const;
    private:
        Data callId;
        SipParameterList params;

        void decode(const Data& data);
        void parse(const Data& data);

        friend class SipMsg;
};
 
} // namespace Vocal

#endif
