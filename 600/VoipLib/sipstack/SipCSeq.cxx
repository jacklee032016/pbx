/*
 * $Id: SipCSeq.cxx,v 1.2 2007/03/01 20:04:31 lizhijie Exp $
 */

#include "global.h"

#include <cmath>
#include <cassert>

#include "SipCSeq.hxx"
#include "SipRequestLine.hxx"
#include "SipMethod.hxx"

using namespace Vocal;

string
SipCSeqParserException::getName( void ) const
{
    return "SipCSeqParserException";
}


SipCSeq::SipCSeq()
        :
        flag(false)
{
}


SipCSeq::SipCSeq(Method newMethod, const Data& cseqvalue)
        :
        flag(false)
{
    method = methodToData(newMethod);
    setCSeqData(cseqvalue);
}


SipCSeq::SipCSeq(const Data& newMethod, const Data& cseqvalue)
        :
        flag(false)
{
    setMethod(newMethod);
    setCSeqData(cseqvalue);
}


SipCSeq::SipCSeq(const Data& data)
        :
        flag(false)
{
    try
    {
        decode(data);
    }
    catch (SipCSeqParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in CSeq Constructor :( ");
            throw SipCSeqParserException(
                "failed to decode the CallId string",
                __FILE__,
                __LINE__, DECODE_CSeq_FAILED);
        }
    }

}


SipCSeq::SipCSeq( const SipCSeq& src)
        :
        cseq(src.cseq),
        method(src.method),
        flag(src.flag)
{
}


const SipCSeq& 
SipCSeq::operator =(const SipCSeq& src)
{
    if ( &src != this)
    {
        cseq = src.cseq;
        method = src.method;
        flag = src.flag;
    }
    return *this;
}


void
SipCSeq::parseCSeq(const Data & ldata)
{
    if (flag)
    {

        setCSeqData(ldata);
    }
    else
    {
        //Exceptions!
    }
}


void
SipCSeq::parseMethod(const Data & data)
{
    flag = true;
    Data tmp(data);
    tmp.removeSpaces();
    setMethod(tmp);
}


void
SipCSeq::decode( const Data& cseqstr )
{

    try
    {
        parse(cseqstr);
    }
    catch (SipCSeqParserException exception)
    {

        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in CSeq Constructor :( ");
            throw SipCSeqParserException(
                "failed to decode the CallId string",
                __FILE__,
                __LINE__, DECODE_CSeq_FAILED);
        }

    }
}


void
SipCSeq::scanSipCSeq( const Data & tmpdata)
{
    Data sipdata;
    Data data = tmpdata;
    int ret = data.match(" ", &sipdata, true);
    if (ret == FOUND)
    {

        parseMethod(data);
        parseCSeq(sipdata);
    }
    else if (ret == NOT_FOUND)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in CSeq Constructor scanSipCSeq :( ");
            throw SipCSeqParserException(
                "failed to decode the CallId string",
                __FILE__,
                __LINE__, DECODE_CSeq_FAILED);
        }

    }
    else if (ret == FIRST)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in CSeq Constructor scanSipCSeq :( ");
            throw SipCSeqParserException(
                "failed to decode the CallId string",
                __FILE__,
                __LINE__, DECODE_CSeq_FAILED);
        }

    }
}


void
SipCSeq::parse( const Data& cseqdata)
{
    Data data = cseqdata;

    try
    {
        scanSipCSeq(data);
    }
    catch (SipCSeqParserException exception)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in CSeq Pares() :( ");
            throw SipCSeqParserException(
                "failed to decode the CallId string",
                __FILE__,
                __LINE__, DECODE_CSeq_FAILED);
        }
    }
    //everything allright.
}


/**it is up to the user to set the correct value of the CSeq.
when incrementing it, the user should convert to uint and increment */
void 
SipCSeq::setCSeqData(const Data& data)
{
    cseq = data;
}


void 
SipCSeq::setCSeq(unsigned int seqnum)
{
    if( seqnum < static_cast<unsigned int>(0x80000000) )   // 2**31
    {
        cseq = Data(int(seqnum));
    }
    else
    {
        cpLog( LOG_ERR, "seqnum is out of range, defaulting to 2**31 - 1" );
        cseq = Data(0x7fffffff);
    }
}


Data 
SipCSeq::encode() const
{
    Data data;
    data = CSEQ;
    data += SP;

    data += cseq;
    data += SP;

    data += method;
    data += CRLF;
    return data;
}


int
SipCSeq::getCSeq() const
{
    unsigned int val = cseq.convertInt();
    return val;
}

int
SipCSeq::getNextCSeq() const
{
    unsigned int val = cseq.convertInt();
    if( val < static_cast<unsigned int>(0x7fffffff) )   // 2**31
    {
        ++val;
    }
    else
    {
        cpLog( LOG_ERR, "next seqnum is out of range, defaulting to 2**31 - 1" );
        val = 0x7fffffff;
    }
    return val;
}


void
SipCSeq::incrCSeq()
{
    cseq = Data(getNextCSeq());
}


bool 
SipCSeq::operator == (const SipCSeq& other) const
{
    cpDebug(LOG_DEBUG_STACK, "SipCSeq == operator");

    if ((cseq == other.cseq) && (method == other.method))
    {
        cpDebug(LOG_DEBUG_STACK, "SipCSeq == operator returning true");

        return true;
    }
    else
    {
        cpDebug(LOG_DEBUG_STACK, "SipCSeq == operator returning false");

        return false;
    }
}


bool 
SipCSeq::operator < (const SipCSeq& other) const
{
    if (cseq < other.cseq)
    {
        return true;
    }
    else if (cseq > other.cseq)
    {
        return false;
    }

    else if (method < other.method)
    {
        return true;
    }
    else if (method > other.method)
    {
        return false;
    }
    else
    {
        return false;
    }
}


bool 
SipCSeq::operator > (const SipCSeq& other) const
{
    if (cseq > other.cseq)
    {
        return true;
    }
    else if (cseq < other.cseq)
    {
        return false;
    }

    else if (method > other.method)
    {
        return true;
    }
    else if (method < other.method)
    {
        return false;
    }
    else
    {
        return false;
    }
}


void
SipCSeq::setMethod(const Data & newMethod)
{
    method = newMethod;
}


#if USE_HASH_MAP
size_t
SipCSeq::hashfn() const
{
    return cseq.hashfn() ^ method.hashfn();
}
#endif


SipHeader*
SipCSeq::duplicate() const
{
    return new SipCSeq(*this);
}


bool
SipCSeq::compareSipHeader(SipHeader* msg) const
{
    SipCSeq* otherMsg = dynamic_cast<SipCSeq*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}
