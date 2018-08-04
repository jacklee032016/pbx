/*
 * $Id: SipTransactionId.cxx,v 1.2 2007/03/01 20:04:32 lizhijie Exp $
 */

#include "SipTransactionId.hxx"
#include "SipMsg.hxx"
#include "SipTo.hxx"
#include "SipFrom.hxx"
#include "SipCallId.hxx"
#include "SipCSeq.hxx"
#include "SipVia.hxx"
#include "BaseUrl.hxx"

using namespace Vocal;

SipTransactionId :: SipTransactionId(const SipMsg& sipMsg)
{
    valid = true;
    Sptr<BaseUrl> toUrl = sipMsg.getTo().getUrl();
    if(toUrl != 0)
    {
        level1 = toUrl->getUniqueKey();
    }
    else
    {
        level1 = "invalid-url";
        valid = false;
    }

    Sptr<BaseUrl> fromUrl = sipMsg.getFrom().getUrl();
    if(fromUrl != 0)
    {
        level1+= fromUrl->getUniqueKey();
    }
    else
    {
        level1 += "invalid-url";
        valid = false;
    }
    level1+= sipMsg.getCallId().getCallIdData();
    level2 = sipMsg.getCSeq().getCSeqData() + "V"; // add 'V' to end of CSeq number
    level2+= sipMsg.getVia(0).getBranch();
    level3 = sipMsg.getCSeq().getMethod();
}


SipTransactionId :: SipTransactionId(const SipTransactionId& sipTransactionId)
    : valid(sipTransactionId.valid),
      level1(sipTransactionId.level1),
      level2(sipTransactionId.level2),
      level3(sipTransactionId.level3)
{
}


SipTransactionId :: ~SipTransactionId()
{
}


SipTransactionId&
SipTransactionId :: operator= (const SipTransactionId& sipTransactionId)
{
    if (this != &sipTransactionId)
    {
        valid  = sipTransactionId.valid;
	level1 = sipTransactionId.level1;
	level2 = sipTransactionId.level2;
	level3 = sipTransactionId.level3;
    }
    return *this;
}

bool
SipTransactionId::operator==(const SipTransactionId& sipTransactionId) const
{
    return (valid  == sipTransactionId.valid  &&
            level1 == sipTransactionId.level1 &&
            level2 == sipTransactionId.level2 &&
            level3 == sipTransactionId.level3);
}

bool
SipTransactionId::operator<(const SipTransactionId& sipTransactionId) const
{
    if(this == &sipTransactionId)
    {
        return false;
    }
    else
    {
        if (level1 < sipTransactionId.level1)
            return true;
        else if (level1 > sipTransactionId.level1)
            return false;
        else if (level2 < sipTransactionId.level2)
            return true;
        else if (level2 > sipTransactionId.level2)
            return false;
        else if (level3 < sipTransactionId.level3)
            return true;
        else if (level3 > sipTransactionId.level3)
            return false;
        else if (valid  < sipTransactionId.valid)
            return true;
        else
            return false;
    }
}


SipTransactionId::KeyTypeI&
SipTransactionId :: getLevel1() const
{
    return level1;
}


SipTransactionId::KeyTypeII&
SipTransactionId :: getLevel2() const
{
    return level2;
}


SipTransactionId::KeyTypeIII&
SipTransactionId :: getLevel3() const
{
    return level3;
}


bool
SipTransactionId :: getValid() const
{
    return valid;
}
